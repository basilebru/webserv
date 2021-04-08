/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBlock.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:29:05 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/08 20:37:50 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpBlock.hpp"

HttpBlock::HttpBlock(void)
: _autoindex(false), _chunked_transfer_encoding(true), _root(),
_error_pages(), _client_max_body_size(10),
_index(), _servers()
{
	return;
}

/*HttpBlock::HttpBlock(HttpBlock const & copy)
{
	
}
*/
HttpBlock::~HttpBlock(void)
{
	// delete this->_error_pages;
	// delete this->_index;
	// delete this->_servers;
	return;
}

/*HttpBlock& operator=(HttpBlock const & rhs)
{

}
*/

// Setters
void	HttpBlock::setMaxBdySize(size_type size)
{
	this->_client_max_body_size = size;
}

// Getters
ServerBlock&	HttpBlock::getLastServer()
{
	return this->_servers.back();
}



void HttpBlock::addServer()
{
	ServerBlock srv;
	this->_servers.push_back(srv);
}

