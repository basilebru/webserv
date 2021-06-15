/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:40:40 by julnolle          #+#    #+#             */
/*   Updated: 2021/06/15 12:19:28 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationBlock.hpp"

LocationBlock::LocationBlock(void)
{
	// std::cout << GREEN << "DEF LOC CTOR" << NOCOLOR <<std::endl;
	// this->_root.clear();
	// this->_autoindex = NOT_SET;
	// this->_indexes.clear(),
	// this->_client_max_body_size = NOT_SET;
	// this->_keepalive_timeout = NOT_SET;
	// this->_chunked_transfer_encoding = NOT_SET;
	// this->_auth_basic = DEFAULT_AUTH_BASIC;
	// this->_cgi_port = DEFAULT_CGI_PORT;

	return;
}

LocationBlock::LocationBlock(LocationBlock const & copy)
{
	// std::cout << YELLOW << "CPY LOC CTOR" << NOCOLOR <<std::endl;
	this->_path = copy._path;
	this->_cgi_path = copy._cgi_path;
	this->_root = copy._root;
	this->_autoindex = copy._autoindex;
	this->_indexes = copy._indexes;
	this->_limit_except = copy._limit_except;
	this->_error_pages = copy._error_pages;
	this->_client_max_body_size = copy._client_max_body_size;
	this->_keepalive_timeout = copy._keepalive_timeout;
	this->_chunked_transfer_encoding = copy._chunked_transfer_encoding;
	this->_auth_basic = copy._auth_basic;
	this->_auth_basic_user_file = copy._auth_basic_user_file;
	this->_cgi_allowed_ext = copy._cgi_allowed_ext;
	this->_return = copy._return;

	return ;
}

LocationBlock::~LocationBlock(void)
{
	return;
}

LocationBlock& LocationBlock::operator=(LocationBlock const & rhs)
{
	this->_path = rhs._path;
	this->_cgi_path = rhs._cgi_path;
	this->_root = rhs._root;
	this->_autoindex = rhs._autoindex;
	this->_indexes = rhs._indexes;
	this->_limit_except = rhs._limit_except;
	this->_error_pages = rhs._error_pages;
	this->_client_max_body_size = rhs._client_max_body_size;
	this->_keepalive_timeout = rhs._keepalive_timeout;
	this->_chunked_transfer_encoding = rhs._chunked_transfer_encoding;
	this->_auth_basic = rhs._auth_basic;
	this->_auth_basic_user_file = rhs._auth_basic_user_file;
	this->_cgi_allowed_ext = rhs._cgi_allowed_ext;
	this->_return = rhs._return;

	return *this;
}

// Setters
void	LocationBlock::setPath(std::string const& path)
{
	this->_path = path;
}

void	LocationBlock::setCgiPath(std::string const& path)
{
	this->_cgi_path = path;
}

// Getters
const std::string&		LocationBlock::getPath(void) const
{
	return this->_path;
}

const std::string&		LocationBlock::getCgiPath(void) const
{
	return this->_cgi_path;
}


std::ostream & operator<<(std::ostream & o, LocationBlock const & rhs)
{
	static std::string pad("    ");

	o << pad << "PATH: " << rhs.getPath() << std::endl;
	o << pad << "ROOT: " << rhs.getRoot() << std::endl;
	o << pad << "AUTOINDEX: "; putState(o, rhs.getAutoindex());

	o << pad << "INDEXES: ";
	putVecToOstream(o, rhs.getIndexes().begin(), rhs.getIndexes().end());
	
	o << pad << "LIMIT_EXCEPT: ";
	putVecToOstream(o, rhs.getLimitExcept().begin(), rhs.getLimitExcept().end());

	o << pad << "ERROR PAGES: ";
	putMapToOstream(o, rhs.getErrorPages().begin(), rhs.getErrorPages().end());	

	o << pad << "REDIRECTIONS: ";
	o << "code " << rhs.getReturn().first << ", URL: " << rhs.getReturn().second << std::endl;	

	o << pad << "MAX BDY SIZE: " << rhs.getMaxBdySize() << std::endl;
	o << pad << "KEEP. TIMEOUT: " << rhs.getKeepaliveTimeout() << std::endl;

	o << pad << "CHUNKED ENC.: "; putState(o, rhs.getChunkedEncoding());
	
	o << pad << "AUTH BASIC: " << rhs.getAuthBasic() << std::endl;
	o << pad << "AUTH BASIC FILE: " << rhs.getAuthBasicFile() << std::endl;

	o << pad << "CGI PATH: " << rhs.getCgiPath() << std::endl;
	o << pad << "CGI ALLOWED EXT: ";
	putVecToOstream(o, rhs.getCgiAllowedExt().begin(), rhs.getCgiAllowedExt().end());

	return o;
}