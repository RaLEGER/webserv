/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:36:10 by rleger            #+#    #+#             */
/*   Updated: 2024/04/16 11:07:57 by rleger           ###   ########.fr       */
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

int Server::createSocket() {
    int newSocket = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;
    
	if (newSocket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
	if( setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
          sizeof(opt)) < 0 )   
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }   
    setNonBlocking(newSocket);

    return newSocket;
}

void Server::setNonBlocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    
    if (flags == -1) {
    	std::cerr << "fcntl" << std::endl;
		//error
    }

    // Set the non-blocking and close-on-exec flags
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK | FD_CLOEXEC) == -1) {
    	std::cerr << "fcntl" << std::endl;
		//error
    }

}

void Server::setupServerSocket() {
    _serverSocket = createSocket();

    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // need to convert host to address _defLoc->getHost();
    serverAddr.sin_port = htons(_defLoc->getPort()); // Adjust port as needed

    if (bind(_serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
	
    if (listen(_serverSocket, 100) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
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
		std::cerr << "Error" << std::endl;
	}
	else {
		if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
			std::cerr << "Error" << std::endl;	
	}
	_readData.insert(std::make_pair(clientSocket, ""));
	return clientSocket;
}

int	Server::readData(int clientSocket) {
	char	buffer[BUFF_SIZE];
	int bytes_received = recv(clientSocket, buffer, BUFF_SIZE, 0);
	
	if (!bytes_received || bytes_received == -1) {
		std::cerr << "Error or no data received" << std::endl;
		
	}
	
	_readData[clientSocket] += buffer;
	if (_readData[clientSocket].find("\r\n\r\n") != std::string::npos) {
		if (_readData[clientSocket].find("Transfer-Encoding: chunked") != std::string::npos)
			return (0);
		//determine if chunked or not
	}
	return 1; 
}

int	Server::processRequest(int clientSocket) {
	RequestHandler requestHandler(_readData[clientSocket]);

	_requestHandlers.insert(std::make_pair(clientSocket, requestHandler));

	requestHandler.process();

	// clear buffer 
	_readData.erase(clientSocket);

	return 1;
}

int	Server::sendResponse(int clientSocket,) {
	int flags = 0;
	send(clientSocket, _response[clientSocket].c_str(), _response[clientSocket].size(), flags);
	//check if all is sent//
}