/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:36:10 by rleger            #+#    #+#             */
/*   Updated: 2024/01/24 12:13:34 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server( ) {
	
}

Server::Server(const Server &rhs) {

	int vecSize = rhs._locations.size();
	_host = rhs._host;
	_port = rhs._port;
	for (int i = 0; i < vecSize; i++) {
		addLocation(rhs._locations[i]);
	}
}

Server::~Server( ) {
	int vecSize = _locations.size();
	for (int i = 0; i < vecSize; i++) {
		delete _locations[i];
	}
}

Server& Server::operator=(const Server &rhs) {
	if (this == &rhs) return *this;
	int vecSize = rhs._locations.size();
	_host = rhs._host;
	_port = rhs._port;
	for (int i = 0; i < vecSize; i++) {
		addLocation(rhs._locations[i]);
	}
	return *this;
}

Server::Server(std::string host, int port) {
	_host = host;
	_port = port;
}

void	Server::addLocation(Location* location) {
	Location* newLocation = new Location;
	newLocation = location;
	_locations.push_back(newLocation);
}