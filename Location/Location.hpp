/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:44:02 by rleger            #+#    #+#             */
/*   Updated: 2024/02/02 17:58:15 by rleger           ###   ########.fr       */
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
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>

class Location {
	private:
		std::map <std::string, void (Location::*)(const std::string &)> _fnSetter;
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

		Location (std::map <std::string, std::string> dictLoc, std::string name);

		bool	isAllowed(std::string method);
		//void	handleRequest(Request* requestHandler);

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

		void	setFnSetter();

		void	addErrPage(const std::string& key, const std::string&value);
		
		std::string	getHost();
		int			getPort();
};

#endif