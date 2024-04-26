/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teliet <teliet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:07:30 by rleger            #+#    #+#             */
/*   Updated: 2024/04/26 11:39:36 by teliet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket(Server *server) {
	_port = server->getPort();
	_host = server->getHost();
	_setupSocket();
	
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

	std::cout << "created a new socket with fd: " << _fd << std::endl;
	std::cout << "port: " << _port << std::endl;
	std::cout << "host: " << _host << std::endl;

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

    std::cout << "Socket setup completed." << std::endl;
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
	char		buffer[BUFF_SIZE];
	
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
	if(!_requestHandlers[clientSocket]->parseHeaders(_headers[clientSocket]))
		throw CustomError(1, "Request Error: wrong header");
	std::cout << "************** bytesread1: ****************" << std::endl;
	std::cout << bytesRead << std::endl;
	std::cout << "***********************************************" << std::endl;
	if (bytesRead < BUFF_SIZE) {
		_requestHandlers[clientSocket]->setIsBodyComplete(true);
	}
}

int	Socket::_readBody(int clientSocket, bool firstIt) {
	char	buffer[BUFF_SIZE];
	
	if (_requestHandlers[clientSocket]->getIsBodyComplete())
		return 1;
		
	int	bytesRead = read(clientSocket, buffer, sizeof(buffer));
	
	if ((bytesRead == 0 && _bodies[clientSocket].empty()) || bytesRead < 0)
		return -1;
		
	_bodies[clientSocket].append(buffer, bytesRead);
	std::cout << "************** bytesread2: ****************" << std::endl;
	std::cout << bytesRead << std::endl;
	std::cout << firstIt << std::endl;
	std::cout << "***********************************************" << std::endl;
	if (_bodies[clientSocket].size() >= (size_t) _requestHandlers[clientSocket]->getContentLength()) {
		return 1;
	}
	return 0;
}
	
int	Socket::readData(int clientSocket) {
	int status;
	
	if (_requestHandlers.find(clientSocket) == _requestHandlers.end()) {
		RequestHandler* handler = new RequestHandler(clientSocket);
		_requestHandlers.insert(std::make_pair(clientSocket, handler));
		try {
			_readHeader(clientSocket);
		} catch (const CustomError& e) {
			std::cerr << e.what() << std::endl;
			return e.getErrorCode();
		}
		status = _readBody(clientSocket, true);
	}
	else
		status = _readBody(clientSocket, false);
	if (status == -1)
		std::cerr << "Error reading from clientsocket" << std::endl;
	if (status == 0) {
		return 0;
	}
		
	_requestHandlers[clientSocket]->setIsBodyComplete(true);
	_requestHandlers[clientSocket]->setBody(_bodies[clientSocket]);
	_requestHandlers[clientSocket]->process(_servers);
	return 1;
}


void	Socket::sendResponse(int clientSocket) {
	int flags = 0;
	std::string responseString = _requestHandlers[clientSocket]->getResponseString();
	std::cout << "************** responseString: ****************" << std::endl;
	std::cout << responseString << std::endl;
	std::cout << "***********************************************" << std::endl;
	std::cout << "send value " << send(clientSocket, responseString.c_str(), responseString.size(), flags) << std::endl;
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
