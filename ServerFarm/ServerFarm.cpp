/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerFarm.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 13:12:54 by rleger            #+#    #+#             */
/*   Updated: 2024/04/18 19:05:21 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerFarm.hpp"

ServerFarm::ServerFarm( ) {
	
}

ServerFarm::ServerFarm(const ServerFarm &rhs) {
(void) rhs;	
}

ServerFarm::~ServerFarm( ) {
	
}

ServerFarm& ServerFarm::operator=(const ServerFarm &rhs) {
	if (this == &rhs) return *this;
	
	return *this;
}

ServerFarm::ServerFarm(std::vector <Server*> servers) {
	_servers = servers;
	set();	
	
}

void ServerFarm::set() {
	
	std::vector <Server*>::iterator	it;
	_maxFd = 0;
	FD_ZERO(&_read_fds);
	
	for (it = _servers.begin(); it < _servers.end(); it++) {
		try {
			(*it)->start();
		} catch (const CustomError& e) {
			std::cerr << "ServerFarm setup Error: ";
			throw;
		}
		int server_socket = (*it)->getSocket();
		FD_SET(server_socket, &_read_fds);
		_serverSocketServer.insert(std::make_pair(server_socket, *it));
		_maxFd = std::max(server_socket, _maxFd);
		
	}
	if (!_maxFd) {
		std::cerr << "No socket found" << std::endl;
	}
}

void ServerFarm::run() {

	fd_set runningReadFds;
	fd_set runningWriteFds;
	while (true) {

		int	activity = 0;
		struct timeval timeout;

		while (!activity) {
			runningReadFds = _read_fds;
			
			timeout.tv_sec = 1; // Timeout of 1 second
			timeout.tv_usec = 0; // Microseconds (optional, set to 0 for whole seconds)
			FD_ZERO(&_write_fds);
			//add fd to send to writing set
			std::cout << "running" << std::endl;
			for (std::vector<int>::iterator it = _fdsToSend.begin() ; it != _fdsToSend.end() ; it++) {
				std::cout << "fd to send " <<*it << std::endl;
				FD_SET(*it, &_write_fds);
			}
			runningWriteFds = _write_fds;

			activity = select(_maxFd + 1, &runningReadFds, &runningWriteFds, NULL, &timeout);
		}
		
		if (activity < 0) {
			std::cerr << "Activity error" << std::endl;
		}
		else {
			//send data
			for (std::vector<int>::iterator it = _fdsToSend.begin(); it != _fdsToSend.end(); it++) {
				if (FD_ISSET(*it, &runningWriteFds)) {
					std::cout << "fd to send loop" << std::endl;
					_clientSocketServer[*it]->sendResponse(*it);
					_fdsToSend.erase(it);
					FD_CLR(*it, &_write_fds);
					close(*it);
					break;
				}
			}
			
			//process client sockets (create fds)
			for (std::map<int, Server *>::iterator it = _clientSocketServer.begin() ; it != _clientSocketServer.end(); it++) {
				int	clientSocket = it->first;
				std::cout << "client socket loop" << std::endl;
				
				if (FD_ISSET(clientSocket, &runningReadFds)) {
					std::cout << "activated client loop" << std::endl;
					int readStatus = it->second->readData(clientSocket);
					if (readStatus) {
						std::cout << "cleint data read" << std::endl;
						it->second->processRequest(clientSocket);
						FD_CLR(clientSocket, &_read_fds); //mandatory ? %
						_fdsToSend.push_back(clientSocket);
						std::cout << "fd1: " << _fdsToSend[0] << std::endl;
					}
					else if (readStatus == -1) {
						FD_CLR(clientSocket, &_read_fds);
						close(clientSocket);	
					}
					break;
				}
			}
			
			//detect activated server socket, get client socket (accept) and associate to server socket
			for (std::map<int, Server*>::iterator it = _serverSocketServer.begin(); it != _serverSocketServer.end(); it++) {
				int serverSocket = it->first;
				std::cout << "sever socket loop" << std::endl;
				if (FD_ISSET(serverSocket, &runningReadFds))
				{
					std::cout << "accepted new co" << std::endl;
					int clientSocket = it->second->getClientSocket();
					std::cout << "client socket " << clientSocket << std::endl;
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