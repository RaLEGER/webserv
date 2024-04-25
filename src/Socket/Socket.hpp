/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:07:33 by rleger            #+#    #+#             */
/*   Updated: 2024/04/24 19:28:12 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Server.hpp"
#include <netdb.h>
#include <arpa/inet.h>
#include "RequestHandler.hpp"

class Socket
{
	private:
		int 		_fd;
		std::string	_host;
		int			_port;
		
		std::vector <Server*>	_servers;
		
    	std::vector<int> 					_clientSockets;
		
		std::map <int, std::string >		_headers;
		std::map <int, std::string >		_body;

		std::map <int, RequestHandler* >	_requestHandlers;
		
		bool	_isDuplicate(std::string serverName);
		Socket(/* args */);
		void	_setNonBlocking(int socket);
		int		_createSocket();
		void	_setupSocket();
	public:
		~Socket();
		Socket(Server* server);

		void					addServer(Server* server);
		std::vector <Server*>	getServers();

		int		getSocket();
		int		getClientSocket();
		bool 	readHeaders(int clientSocket);
		bool	readBody(int clientSocketFd);
		int		readData(int clientSocket);
		void	sendResponse(int clientSocket);
		
};

#endif