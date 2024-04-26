/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:36:10 by rleger            #+#    #+#             */
/*   Updated: 2024/04/26 11:56:47 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server( ) {
	
}

Server::~Server( ) {
	for (std::map<std::string, Location *>::iterator it = _locations.begin(); it != _locations.end(); it++) {
		delete it->second;
	}
	delete _defLoc;
}

void	Server::setAddress(std::string host, std::string port) {
	_address = host + ":" + port;
}

std::string	Server::getAddress() {
	return _address;
}

void	Server::setSocket(int socket) {
	_serverSocket = socket;	
}

void	Server::addLocation(Location* location) {
	_locations[location->getName()] = location;
}

void	Server::addDefLoc(Location* location) {
	_defLoc = location;
}

std::string Server::getServerName( ) {
	return (_defLoc->getName());
}

Location* Server::getLocation(const std::string& path) {
	return _locations[path];
}

std::map<std::string, Location*>	Server::getLocations()
{
	return _locations;
}

int	Server::getSocket( ) {
	return _serverSocket;
}
std::string	Server::getHost( ) {
	return _defLoc->getHost();
}

int	Server::getPort( ) {
	return _defLoc->getPort();
}

Location* 	Server::getDefaultLocation() {
	return _defLoc;
}