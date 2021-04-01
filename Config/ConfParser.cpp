/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:55:10 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/01 19:41:50 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"
#include <stdio.h>
#include <cstring>

#define NB_BLOKS	3

/*Blocks*/
#define HTTP		0
#define SERVER		1
#define LOCATION	2
#define NOBLOCK	   -1

ConfParser::ConfParser(void)
: _configFile("nginx.conf"), _httpBlock(new HttpBlock()), _block_type(-1), _nbr_of_srv(0), _nbr_of_loc(0)
{
	return;
}

ConfParser::ConfParser(const std::string filename)
: _configFile(filename), _httpBlock(new HttpBlock()), _block_type(-1), _nbr_of_srv(0), _nbr_of_loc(0)
{
	return; 
}

ConfParser::~ConfParser(void)
{
	delete this->_httpBlock;
	return ;
}

void ConfParser::readConfFile()
{
	std::ifstream	file;
	std::string		line;

	file.open(this->_configFile.c_str());
	if (file.fail())
	{
		std::cerr << "cant open file" << std::endl;
	}
	while (getline(file, line))
	{
		this->parseLine(line);
		file.clear();
	}
}

/*void trim_whitespace(std::string & line)
{
	size_t pos;

	pos = line.find_first_not_of(" ");
	line.erase(0, pos);
}*/

void erase_comments(std::string& line)
{
	size_t pos;
	size_t len = 0;

	pos = line.find_first_of("#");
	len = line.size() - pos;

	if (pos != std::string::npos)
		line.erase(pos, len);
}

/*	typedef	void		(ConfParser::*t_parse)(std::string&);
	static std::string	blocks[NB_BLOKS] = {"http", "server", "location"};
	static t_parse		func[NB_BLOKS] = {&ConfParser::parseHttp, &ConfParser::parseServer, &ConfParser::ParseLocation};
*/

void ConfParser::check_block(std::string& token)
{
	static std::string	blocks[NB_BLOKS] = {"http", "server", "location"};
	static int			ret[NB_BLOKS] = {HTTP, SERVER, LOCATION};


	size_t i(0);
	while (i < NB_BLOKS)
	{
		if (blocks[i] == token)
		{
			this->_block_type = ret[i];
			return ;
		}
		++i;
	}
	return ;
}


void ConfParser::parseLine(std::string& line)
{
	size_t	line_nb = 1;

	std::cout << "NATIVE LINE  : " << line << std::endl;
	erase_comments(line);
	std::cout << "LINE w/o com.: " << line << std::endl << std::endl;

	std::istringstream iss(line); 
	for(std::string token; iss >> token; )
	{
		std::cout << "TOKEN " << line_nb << ": " << token << std::endl;
		
		this->check_block(token);
		if (this->_block_type == HTTP)
		{
			this->parseHttp(token);
		}
		else if (this->_block_type == SERVER)
		{
			this->parseServer(token);
		}
		else if (this->_block_type == LOCATION)
		{
			this->parseLocation(token);
		}
		line_nb++;
	}
	std::cout << "===================" << std::endl;
}


int ConfParser::parseHttp(std::string& token)
{
	(void)token;
	std::cout << "==> HTTP BLOCK" << std::endl << std::endl;

	return HTTP;
}

int ConfParser::parseServer(std::string& token)
{
	if (token == "{")
		this->_nbr_of_srv++;
	if (this->_nbr_of_srv != 0)
		std::cout << "-> SERVER No " << this->_nbr_of_srv << std::endl << std::endl;

	std::cout << "==> SERVER BLOCK" << std::endl << std::endl;
	if (token == "}")
		this->_block_type = HTTP;

	return SERVER;
}

int ConfParser::parseLocation(std::string& token)
{
	if (token == "{")
		this->_nbr_of_loc++;
	if (this->_nbr_of_loc != 0)
		std::cout << "-> LOCATION No " << this->_nbr_of_loc << std::endl << std::endl;
	std::cout << "==> LOCATION BLOCK" << std::endl << std::endl;
	if (token == "}")
		this->_block_type = SERVER;

	return LOCATION;
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
