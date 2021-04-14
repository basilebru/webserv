/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:40:40 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/14 17:41:04 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationBlock.hpp"

LocationBlock::LocationBlock(void)
{
	this->_path = "";
	this->_root = DEFAULT_ROOT;
	this->_autoindex = DEFAULT_AUTOINDEX;
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
	this->_autoindex = copy._autoindex;
	this->_index = copy._index;
	this->_limit_except = copy._limit_except;
	this->_error_pages = copy._error_pages;
	this->_client_max_body_size = copy._client_max_body_size;
	this->_keepalive_timeout = copy._keepalive_timeout;
	this->_chunked_transfer_encoding = copy._chunked_transfer_encoding;
	this->_auth_basic = copy._auth_basic;
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
	this->_autoindex = rhs._autoindex;
	this->_index = rhs._index;
	this->_limit_except = rhs._limit_except;
	this->_error_pages = rhs._error_pages;
	this->_client_max_body_size = rhs._client_max_body_size;
	this->_keepalive_timeout = rhs._keepalive_timeout;
	this->_chunked_transfer_encoding = rhs._chunked_transfer_encoding;
	this->_auth_basic = rhs._auth_basic;
	this->_cgi_pass = rhs._cgi_pass;
	this->_cgi_port = rhs._cgi_port;
	this->_cgi_index = rhs._cgi_index;
	this->_cgi_params = rhs._cgi_params;

	return *this;
}

// Setters

void	LocationBlock::setPath(std::string path)
{
	this->_path = path;
}

void	LocationBlock::setRoot(std::string path)
{
	this->_root = path;
}

void	LocationBlock::setAutoIndex(std::string state)
{
	if (state == "on")
		this->_autoindex = true;
	else if (state == "off")
		this->_autoindex = false;
}

void	LocationBlock::setIndex(strVecIterator first, strVecIterator last)
{
	this->_index.assign(first, last);
}

void	LocationBlock::setLimitExcept(std::string method)
{
	this->_limit_except.push_back(method) ;
}

void	LocationBlock::setErrorPages(strVecIterator first, strVecIterator last, std::string& val)
{
	while (first != last)
	{
		this->_error_pages.insert(std::make_pair(atoi(first->c_str()), val));
		++first;
	}
}

void LocationBlock::setMaxBdySize(size_type size)
{
	this->_client_max_body_size = size;
}

void LocationBlock::setKeepaliveTimeout(size_type timeout)
{
	this->_keepalive_timeout = timeout;

}

void	LocationBlock::setChunkedEncoding(char state)
{
	if (state == '1')
		this->_chunked_transfer_encoding = true;
	if (state == '0')
		this->_chunked_transfer_encoding = false;
}

void	LocationBlock::setAuthBasic(std::string value)
{
	this->_auth_basic.push_back(value);
}

void	LocationBlock::setCgiPass(std::string value)
{
	this->_cgi_pass = value;
}

void	LocationBlock::setCgiPort(size_type port)
{
	this->_cgi_port = port;
}

void	LocationBlock::setCgiIndex(std::string index)
{
	this->_cgi_index = index;
}

void	LocationBlock::setCgiParams(mapCgiParams params)
{
	(void)params;
	// this->_;
}
