/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:37:57 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/12 18:38:23 by julnolle         ###   ########.fr       */
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
	this->_index.push_back(DEFAULT_INDEX);
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
	this->_index = copy._index;
	this->_chunked_transfer_encoding = copy._chunked_transfer_encoding;

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
	this->_index = rhs._index;
	this->_chunked_transfer_encoding = rhs._chunked_transfer_encoding;

	return *this;
}


// Setters
void	ServerBlock::setMaxBdySize(size_type size)
{
	this->_client_max_body_size = size;
}
void	ServerBlock::setKeepaliveTimeout(size_type timeout)
{
	this->_keepalive_timeout = timeout;
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

const bool&		ServerBlock::getAutoindex(void) const
{
	return this->_autoindex;
}

const std::string&		ServerBlock::getRoot(void) const
{
	return this->_root;
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

const stringVec&	ServerBlock::getIndexes(void) const
{
	return this->_index;
}

const bool&			ServerBlock::getChunkedEncoding(void) const
{
	return this->_chunked_transfer_encoding;
}

const stringVec&	ServerBlock::getAuthBasic(void) const
{
	return this->_auth_basic;
}

std::ostream & operator<<(std::ostream & o, ServerBlock const & rhs)
{
	o << "LISTEN IP: " << rhs.getListenIP() << std::endl;
	o << "LISTEN PORT: " << rhs.getListenPort() << std::endl;
	// o << "SERVER NAMES: " << rhs.getServerNames() << std::endl;
	o << "AUTOINDEX: " << rhs.getAutoindex() << std::endl;
	o << "ROOT: " << rhs.getRoot() << std::endl;
	// o << "LOCATIONS: " << rhs.getLocations() << std::endl;
	// o << "ERROR PAGES: " << rhs.getErrorPages() << std::endl;
	o << "MAX BDY SIZE: " << rhs.getMaxBdySize() << std::endl;
	o << "KEEP. TIMEOUT: " << rhs.getKeepaliveTime() << std::endl;
	// o << "INDEXES: " << rhs.getIndexes() << std::endl;
	o << "CHUNKED ENC.: " << rhs.getChunkedEncoding() << std::endl;
	// o << "AUTH BASIC: " << rhs.getAuthBasic() << std::endl;

	return o;
}
