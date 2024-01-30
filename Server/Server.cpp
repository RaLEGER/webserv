/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:36:10 by rleger            #+#    #+#             */
/*   Updated: 2024/01/30 16:21:39 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server( ) {
	
}

Server::~Server( ) {

}

void	Server::addLocation(Location* location) {
	_locations.push_back(location);
}

void	Server::setHost(std::string host) {
	_host = host;	
}

void	Server::setPort(std::string strPort) {
	int port = 0;
	std::stringstream temp(strPort);
	temp >> port;
	_port = port;
}

void	Server::setClientBodySize(std::string strSize) {
	int bodySize = 0;
	std::stringstream temp(strSize);
	temp >> bodySize;
	_clientBodySize = bodySize;
}

void	Server::addErrPage(std::string key, std::string value) {
	_errPages[key] = value;
}

void	Server::setRootDirName(std::string name) {
	_rootDirName = name;
}

void	Server::setIndex(std::string index) {
	_index = index;
}

void	Server::setMethods(std::string methods) {
	
}

