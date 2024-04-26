/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teliet <teliet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:07:33 by rleger            #+#    #+#             */
/*   Updated: 2024/04/26 11:37:39 by teliet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "RequestHandler.hpp"
#include "Server.hpp"

#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

class Socket
{
	private:
		int 		_fd;
		std::string	_host;
		int			_port;
		
		std::vector <Server*>	_servers;
		
    	std::vector<int> 					_clientSockets;
		
		std::map <int, std::string >		_headers;
		std::map <int, std::string >		_bodies;

		std::map <int, RequestHandler* >	_requestHandlers;
		
		bool	_isDuplicate(std::string serverName);
		Socket(/* args */);
		void	_setNonBlocking(int socket);
		int		_createSocket();
		void	_setupSocket();
		
		void 	_readHeader(int clientSocket);
		int		_readBody(int clientSocket, bool firstIt);
	public:
		~Socket();
		Socket(Server* server);

		void	addServer(Server* server);

		int		getSocket();
		int		getClientSocket();
		int		readData(int clientSocket);
		void	sendResponse(int clientSocket);
		
};

#endif