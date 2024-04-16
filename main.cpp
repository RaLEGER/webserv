/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 15:04:29 by rleger            #+#    #+#             */
/*   Updated: 2024/02/02 19:32:25 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

int main() {

    std::string filename = "/Users/rleger/Desktop/42off/webserv/default copy.conf";
	Parser parser_obj(filename);

	std::vector <Server*> servers;
	servers = parser_obj.getServers();
	for (size_t i = 0; i < servers.size(); ++i) {
		std::cout << servers[i]->getServerName() << std::endl;
		//Location* defLoc = servers[i]->getDefLoc();
		//std::cout << defLoc->getHost() << std::endl;
		std::map<std::string, Location*> locations;
		
		//locations = servers[i]->getLocations();
		std::map<std::string, Location*>::iterator it;
		for (it = locations.begin(); it != locations.end(); ++it) {
			std::cout << it->first << std::endl;
		}
	}	
}