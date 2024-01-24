/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:44:02 by rleger            #+#    #+#             */
/*   Updated: 2024/01/24 13:53:15 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <iostream>
//#include "Request.hpp"

class Location {
	private:
		std::string					_path;
		std::string					_type;
		std::string					_extension;
		std::vector<std::string>	_methods;
	public:
		Location( );
		Location(const Location &rhs);
		~Location( );
		Location& operator=(const Location &rhs);
		bool	isAllowed(std::string method);
		
		//void	handleRequest(Request* request);
};

#endif