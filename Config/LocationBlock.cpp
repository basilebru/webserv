/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:40:40 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/12 16:43:25 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationBlock.hpp"

LocationBlock::LocationBlock(void)
{
	this->_path = "";
	this->_root = DEFAULT_ROOT;
	this->_chunked_transfer_encoding = DEFAULT_CHUNKED_ENC;
	this->_client_max_body_size = DEFAULT_MAX_BDY_SIZE;
	this->_keepalive_timeout = DEFAULT_KEEPALIVE_T;
	this->_cgi_pass = "";
	this->_cgi_port = 9000;
	this->_cgi_index = "";
	this->_index.push_back(DEFAULT_INDEX);
	this->_auth_basic.push_back(DEFAULT_AUTH_BASIC);

	return;
}

LocationBlock::LocationBlock(LocationBlock const & copy)
{
	this->_path = copy._path;
	this->_root = copy._root;
	this->_limit_except = copy._limit_except;
	this->_error_pages = copy._error_pages;
	this->_index = copy._index;
	this->_chunked_transfer_encoding = copy._chunked_transfer_encoding;
	this->_client_max_body_size = copy._client_max_body_size;
	this->_keepalive_timeout = copy._keepalive_timeout;
	this->_cgi_pass = copy._cgi_pass;
	this->_cgi_port = copy._cgi_port;
	this->_cgi_index = copy._cgi_index;
	this->_cgi_params = copy._cgi_params;

	return ;
}

LocationBlock::~LocationBlock(void)
{
	return;
}

LocationBlock& LocationBlock::operator=(LocationBlock const & rhs)
{
	this->_path = rhs._path;
	this->_root = rhs._root;
	this->_limit_except = rhs._limit_except;
	this->_error_pages = rhs._error_pages;
	this->_index = rhs._index;
	this->_chunked_transfer_encoding = rhs._chunked_transfer_encoding;
	this->_client_max_body_size = rhs._client_max_body_size;
	this->_keepalive_timeout = rhs._keepalive_timeout;
	this->_cgi_pass = rhs._cgi_pass;
	this->_cgi_port = rhs._cgi_port;
	this->_cgi_index = rhs._cgi_index;
	this->_cgi_params = rhs._cgi_params;

	return *this;
}

// Setters
void	LocationBlock::setMaxBdySize(size_type size)
{
	this->_client_max_body_size = size;
}
void	LocationBlock::setKeepaliveTimeout(size_type timeout)
{
	this->_keepalive_timeout = timeout;
}
