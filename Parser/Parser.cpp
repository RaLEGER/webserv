/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 13:00:42 by rleger            #+#    #+#             */
/*   Updated: 2024/01/25 18:18:03 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser( ) {
	
}


Parser::~Parser( ) {
	
}

std::string Parser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

Parser::Parser(const std::string& filename) {	
    std::ifstream 	file(filename.c_str());
	if (file.is_open()) {
		std::string	line;
		
		
		while (std::getline(file, line, ';')) {
			std::string	line = trim(line);
			if (line.empty())
				continue;
			size_t comment = line.find('#');
			if (comment != std::string::npos)
				line = line.substr(0, comment);
			size_t minPos = 0;
			while (minPos != std::string::npos) {
				size_t bracePos = line.find('{');
				size_t eqPos = line.find('=');
				size_t closeBracePos = line.find('}');
				if (bracePos < eqPos) {
					if (closeBracePos < bracePos) {
						minPos = closeBracePos;
						if (minPos != 0 || bracePos != std::string::npos) 
							throw;
							//raise error truc inattendu avant }
						closeBrace(line, closeBracePos);
					}
					else
						throw;
						//raise error nimporte quoi
				}

				size_t minPos = std::min<size_t>(bracePos, eqPos);
			}
			
			
			
		}
	}
}

void	Parser::openBrace(const std::string& line, size_t pos) {
	if (pos != std::string::npos) {
		_depth ++;
		if (_depth == 1)
			_servCount++;
		else 
			_routeCount++;
	}
}

void	Parser::closeBrace(const std::string& line, size_t pos) {
	if (pos != std::string::npos) {
		_depth --;
		if (_depth == 1)
			_routeCount = 0;
		
	}
}

void	Parser::addPair(const std::string& line, size_t lhs, size_t rhs) {
	if (lhs != std::string::npos) {
		std::string key = trim(line.substr(0, lhs));
		std::string value = trim(line.substr(lhs + 1, rhs));
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
	}
}