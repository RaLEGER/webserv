/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 15:04:29 by rleger            #+#    #+#             */
/*   Updated: 2024/04/26 18:52:27 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "ServerFarm.hpp"

int main(int ac, char **av) {
	std::vector <Server*> servers;
	
	signal(SIGINT, SIG_IGN);
	if (ac != 2)
		return 1;
    std::string filename = av[1];
	if (filename.substr(std::max((int) filename.size() - 5, 0)).compare(".conf"))
		return 1;
	try {
		Parser parser_obj(filename);
		servers = parser_obj.getServers();
	} catch (const CustomError& e) {
		std::cerr << "Error parsing " << e.what() << std::endl;
		return 1;
	}

	try {
		ServerFarm serverfarm_obj(servers);
		serverfarm_obj.set();
		serverfarm_obj.run();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}