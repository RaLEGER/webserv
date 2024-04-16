/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:36:10 by rleger            #+#    #+#             */
/*   Updated: 2024/04/16 19:13:00 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server( ) {
	
}

Server::~Server( ) {

}

void	Server::addLocation(Location* location) {
	_locations[location->getHost()] = location;
}

void	Server::addDefLoc(Location* location) {
	_defLoc = location;
}

void Server::start() {
	setupServerSocket();
}

int Server::createSocket() {
    int newSocket = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;
    
	if (newSocket < 0) {
		throw CustomError(1 ,"Socket creation failed");
	}
	if( setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
        sizeof(opt)) < 0 )   {   
		throw CustomError(1, "setsockopt failed");
    }
	try {
    	setNonBlocking(newSocket);
	} catch (const std::exception& e){ 
		std::cerr << "Non blocking initialisation failed: " << e.what() << std::endl; 
		throw;
	}

    return newSocket;
}

void Server::setNonBlocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    
    if (flags == -1) {
		throw CustomError(1, "fcntl flags");
    }

    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK | FD_CLOEXEC) == -1) {
		throw CustomError(1, "setup fcntl");
    }

}


void Server::setupServerSocket() {
    _serverSocket = createSocket();

	struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
	
	serverAddr.sin_addr.s_addr  = inet_addr(_defLoc->getHost().c_str());	
	serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_defLoc->getPort());

    if (bind(_serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
  		throw CustomError(1, "Bind failed");
    }
	
    if (listen(_serverSocket, 100) < 0) {
  		throw CustomError(1, "Listen failed");
    }

    std::cout << "Server socket setup completed." << std::endl;
}

std::string Server::getServerName( ) {
	return (_defLoc->getHost());
}

Location* Server::getLocation(const std::string& path) {
	return _locations[path];
}

int	Server::getSocket( ) {
	return _serverSocket;
}

int	Server::getClientSocket() {
	int	clientSocket = accept(_serverSocket, NULL, NULL);

	if (clientSocket == -1) {
		std::cerr << "Accept error, could not retrieve client Socket" << std::endl;	
		return -1;
	}
	else {
		if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
			std::cerr << "Accept error, could not retrieve client Socket" << std::endl;	
			return -1;
		}
	}
	_readData.insert(std::make_pair(clientSocket, ""));
	return clientSocket;
}

int	Server::readData(int clientSocket) {
	char	buffer[BUFF_SIZE];
	ssize_t bytes_received;
	ssize_t total_bytes = 0;
    while ((bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
		_readData[clientSocket] += buffer;
		total_bytes += bytes_received;
    }

	if (!total_bytes) {
		std::cerr << "Connection closed, nothing to read" << std::endl;
		return -1;
	}
	if (bytes_received == -1) {
		_readData.erase(clientSocket);
		std::cerr << "Error reading from client socket" << std::endl;
		return -1;
	}
	
	if (_readData[clientSocket].find("Transfer-Encoding: chunked") != std::string::npos)
			return 0;
	return 1; 
}

int	Server::processRequest(int clientSocket) {
	RequestHandler *requestHandler = new RequestHandler(_readData[clientSocket], clientSocket);
	_requestHandlers.insert(std::make_pair(clientSocket, requestHandler));
	requestHandler->process();
	_readData.erase(clientSocket);
	return 1;
}

void	Server::sendResponse(int clientSocket) {
	int flags = 0;

	std::string responseString = _requestHandlers[clientSocket]->getResponseString();
	// std::cout << "************** responseString: ****************" << std::endl;
	// std::cout << responseString << std::endl;
	// std::cout << "***********************************************" << std::endl;
	std::cout << "send value " << send(clientSocket, responseString.c_str(), responseString.size(), flags) << std::endl;
	send(clientSocket, responseString.c_str(), responseString.size(), flags);
	//check if all is sent//
	delete _requestHandlers[clientSocket];
	_requestHandlers.erase(clientSocket);
}