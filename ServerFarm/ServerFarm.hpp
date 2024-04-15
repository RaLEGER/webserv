/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerFarm.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 13:12:57 by rleger            #+#    #+#             */
/*   Updated: 2024/04/08 11:35:09 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERFARM_HPP
# define SERVERFARM_HPP

# include <iostream>
# include "Server.hpp"

class ServerFarm {
	private:
		std::vector <Server*> _servers;
		ServerFarm(const ServerFarm &rhs);
		ServerFarm& operator=(const ServerFarm &rhs);
		ServerFarm( );
		std::vector <int> _fdsToSend;
		std::map <int, Server*> _serverSocketServer;
		std::map <int, Server*>	_clientSocketServer;
		fd_set	_read_fds;
		fd_set	_write_fds;	
		int		_maxFd;	
		
	public:
		ServerFarm(std::vector <Server*> servers);
		~ServerFarm( );
		void	set();
		void	run();
};

#endif