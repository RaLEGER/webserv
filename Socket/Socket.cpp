/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:07:30 by rleger            #+#    #+#             */
/*   Updated: 2024/04/19 21:04:08 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

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


Socket::Socket(Server *server) {
	_port = server->getPort();
	_host = server->getHost();
	_setupSocket();
	
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
	else {
		if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
			std::cerr << "Accept error, could not retrieve client Socket" << std::endl;	
			return -1;
		}
	}
	_readData.insert(std::make_pair(clientSocket, ""));
	return clientSocket;
}

int	Socket::readData(int clientSocket) {
	char	buffer[BUFF_SIZE];
	
    int bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
	
	if (!bytes_received) {
		_readData.erase(clientSocket);
		std::cerr << "Connection closed, nothing to read" << std::endl;
		return -1;
	}
	if (bytes_received == -1) {
		_readData.erase(clientSocket);
		std::cerr << "Error reading from client socket" << std::endl;
		return -1;
	}
	_readData[clientSocket] += buffer;
	if (_readData[clientSocket].find("Transfer-Encoding: chunked") != std::string::npos) {
		return 0;
	}
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
	std::cout << "************** responseString: ****************" << std::endl;
	std::cout << responseString << std::endl;
	std::cout << "***********************************************" << std::endl;
	std::cout << "send value " << send(clientSocket, responseString.c_str(), responseString.size(), flags) << std::endl;
	send(clientSocket, responseString.c_str(), responseString.size(), flags);
	//check if all is sent//
	delete _requestHandlers[clientSocket];
	_requestHandlers.erase(clientSocket);
}
