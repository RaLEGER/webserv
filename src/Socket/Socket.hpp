/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:07:33 by rleger            #+#    #+#             */
/*   Updated: 2024/04/26 18:17:08 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "RequestHandler.hpp"
#include "Server.hpp"

#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>


class Socket
{
	private:
		int 		_fd;
		std::string	_host;
		int			_port;
		
		std::vector <Server*>	_servers;
		
		std::map <int, std::string >		_headers;
		std::map <int, std::string >		_bodies;

		std::map <int, RequestHandler* >	_requestHandlers;
		
		bool	_isDuplicate(std::string serverName);
		Socket(/* args */);
		void	_setNonBlocking(int socket);
		int		_createSocket();
		void	_setupSocket();
		
		void 	_readHeader(int clientSocket);
		int		_readBody(int clientSocket);

		std::string	_formatChunk(std::string data, int clientSocket);
		int			_readChunk(int clientSocket);
		
	public:
		~Socket();
		Socket(Server* server);

		void	addServer(Server* server);

		int		getSocket();
		int		getClientSocket();
		int		readData(int clientSocket);
		void	sendResponse(int clientSocket);
		
		void	updateActivity(int clientSocket);
		int		checkTimeout(); 
		
};

#endif