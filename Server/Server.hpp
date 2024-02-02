/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:36:27 by rleger            #+#    #+#             */
/*   Updated: 2024/02/02 17:56:22 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_HPP
# define SERVER_HPP

#include "Location.hpp"

class Server {
	private:
		Location* 							_defLoc; 
		std::map <std::string, Location*>	_locations;
		int									_serverSocket;
    	std::vector<int> 					_clientSockets;
		
		void setupServerSocket();

		int createSocket();
		void setNonBlocking(int socket);

	public:
		Server( );
		~Server( );
		
		void	start();
		void	stop();
		void	addLocation(Location* location);
		void	addDefLoc(Location* location);
		
		std::string getServerName( );
		Location* getLocation(const std::string& path);	
};

#endif