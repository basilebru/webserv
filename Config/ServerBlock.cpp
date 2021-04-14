/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:37:57 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/14 17:51:01 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerBlock.hpp"

ServerBlock::ServerBlock(void)
{
	this->_listenIP = DEFAULT_LISTEN_IP;
	this->_listenPort = DEFAULT_LISTEN_PORT;
	this->_autoindex = DEFAULT_AUTOINDEX;
	this->_root = DEFAULT_ROOT;
	this->_client_max_body_size = DEFAULT_MAX_BDY_SIZE;
	this->_keepalive_timeout = DEFAULT_KEEPALIVE_T;
	this->_indexes.push_back(DEFAULT_INDEX);
	this->_chunked_transfer_encoding = DEFAULT_CHUNKED_ENC;
	this->_auth_basic.push_back(DEFAULT_AUTH_BASIC);
	return ;
}

ServerBlock::ServerBlock(ServerBlock const & copy)
{
	this->_listenIP = copy._listenIP;
	this->_listenPort = copy._listenPort;
	this->_server_names = copy._server_names;
	this->_autoindex = copy._autoindex;
	this->_root = copy._root;
	this->_locations = copy._locations;
	this->_error_pages = copy._error_pages;
	this->_client_max_body_size = copy._client_max_body_size;
	this->_keepalive_timeout = copy._keepalive_timeout;
	this->_indexes = copy._indexes;
	this->_chunked_transfer_encoding = copy._chunked_transfer_encoding;
	this->_limit_except = copy._limit_except;

	return ;
}

ServerBlock::~ServerBlock(void)
{
	return ;
}

ServerBlock& ServerBlock::operator=(ServerBlock const & rhs)
{
	this->_listenIP = rhs._listenIP;
	this->_listenPort = rhs._listenPort;
	this->_server_names = rhs._server_names;
	this->_autoindex = rhs._autoindex;
	this->_root = rhs._root;
	this->_locations = rhs._locations;
	this->_error_pages = rhs._error_pages;
	this->_client_max_body_size = rhs._client_max_body_size;
	this->_keepalive_timeout = rhs._keepalive_timeout;
	this->_indexes = rhs._indexes;
	this->_chunked_transfer_encoding = rhs._chunked_transfer_encoding;
	this->_limit_except = rhs._limit_except;

	return *this;
}


// Setters

int	ServerBlock::setListenIp(std::string ip)
{
	if (ip == "localhost")
		ip = "127.0.0.1";
	if (ip.find_first_not_of("0123456789.") == std::string::npos && !ip.empty())
	{
		this->_listenIP = ip;
		return SUCCESS;
	}
	return FAILURE;
}

int	ServerBlock::setListenPort(size_type port)
{
	if (port)
	{
		this->_listenPort = port;
		return SUCCESS;
	}
	return FAILURE;
}

void	ServerBlock::setServerNames(strVecIterator first, strVecIterator last)
{
	this->_server_names.assign(first, last);
}

void	ServerBlock::setAutoIndex(std::string state)
{
	if (state == "on")
		this->_autoindex = true;
	else if (state == "off")
		this->_autoindex = false;
}

void	ServerBlock::setIndex(strVecIterator first, strVecIterator last)
{
	this->_indexes.assign(first, last);
}

void	ServerBlock::setRoot(std::string path)
{
	this->_root = path;
}

void	ServerBlock::setLimitExcept(std::string method)
{
	this->_limit_except.push_back(method);
}

void	ServerBlock::setErrorPages(strVecIterator first, strVecIterator last, std::string& val)
{
	while (first != last)
	{
		this->_error_pages.insert(std::make_pair(atoi(first->c_str()), val));
		++first;
	}
}

void	ServerBlock::setMaxBdySize(size_type size)
{
	this->_client_max_body_size = size;
}

void	ServerBlock::setKeepaliveTimeout(size_type timeout)
{
	this->_keepalive_timeout = timeout;
}

void	ServerBlock::setChunkedEncoding(char state)
{
	if (state == '1')
		this->_chunked_transfer_encoding = true;
	if (state == '0')
		this->_chunked_transfer_encoding = false;
}

void	ServerBlock::setAuthBasic(std::string value)
{
	this->_auth_basic.push_back(value);
}


// Getters

const std::string&		ServerBlock::getListenIP(void) const
{
	return this->_listenIP;
}

const size_type&		ServerBlock::getListenPort(void) const
{
	return this->_listenPort;
}

const stringVec&		ServerBlock::getServerNames(void) const
{
	return this->_server_names;
}

const std::string&		ServerBlock::getRoot(void) const
{
	return this->_root;
}

const bool&		ServerBlock::getAutoindex(void) const
{
	return this->_autoindex;
}

const stringVec&	ServerBlock::getIndexes(void) const
{
	return this->_indexes;
}

const size_type&		ServerBlock::getMaxBdySize(void) const
{
	return this->_client_max_body_size;
}

const std::vector<LocationBlock>&	ServerBlock::getLocations(void) const
{
	return this->_locations;
}

LocationBlock&	ServerBlock::getLastLocation(void)
{
	return this->_locations.back();
}

const errorMap&		ServerBlock::getErrorPages(void) const
{
	return this->_error_pages;
}

const size_type&	ServerBlock::getKeepaliveTime(void) const
{
	return this->_keepalive_timeout;
}

const bool&			ServerBlock::getChunkedEncoding(void) const
{
	return this->_chunked_transfer_encoding;
}

const stringVec&	ServerBlock::getAuthBasic(void) const
{
	return this->_auth_basic;
}


template<typename InputIterator>
void putVecToOstream(std::ostream& o, InputIterator first, InputIterator last)
{
	while(first != last) {
	    o << *first << ' ';
	    ++first;
	}
	o << std::endl;	
}

template<typename InputIterator>
void putMapToOstream(std::ostream& o, InputIterator first, InputIterator last)
{
	while(first != last) {
	    o << first->first << "->" << first->second << ' ';
	    ++first;
	}
	o << std::endl;	
}

std::ostream & operator<<(std::ostream & o, ServerBlock const & rhs)
{
	o << "LISTEN IP: " << rhs.getListenIP() << std::endl;
	o << "LISTEN PORT: " << rhs.getListenPort() << std::endl;
	
	o << "SERVER NAMES: ";
	putVecToOstream(o, rhs.getServerNames().begin(), rhs.getServerNames().end());

	o << "AUTOINDEX: " << std::boolalpha << rhs.getAutoindex() << std::endl;
	o << "ROOT: " << rhs.getRoot() << std::endl;
	// o << "LOCATIONS: " << rhs.getLocations() << std::endl;
	
	o << "ERROR PAGES: ";
	putMapToOstream(o, rhs.getErrorPages().begin(), rhs.getErrorPages().end());
	
	o << "MAX BDY SIZE: " << rhs.getMaxBdySize() << std::endl;
	o << "KEEP. TIMEOUT: " << rhs.getKeepaliveTime() << std::endl;
	
	o << "INDEXES: ";
	putVecToOstream(o, rhs.getIndexes().begin(), rhs.getIndexes().end());

	o << "CHUNKED ENC.: " << std::boolalpha << rhs.getChunkedEncoding() << std::endl;
	
	o << "AUTH BASIC: ";
	putVecToOstream(o, rhs.getAuthBasic().begin(), rhs.getAuthBasic().end());

	return o;
}

void ServerBlock::addLocation(void)
{
	LocationBlock loc;
	this->_locations.push_back(loc);
}
