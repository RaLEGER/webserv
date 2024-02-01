/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:36:10 by rleger            #+#    #+#             */
/*   Updated: 2024/02/01 12:17:56 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server( ) {
	
}

Server::~Server( ) {

}

void	Server::addLocation(Location* location) {
	_locations[location->getHost()] = location;
}

void	Server::addDefLoc(Location* location) {
	_defLoc = location;
}


std::string Server::getServerName( ) {
	return (_defLoc->getHost());
}

std::map <std::string, Location*> Server::getLocations( ) {
	return _locations;
}	
Location* Server::getDefLoc( ) {
	return _defLoc;
}
