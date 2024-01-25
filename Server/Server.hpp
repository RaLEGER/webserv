/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:36:27 by rleger            #+#    #+#             */
/*   Updated: 2024/01/25 13:25:27 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
#include "Location.hpp"

class Server {
	private:
		std::string				_host;
		int						_port;
		std::vector<Location*> _locations;
	public:
		Server( );
		Server(const Server &rhs);
		~Server( );
		Server& operator=(const Server &rhs);
		Server(std::string host, int port);
		void	addLocation(Location* location);
		void	start();
		void	stop();
};

#endif