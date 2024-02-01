/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:36:27 by rleger            #+#    #+#             */
/*   Updated: 2024/01/31 19:53:10 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_HPP
# define SERVER_HPP

#include "Location.hpp"

class Server {
	private:
		Location* _defLoc; 
		std::map <std::string, Location*>	_locations;
	public:
		Server( );
		~Server( );
		void	start();
		void	stop();
		void	addLocation(Location* location);
		void	addDefLoc(Location* location);
		
		std::string getServerName( );
		std::map <std::string, Location*> getLocations( );	
		Location* getDefLoc( );	
};

#endif