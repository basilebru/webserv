/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:55:10 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/12 19:52:08 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"
#include <stdio.h>
#include <cstring>

ConfParser::ConfParser(void)
: _configFile("nginx.conf"), _httpBlock(),
_block_type(HTTP), _line_nb(1), _nbr_of_srv(0),
_nbr_of_loc(0), _in_block(FALSE), _semi_col_not_found(0),
_curr_dir(), _dir_line()
{
	return;
}

ConfParser::ConfParser(const std::string filename)
: _configFile(filename), _httpBlock(),
_block_type(HTTP), _line_nb(1), _nbr_of_srv(0),
_nbr_of_loc(0), _in_block(FALSE), _semi_col_not_found(0),
_curr_dir(), _dir_line()
{
	return; 
}

ConfParser::~ConfParser(void)
{
	std::cout << "DESTRUCTOR CALLED" << std::endl;
	// delete this->_httpBlock;
	// delete this->_dir_line;
	return ;
}

/**
 * Static attributs
 *
 * Contains directive listing assiociated
 * with their function handler 
 */

typedef std::map<std::string, int (ConfParser::*)(void)>	dirMap; //move this typedef in higher level ?

dirMap	ConfParser::http_map = setHttpMap();
dirMap	ConfParser::srv_map = setSrvMap();
dirMap	ConfParser::loc_map = setLocMap();

dirMap	ConfParser::setHttpMap()
{
	dirMap map;

	map["root"] = &ConfParser::setRoot;
	map["error_page"] = &ConfParser::setErrorPage;
	map["keepalive_timeout"] = &ConfParser::setKeepAlive;
	map["client_max_body_size"] = &ConfParser::setMaxBdySize;
	map["cgi_param"] = &ConfParser::setCgiParam;
	map["cgi_pass"] = &ConfParser::setCgiPass;
	map["allow_methods"] = &ConfParser::setAllowedMethods;
	map["index"] = &ConfParser::setIndex;
	map["autoindex"] = &ConfParser::setAutoIndex;

	return map;
}

dirMap	ConfParser::setSrvMap()
{
	dirMap map;

	map["listen"] = &ConfParser::setListen;
	map["root"] = &ConfParser::setRoot;
	map["server_name"] = &ConfParser::setServerName;
	map["error_page"] = &ConfParser::setErrorPage;
	map["client_max_body_size"] = &ConfParser::setMaxBdySize;
	map["cgi_param"] = &ConfParser::setCgiParam;
	map["cgi_pass"] = &ConfParser::setCgiPass;
	map["allow_methods"] = &ConfParser::setAllowedMethods;
	map["index"] = &ConfParser::setIndex;
	map["autoindex"] = &ConfParser::setAutoIndex;

	return map;
}

dirMap	ConfParser::setLocMap()
{
	dirMap map;

	map["listen"] = &ConfParser::setListen;
	map["root"] = &ConfParser::setRoot;
	map["error_page"] = &ConfParser::setErrorPage;
	map["client_max_body_size"] = &ConfParser::setMaxBdySize;
	map["cgi_param"] = &ConfParser::setCgiParam;
	map["cgi_pass"] = &ConfParser::setCgiPass;
	map["allow_methods"] = &ConfParser::setAllowedMethods;
	map["index"] = &ConfParser::setIndex;
	map["autoindex"] = &ConfParser::setAutoIndex;

	return map;
}

int ConfParser::setListen(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);
	std::cout << "SET LISTEN FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setServerName(void)
{
	this->checkNbrOfArgs(1, &is_higher<size_t>);
	std::cout << "SET SRVNAME FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setRoot(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);
	std::cout << "SET ROOT FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setErrorPage(void)
{
	std::cout << "SET ERROPAGE FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setKeepAlive(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	if (this->_dir_line[1].find_first_not_of("0123456789") != std::string::npos)
		throw InvalidValue(this->_dir_line[0], this);

	int mbs = atoi(this->_dir_line[1].c_str());

	if (this->_block_type == HTTP)
		this->_httpBlock.setKeepaliveTimeout(mbs);
	else if (this->_block_type == SERVER)
	{
		this->_httpBlock.getLastServer().setKeepaliveTimeout(mbs);
	}
	if (this->_block_type == LOCATION)
	{
		this->_httpBlock.getLastServer().getLastLocation().setKeepaliveTimeout(mbs);
	}

	std::cout << "SET keepalive FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setMaxBdySize(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	if (this->_dir_line[1].find_first_not_of("0123456789") != std::string::npos)
		throw InvalidValue(this->_dir_line[0], this);

	int mbs = atoi(this->_dir_line[1].c_str());

	if (this->_block_type == HTTP)
	{
		this->_httpBlock.setMaxBdySize(mbs);
		// this->_httpBlock.getLastServer().setMaxBdySize(mbs);
	}
	else if (this->_block_type == SERVER)
	{
		this->_httpBlock.getLastServer().setMaxBdySize(mbs);
	}
	if (this->_block_type == LOCATION)
	{
		this->_httpBlock.getLastServer().getLastLocation().setMaxBdySize(mbs);
	}
	std::cout << "SET MAXBDYSIZE FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setCgiParam(void)
{
	std::cout << "SET CGIPARAM FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setCgiPass(void)
{
	std::cout << "SET CGIPASS FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setAllowedMethods(void)
{
	std::cout << "SET ALLMETH FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setIndex(void)
{
	std::cout << "SET INDX FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setAutoIndex(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	std::cout << "SET AUTOIDX FUNCTION" << std::endl;
	return 0;
}

void ConfParser::setLocation()
{
	this->checkNbrOfArgs(3, &same_as<size_t>);
	std::cout << "SET LOCATION FUNCTION" << std::endl;
}

template <class Compare>
void ConfParser::checkNbrOfArgs(size_t expected_nbr, Compare comp)
{
	if (!comp(expected_nbr, this->_dir_line.size()))
		throw InvalidNbrOfArgs(this->_dir_line[0], this);
}

void ConfParser::readConfFile()
{
	std::ifstream	file;
	std::string		line;

	file.open(this->_configFile.c_str());
	if (file.fail())
		throw CannotOpenFile(this);

	while (getline(file, line))
	{
		this->parseLine(line);

		file.clear();
		this->_line_nb++;
	}
	if (this->_in_block.any())
		throw UnexpectedEOF("end of file", this);
	else
		std::cout << GREEN << "FIIIIIIIIIINNNNNNNN" << NOCOLOR << std::endl;
	return ;
}

void erase_comments(std::string& line)
{
	size_t pos;
	size_t len = 0;

	pos = line.find_first_of("#");
	len = line.size() - pos;

	if (pos != std::string::npos)
		line.erase(pos, len);
}

void ConfParser::parseLine(std::string& line)
{

/*	typedef	int		(ConfParser::*t_parse)();
	static t_parse	func[NB_BLOCKS] = {&ConfParser::parseHttp, &ConfParser::parseServer, &ConfParser::parseLocation};
	ret = (this->*func[this->_block_type])();
*/
	// std::cout << "NATIVE LINE  : " << line << std::endl;
	erase_comments(line);
	// std::cout << "LINE w/o com.: " << line << std::endl << std::endl;

	std::istringstream iss(line); 
	for(std::string token; iss >> token;)
	{
		this->_dir_line.push_back(token);
		if (this->_dir_line.back().find_first_of("{;}") != std::string::npos)
		{
			this->parseDirective();
			this->_dir_line.clear();
		}
	}
	if (line != "")
		std::cout << "===================" << std::endl;
}

void ConfParser::handleBlockIn(const std::string& directive)
{
	if (directive == "http")
	{
		if (this->_dir_line.at(1) == "{")
			this->_in_block[HTTP] = TRUE;
		else
			throw NoOpeningBracket("http", this);
		return ;
	}
	if (directive == "server")
	{
		this->_block_type = SERVER;
		if (this->_dir_line.at(1) == "{")
		{
			if (this->_in_block[SERVER] == FALSE)
				this->_in_block[SERVER] = TRUE;
			else
				throw NoClosingBracket("server", this);

			this->_httpBlock.addServer();
			this->_nbr_of_srv++;
			std::cout << "-> SERVER No " << this->_nbr_of_srv << std::endl << std::endl;
		}
		else
			throw NoOpeningBracket("server", this);
		return ;
	}
	if (directive == "location")
	{
		this->_block_type = LOCATION;
		if (this->_dir_line.at(2) == "{")
		{
			if (this->_in_block[LOCATION] == FALSE)
				this->_in_block[LOCATION] = TRUE;
			else
				throw NoClosingBracket("location", this);
			this->_nbr_of_loc++;
			std::cout << "-> LOCATION No " << this->_nbr_of_loc << std::endl << std::endl;
		}
		else
			throw NoOpeningBracket("location", this);
		this->setLocation();
	}
}

void ConfParser::handleBlockOut()
{
	if (this->_block_type == HTTP)
	{
		this->_in_block[HTTP] = FALSE;
		// this->_block_type = SERVER;
	}
	else if (this->_block_type == SERVER)
	{
		this->_in_block[SERVER] = FALSE;
		this->_block_type = HTTP;
	}
	else if (this->_block_type == LOCATION)
	{
		this->_in_block[LOCATION] = FALSE;
		this->_block_type = SERVER;
	}
}

void ConfParser::parseDirective()
{
	dirMap::iterator it;
	std::string directive = this->_dir_line.at(0);
	
	if (this->_block_type == HTTP)
		std::cout << "==> HTTP BLOCK" << std::endl << std::endl;
	else if (this->_block_type == SERVER)
		std::cout << "==> SERVER BLOCK" << std::endl << std::endl;
	if (this->_block_type == LOCATION)
		std::cout << "==> LOCATION BLOCK" << std::endl << std::endl;

	size_t pos = this->_dir_line.back().find_last_of(";");
	if (pos != std::string::npos)
		this->_dir_line.back().erase(pos, 1);
	
	std::cout << "VECTOR: "; displayVec(this->_dir_line);
	
	if (directive == "http" || directive == "server" || directive == "location")
	{
		this->handleBlockIn(directive);
		return ;
	}

	if (this->_dir_line.at(0) == "{")
		throw UnexpectedTocken("{", this);

	if (this->_dir_line.at(0) == "}")
	{
		this->handleBlockOut();
		return ;
	}


	
	if (this->_block_type == HTTP)
	{
		if (ConfParser::http_map.find(directive) != http_map.end())
			(this->*ConfParser::http_map[directive])();
		else
			throw UnknownDirective(directive, this);
	}
	else if (this->_block_type == SERVER)
	{
		if (ConfParser::srv_map.find(directive) != srv_map.end())
			(this->*ConfParser::srv_map[directive])();
		else
			throw UnknownDirective(directive, this);
	}
	if (this->_block_type == LOCATION)
	{
		if (ConfParser::loc_map.find(directive) != loc_map.end())
			(this->*ConfParser::loc_map[directive])();
		else
			throw UnknownDirective(directive, this);
	}
}

/*Getters*/

HttpBlock&		ConfParser::getHttpBlock(void)
{
	return this->_httpBlock;
}


/*Exceptions*/

ConfParser::UnexpectedTocken::UnexpectedTocken(const std::string token, ConfParser *p)
: _msg("webserv: unexpected \"" + token + "\" in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::UnexpectedTocken::what() const throw()
{
	return (this->_msg.c_str());
}


ConfParser::NoOpeningBracket::NoOpeningBracket(const std::string token, ConfParser *p)
: _msg("webserv: directive \"" + token + "\" has no opening \"{\" in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::NoOpeningBracket::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::NoClosingBracket::NoClosingBracket(const std::string token, ConfParser *p)
: _msg("webserv: directive \"" + token + "\" has no closing \"}\" in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::NoClosingBracket::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::UnexpectedEOF::UnexpectedEOF(const std::string token, ConfParser *p)
: _msg("webserv: unexpected " + token + ", expecting \"}\" in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::UnexpectedEOF::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::UnknownDirective::UnknownDirective(const std::string token, ConfParser *p)
: _msg("webserv: Unknown directive \"" + token + "\" in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::UnknownDirective::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::InvalidNbrOfArgs::InvalidNbrOfArgs(const std::string token, ConfParser *p)
: _msg("webserv: invalid number of arguments in \"" + token + "\" directive in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::InvalidNbrOfArgs::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::InvalidValue::InvalidValue(const std::string token, ConfParser *p)
: _msg("webserv: \"" + token + "\" directive invalid value in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::InvalidValue::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::CannotOpenFile::CannotOpenFile(ConfParser *p)
: _msg("webserv: cannot open file \"" + p->_configFile + "\"")
{
	return;
}

const char* ConfParser::CannotOpenFile::what() const throw()
{
	return (this->_msg.c_str());
}
