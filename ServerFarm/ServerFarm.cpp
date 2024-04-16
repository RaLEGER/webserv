/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerFarm.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 13:12:54 by rleger            #+#    #+#             */
/*   Updated: 2024/04/16 10:59:01 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerFarm.hpp"

ServerFarm::ServerFarm( ) {
	
}

ServerFarm::ServerFarm(const ServerFarm &rhs) {
	
}

ServerFarm::~ServerFarm( ) {
	
}

ServerFarm& ServerFarm::operator=(const ServerFarm &rhs) {
	if (this == &rhs) return *this;
	
	return *this;
}

ServerFarm::ServerFarm(std::vector <Server*> servers) {
	_servers = servers;
	
	
}

void ServerFarm::set() {
	
	std::vector <Server*>::iterator	it;
	_maxFd = 0;
	FD_ZERO(&_read_fds);
	
	for (it = _servers.begin(); it < _servers.end(); it++) {
		int server_socket = (*it)->getSocket();
		FD_SET(server_socket, &_read_fds);
		_serverSocketServer.insert(std::make_pair(server_socket, *it));
		_maxFd = std::max(server_socket, _maxFd);
		
	}
	if (!_maxFd) {
		std::cerr << "error" << std::endl;
	}
}

void ServerFarm::run() {

	while (true) {

		int	activity = 0;
		struct timeval timeout;

		while (!activity) {
			timeout.tv_sec = 1; // Timeout of 1 second
			timeout.tv_usec = 0; // Microseconds (optional, set to 0 for whole seconds)
			FD_ZERO(&_write_fds);
			//add fd to send to writing set
			for (std::vector<int>::iterator it = _fdsToSend.begin() ; it != _fdsToSend.end() ; it++)
				FD_SET(*it, &_write_fds);

			int activity = select(_maxFd + 1, &_read_fds, &_write_fds, nullptr, &timeout);
		}
		
		if (activity < 0) {
			//error
			std::cerr << "Error" << std::endl;
			//close clear 
			//throw raiseError();
		}
		else {
			//send data
			for (std::vector<int>::iterator it = _fdsToSend.begin(); it != _fdsToSend.end(); it++) {
				if (FD_ISSET(*it, &_write_fds)) {
					_clientSocketServer[*it]->sendResponse(*it);
					_fdsToSend.erase(it);
					FD_CLR(*it, &_write_fds);
					FD_SET(*it, &_read_fds); //mandatory ? %
					break;
				}
			}
			
			//process client sockets (create fds)
			for (std::map<int, Server *>::iterator it = _clientSocketServer.begin() ; it != _clientSocketServer.end(); it++) {
				int	clientSocket = it->first;

				if (FD_ISSET(clientSocket, &_read_fds)) {
					
					if (it->second->readData(clientSocket)) {
						it->second->processRequest(clientSocket);
						FD_CLR(clientSocket, &_read_fds); //mandatory ? %
						_fdsToSend.push_back(clientSocket);
					}
					break;
				}
			}
			
			//detect activated server socket, get client socket (accept) and associate to server socket
			for (std::map<int, Server*>::iterator it = _serverSocketServer.begin(); it != _serverSocketServer.end(); it++) {
				int serverSocket = it->first;

				if (FD_ISSET(serverSocket, &_read_fds))
				{
					int clientSocket = it->second->getClientSocket();
					if (clientSocket != -1) {
						FD_SET(clientSocket, &_read_fds);
						_clientSocketServer.insert(std::make_pair(clientSocket, it->second));
						_maxFd = std::max(_maxFd, clientSocket);
					}
					break;
				}
			}
		}
	}	
}