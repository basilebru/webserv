/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:55:10 by julnolle          #+#    #+#             */
/*   Updated: 2021/03/16 16:09:18 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"

ConfParser::ConfParser(void) : _configFile("../nginx.conf")
{
	return; 
}

ConfParser::ConfParser(const std::string filename) : _configFile(filename)
{
	return; 
}

ConfParser::~ConfParser(void)
{
	return ;
}

void ConfParser::readConfFile(void)
{
	std::ifstream file;
	file.open(this->_configFile);
	if (file.fail())
	{
		std::cout << "cant open file" << std::endl;
	}
	std::string line;
	int i = 0;
	while (getline(file, line) && i < 15)
	{
		std::cout << line;
		std::cout << std::endl;
		++i;
	}
}
