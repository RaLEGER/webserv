/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 15:04:29 by rleger            #+#    #+#             */
/*   Updated: 2024/04/21 16:55:27 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "ServerFarm.hpp"

int main() {

    std::string filename = "./conf.conf";
	Parser parser_obj(filename);
	std::cout << "Parsed" << std::endl;
	std::vector <Server*> servers;
	servers = parser_obj.getServers();
	try {
		ServerFarm serverfarm_obj(servers);
		serverfarm_obj.set();
		serverfarm_obj.run();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}