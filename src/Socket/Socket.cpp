/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:07:30 by rleger            #+#    #+#             */
/*   Updated: 2024/04/26 19:48:06 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket(Server *server) {
	_port = server->getPort();
	_host = server->getHost();
	_setupSocket();
	
}
Socket::~Socket( ) {
	for (std::map <int, RequestHandler* >::iterator it = _requestHandlers.begin(); it != _requestHandlers.end(); it++) {
		delete it->second;
	}
}

bool	Socket::_isDuplicate(std::string serverName) {
	
	for (std::vector<Server*>::iterator it = _servers.begin(); it != _servers.end(); it++) {
		if (!serverName.compare((*it)->getServerName()))
			return true;
	}
	return false;
}

void	Socket::addServer(Server* server) {

	if (_isDuplicate(server->getServerName()))
		throw CustomError(1,"Duplicate server");
	_servers.push_back(server);
}

void Socket::_setNonBlocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    
    if (flags == -1) {
		throw CustomError(1, "fcntl flags");
    }

    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK | FD_CLOEXEC) == -1) {
		throw CustomError(1, "setup fcntl");
    }
}

int Socket::_createSocket() {
    int newSocket = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;
    
	if (newSocket < 0) {
		throw CustomError(1 ,"Socket creation failed");
	}
	if(setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
        sizeof(opt)) < 0 )   {   
		throw CustomError(1, "setsockopt failed");
    }
	try {
    	_setNonBlocking(newSocket);
	} catch (const std::exception& e){ 
		std::cerr << "Non blocking initialisation failed: " << e.what() << std::endl; 
		throw;
	}

    return newSocket;
}

void	Socket::_setupSocket() {
	_fd = _createSocket();

	//std::cout << "created a new socket with fd: " << _fd << std::endl;
	//std::cout << "port: " << _port << std::endl;
	//std::cout << "host: " << _host << std::endl;

	struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
	
	serverAddr.sin_addr.s_addr  = inet_addr(_host.c_str());	
	serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);

    if (bind(_fd, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
  		throw CustomError(1, "Bind failed");
    }
	
    if (listen(_fd, 100) < 0) {
  		throw CustomError(1, "Listen failed");
    }

    //std::cout << "Socket setup completed." << std::endl;
}

int Socket::getSocket() {
	return _fd;
}

int	Socket::getClientSocket() {
	int	clientSocket = accept(_fd, NULL, NULL);

	if (clientSocket == -1) {
		std::cerr << "Accept error, could not retrieve client Socket" << std::endl;	
		return -1;
	}
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "fcntl error, could not retrieve client Socket" << std::endl;	
		return -1;
	}
	return clientSocket;
}


// Returns true if the body is complete, false otherwise
void	Socket::_readHeader(int clientSocket) {
	char		buffer[BUFF_SIZE] = {};
	
	int	bytesRead = read(clientSocket, buffer, sizeof(buffer));
	if (bytesRead <= 0)
		throw CustomError(-1, "Error reading from client socket");
	std::string	rawData(buffer, bytesRead);
	size_t headerPos = rawData.find("\r\n\r\n");
	if (headerPos != std::string::npos) {
		headerPos += 4;
		_bodies.insert(std::make_pair(clientSocket, rawData.substr(headerPos)));
		_headers.insert(std::make_pair(clientSocket, rawData.substr(0, headerPos)));
	}
	else
		throw CustomError(-1, "Request Error: header too long");
	if(!_requestHandlers[clientSocket]->parseHeaders(_headers[clientSocket], _servers))
		throw CustomError(1, "Request Error: wrong header");
	//std::cout << "************** bytesread1: ****************" << std::endl;
	//std::cout << bytesRead << std::endl;
	//std::cout << "***********************************************" << std::endl;
	if (_requestHandlers[clientSocket]->getIsChunkedRequest()) {
		_bodies[clientSocket] = _formatChunk(_bodies[clientSocket], clientSocket);
	}
	else if (_bodies[clientSocket].size() >= (size_t) _requestHandlers[clientSocket]->getContentLength()){
		_requestHandlers[clientSocket]->setIsBodyComplete(true);
		//std::cout << "SORTIE " << _bodies[clientSocket].size() << " " <<  _requestHandlers[clientSocket]->getContentLength() << std::endl;
	}
}

std::string	Socket::_formatChunk(std::string data, int clientSocket) {
	std::string	body;
	size_t pos = 0;
    while (pos < data.size()) {
        // Find the length of the chunk
        size_t length_end = data.find("\r\n", pos);
		//std::cout << "length " << length_end << std::endl;
        if (length_end == std::string::npos || length_end - pos > 10) {
            throw CustomError(-1, "Invalid chunk format");
        }
        size_t length = std::strtol(data.substr(pos, length_end - pos).c_str(), NULL, 16);

        // Check for terminating chunk
        if (length == 0) {
			_requestHandlers[clientSocket]->setIsBodyComplete(true);
            break;
        }

        // Extract chunk data
        std::size_t chunk_data_start = length_end + 2;
        std::size_t chunk_data_end = chunk_data_start + length;

        // Store chunk data
        body.append(data.substr(chunk_data_start, length));

        // Move to the next chunk
        pos = chunk_data_end + 2;
    }
    return body;
}

int	Socket::_readChunk(int clientSocket) {
	char	buffer[BUFF_SIZE];

	if (_requestHandlers[clientSocket]->getIsBodyComplete())
		return 1;
	int	bytesRead = read(clientSocket, buffer, sizeof(buffer));
	if (bytesRead <= 0)
		return -1;
	try {
		_bodies[clientSocket].append(_formatChunk(std::string(buffer, bytesRead), clientSocket));
	} catch (const CustomError& e) {
		std::cerr << e.what() << std::endl;
		return e.getErrorCode();
	}
	if (_requestHandlers[clientSocket]->getIsBodyComplete())
		return 1;
	return 0;
}

int	Socket::_readBody(int clientSocket) {
	char	buffer[BUFF_SIZE];
	
	// if body is complete, or no content length header, return 1
	if (_requestHandlers[clientSocket]->getIsBodyComplete() || _requestHandlers[clientSocket]->getContentLength() < 0) {
		//std::cout << "SORTIE2 " << _bodies[clientSocket].size() << " " <<  _requestHandlers[clientSocket]->getContentLength() << std::endl;
		return 1;
	}
	int	bytesRead = read(clientSocket, buffer, sizeof(buffer));
	
	if ((bytesRead == 0 && _bodies[clientSocket].empty()) || bytesRead < 0)
		return -1;
	
	_bodies[clientSocket].append(buffer, bytesRead);

	//std::cout << "************** bytesread2: ****************" << std::endl;
	//std::cout << bytesRead << std::endl;
	//std::cout << "***********************************************" << std::endl;
	
	if (_bodies[clientSocket].size() >= (size_t) _requestHandlers[clientSocket]->getContentLength()) {
		//std::cout << "SORTIE3 " << _bodies[clientSocket].size() << " " <<  _requestHandlers[clientSocket]->getContentLength() << std::endl;
		return 1;
	}
	return 0;
}
	
int	Socket::readData(int clientSocket) {
	int status;
	
	if (_requestHandlers.find(clientSocket) == _requestHandlers.end()) {
		RequestHandler* handler = new RequestHandler(clientSocket);
		handler->setIsBodyComplete(false);
		_requestHandlers.insert(std::make_pair(clientSocket, handler));
		try {
			_readHeader(clientSocket);
		} catch (const CustomError& e) {
			std::cerr << e.what() << std::endl;
			return e.getErrorCode();
		}
	}
	updateActivity(clientSocket);
	if (_requestHandlers[clientSocket]->getIsChunkedRequest())
		status = _readChunk(clientSocket);
	else
		status = _readBody(clientSocket);
		
	if (status == -1) {
		std::cerr << "Error reading from clientsocket" << std::endl;
		return -1;
	}
	if (status == 0) {
		return 0;
	}
		
	_requestHandlers[clientSocket]->setIsBodyComplete(true);
	_requestHandlers[clientSocket]->setBody(_bodies[clientSocket]);
	_requestHandlers[clientSocket]->process(_servers);
	//std::cout << _bodies[clientSocket] << std::endl;
	return 1;
}


void	Socket::sendResponse(int clientSocket) {
	int flags = 0;
	std::string responseString = _requestHandlers[clientSocket]->getResponseString();
	//std::cout << "************** responseString: ****************" << std::endl;
	//std::cout << responseString << std::endl;
	//std::cout << "***********************************************" << std::endl;
	//std::cout << "send value " << send(clientSocket, responseString.c_str(), responseString.size(), flags) << std::endl;
	send(clientSocket, responseString.c_str(), responseString.size(), flags);
	//check if all is sent//

	try {
		delete _requestHandlers[clientSocket];
	} catch (const std::exception& e) {
		std::cerr << "Error deleting RequestHandler: " << e.what() << std::endl;
	}
	try {
		_requestHandlers.erase(clientSocket);
		_headers.erase(clientSocket);
		_bodies.erase(clientSocket);
	} catch (const std::exception& e) {
		std::cerr << "Error erasing RequestHandler: " << e.what() << std::endl;
	}
	// remove from 
}

void	Socket::updateActivity(int clientSocket) {
	_requestHandlers[clientSocket]->_lastActivity = time(0);
}

int	Socket::checkTimeout() {
	for (std::map <int, RequestHandler* >::iterator it =_requestHandlers.begin(); it != _requestHandlers.end(); it++) {
		if (difftime(time(0), it->second->_lastActivity) > 10) {
			int fd = it->first;
			_requestHandlers.erase(fd);
			return fd;
		}
	}
	return -1;

}