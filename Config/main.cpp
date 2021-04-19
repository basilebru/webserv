/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:55:26 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/19 15:30:37 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"
#include "HttpBlock.hpp"

int main()
{
	ConfParser parser;

	try {
		parser.readConfFile("webserv.conf"); // Default config file containing http directive
		parser.readConfFile("server2.conf"); // Config file containing server blocks


		HttpBlock config = parser.getHttpBlock();
		std::vector<ServerBlock> servers = config.getServers();

		
		std::cout << std::endl << "WEBSERV CONFIGURATION: " << std::endl;
		std::cout << "Nbr of servers: " << servers.size() << std::endl << std::endl;
		std::cout << config << std::endl;
		// displayVec(servers, '\n');
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}

	// servers.getbyIP(ip, port, host);

	return 0;
}