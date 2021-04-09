/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:55:26 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/09 12:52:22 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"
#include "HttpBlock.hpp"

int main()
{
	ConfParser parser("nginx.conf");

	parser.readConfFile();

	
	std::cout << std::endl << "EXTRACT CONFIG AND GET ALL SERVER BLOCKS: " << std::endl;
	
	HttpBlock config = parser.getHttpBlock();

	std::vector<ServerBlock> servers = config.getServers();
	std::cout << "Number of servers: " << servers.size() << std::endl;

	std::vector<ServerBlock>::iterator it = servers.begin();
	while(it != servers.end()) {
		std::cout << "MAX BODY SIZE: " << it->getMaxBdySize() << std::endl;
		++it;
	}

	return 0;
}