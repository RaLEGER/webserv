/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 13:00:42 by rleger            #+#    #+#             */
/*   Updated: 2024/01/31 17:33:35 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser( ) {

}


Parser::~Parser( ) {
	
}

void Parser::removeCharset(std::string &str) {
	size_t comment = 0;
    size_t pos = 0;
	const std::string charset = " \t\r\n";
		
		
	while ((comment = str.find('#')) != std::string::npos) {
		size_t endline = str.find('\n');
		while (endline < comment) {
			endline = str.find('\n', endline + 1);
		}
		if (endline == std::string::npos)
			endline = str.size();
		str.erase(comment, endline - comment + 1);
	}
	while ((pos = str.find_first_of(charset, pos)) != std::string::npos) {
        str.erase(pos, 1);
    }
}

void Parser::setPos() {
	_bracePos = _line.find('{');
	_eqPos = _line.find('=');
	_closeBracePos = _line.find('}');
}

Parser::Parser(const std::string& filename) {
	_servCount = 0;
	_routeCount = -1;
	_depth = 0;
	
    std::ifstream 	file(filename.c_str());
	if (file.is_open()) {
		while (std::getline(file, _line, ';')) {
			removeCharset(_line);
			if (_line.empty())
				continue;
			size_t minPos = 0;
			while (minPos != std::string::npos) {
				setPos();	
				if (_bracePos < _eqPos) {
					// { =
					if (_eqPos < _closeBracePos) {
						// { = }
						minPos = _bracePos;
						openBrace(_bracePos);
						addPair(_eqPos, _closeBracePos);
						closeBrace(_closeBracePos);
					}
					else if (_closeBracePos < _bracePos) {
						// } { =
						minPos = _bracePos;
						closeBrace(_closeBracePos);
						openBrace(_bracePos);
						addPair(_eqPos, _line.size());
					}
					else {
						// { } =
						minPos = _bracePos;
						openBrace(_bracePos);
						closeBrace(_closeBracePos);
						addPair(_eqPos, _line.size());
					}
				}
				else {
					// = {
					if (_bracePos < _closeBracePos) {
						// = { }
						minPos = _eqPos;
						addPair(_eqPos, std::min(_bracePos, _closeBracePos));
						openBrace(_bracePos);
						closeBrace(_closeBracePos);
					}
					else if (_closeBracePos < _eqPos) {
						// } = {
						minPos = _closeBracePos;
						closeBrace(_closeBracePos);
						addPair(_eqPos, _bracePos);
						openBrace(_bracePos);
					}
					else {
						// = } {
						minPos = _eqPos;
						addPair(_eqPos, std::min(_bracePos, _closeBracePos));
						closeBrace(_closeBracePos);
						openBrace(_bracePos);
					}	
				}
			}
			
		}
	}
}

void	Parser::openBrace(size_t pos) {
	if (pos != std::string::npos) {
		_depth ++;
		if (_depth == 1) {
			_servCount++;
			if (_line.substr(0, _bracePos).compare("server"))
				//eroor 
				std::cout << "pas biueno" << std::endl;
			_serverDict.push_back(std::map<std::string, std::string>());
			_routeDict.push_back(std::vector <std::map<std::string, std::string> >());
			_routeNames.push_back(std::vector <std::string>());
		}
		else {
			_routeCount++;
			_routeDict[_servCount - 1].push_back(std::map<std::string, std::string>());
		}
		if (pos != _line.size())
			pos++;
		_line = _line.substr(pos, _line.size() - pos);
		setPos();	
	}
}

void	Parser::closeBrace(size_t pos) {
	if (pos != std::string::npos) {
		_depth --;
		if (_depth == 0)
			_routeCount = 0;
		if (pos != _line.size())
			pos++;
		_line = _line.substr(pos, _line.size() - pos);
		setPos();	
	}
}

void	Parser::addPair(size_t lhs, size_t rhs) {
	if (lhs != std::string::npos) {
		if (rhs == std::string::npos)
			rhs = _line.size();
		std::string key = _line.substr(0, lhs);
		std::string value = _line.substr(lhs + 1, rhs - lhs - 1);
		if (!key.compare("location")) {
			if (_line.find('{') == std::string::npos)
				//throw error
				std::cout << "pas bon? ";
			else {
				_routeNames[_servCount - 1].push_back(value);
			}
		}
		switch (_depth) {
			case 0: {
				_globalVar[key] = value;
				// std::cout << "glob : ";
				// std::cout << key << "=" << value;
				// std::cout << std::endl;
				break;
			}
			case 1: {
				_serverDict[_servCount - 1][key] = value;
				// std::cout << "serv : ";
				// std::cout << key << "=" << value;
				// std::cout << std::endl;
				break;
			}
			case 2: {
				_routeDict[_servCount - 1][_routeCount][key] = value;
				// std::cout << "route " << _routeNames[_routeCount] << " : ";
				// std::cout << key << "=" << value;
				// std::cout << std::endl;
				break;
			}
			default: {
				break;
			}
		}
		_line = _line.substr(rhs, _line.size() - rhs);
		setPos();	
	}
}

std::map <std::string, std::string> Parser::getGlobalVars() {
	return _globalVar;
}

std::vector <Server*> Parser::getServers( ) {
	std::vector <Server*> servers;
	std::vector <std::map<std::string, std::string> >::iterator servIt;
	int	servCount = 0;
	
	for (servIt = _serverDict.begin(); servIt < _serverDict.end(); servIt++) {
		
		std::map<std::string, std::string>& serverMap = *servIt;
		Server* tempServ = new Server();
		servers.push_back(tempServ);
		
			
		Location* defLoc = new Location(serverMap, serverMap.at("server_name"));
		tempServ->addLocation(defLoc);	
		std::cout << "def loc vcreated" << std::endl;
		for (size_t i = 0; i < _routeNames[servCount].size(); i ++) {
			Location* tempLoc = new Location(_routeDict[servCount][i], _routeNames[servCount][i]);
			tempServ->addLocation(tempLoc);
		}	
		servCount ++;
		std::cout << servCount << std::endl;
	}
	return (servers);
}