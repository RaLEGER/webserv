/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 12:58:40 by rleger            #+#    #+#             */
/*   Updated: 2024/01/26 18:48:59 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>

class Parser {
	private:
		std::map <std::string, std::string> _globalVar;
		std::vector <std::map<std::string, std::string> > _serverDict;
		std::vector <std::vector <std::map<std::string, std::string> > > _routeDict;
		int			_servCount;
		int			_routeCount;
		int			_depth;
		std::string	_line;
		size_t		_bracePos;
		size_t		_eqPos;
		size_t		_closeBracePos;
	public:
		Parser( );
		~Parser( ); 
		Parser(const std::string& filename);
		void	openBrace(size_t pos);
		void	addPair(size_t lhs, size_t rhs);
		void	closeBrace(size_t pos);
		void	removeChar(std::string &str, char ch);
		void	setPos();
		
};

#endif