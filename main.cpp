/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 15:04:29 by rleger            #+#    #+#             */
/*   Updated: 2024/01/31 17:51:33 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser/Parser.hpp"


int main() {

    std::string filename = "/Users/rleger/Desktop/42off/webserv/default copy.conf";
	Parser parser_obj(filename);

	std::vector <Server*> servers;
	servers = parser_obj.getServers();

	for (size_t i = 0; i < servers.size(); ++i) {
		std::cout << servers[i]->getServerName() << std::endl;
		servers[i]->getLocations();
		for (size_t j = 0; j < >)
	}
	
	
}