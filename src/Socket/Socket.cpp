/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:07:30 by rleger            #+#    #+#             */
/*   Updated: 2024/04/23 13:41:47 by rleger           ###   ########.fr       */
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

bool Socket::readHeaders(int clientSocketFd) {
	// by default, nginx sets 4 buffers of 8k bytes
	char	buffer[8192];
	int 	nb_buffers = 4;
	int 	bytesRead;

	std::string headers;
	std::string body = "";

	// Read headers until a blank line (end of headers) is encountered
    while (nb_buffers && (bytesRead = recv(clientSocketFd, buffer, sizeof(buffer), 0)) > 0) {
        headers.append(buffer, bytesRead);
        // Check for end of headers (double CRLF)
        if (headers.find("\r\n\r\n") != std::string::npos) {
			std::cout << "End of headers found" << std::endl;

			// Splice the beginning of the body from the headers
			body = headers.substr(headers.find("\r\n\r\n") + 4);
			headers = headers.substr(0, headers.find("\r\n\r\n") + 4);

			// std::cout << "Headers: " << std::endl << headers << std::endl;
			// std::cout << "Body: " << std::endl << body << std::endl;

            break;
        }
		nb_buffers--;
    }

	if (bytesRead < 0) {
		std::cerr << "Error reading from client socket" << std::endl;
		throw CustomError(1, "Error reading from client socket");
	}

	// If no data was read, return an error
	if (headers.empty()) {
		std::cerr << "No headers read from client socket" << std::endl;
		return false;
	}

	// If headers are too long, return an error
	if (headers.find("\r\n\r\n") == std::string::npos)
	{
		std::cerr << "Headers too long" << std::endl;
		// TODO : set a response
		return false;
	}


	// Else add headers and beginning of body to the maps and return success
	_headers.insert(std::make_pair(clientSocketFd, headers)); // USELESS ? better to return headers directly

	// check if the body is fully read
	// todo : fix issue of perfectly sized request
	if(bytesRead < 8192) {
		std::cout << "request fully read" << std::endl;
		_requestHandlers[clientSocketFd]->setIsBodyComplete(true);
		_requestHandlers[clientSocketFd]->isRequestFullyRead = true;
	}

	// put the beginning of the body in the readData
	_readData.insert(std::make_pair(clientSocketFd, body));
	return true;
}

// Returns true if the body is complete, false otherwise
bool Socket::readBody(int clientSocketFd) {
	// max 1MB body size in 8k buffers
	char	buffer[8192];
	int 	nb_buffers = 4;
	int 	bytesRead;


	if(_requestHandlers[clientSocketFd]->getIsChunkedRequest()) {
		// Chunked body handling 

		// Read a certain amount of data (4 buffers of 4k bytes ?) and append it to the readData

		// Parse the chunked body into blocks in _body(clientSocket)

		// If the request is fully read, set the body and isBodyComplete to true
		// _request_handler[clientSocketFd]->setBody(_body[clientSocketFd])
		// _request_handler[clientSocketFd]->setIsBodyComplete(true)
		

	}
	else {
		std::cout << "reading body in socket " << clientSocketFd << std::endl;
		std::cout << "readData " << _readData[clientSocketFd] << std::endl;
		// take remaining body from readData
		_body[clientSocketFd] = _readData[clientSocketFd];
		_readData[clientSocketFd] = "";

		// If the request is fully read, return
		if (_requestHandlers[clientSocketFd]->isRequestFullyRead) 
			return true;

		// TODO :until content length is read or else error
		std::string body = "";

		std::cout << "reading body in socket " << clientSocketFd << std::endl;

		while (nb_buffers && (bytesRead = recv(clientSocketFd, buffer, sizeof(buffer), 0)) > 0) {
			body.append(buffer, bytesRead);
			nb_buffers--;
		}

		if (bytesRead < 0) {
			std::cerr << "Error reading from client socket" << std::endl;
			std::cerr << strerror(errno) << std::endl;
			throw CustomError(1, "Error reading from client socket");
		}


		_body[clientSocketFd] = body;
		return true;
	}
	return false;
}

int	Socket::readData(int clientSocket) {
	// check if a request handler already exists for this client socket
	bool requestHandlerExists = _requestHandlers.find(clientSocket) != _requestHandlers.end();
	bool isBodyComplete = false;

	// if not, create a new one and try to parse the headers
	if(!requestHandlerExists) {
		_requestHandlers[clientSocket] = new RequestHandler(clientSocket);

		// read the headers
		try {
			readHeaders(clientSocket);
		} catch (const std::exception& e) {
			std::cerr << "Error reading headers: " << e.what() << std::endl;
			return -1;
		}

		// std::cout << "Headers read" << std::endl;
		// std::cout << "Headers: " << _headers[clientSocket] << std::endl;

		// parse the headers
		if(!_requestHandlers[clientSocket]->parseHeaders(_headers[clientSocket]))
		{
			std::cerr << "Error parsing headers" << std::endl;
			// TODO : clear the socket to prepare to send response
			return -1;
		}

	}

	// read the body 
	try {
		isBodyComplete = readBody(clientSocket);
	} catch (const std::exception& e) {
		std::cerr << "Error reading body: " << e.what() << std::endl;
		return -1;
	}

	// If the request body is complete, process it and send the response
	// Else, keep reading
	if(isBodyComplete)
	{
		std::cout << "Body is complete" << std::endl;
		std::cout << "processing request" << std::endl;
		_requestHandlers[clientSocket]->setBody(_body[clientSocket]);
		_requestHandlers[clientSocket]->process(_servers);
		return 1;
	}
	else 
	{
		return 0;
	}
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
		_readData.erase(clientSocket);
	} catch (const std::exception& e) {
		std::cerr << "Error erasing RequestHandler: " << e.what() << std::endl;
	}
	// remove from 
}
