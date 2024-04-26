/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:44:00 by rleger            #+#    #+#             */
/*   Updated: 2024/04/26 20:05:14 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location( ) {
	
}

Location& Location::operator=(const Location& rhs) {
	if (this == &rhs) return *this;
	_errPages = _deepCopyMap(rhs._errPages);
	setName(rhs._name);
	setHost(rhs._host);
	_port = rhs._port;
 	_clientBodySize = rhs._clientBodySize;
	setRootDirName(rhs._rootDirName);
	setIndex(rhs._index);
	_methods = _deepCopyVector(rhs._methods);
	setAutoIndex(rhs._autoIndex);
	setReturn(rhs._return);
	setCgi(rhs._cgiPath);
	setExtenstion(rhs._extension);
	return *this;
}

Location::Location(const Location& defLoc, std::map <std::string, std::string> dictLoc, std::string name) {
	*this = defLoc;
	_setFnSetter(false);
	if (name != "")
		setName(name);
	std::map <std::string, void (Location::*)(const std::string &)>::iterator it1;
	for (it1 = _fnSetter.begin(); it1 != _fnSetter.end(); ++it1) {	
		if (dictLoc.find(it1->first) != dictLoc.end())
			(this->*it1->second)(dictLoc.at(it1->first));
	}
	
	std::map<std::string, std::string>::iterator it2;
	for (it2 = dictLoc.begin(); it2 != dictLoc.end(); ++it2) {
		const std::string& key = it2->first;
		const std::string& value = it2->second;
		
		if (key.compare(0, 11, "error_page:") == 0) {
			addErrPage(key.substr(key.find(':')), value);
		}
		else if (_fnSetter.find(key) == _fnSetter.end() ) {
			throw CustomError(1, std::string("Invalid Field: " + key).c_str());
		}
	}
	
}


Location::~Location( ) {
	
}

Location::Location (std::map <std::string, std::string> globVar, std::map <std::string, std::string> dictLoc) {
	
	_setFnSetter(true);
	std::map <std::string, void (Location::*)(const std::string &)>::iterator it1;
	for (it1 = _fnSetter.begin(); it1 != _fnSetter.end(); ++it1) {	
		if (globVar.find(it1->first) != globVar.end())
			(this->*it1->second)(globVar.at(it1->first));
		else
			(this->*it1->second)("");
	}
	for (it1 = _fnSetter.begin(); it1 != _fnSetter.end(); ++it1) {	
		if (dictLoc.find(it1->first) != dictLoc.end())
			(this->*it1->second)(dictLoc.at(it1->first));
		else if (globVar.find(it1->first) == globVar.end())
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
					&& key.compare("location") != 0) {
			throw CustomError(1, std::string("Invalid Field: " + key).c_str());
		}
	}
}


bool	Location::isMethodAllowed(std::string method) {
	
	for (size_t i = 0; i < _methods.size(); i++) {
		if (!_methods[i].compare(method))
			return true;
	}
	return false;
}

void	Location::_setFnSetter(bool def) {
	if (def) {
		_fnSetter["server_name"] = &Location::setName;
		_fnSetter["host"] = &Location::setHost;
	}
	_fnSetter["listen"] = &Location::setPort;
	_fnSetter["client_max_body_size"] = &Location::setClientBodySize;
	_fnSetter["root"] = &Location::setRootDirName;
	_fnSetter["index"] = &Location::setIndex;
	_fnSetter["methods"] = &Location::setMethods;
	_fnSetter["autoindex"] = &Location::setAutoIndex;
	_fnSetter["return"] = &Location::setReturn;
	_fnSetter["cgi_path"] = &Location::setCgi;
	_fnSetter["extension"] = &Location::setExtenstion;
}

void	Location::print() {
	std::cout << "-------- Location ------------- " << _name << std::endl;
	std::cout << "Host: " << _host << std::endl;
	std::cout << "Port: " << _port << std::endl;
	std::cout << "Client body size: " << _clientBodySize << std::endl;
	std::cout << "Root dir name: " << _rootDirName << std::endl;
	std::cout << "Index: " << _index << std::endl;
	std::cout << "Methods: ";
	for (size_t i = 0; i < _methods.size(); i++) {
		std::cout << _methods[i] << " ";
	}
	std::cout << std::endl;
	std::cout << "Autoindex: " << _autoIndex << std::endl;
	std::cout << "Return: " << _return << std::endl;
	std::cout << "Cgi path: " << _cgiPath << std::endl;
	std::cout << "Extension: " << _extension << std::endl;
	std::cout << "-------------------------------- " << std::endl;
}

void	Location::setHost(const std::string& host) {
	_host = host;	
}

void	Location::setName(const std::string& name) {
	_name = name;	
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

void Location::setMethods(const std::string& methods) {
    std::istringstream iss(methods);
    std::string token;
    
    while (std::getline(iss, token, ',')) {
        _methods.push_back(token);
    }
}

void	Location::setAutoIndex(const std::string& autoIndex) {
	_autoIndex = autoIndex;
}

void	Location::setReturn(const std::string& value) {
	_return = value;
}

void	Location::setCgi(const std::string& path) {
	_cgiPath = path;
}

void	Location::setExtenstion(const std::string& extension) {
	_extension = extension;
}

std::string	Location::getHost( ) {
	return _host;	
}

std::string	Location::getName( ) {
	return _name;	
}
int	Location::getPort( ) {
	return _port;	
}
std::string	Location::getReturn( ) {
	return _return;	
}


std::string	Location::getIndex() {
	return _index;	
}

std::string	Location::getAutoIndex( ) {
	return _autoIndex;	
}

std::string	Location::getRootDirName() {
	return _rootDirName;
}

std::string	Location::getCGIPath() {
	return _cgiPath;
}

std::map<std::string, std::string> Location::_deepCopyMap(const std::map<std::string, std::string>& original) {
    std::map<std::string, std::string> copyMap;

    // Iterate through the original map
    for (std::map<std::string, std::string>::const_iterator it = original.begin(); it != original.end(); ++it) {
        // Insert each key-value pair into the copy map
        copyMap.insert(std::make_pair(it->first, it->second));
    }

    return copyMap;
}

std::vector<std::string> Location::_deepCopyVector(const std::vector<std::string>& original) {
    std::vector<std::string> copyVector;

    // Iterate through the original vector
    for (std::vector<std::string>::const_iterator it = original.begin(); it != original.end(); ++it) {
        // Copy each element to the copy vector
        copyVector.push_back(*it);
    }

    return copyVector;
}

int			Location::getClientBodySize()
{
	return _clientBodySize;
}

std::string	Location::getExtension()
{
	return _extension;
}