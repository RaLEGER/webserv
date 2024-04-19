/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:36:27 by rleger            #+#    #+#             */
/*   Updated: 2024/04/19 20:48:28 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_HPP
# define SERVER_HPP

#include <cstring>
#include "Location.hpp"
#include <iostream>

# define BUFF_SIZE 2048

class Server {
	private:
		std::string							_address;

		Location* 							_defLoc; 
		std::map <std::string, Location*>	_locations;
		int									_serverSocket;
		
	public:
		Server( );
		~Server( );
		
		void	addLocation(Location* location);
		void	addDefLoc(Location* location);
		
		std::string	getServerName( );
		std::string	getHost();
		int			getPort();
		void		setAddress(std::string host, std::string port);
		std::string	getAddress();
		Location*	getLocation(const std::string& path);
		
		void		setSocket(int socket);
		int			getSocket();
};



#endif