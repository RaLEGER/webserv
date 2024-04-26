/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerFarm.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 13:12:54 by rleger            #+#    #+#             */
/*   Updated: 2024/04/26 19:43:40 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerFarm.hpp"


ServerFarm::ServerFarm(std::vector <Server*> servers){
	_servers = servers;
}

ServerFarm::~ServerFarm( ) {	
	for (std::vector<Server*>::iterator it = _servers.begin(); it != _servers.end(); it++) {
		delete *it;
	}
	for (std::map <std::string, Socket*>::iterator it = _addressSocket.begin(); it != _addressSocket.end(); it++) {
		delete it->second;
	}
	
	
}

void ServerFarm::set() {
	
	std::cout << "Setting sockets " << std::endl;
	std::vector <Server*>::iterator	it;
	_maxFd = 0;
	FD_ZERO(&_read_fds);

	std::map<std::string, Socket*>::iterator addressSocketPair;

	try {
		// Iterate through servers and create sockets 
		for (it = _servers.begin(); it < _servers.end(); it++) {
			std::string	serverAdress = (*it)->getAddress();
			addressSocketPair = _addressSocket.find(serverAdress);

			std::cout << "Checking server : " << (*it)->getServerName() << " - " <<(*it)->getAddress() << std::endl;
			if (addressSocketPair != _addressSocket.end())
			{
				// If a socket already exists for this address (host:port), add the server to the socket object
				addressSocketPair->second->addServer(*it);
			}
			else {
				// If no socket exists for this address, create a new socket and add the server to it
				std::cout << " Creating new socket " << std::endl;
				Socket *socket = new Socket(*it);
				_addressSocket.insert(std::make_pair(serverAdress, socket));
				socket->addServer(*it);

				int fd = socket->getSocket();
				FD_SET(fd, &_read_fds);
				_maxFd = std::max(fd, _maxFd);
			}
		}
	} catch (CustomError& e) {
		std::cerr << "ServerFarm setup Error: ";
		throw;
	}	
	// Check if there are no sockets
	if (!_maxFd) {
		std::cerr << "No socket found" << std::endl;
	}
}


void printFdSet(const fd_set& fds) {
    std::cout << "{ ";
    for (int fd = 0; fd < FD_SETSIZE; ++fd) {
        if (FD_ISSET(fd, &fds)) {
            std::cout << fd << " ";
        }
    }
    std::cout << "}" << std::endl;
}



void ServerFarm::printClientSocketReady() {
	std::cout << "Client socket ready: ";
	std::cout << "{ ";
	for (std::vector<int>::iterator it = _clientSocketReady.begin() ; it != _clientSocketReady.end() ; it++) {
		std::cout << *it << " ";
	}
	std::cout << "}" << std::endl;
	std::cout << "Client socket socket: ";
	std::cout << "{ ";
	for (std::map<int, Socket*>::iterator it = _clientSocketSocket.begin() ; it != _clientSocketSocket.end(); it++) {
		std::cout << it->first << " ";
	}
	std::cout << "}" << std::endl;
	std::cout << "Server socket ready: ";
	std::cout << "{ ";
	for (std::map<std::string, Socket*>::iterator it = _addressSocket.begin(); it != _addressSocket.end(); it++) {
		std::cout << it->second->getSocket() << " ";
	}
	std::cout << "}" << std::endl;
}

void INThandler(int sig) {
	if (sig == SIGINT) {
		keepRunning = 0;
	}
}

void ServerFarm::run() {
	fd_set runningReadFds;
	fd_set runningWriteFds;

	signal(SIGINT, INThandler);
	while (keepRunning) {

		int	activity = 0;
		struct timeval timeout;
		while (!activity) {
			runningReadFds = _read_fds;
			timeout.tv_sec = 1; // Timeout of 1 second
			timeout.tv_usec = 0; // Microseconds (optional, set to 0 for whole seconds)
			for (std::map<std::string, Socket*>::iterator it = _addressSocket.begin(); it != _addressSocket.end(); it++) {
				int fdTimedOut = it->second->checkTimeout();
				if (fdTimedOut > 0) {
					//try {
						FD_CLR(fdTimedOut, &_write_fds);
						std::cerr << "Cleared timed out write socket" << std::endl;
					//}
					//catch (const std::exception& e) {
						FD_CLR(fdTimedOut, &_read_fds);
						std::cerr << "Cleared timed out read socket" << std::endl;
					//}
					std::vector<int>::iterator vecSup = std::find(_clientSocketReady.begin(), _clientSocketReady.end(), fdTimedOut);
					if ( vecSup != _clientSocketReady.end())
						_clientSocketReady.erase(vecSup);
					if (_clientSocketSocket.find(fdTimedOut) != _clientSocketSocket.end())
						_clientSocketSocket.erase(fdTimedOut);
					close(fdTimedOut);
				}
					
			}
			FD_ZERO(&_write_fds);
			//add fd to send to writing set
			std::cout << "running" << std::endl;
			for (std::vector<int>::iterator it = _clientSocketReady.begin() ; it != _clientSocketReady.end() ; it++) {
				std::cout << "client socket ready " << *it << std::endl;
				FD_SET(*it, &_write_fds);
			}
			runningWriteFds = _write_fds;
			std::cout << "selecting" << std::endl;
			std::cout << "max fd: " << _maxFd << std::endl;
            std::cout << "Read fds: ";
            printFdSet(runningReadFds);
            std::cout << "Write fds: ";
            printFdSet(runningWriteFds);
			printClientSocketReady();
			
			activity = select(_maxFd + 1, &runningReadFds, &runningWriteFds, NULL, &timeout);
		}
		
		if (activity < 0) {
			std::cerr << "Activity error" << std::endl;
		}
		else {
			//send data
			for (std::vector<int>::iterator it = _clientSocketReady.begin(); it != _clientSocketReady.end(); it++) {
				if (FD_ISSET(*it, &runningWriteFds)) {
					_clientSocketSocket[*it]->sendResponse(*it);
					_clientSocketSocket.erase(*it);
					_clientSocketReady.erase(it);
					FD_CLR(*it, &_write_fds);
					close(*it);
					break;
				}
			}
			
			//process client sockets (create fds)
			for (std::map<int, Socket*>::iterator it = _clientSocketSocket.begin() ; it != _clientSocketSocket.end(); it++) {
				int	clientSocket = it->first;
				std::cout << "client socket loop" << std::endl;
				
				if (FD_ISSET(clientSocket, &runningReadFds)) {
					std::cout << "activated client loop" << std::endl;
					// 1 : socket wants to go in write mode 
					// 0 : socket wants to stay in read mode
					// -1 : socket wants to be closed
					int readStatus = it->second->readData(clientSocket);


					if (readStatus == 1) {
						std::cout << "cleint data read" << std::endl;
						FD_CLR(clientSocket, &_read_fds);
						_clientSocketReady.push_back(clientSocket);
					}
					else if (readStatus == -1) {
						_clientSocketSocket.erase(it);
						FD_CLR(clientSocket, &_read_fds);
						close(clientSocket);	
					}
					break;
				}
			}
			
			//detect activated server socket, get client socket (accept) and associate to server socket
			for (std::map<std::string, Socket*>::iterator it = _addressSocket.begin(); it != _addressSocket.end(); it++) {
				int serverSocket = it->second->getSocket();
				std::cout << "sever socket loop" << std::endl;
				if (FD_ISSET(serverSocket, &runningReadFds))
				{
					std::cout << "accepted new co " << serverSocket << std::endl;
					int clientSocket = it->second->getClientSocket();
					std::cout << "client socket " << clientSocket << std::endl;
					if (clientSocket != -1) {
						FD_SET(clientSocket, &_read_fds);
						_clientSocketSocket.insert(std::make_pair(clientSocket, it->second));
						_maxFd = std::max(_maxFd, clientSocket);
					}
					break;
				}
			}
		}
	}	
}