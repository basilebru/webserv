/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:55:10 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/07 12:09:05 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"
#include <stdio.h>
#include <cstring>

ConfParser::ConfParser(void)
: _configFile("nginx.conf"), _httpBlock(new HttpBlock()),
_block_type(HTTP), _line_nb(1), _nbr_of_srv(0),
_nbr_of_loc(0), _in_block(FALSE), _semi_col_not_found(0),
_curr_dir(-1)
{
	return;
}

ConfParser::ConfParser(const std::string filename)
: _configFile(filename), _httpBlock(new HttpBlock()),
_block_type(HTTP), _line_nb(1), _nbr_of_srv(0),
_nbr_of_loc(0), _in_block(FALSE), _semi_col_not_found(0),
_curr_dir(-1)
{
	return; 
}

ConfParser::~ConfParser(void)
{
	std::cout << "DESTRUCTOR CALLED" << std::endl;
	delete this->_httpBlock;
	return ;
}

/**
 * Static attributs
 *
 * Contains directive listing assiociated
 * with their function handler 
 */

typedef std::map<std::string, int (ConfParser::*)(std::string&)>	dirMap; //move this typedef in higher level ?

dirMap	ConfParser::http_map = setHttpMap();
dirMap	ConfParser::srv_map = setSrvMap();
dirMap	ConfParser::loc_map = setLocMap();

dirMap	ConfParser::setHttpMap()
{
	dirMap map;

	map["root"] = &ConfParser::setRoot;
	map["error_page"] = &ConfParser::setErrorPage;
	map["keepalive_timeout"] = &ConfParser::setTimeout;
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
	map["cgi_param"] = &ConfParser::setCgiParam;
	map["cgi_pass"] = &ConfParser::setCgiPass;
	map["allow_methods"] = &ConfParser::setAllowedMethods;
	map["index"] = &ConfParser::setIndex;
	map["autoindex"] = &ConfParser::setAutoIndex;

	return map;
}




std::string	ConfParser::http_dir[NB_HTTP_DIR] = {
	"server",
	"keepalive_timeout",
	"client_max_body_size",
	"include",
};

std::string	ConfParser::srv_dir[NB_SRV_DIR] = {
	"listen",
	"server_name",
	"autoindex",
	"root",
	"location",
	"index",
	"error_page",
	"client_max_body_size",
};

std::string	ConfParser::loc_dir[NB_LOC_DIR] = {
	"root",
	"location",
	"cgi_pass"
};


int ConfParser::setListen(std::string& token)
{
	(void)token;
	std::cout << "SET LISTEN FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setServerName(std::string& token)
{
	(void)token;
	std::cout << "SET SRVNAME FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setRoot(std::string& token)
{
	(void)token;
	std::cout << "SET ROOT FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setErrorPage(std::string& token)
{
	(void)token;
	std::cout << "SET ERROPAGE FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setTimeout(std::string& token)
{
	(void)token;
	std::cout << "SET TIME FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setMaxBdySize(std::string& token)
{
	(void)token;
	std::cout << "SET MAXBDYSIZE FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setCgiParam(std::string& token)
{
	(void)token;
	std::cout << "SET CGIPARAM FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setCgiPass(std::string& token)
{
	(void)token;
	std::cout << "SET CGIPASS FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setAllowedMethods(std::string& token)
{
	(void)token;
	std::cout << "SET ALLMETH FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setIndex(std::string& token)
{
	(void)token;
	std::cout << "SET INDX FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setAutoIndex(std::string& token)
{
	(void)token;
	std::cout << "SET AUTOIDX FUNCTION" << std::endl;
	return 0;
}


int ConfParser::readConfFile()
{
	std::ifstream	file;
	std::string		line;
	size_t			ret = 0;


	file.open(this->_configFile.c_str());
	if (file.fail())
	{
		std::cerr << "webserv: cannot open file \"" << this->_configFile << "\"" << std::endl;
		return 1;
	}
	try {
		while (getline(file, line))
		{
			ret = this->parseLine(line);
			
			file.clear();
			if (ret == 1)
				break;
			this->_line_nb++;
		}
		if (this->_in_block[HTTP] == TRUE)
			throw NoClosingBracket("http", this->_line_nb);
		else
		std::cout << GREEN << "FIIIIIIIIIINNNNNNNN" << NOCOLOR << std::endl;
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		ret = 1;
	}
	return ret;
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

/*void ConfParser::identify_block(std::string& token)
{
	typedef	int			(ConfParser::*t_parse)(std::string&);
	static t_parse		func[NB_BLOCKS] = {&ConfParser::parseHttp, &ConfParser::parseServer, &ConfParser::parseLocation};
	static std::string	blocks[NB_BLOCKS] = {"http", "server", "location"};
	// static int			ret[NB_BLOCKS] = {HTTP, SERVER, LOCATION};

	size_t i(0);
	while (i < NB_BLOCKS)
	{
		if (blocks[i] == token)
		{
			(this->*func[i])(token);
			return ;
		}
		++i;
	}
	return ;
}
*/

int ConfParser::isHttpDirective(std::string& token) const
{
	int i(0);
	while (i < NB_HTTP_DIR)
	{
		if (ConfParser::http_dir[i] == token)
		{
			return i;
		}
		++i;
	}
	return FAILIURE;
}

bool ConfParser::isSrvDirective(std::string& token) const
{
	size_t i(0);
	while (i < NB_SRV_DIR)
	{
		if (ConfParser::srv_dir[i] == token)
		{
			return true;
		}
		++i;
	}
	return false;
}

bool ConfParser::isLocDirective(std::string& token) const
{
	size_t i(0);
	while (i < NB_LOC_DIR)
	{
		if (ConfParser::loc_dir[i] == token)
		{
			return true;
		}
		++i;
	}
	return false;
}

int ConfParser::parseLine(std::string& line)
{
	size_t	line_nb = 1;
	int		ret = 0;

	typedef	int		(ConfParser::*t_parse)(std::string&);
	static t_parse	func[NB_BLOCKS] = {&ConfParser::parseHttp, &ConfParser::parseServer, &ConfParser::parseLocation};

	std::cout << "NATIVE LINE  : " << line << std::endl;
	erase_comments(line);
	std::cout << "LINE w/o com.: " << line << std::endl << std::endl;

	std::istringstream iss(line); 
	for(std::string token; iss >> token && ret == 0; )
	{
		std::cout << "TOKEN " << line_nb << ": " << token << std::endl;
		
		ret = (this->*func[this->_block_type])(token);
		if (ret == 1)
			break ;

		line_nb++;
	}
	std::cout << "===================" << std::endl;
	return ret;
}


int ConfParser::parseHttp(std::string& token)
{
	int i = FAILIURE;
	std::cout << "==> HTTP BLOCK" << std::endl << std::endl;
	
	if (token == "http")
		return 0;
	if (token == "server")
	{
		this->_block_type = SERVER;
		return 0;
	}
	if (token == "{")
	{
		if (this->_in_block[HTTP] == FALSE)
		{
			std::cout << "FOUND BRACKET" << std::endl;
			this->_in_block[HTTP] = TRUE;
			return 0;			
		}
		else
			throw UnexpectedTocken("{", this->_line_nb);
	}
	if (this->_in_block[HTTP] == FALSE)
		throw NoOpeningBracket("http", this->_line_nb);
	if (this->_curr_dir == FAILIURE)
	{
		if (((i = isHttpDirective(token)) != FAILIURE) && this->_in_block[HTTP] == TRUE)
		{
			std::cout << "STORE DIRECTIVE" << std::endl;
			this->_curr_dir = i;
			return 0;
		}
		else if (token != "}")
			throw UnknownDirective(token, this->_line_nb);
	}
	if (this->_curr_dir != FAILIURE)
	{
		std::cout << "STORE " << ConfParser::http_dir[this->_curr_dir] << " VALUE" << std::endl;
		if (token.find(";"))
		{	
			this->_curr_dir = FAILIURE;
		}
		return 0;
	}
	if (token == "}")
		this->_in_block[HTTP] = FALSE;
	return 0;
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

	if (token == "location")
		this->_block_type = LOCATION;

	return 0;
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

	return 0;
}




/*Exceptions*/

ConfParser::UnexpectedTocken::UnexpectedTocken(const std::string token, const size_t line_nb)
: _msg("webserv: unexpected \"" + token + "\":")
{
	std::ostringstream tmp;
	tmp << line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::UnexpectedTocken::what() const throw()
{
	return (this->_msg.c_str());
}


ConfParser::NoOpeningBracket::NoOpeningBracket(const std::string token, const size_t line_nb)
: _msg("webserv: directive \"" + token + "\" has no opening \"{\":")
{
	std::ostringstream tmp;
	tmp << line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::NoOpeningBracket::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::NoClosingBracket::NoClosingBracket(const std::string token, const size_t line_nb)
: _msg("webserv: directive \"" + token + "\" has no closing \"}\":")
{
	std::ostringstream tmp;
	tmp << line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::NoClosingBracket::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::UnknownDirective::UnknownDirective(const std::string token, const size_t line_nb)
: _msg("webserv: Unknown directive \"" + token + "\":")
{
	std::ostringstream tmp;
	tmp << line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::UnknownDirective::what() const throw()
{
	return (this->_msg.c_str());
}
