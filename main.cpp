/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rleger <rleger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 15:04:29 by rleger            #+#    #+#             */
/*   Updated: 2024/04/16 12:29:54 by rleger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "ServerFarm.hpp"

int main() {

    std::string filename = "./default_copy.conf";
	Parser parser_obj(filename);

	std::vector <Server*> servers;
	servers = parser_obj.getServers();
	ServerFarm serverfarm_obj(servers);
	serverfarm_obj.run();
}