/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teliet <teliet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:44:02 by rleger            #+#    #+#             */
/*   Updated: 2024/04/26 17:23:47 by teliet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <iostream>
//#include "Request.hpp"
#include <sstream>
#include <map>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <vector>
#include "Utils.hpp"

class Location {

	private:
		std::map <std::string, void (Location::*)(const std::string &)> _fnSetter;
		std::string							_name;
		std::string							_host;
		int									_port;
		int									_clientBodySize;
		std::map <std::string, std::string> _errPages;
		std::string							_rootDirName;
		std::string							_index;
		std::vector<std::string>			_methods;
		std::string							_autoIndex;
		std::string							_return;
		std::string							_cgiPath;
		std::string							_extension;
	public:
		Location( );
		~Location( );
		Location& operator=(const Location& rhs);
		Location (std::map <std::string, std::string> globVar, std::map <std::string, std::string> dictLoc);
		Location(const Location &defLoc, std::map <std::string, std::string> dictLoc,  std::string name);

		bool	isMethodAllowed(std::string method);

		void	setName(const std::string& host);
		void	setHost(const std::string& host);
		void	setPort(const std::string& strPort);
		void	setClientBodySize(const std::string& strSize);
		void	setRootDirName(const std::string& name);
		void	setIndex(const std::string& index);
		void	setMethods(const std::string& methods);
		void	setAutoIndex(const std::string& autoIndex);
		void	setReturn(const std::string& value);
		void	setCgi(const std::string& path);
		void	setExtenstion(const std::string& extension);
		void	print();

		void	_setFnSetter(bool def);

		void	addErrPage(const std::string& key, const std::string&value);
		
		std::string	getName();
		std::string	getHost();
		int			getPort();
		std::string	getReturn();
		std::string	getIndex();
		std::string	getAutoIndex();
		std::string	getRootDirName();
		int			getClientBodySize();
		std::string	getCGIPath();

		std::vector<std::string> _deepCopyVector(const std::vector<std::string>& original);
		std::map<std::string, std::string> _deepCopyMap(const std::map<std::string, std::string>& original);
};

#endif