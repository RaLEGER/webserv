/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:07:30 by rleger            #+#    #+#             */
/*   Updated: 2024/04/23 13:34:03 by rleger           ###   ########.fr       */
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

int	Socket::readData(int clientSocket) {
	char	buffer[BUFF_SIZE];
	int		bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
	bool	firstRecv = _readData.find(clientSocket) == _readData.end();

	
	if (bytes_received < 0) {
		std::cerr << "Error reading from client socket" << std::endl;
		if (!firstRecv)
			_readData.erase(clientSocket);
		return -1;
	}
	if (bytes_received  == 0) {
		if (firstRecv) {
			std::cerr << "Connection closed, nothing to read" << std::endl;
			return -1;
		}
		else 
			return 1;
	}
	if (firstRecv)
		_readData.insert(std::make_pair(clientSocket, std::string(buffer, bytes_received)));
	else if (bytes_received)
		_readData[clientSocket].append(std::string(buffer, bytes_received));

	if (bytes_received == BUFF_SIZE) {
		std::cout << "not all read" << std::endl;
		return 0;
	}

	// Check if the request is chunked
	if (_readData[clientSocket].find("Transfer-Encoding: chunked") != std::string::npos) {
		// Process the chunked data
		while (true) {
			// Find the end of the current chunk size
			size_t pos = _readData[clientSocket].find("\r\n");
			if (pos == std::string::npos) {
				// Chunk size not complete, need more data
				return 0;
			}

			// Extract the chunk size
			std::string chunkSizeStr = _readData[clientSocket].substr(0, pos);
			size_t chunkSize = std::atol(chunkSizeStr.c_str());
			if (chunkSize == 0) {
				// End of chunks
				return 1;
			}

			// Check if we have received the entire chunk
			size_t totalChunkSize = pos + 2 /* for \r\n */ + chunkSize + 2 /* for \r\n */;
			if (_readData[clientSocket].size() < totalChunkSize) {
				// Chunk not complete, need more data
				return 0;
			}

			// Extract the chunk data
			std::string chunkData = _readData[clientSocket].substr(pos + 2, chunkSize);
			// Remove the processed chunk from the buffer
			_readData[clientSocket] = _readData[clientSocket].substr(totalChunkSize);

			// Process the chunkData (e.g., handle or store it)
			// Example: std::cout << "Received chunk: " << chunkData << std::endl;
		}
	}

	// If not chunked, return 0 if entire request is not loaded
	std::cout << _readData[clientSocket] << std::endl;
	return 1;
}

int	Socket::processRequest(int clientSocket) {
	RequestHandler *requestHandler = new RequestHandler(_readData[clientSocket], clientSocket); //+ host:port et ou tous les serverus associés a la pair
	_requestHandlers.insert(std::make_pair(clientSocket, requestHandler));
	requestHandler->process(_servers);
	_readData.erase(clientSocket);
	return 1;
}

void	Socket::sendResponse(int clientSocket) {
	int flags = 0;

	std::string responseString = _requestHandlers[clientSocket]->getResponseString();
	// std::cout << "************** responseString: ****************" << std::endl;
	// std::cout << responseString << std::endl;
	// std::cout << "***********************************************" << std::endl;
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
	} catch (const std::exception& e) {
		std::cerr << "Error erasing RequestHandler: " << e.what() << std::endl;
	}
	// remove from 
}
