/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:37:57 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/09 14:32:23 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerBlock.hpp"

ServerBlock::ServerBlock(void)
{
	this->_listenIP = "0.0.0.0";
	this->_listenPort = 80;
	this->_autoindex = false;
	this->_root = "";
	this->_client_max_body_size = 50;
	this->_keepalive_timeout = 0;
	this->_chunked_transfer_encoding = true;
	return;
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

	return;
}

ServerBlock::~ServerBlock(void)
{

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
std::vector<LocationBlock>&	ServerBlock::getLocations(void)
{
	return this->_locations;
}

LocationBlock&	ServerBlock::getLastLocation(void)
{
	return this->_locations.back();
}

size_type		ServerBlock::getMaxBdySize(void) const
{
	return this->_client_max_body_size;
}

