/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:55:26 by julnolle          #+#    #+#             */
/*   Updated: 2021/06/02 17:01:01 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"
#include "HttpBlock.hpp"

int main()
{
	ConfParser parser;

	try {
		parser.readConfFile(DEFAULT_CONF_FILE); // Default config file containing http directive
		parser.readConfFile("../conf.d/server2.conf"); // Config file containing server blocks


		HttpBlock baseConfig = parser.getHttpBlock();
		std::vector<ServerBlock> servers = parser.getServers();

		
		std::cout << std::endl << "WEBSERV CONFIGURATION: " << std::endl;
		std::cout << "Nbr of servers: " << servers.size() << std::endl << std::endl;
		
		std::cout << baseConfig;
		displayVec(servers, '\n');
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}

	// servers.getbyIP(ip, port, host);

	return 0;
}