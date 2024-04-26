/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 13:00:42 by rleger            #+#    #+#             */
/*   Updated: 2024/04/26 19:50:01 by rleger           ###   ########.fr       */
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
	_minPos = std::min(std::min(_bracePos, _eqPos), _closeBracePos);
}

Parser::Parser(const std::string& filename) {
	_servCount = 0;
	_routeCount = 0;
	_depth = 0;
	
    std::ifstream 	file(filename.c_str());
	if (file.is_open()) {
		while (std::getline(file, _line, ';')) {
			removeCharset(_line);
			if (_line.empty())
				continue;
			_minPos = 0;
			while (_minPos != std::string::npos) {
				setPos();	
				if (_bracePos < _eqPos) {
					// { =
					if (_eqPos < _closeBracePos) {
						// { = }
						openBrace(_bracePos);
						addPair(_eqPos, _closeBracePos);
						closeBrace();
					}
					else if (_closeBracePos < _bracePos) {
						// } { =
						closeBrace();
						openBrace(_bracePos);
						addPair(_eqPos, _line.size());
					}
					else {
						// { } =
						openBrace(_bracePos);
						closeBrace();
						addPair(_eqPos, _line.size());
					}
				}
				else {
					// = {
					if (_bracePos < _closeBracePos) {
						// = { }
						addPair(_eqPos, _bracePos);
						openBrace(_bracePos);
						closeBrace();
					}
					else if (_closeBracePos < _eqPos) {
						// } = {
						closeBrace();
						addPair(_eqPos, _bracePos);
						openBrace(_bracePos);
					}
					else {
						// = } {
						addPair(_eqPos, _closeBracePos);
						closeBrace();
						openBrace(_bracePos);
					}	
				}
			}
			
		}
	}
	else
		throw CustomError(0, "Invalid file");
}

void	Parser::openBrace(size_t pos) {
	if (pos != std::string::npos) {
		_depth ++;
		if (_depth == 1) {
			_servCount++;
			if (_line.substr(0, _bracePos).compare("server")) 
				throw CustomError(1, "Parsing Error: expected 'server' key");
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

void	Parser::closeBrace() {

	while (_minPos == _closeBracePos && _closeBracePos != std::string::npos) {
		_depth --;
		if (_depth == 0) {
			_routeCount = 0;
		}
		if (_depth < 0)
			throw CustomError(1, "Parsing error: unexpected closed bracket");
		if (_closeBracePos != _line.size())
			_closeBracePos++;
		_line = _line.substr(_closeBracePos, _line.size() - _closeBracePos);
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
				throw CustomError(1, "Parsing error: missing bracket");
			else {
				_routeNames[_servCount - 1].push_back(value);
			}
		}
		switch (_depth) {
			case 0: {
				_globalVar[key] = value;
				break;
			}
			case 1: {
				_serverDict[_servCount - 1][key] = value;
				break;
			}
			case 2: {
				_routeDict[_servCount - 1][_routeCount - 1][key] = value;
				break;
			}
			default: {
				throw CustomError(1, "Parsing error: unexpected depth");
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
		std::string host, port;
		try {
			host = serverMap.at("host");
		}
		catch (const std::exception &e) {
			throw CustomError(1, "Parsing Error: 'host' not found");
		}
		try {
			port = serverMap.at("listen");
		}
		catch (const std::exception &e) {
			throw CustomError(1, "Parsing Error: 'port' not found");
		}
		tempServ->setAddress(host, port);
		
		Location* defLoc = new Location(_globalVar, serverMap);
		tempServ->addDefLoc(defLoc);	
		for (size_t i = 0; i < _routeNames[servCount].size(); i ++) {
			Location* tempLoc = new Location(*defLoc, _routeDict[servCount][i], _routeNames[servCount][i]);
			tempServ->addLocation(tempLoc);
		}	
		servCount ++;
	}
	return (servers);
}