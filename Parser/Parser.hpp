/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 12:58:40 by rleger            #+#    #+#             */
/*   Updated: 2024/01/25 17:20:55 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "Server.hpp"

class Parser {
	private:
		std::map <std::string, std::string> _globalVar;
		std::vector <std::map<std::string, std::string> > _serverDict;
		std::vector <std::vector <std::map<std::string, std::string> > > _routeDict;
		int		_servCount = -1;
		int		_routeCount = -1;
		int		_depth = 0;
	public:
		Parser( );
		~Parser( ); 
		Parser(const std::string& filename);
		std::string	trim(const std::string& str);
		void	openBrace(const std::string& line, size_t pos);
		void	addPair(const std::string& line, size_t lhs, size_t rhs);
		void	closeBrace(const std::string& line, size_t pos);
		
		
};

#endif