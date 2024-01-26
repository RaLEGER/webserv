/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 13:00:42 by rleger            #+#    #+#             */
/*   Updated: 2024/01/26 18:48:29 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser( ) {
	std::cout << "Def" << std::endl;
	_servCount = -1;
	_routeCount = -1;
	_depth = 0;
}


Parser::~Parser( ) {
	
}

void Parser::removeChar(std::string &str, char ch) {
    size_t pos = 0;

    while ((pos = str.find(ch, pos)) != std::string::npos) {
        str.erase(pos, 1);
    }
}

void Parser::setPos() {
	_bracePos = _line.find('{');
	_eqPos = _line.find('=');
	_closeBracePos = _line.find('}');
}

Parser::Parser(const std::string& filename) {
	_servCount = -1;
	_routeCount = -1;
	_depth = 0;
	
    std::ifstream 	file(filename.c_str());
	if (file.is_open()) {
		std::cout << "yes" << std::endl;
		while (std::getline(file, _line,';')) {
			std::cout << "yeso" << std::endl;
			removeChar(_line, ' ');
			removeChar(_line, '\t');
			removeChar(_line, '\r');
			removeChar(_line, '\n');
			size_t comment = _line.find('#');
			if (comment != std::string::npos)
				_line = _line.substr(0, comment);
			if (_line.empty())
				continue;
			size_t minPos = 0;
			while (minPos != std::string::npos) {
				std::cout << _line << std::endl;
				setPos();	
				if (_bracePos < _eqPos) {
					// { =
					if (_eqPos < _closeBracePos) {
						// { = }
						minPos = _bracePos;
						openBrace(_bracePos);
						addPair(_eqPos, std::min(_closeBracePos, _line.size()));
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
						addPair(_eqPos, _line.size());
						openBrace(_bracePos);
						closeBrace(_closeBracePos);
					}
					else if (_closeBracePos < _eqPos) {
						// } = {
						minPos = _closeBracePos;
						closeBrace(_closeBracePos);
						addPair(_eqPos, std::min(_closeBracePos, _line.size()));
						openBrace(_bracePos);
					}
					else {
						// = } {
						minPos = _eqPos;
						addPair(_eqPos, _line.size());
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
		if (_depth == 1)
			_servCount++;
		else 
			_routeCount++;
		_line = _line.substr(pos, _line.size());
		setPos();	
	}
}

void	Parser::closeBrace(size_t pos) {
	if (pos != std::string::npos) {
		_depth --;
		if (_depth == 1)
			_routeCount = 0;
		_line = _line.substr(pos, _line.size());
		setPos();	
	}
}

void	Parser::addPair(size_t lhs, size_t rhs) {
	std::cout << "oui" << std::endl;
	if (lhs != std::string::npos) {
		std::string key = _line.substr(0, lhs);
		std::string value = _line.substr(lhs + 1, rhs);
		switch (_depth) {
			case 0: {
				_globalVar[key] = value;
				break;
			}
			case 1: {
				_serverDict[_servCount][key] = value;
				break;
			}
			case 2: {
				_routeDict[_servCount][_routeCount][key] = value;
				break;
			}
			default: {
				break;
			}
		}
		_line = _line.substr(rhs, _line.size());
		setPos();	
	}
}