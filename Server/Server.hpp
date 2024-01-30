/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:36:27 by rleger            #+#    #+#             */
/*   Updated: 2024/01/29 16:44:12 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
#include "Location.hpp"
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>

class Server {
	private:
		std::string				_host;
		int						_port;
		int						_clientBodySize;
		std::map <std::string, std::string> _errPages;
		std::string					_rootDirName;
		std::string					_index;
		std::vector<std::string>	_methods;
		std::vector<Location*>		_locations;
	public:
		Server( );
		Server(const Server &rhs);
		~Server( );
		Server& operator=(const Server &rhs);
		void	addLocation(Location* location);
		void	start();
		void	stop();
		
		void	setHost(std::string host);
		void	setPort(std::string strPort);
		void	setClientBodySize(std::string strSize);
		void	setErrPages(std::map <std::string, std::string> pages);
		void	setRootDirName(std::string name);
		void	setIndex(std::string index);
		void	setMethods(std::vector<std::string> methods);
};

#endif