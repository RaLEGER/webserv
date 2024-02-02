/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:36:10 by rleger            #+#    #+#             */
/*   Updated: 2024/02/02 16:11:41 by rleger           ###   ########.fr       */
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
    if (newSocket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    setNonBlocking(newSocket);

    return newSocket;
}

void Server::setNonBlocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    
    if (flags == -1) {
        // Handle error, for example, print an error message and return an error code.
        perror("fcntl");
		//error
    }

    // Set the non-blocking and close-on-exec flags
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK | FD_CLOEXEC) == -1) {
        // Handle error, for example, print an error message and return an error code.
        perror("fcntl");
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
	return _defLoc;
}
