/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:36:27 by rleger            #+#    #+#             */
/*   Updated: 2024/04/16 11:01:27 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_HPP
# define SERVER_HPP

#include <cstring>
#include "Location.hpp"
#include "RequestHandler.hpp"

# define BUFF_SIZE 32768

class Server {
	private:
		Location* 							_defLoc; 
		std::map <std::string, Location*>	_locations;
		int									_serverSocket;
    	std::vector<int> 					_clientSockets;

		std::map <int, std::string > _readData;
		std::map <int, RequestHandler > _requestHandlers;
		
		void setupServerSocket();

		int createSocket();
		void setNonBlocking(int socket);

	public:
		Server( );
		~Server( );
		
		void	start();
		void	stop();
		
		int		getClientSocket();
		int		readData(int clientSocket);
		int		processRequest(int clientSocket);
		int		sendResponse(int clientSocket); 

		
		void	addLocation(Location* location);
		void	addDefLoc(Location* location);
		
		std::string getServerName( );
		Location* getLocation(const std::string& path);	
		int		getSocket();
};

#endif