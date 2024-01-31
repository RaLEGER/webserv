/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:44:00 by rleger            #+#    #+#             */
/*   Updated: 2024/01/31 17:49:48 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location( ) {
	
}

Location::~Location( ) {
	
}

Location::Location (std::map <std::string, std::string> dictLoc, std::string name) {

	
	setHost(name);
	
	setFnSetter();
	std::map <std::string, void (Location::*)(const std::string &)>::iterator it1;
	for (it1 = _fnSetter.begin(); it1 != _fnSetter.end(); ++it1) {	
		if (dictLoc.find(it1->first) != dictLoc.end())
			(this->*it1->second)(dictLoc.at(it1->first));
		else
			(this->*it1->second)("");
	}
	
	std::map<std::string, std::string>::iterator it2;
	for (it2 = dictLoc.begin(); it2 != dictLoc.end(); ++it2) {
		const std::string& key = it2->first;
		const std::string& value = it2->second;
		
		if (key.compare(0, 11, "error_page:") == 0) {
			addErrPage(key.substr(key.find(':')), value);
		}
		else if (_fnSetter.find(key) == _fnSetter.end() 
					&& key.compare("host") != 0
					&& key.compare("server_name")
					&& key.compare("location")) {
			std::cout << "err " << key  << std::endl;
			//throw;
		}
	}
}

void	Location::setFnSetter( ) {
	
	_fnSetter["listen"] = &Location::setPort;
	_fnSetter["client_max_body_size"] = &Location::setClientBodySize;
	_fnSetter["root"] = &Location::setRootDirName;
	_fnSetter["index"] = &Location::setIndex;
	_fnSetter["methods"] = &Location::setMethods;
}

void	Location::setHost(const std::string& host) {
	_host = host;	
}

void	Location::setPort(const std::string& strPort) {
	int port = 0;
	std::stringstream temp(strPort);
	temp >> port;
	_port = port;
}

void	Location::setClientBodySize(const std::string& strSize) {
	int bodySize = 0;
	std::stringstream temp(strSize);
	temp >> bodySize;
	_clientBodySize = bodySize;
}

void	Location::addErrPage(const std::string& key, const std::string& value) {
	_errPages[key] = value;
}

void	Location::setRootDirName(const std::string& name) {
	_rootDirName = name;
}

void	Location::setIndex(const std::string& index) {
	_index = index;
}

void	Location::setMethods(const std::string& methods) {
	(void) methods;
	//split puis vecteur
}


std::string	Location::getHost( ) {
	return _host;	
}
