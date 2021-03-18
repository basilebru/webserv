/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:55:10 by julnolle          #+#    #+#             */
/*   Updated: 2021/03/18 09:26:29 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"

ConfParser::ConfParser(HttpBlock & http_block)
: _configFile("../nginx.conf"), _httpBlock(http_block)
{
	return; 
}

ConfParser::ConfParser(const std::string filename, HttpBlock & http_block)
: _configFile(filename), _httpBlock(http_block)
{
	return; 
}

ConfParser::~ConfParser(void)
{
	return ;
}

void ConfParser::readConfFile()
{
	std::ifstream file;
	file.open(this->_configFile);
	if (file.fail())
	{
		std::cerr << "cant open file" << std::endl;
	}
	int i = 0;
	while (getline(file, this->_line) && i < 15)
	{
		std::cout << this->_line;
		std::cout << std::endl;
		++i;
	}
}
