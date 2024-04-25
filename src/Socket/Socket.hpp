/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:07:33 by rleger            #+#    #+#             */
/*   Updated: 2024/04/25 10:42:33 by rleger           ###   ########.fr       */
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
		
		int 	_readHeader(int clientSocket);
		int		_readBody(int clientSocket, int nb_buffers);
	public:
		~Socket();
		Socket(Server* server);

		void					addServer(Server* server);
		std::vector <Server*>	getServers();

		int		getSocket();
		int		getClientSocket();
		int		readData(int clientSocket);
		void	sendResponse(int clientSocket);
		
};

#endif