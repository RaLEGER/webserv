/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:36:27 by rleger            #+#    #+#             */
/*   Updated: 2024/01/30 16:16:25 by rleger           ###   ########.fr       */
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
		void	start();
		void	stop();
		
		void	setHost(std::string host);
		void	setPort(std::string strPort);
		void	setClientBodySize(std::string strSize);
		void	setRootDirName(std::string name);
		void	setIndex(std::string index);
		void	setMethods(std::string methods);
		
		void	addLocation(Location* location);
		void	addErrPage(std::string key, std::string value);
};

#endif