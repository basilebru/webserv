/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:55:10 by julnolle          #+#    #+#             */
/*   Updated: 2021/03/18 16:11:55 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"
#include <stdio.h>
#include <cstring>

#define NB_DIR 4

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
	while (getline(file, line))
	{
		this->parseLine(line);
	}
}

void ConfParser::parseHttp(char *line)
{
	std::cout << "PARSEHTTP" << std::endl;
	while (line != NULL && line[0] != '#')
	{
		std::cout << line << std::endl;
		line = strtok (NULL, " ");
	}
}

void ConfParser::paseServer(char *line)
{
	std::cout << "PASESERVER" << std::endl;
	while (line != NULL && line[0] != '#')
	{
		std::cout << line << std::endl;
		line = strtok (NULL, " ");
	}
}

void ConfParser::ParseAutoindex(char *line)
{
	std::cout << "PARSEAUTOINDEX" << std::endl;
	while (line != NULL && line[0] != '#')
	{
		std::cout << line << std::endl;
		line = strtok (NULL, " ");
	}
}

void ConfParser::ParseListen(char *line)
{
	std::cout << "PARSELISTEN" << std::endl;
	while (line != NULL && line[0] != '#')
	{
		std::cout << line << std::endl;
		line = strtok (NULL, " ");
	}
}

void ConfParser::parseLine(std::string& line)
{
	// std::cout << "-- PARSE LINE --" << std::endl;
	typedef	void		(ConfParser::*t_parse)(char	*);
	static std::string	dir[NB_DIR] = {"http", "server", "autoindex", "listen"};
	static t_parse		func[NB_DIR] = {&ConfParser::parseHttp, &ConfParser::paseServer, &ConfParser::ParseAutoindex, &ConfParser::ParseListen};

	char *split;
	char *cline = new char [line.length() + 1];
	std::strcpy (cline, line.c_str());

	split = strtok ( cline, " " );
	if (split && split[0] == '#')
	{
		delete[] cline;
		return ;
	}
	
	while (split != NULL)
	{
		int i(0);
		while (i < NB_DIR && split != NULL)
		{
			if (std::strcmp(split, dir[i].c_str()) == 0)
			{
				(this->*func[i])(split);
				delete[] cline;
				return ;
			}
			++i;
		}
		split = strtok (NULL, " ");
	}
	delete[] cline;
}



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

