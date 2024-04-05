/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerFarm.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 13:12:54 by rleger            #+#    #+#             */
/*   Updated: 2024/04/05 19:53:34 by rleger           ###   ########.fr       */
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
			for (std::vector<int>::iterator it = _fdsToSend.begin() ; it != _fdsToSend.end() ; it++) {
				if (FD_ISSET(*it, &_write_fds)) {
					//get fd from fds (*it) and process it thorugh server
					// with the _clientSocketServer map
					break;
				}
			}
			
			//process client sockets (create fds)
			for (std::map<int, Server *>::iterator it = _clientSocketSever.begin() ; it != _clientSocketSever.end() ; it++){
				long	socket = it->first;

				if (FD_ISSET(socket, &_read_fds)) {
					//process socket
					//write to fd
					//add to fdsToSend
					break;
				}
			}
			
			//detect activated server socket, get client socket (accept) and associate to server socket
			for (std::map<int, Server*>::iterator it = _serverSocketServer.begin(); it != _serverSocketServer.end(); it++) {
				int fd = it->first;

				if (FD_ISSET(fd, &_read_fds))
				{
					//get client socket with accpet func
					int clientSocket =0;
					//add clietn socket to _read_fds
					FD_SET(clientSocket, &_read_fds);
					//associate to server by making a pair
					_clientSocketSever.insert(std::make_pair(clientSocket, it->second));
					break;
				}
			}
		}
	}	
}