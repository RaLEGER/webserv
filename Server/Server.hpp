/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teliet <teliet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:36:27 by rleger            #+#    #+#             */
/*   Updated: 2024/04/19 16:06:53 by teliet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_HPP
# define SERVER_HPP

#include <cstring>
#include "Location.hpp"
#include "RequestHandler.hpp"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>

# define BUFF_SIZE 255

class Server {
	private:
		Location* 							_defLoc; 
		std::map <std::string, Location*>	_locations;
		int									_serverSocket;
    	std::vector<int> 					_clientSockets;

		std::map <int, std::string > _readData;
		std::map <int, RequestHandler* > _requestHandlers;
		
		void setupServerSocket();

		int createSocket();
		void setNonBlocking(int socket);

	public:
		Server( );
		~Server( );
		
		void			start();
		void			stop();
		
		int				getClientSocket();
		int				readData(int clientSocket);
		int				processRequest(int clientSocket);
		int				sendResponse(int clientSocket); 

		
		void			addLocation(Location* location);
		void			addDefLoc(Location* location);
		
		std::string 							getServerName( );
		Location* 								getLocation(const std::string& path);	
		std::map <std::string, Location*> 		getLocations();	
		int										getSocket();
};

#endif