/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerFarm.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 13:12:57 by rleger            #+#    #+#             */
/*   Updated: 2024/04/19 21:12:37 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERFARM_HPP
# define SERVERFARM_HPP

# include "Socket.hpp"
# include <set>

class ServerFarm {
	private:
		ServerFarm(const ServerFarm &rhs);
		ServerFarm& operator=(const ServerFarm &rhs);
		ServerFarm( );
		
		std::vector <Server*>	_servers;
		
		std::map <std::string, Socket*>			_addressSocket;
		std::map <int, Socket*>					_clientSocketSocket;
		std::vector <int>						_clientSocketReady;
		
		fd_set	_read_fds;
		fd_set	_write_fds;	
		int		_maxFd;	
		void	printClientSocketReady();
		
	public:
		ServerFarm(std::vector <Server*> servers);
		~ServerFarm( );
		void	set();
		void	run();
};

#endif