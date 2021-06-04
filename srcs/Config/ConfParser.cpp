/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:55:10 by julnolle          #+#    #+#             */
/*   Updated: 2021/06/04 09:42:46 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"

ConfParser::ConfParser(void) :
_configFile(DEFAULT_CONF_FILE),
_block_type(NOBLOCK), _line_nb(1)
{
	return;
}

ConfParser::ConfParser(const std::string filename) :
_configFile(filename),
_block_type(NOBLOCK), _line_nb(1)
{
	return; 
}

ConfParser::~ConfParser(void)
{
	std::cout << "DESTRUCTOR CALLED" << std::endl;
	return ;
}

/**
 * Static attributs
 *
 * Contains directive listing assiociated
 * with their function handler 
 */

typedef std::map<std::string, int (ConfParser::*)(void)>	dirMap; //move this typedef in higher level ?

dirMap	ConfParser::http_map	= setHttpMap();
dirMap	ConfParser::srv_map		= setSrvMap();
dirMap	ConfParser::loc_map		= setLocMap();

dirMap	ConfParser::setHttpMap()
{
	dirMap map;

	map["root"] = &ConfParser::setRoot;
	map["autoindex"] = &ConfParser::setAutoIndex;
	map["index"] = &ConfParser::setIndex;
	map["limit_except"] = &ConfParser::setAllowedMethods;
	map["error_page"] = &ConfParser::setErrorPage;
	map["client_max_body_size"] = &ConfParser::setMaxBdySize;
	map["keepalive_timeout"] = &ConfParser::setKeepAlive;
	map["chunked_transfer_encoding"] = &ConfParser::setChunkEnc;
	map["auth_basic"] = &ConfParser::setAuthBasic;
	map["auth_basic_user_file"] = &ConfParser::setAuthBasicFile;
	map["cgi_allowed_extensions"] = &ConfParser::setCgiAllowedExt;
	map["include"] = &ConfParser::parseInclude;

	return map;
}

dirMap	ConfParser::setSrvMap()
{
	dirMap map;

	map["listen"] = &ConfParser::setListen;
	map["root"] = &ConfParser::setRoot;
	map["server_name"] = &ConfParser::setServerName;
	map["autoindex"] = &ConfParser::setAutoIndex;
	map["index"] = &ConfParser::setIndex;
	map["limit_except"] = &ConfParser::setAllowedMethods;
	map["error_page"] = &ConfParser::setErrorPage;
	map["client_max_body_size"] = &ConfParser::setMaxBdySize;
	map["keepalive_timeout"] = &ConfParser::setKeepAlive;
	map["chunked_transfer_encoding"] = &ConfParser::setChunkEnc;
	map["auth_basic"] = &ConfParser::setAuthBasic;
	map["auth_basic_user_file"] = &ConfParser::setAuthBasicFile;
	map["cgi_allowed_extensions"] = &ConfParser::setCgiAllowedExt;

	return map;
}

dirMap	ConfParser::setLocMap()
{
	dirMap map;

	map["root"] = &ConfParser::setRoot;
	map["autoindex"] = &ConfParser::setAutoIndex;
	map["index"] = &ConfParser::setIndex;
	map["limit_except"] = &ConfParser::setAllowedMethods;
	map["error_page"] = &ConfParser::setErrorPage;
	map["client_max_body_size"] = &ConfParser::setMaxBdySize;
	map["keepalive_timeout"] = &ConfParser::setKeepAlive;
	map["chunked_transfer_encoding"] = &ConfParser::setChunkEnc;
	map["auth_basic"] = &ConfParser::setAuthBasic;
	map["auth_basic_user_file"] = &ConfParser::setAuthBasicFile;
	map["cgi_allowed_extensions"] = &ConfParser::setCgiAllowedExt;

	return map;
}

/**
 * Directive handling functions
 *
 * Store directives values according to the block
 * where there are found in config file
 */

int ConfParser::setListen(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	size_t pos = this->_dir_line[1].find(":");

	if (pos != std::string::npos)
	{
		if (this->_servers.back().setListenIp(this->_dir_line[1].substr(0, pos)) == FAILURE)
			throw UnknownHost(this->_dir_line[1], this);
		if (this->_servers.back().setListenPort(this->_dir_line[1].substr(pos + 1).c_str()) == FAILURE)
			throw InvalidPort(this->_dir_line[1], this);
	}
	else if  (pos == std::string::npos && ft_isdigit_string(this->_dir_line[1])) 
	{
		if (this->_servers.back().setListenPort(this->_dir_line[1].c_str()) == FAILURE)
			throw InvalidPort(this->_dir_line[1], this);
	}
	else if (pos == std::string::npos)
	{
		if (this->_servers.back().setListenIp(this->_dir_line[1]) == FAILURE)
			throw UnknownHost(this->_dir_line[1], this);
	}
	else
		throw InvalidValue(this->_dir_line[0], this);
	
	// std::cout << "SET LISTEN FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setServerName(void)
{
	this->checkNbrOfArgs(1, &is_higher<size_t>);

	std::vector<std::string>::iterator first = this->_dir_line.begin();

	this->_servers.back().setServerNames(++first, this->_dir_line.end());

	// std::cout << "SET SRVNAME FUNCTION" << std::endl;
	return 0;
}

int ConfParser::setRoot(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	if (this->_block_type == HTTP)
	{
		this->_httpBlock.setRoot(this->_dir_line[1]);
	}
	else if (this->_block_type == SERVER)
	{
		this->_servers.back().setRoot(this->_dir_line[1]);
	}
	if (this->_block_type == LOCATION)
	{
		this->_curr_location->setRoot(this->_dir_line[1]);
	}

	// std::cout << "SET ROOT FUNCTION" << std::endl;
	return 0;
}

int ConfParser::setAutoIndex(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	changeCaseString(this->_dir_line[1], ::tolower);

	if (this->_dir_line[1] != "on" && this->_dir_line[1] != "off")
		throw InvalidValue(this->_dir_line[0], this);

	if (this->_block_type == HTTP)
		this->_httpBlock.setAutoIndex(this->_dir_line[1]);
	else if (this->_block_type == SERVER)
		this->_servers.back().setAutoIndex(this->_dir_line[1]);
	if (this->_block_type == LOCATION)
		this->_curr_location->setAutoIndex(this->_dir_line[1]);
	// std::cout << "SET AUTOIDX FUNCTION" << std::endl;
	return 0;
}

int ConfParser::setIndex(void)
{
	this->checkNbrOfArgs(1, &is_higher<size_t>);

	std::vector<std::string>::iterator first = this->_dir_line.begin();
	if (this->_block_type == HTTP)
		this->_httpBlock.setIndexes(++first, this->_dir_line.end());
	else if (this->_block_type == SERVER)
		this->_servers.back().setIndexes(++first, this->_dir_line.end());
	if (this->_block_type == LOCATION)
		this->_curr_location->setIndexes(++first, this->_dir_line.end());

	// std::cout << "SET INDX FUNCTION" << std::endl;
	return 0;
}

int ConfParser::setAllowedMethods(void)
{
	this->checkNbrOfArgs(1, &is_higher<size_t>);

	std::vector<std::string>::iterator first = this->_dir_line.begin();
	if (this->_block_type == HTTP)
		this->_httpBlock.setLimitExcept(++first, this->_dir_line.end());
	else if (this->_block_type == SERVER)
		this->_servers.back().setLimitExcept(++first, this->_dir_line.end());
	if (this->_block_type == LOCATION)
		this->_curr_location->setLimitExcept(++first, this->_dir_line.end());
	// std::cout << "SET ALLMETH FUNCTION" << std::endl;
	return 0;
}

int ConfParser::setErrorPage(void)
{
	int ret = SUCCESS;
	this->checkNbrOfArgs(2, &is_higher<size_t>);

	std::vector<std::string>::iterator first = this->_dir_line.begin();
	std::vector<std::string>::iterator end = this->_dir_line.end();

	for (size_t i = 1; i < this->_dir_line.size() - 1; ++i)
	{
		if (!ft_isdigit_string(this->_dir_line[i]))
			throw InvalidValue(this->_dir_line[0], this);
	}

	if (this->_block_type == HTTP)
		ret  = this->_httpBlock.setErrorPages(++first, --end, this->_dir_line.back());
	else if (this->_block_type == SERVER)
	{
		this->_servers.back().setErrorPages(++first, --end, this->_dir_line.back());
	}
	if (this->_block_type == LOCATION)
	{
		this->_curr_location->setErrorPages(++first, --end, this->_dir_line.back());
	}
	if (ret == FAILURE)
		throw InvalidValue(this->_dir_line[0], this);

	// std::cout << "SET ERROPAGE FUNCTION" << std::endl;
	return 0;
}

int ConfParser::setMaxBdySize(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	if (!ft_isdigit_string(this->_dir_line[1]))
		throw InvalidValue(this->_dir_line[0], this);

	int mbs = atoi(this->_dir_line[1].c_str());

	if (this->_block_type == HTTP)
	{
		this->_httpBlock.setMaxBdySize(mbs);
		// this->_servers.back().setMaxBdySize(mbs);
	}
	else if (this->_block_type == SERVER)
	{
		this->_servers.back().setMaxBdySize(mbs);
	}
	if (this->_block_type == LOCATION)
	{
		this->_curr_location->setMaxBdySize(mbs);
	}
	// std::cout << "SET MAXBDYSIZE FUNCTION" << std::endl;
	return 0;
}

int ConfParser::setKeepAlive(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	if (!ft_isdigit_string(this->_dir_line[1]))
		throw InvalidValue(this->_dir_line[0], this);

	int mbs = atoi(this->_dir_line[1].c_str());

	if (this->_block_type == HTTP)
		this->_httpBlock.setKeepaliveTimeout(mbs);
	else if (this->_block_type == SERVER)
	{
		this->_servers.back().setKeepaliveTimeout(mbs);
	}
	if (this->_block_type == LOCATION)
	{
		this->_curr_location->setKeepaliveTimeout(mbs);
	}

	// std::cout << "SET keepalive FUNCTION" << std::endl;
	return 0;
}

int ConfParser::setChunkEnc(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	changeCaseString(this->_dir_line[1], ::tolower);

	if (this->_dir_line[1] != "on" && this->_dir_line[1] != "off")
		throw InvalidValue(this->_dir_line[0], this);

	if (this->_block_type == HTTP)
		this->_httpBlock.setChunkedEncoding(this->_dir_line[1]);
	else if (this->_block_type == SERVER)
		this->_servers.back().setChunkedEncoding(this->_dir_line[1]);
	if (this->_block_type == LOCATION)
		this->_curr_location->setChunkedEncoding(this->_dir_line[1]);

	// std::cout << "SET CHUNKED ENC" << std::endl;
	return 0;
}

int ConfParser::setAuthBasic(void)
{
	this->checkNbrOfArgs(1, &is_higher<size_t>);
	
	std::string area_name;
	
	if (this->_dir_line.size() == 2)
		area_name = changeCaseString(this->_dir_line[1], ::tolower);
	else
	{
		size_t size = this->_dir_line.size();
		for (size_t i = 1; i < size - 2; ++i)
		{
			area_name += this->_dir_line[i] += ' ';
		}
		area_name += this->_dir_line[size - 1];

		// pos = area_name.find_first_of("\"");
		// if (pos != std::string::npos)
		// 	area_name.erase(pos, 1);
		// pos = area_name.find_last_of("\"");
		// if (pos != std::string::npos)
		// 	area_name.erase(pos, 1);
	}
	
	if (this->_block_type == HTTP)
		this->_httpBlock.setAuthBasic(area_name);
	else if (this->_block_type == SERVER)
		this->_servers.back().setAuthBasic(area_name);
	if (this->_block_type == LOCATION)
		this->_curr_location->setAuthBasic(area_name);

	// std::cout << "SET AUTH_BASIC" << std::endl;
	return 0;
}

int ConfParser::setAuthBasicFile(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	if (this->_block_type == HTTP)
		this->_httpBlock.setAuthBasicFile(this->_dir_line[1]);
	else if (this->_block_type == SERVER)
		this->_servers.back().setAuthBasicFile(this->_dir_line[1]);
	if (this->_block_type == LOCATION)
		this->_curr_location->setAuthBasicFile(this->_dir_line[1]);

	// std::cout << "SET AUTH_BASIC_FILE" << std::endl;
	return 0;
}

int ConfParser::setCgiAllowedExt(void)
{
	this->checkNbrOfArgs(1, &is_higher<size_t>);

	std::vector<std::string>::iterator first = this->_dir_line.begin();
	if (this->_block_type == HTTP)
		this->_httpBlock.setCgiAllowedExt(++first, this->_dir_line.end());
	else if (this->_block_type == SERVER)
		this->_servers.back().setCgiAllowedExt(++first, this->_dir_line.end());
	if (this->_block_type == LOCATION)
		this->_curr_location->setCgiAllowedExt(++first, this->_dir_line.end());

	return 0;
}

int ConfParser::parseInclude()
{
	this->checkNbrOfArgs(2, &same_as<size_t>);
	
	this->readConfFile(this->_dir_line[1]);
	// std::cout << "PARSE INCLUDE FUNCTION" << std::endl;
	return 0;
}

/**
 * Utility function	
 *
 * Checks the number of args found the current parsed directive.
 * The comparison is made by Compare function.
 * Throws exception if comparison fails
 */

template <class Compare>
void ConfParser::checkNbrOfArgs(size_t expected_nbr, Compare comp)
{
	if (!comp(expected_nbr, this->_dir_line.size()))
		throw InvalidNbrOfArgs(this->_dir_line[0], this);
}

/**
 * Utility function	
 *
 * Erases comments in config file
 * If the '#' is the first char in string, erase all the string,
 * Else, erase characters from '#' to its end of string
 */

void ConfParser::eraseComments(std::string& line)
{
	size_t pos;
	size_t len = 0;

	pos = line.find_first_of("#");
	len = line.size() - pos;

	if (pos != std::string::npos)
		line.erase(pos, len);
}

/**
 * Utility function	
 *
 * Erases the last ';' in _dir_line (vector) if exists.
 * If the vector at the old ';' position becomes empty, delete it
 */

void ConfParser::eraseSemiColon()
{
	size_t pos = this->_dir_line.back().find_last_of(";");

	if (pos != std::string::npos)
	{
		this->_dir_line.back().erase(pos, 1);
		if (this->_dir_line.back().empty())
			this->_dir_line.pop_back();
	}
}

/**
 * Main Parser	
 *
 * Reads confFile line by line and extract tokens in vector.
 * Sets block flag in witch directive belongs to.
 * Calls proper handling directive function.
 */

void ConfParser::readConfFile(const std::string& confFile)
{
	std::ifstream	file;
	std::string		line;
	size_t			line_nb_save = this->_line_nb;

	this->_configFile = confFile;	
	file.open(confFile.c_str());
	if (file.fail())
		throw FileOperationFail("open", this);

	this->_dir_line.clear();
	this->_line_nb = 1;
	while (getline(file, line))
	{
		this->parseLine(line);

		file.clear();
		this->_line_nb++;
	}
	file.clear();

	file.close();
	if (file.fail())
		throw FileOperationFail("close", this);
	
	if (this->_block_type == SERVER
		|| this->_block_type == LOCATION
		|| (this->_block_type == HTTP && confFile == DEFAULT_CONF_FILE)
	)
		throw UnexpectedEOF("end of file", this);

	this->_line_nb = line_nb_save;
	this->_configFile = DEFAULT_CONF_FILE;
	return ;
}


void ConfParser::parseLine(std::string& line)
{
	// delete all text after '#' in a line
	eraseComments(line);

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
}

void ConfParser::handleBlockIn(const std::string& directive)
{
	if (directive == "http")
	{
		if (this->_block_type == HTTP)
			throw UnexpectedTocken("http", this);
		if (this->_dir_line.at(1) == "{")
			this->_block_type = HTTP;
		else
			throw NoOpeningBracket("http", this);
		return ;
	}
	if (directive == "server")
	{
		if (this->_block_type == SERVER)
			throw UnexpectedTocken("server", this);
		if (this->_dir_line.at(1) == "{")
		{
			this->_block_type = SERVER;
			this->_servers.push_back(ServerBlock());
		}
		else
			throw NoOpeningBracket("server", this);
		return ;
	}
	if (directive == "location")
	{
		if (this->_block_type == LOCATION)
			throw UnexpectedTocken("location", this);
		this->checkNbrOfArgs(3, &same_as<size_t>);
		if (this->_dir_line.at(2) == "{")
		{
			this->_block_type = LOCATION;

			std::pair<std::map<std::string, LocationBlock>::iterator, bool> newLoc;
			newLoc = this->_servers.back().addLocation(this->_dir_line[1]);
			if (newLoc.second)
			{
				this->_curr_location = &newLoc.first->second;
				this->_curr_location->setPath(this->_dir_line[1]); //Optional since 
																// locations are stored in map
			}
			else
				throw DuplicateLocation(this->_dir_line[1], this);
		}
		else
			throw NoOpeningBracket("location", this);
	}
}

void ConfParser::handleBlockOut()
{
	if (this->_block_type == HTTP)
		this->_block_type = NOBLOCK;
	else if (this->_block_type == SERVER)
		this->_block_type = HTTP;
	else if (this->_block_type == LOCATION)
		this->_block_type = SERVER;
}

void ConfParser::parseDirective()
{
	std::string directive(this->_dir_line.at(0));
	
	if (this->_block_type == NOBLOCK && this->_line_nb != 1)
	{
		--this->_line_nb;
		throw UnexpectedTocken("}", this);
	}

	if (directive == "{")
		throw UnexpectedTocken("{", this);

	if (directive == "}")
	{
		this->handleBlockOut();
		return ;
	}
	
	if (directive == "http" || directive == "server" || directive == "location")
	{
		this->handleBlockIn(directive);
		return ;
	}

	
	this->eraseSemiColon();
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
	else if (this->_block_type == LOCATION)
	{
		if (ConfParser::loc_map.find(directive) != loc_map.end())
			(this->*ConfParser::loc_map[directive])();
		else
			throw UnknownDirective(directive, this);
	}
	else if (this->_block_type == NOBLOCK)
	{
		throw UnexpectedTocken(directive, this);
	}
}

/*int		ConfParser::checkDuplicateServer(std::vector<ServerBlock>::iterator pos)
{
	std::vector<ServerBlock>::iterator it = this->_servers.begin();

	while (it != pos)
	{
		if (it->_listenIP == pos->_listenIP
			&& it->_listenPort == pos->_listenPort
			&& it->_server_names == pos->_server_names)
	}
	return (SUCCESS);
}

void	ConfParser::sanitizeServers(void)
{
	std::vector<ServerBlock>::iterator it = this->_servers.begin();
	std::vector<ServerBlock>::iterator end = this->_servers.end();

	while (it != end)
	{
		this->checkDuplicateServer(it);
		++it;
	}
}
*/

/*Getters*/

HttpBlock&		ConfParser::getHttpBlock(void)
{
	return this->_httpBlock;
}

const std::vector<ServerBlock>&	ConfParser::getServers() const
{
	return this->_servers;
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

ConfParser::FileOperationFail::FileOperationFail(const std::string op, ConfParser *p)
: _msg("webserv: cannot " + op + " file \"" + p->_configFile + "\"")
{
	return;
}

const char* ConfParser::FileOperationFail::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::UnknownHost::UnknownHost(const std::string token, ConfParser *p)
: _msg("webserv: host not found in \"" + token + "\" of the \"listen\" directive in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::UnknownHost::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::InvalidPort::InvalidPort(const std::string token, ConfParser *p)
: _msg("webserv: invalid port in \"" + token + "\" of the \"listen\" directive in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::InvalidPort::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::DuplicateLocation::DuplicateLocation(const std::string token, ConfParser *p)
: _msg("webserv: duplicate location \"" + token + "\" in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::DuplicateLocation::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::DuplicateDirective::DuplicateDirective(const std::string token, ConfParser *p)
: _msg("webserv: \"" + token + "\" directive is duplicate in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::DuplicateDirective::what() const throw()
{
	return (this->_msg.c_str());
}
