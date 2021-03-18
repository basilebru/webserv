/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:55:10 by julnolle          #+#    #+#             */
/*   Updated: 2021/03/18 12:12:00 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"
#include <stdio.h>
#include <cstring>
#define TOT_LINE 50

ConfParser::ConfParser(void)
: _configFile("nginx.conf")
{
	return; 
}

ConfParser::ConfParser(const std::string filename)
: _configFile(filename)
{
	return; 
}

ConfParser::~ConfParser(void)
{
	return ;
}

void ConfParser::readConfFile()
{
	std::ifstream	file;
	std::string		line;

	file.open(this->_configFile);
	if (file.fail())
	{
		std::cerr << "cant open file" << std::endl;
	}
	int i = 0;
	while (getline(file, line) && i < TOT_LINE)
	{
		this->setDirective(line);
		++i;
	}
}

// void ConfParser::parseParams(std::string& line);

void ConfParser::setDirective(std::string& line)
{
	char *split;
	char *cline = new char [line.length() + 1];
	std::strcpy (cline, line.c_str());

	split = strtok ( cline, " " );
	if (split && split[0] == '#')
	{
		delete[] cline;
		return ;
	}

	if (split != NULL)
	{
		printf ("Directive: %s\n", split);
		split = strtok (NULL, " ");
		int i(1);
		while(split != NULL)
		{
			printf ("Param %.d  : %s\n", i, split);
			split = strtok (NULL, " ");
			if (split && split[0] == '#')
			{
				delete[] cline;
				return ;
			}
			++i;
		}
	}

	delete[] cline;
	// std::cout << line << std::endl;
}

