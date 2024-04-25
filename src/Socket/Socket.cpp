/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:07:30 by rleger            #+#    #+#             */
/*   Updated: 2024/04/25 11:02:59 by rleger           ###   ########.fr       */
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
int	Socket::_readHeader(int clientSocket) {
	char		buffer[BUFF_SIZE];
	int			bytesRead;
	int			nb_buffers = 4;
	std::string	rawData = "";
	
	while (nb_buffers && (bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        rawData.append(buffer, bytesRead);
        // Check for end of headers (double CRLF)
		size_t headerPos = rawData.find("\r\n\r\n");
        if (headerPos != std::string::npos) {
			headerPos += 4;
			std::cout << "End of headers found" << std::endl;

			// Splice the beginning of the body from the headers
			_body.insert(std::make_pair(clientSocket, rawData.substr(headerPos)));
			_headers.insert(std::make_pair(clientSocket, rawData.substr(0, headerPos)));

			// std::cout << "Headers: " << std::endl << headers << std::endl;
			// std::cout << "Body: " << std::endl << body << std::endl;
            break;
        }
		nb_buffers--;
    }

	if (bytesRead < 0)
		throw CustomError(-1, "Error reading from client socket");
	if (_headers[clientSocket].empty())
		throw CustomError(-1, "Request Error: wrong headers");
	if(!_requestHandlers[clientSocket]->parseHeaders(_headers[clientSocket]))
		throw CustomError(1, "Request Error: wrong headers");
	if (bytesRead < BUFF_SIZE) {
		//chunked
		_requestHandlers[clientSocket]->setIsBodyComplete(true);
	}
	return nb_buffers;

}

int	Socket::_readBody(int clientSocket, int nb_buffers) {
	int 	bytesRead;
	char	buffer[BUFF_SIZE];
	int		init_nb_buffers = nb_buffers;
	
	if (_requestHandlers[clientSocket]->getIsBodyComplete())
		return 1;
	if (nb_buffers == 0)
		return 0;
	while (nb_buffers && (bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
		_body[clientSocket].append(buffer, bytesRead);
		nb_buffers--;
		if (bytesRead < BUFF_SIZE)
			break;
	}
	if (bytesRead <= 0 && init_nb_buffers - nb_buffers == 1)
		throw CustomError(1, "Error reading from client socket");		
	if (bytesRead == BUFF_SIZE || !nb_buffers)
		return 0;
	return 1;
}
	
int	Socket::readData(int clientSocket) {
	int status;
	int	nb_buffers = 4;
	
	if (_headers.find(clientSocket) == _headers.end()) {
		RequestHandler* handler = new RequestHandler(clientSocket);
		_requestHandlers.insert(std::make_pair(clientSocket, handler));
		try {
			nb_buffers = _readHeader(clientSocket);
		} catch (const CustomError& e) {
			std::cerr << e.what() << std::endl;
			return e.getErrorCode();
		}
	}
	try {
		status = _readBody(clientSocket, nb_buffers);
	}
	catch (const std::exception& e){
		std::cerr << "Error reading from socket: " << e.what() << std::endl;
		return -1;
	}
	return status;
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
		_headers.erase(clientSocket);
		_body.erase(clientSocket);
	} catch (const std::exception& e) {
		std::cerr << "Error erasing RequestHandler: " << e.what() << std::endl;
	}
	// remove from 
}
