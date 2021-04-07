/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBlock.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:29:05 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/07 09:42:33 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpBlock.hpp"

HttpBlock::HttpBlock(void)
: _autoindex(false), _chunked_transfer_encoding(true), _root(),
_error_pages(new std::map<int, std::string>), _client_max_body_size(10),
_index(new std::vector<std::string>), _servers(new std::vector<ServerBlock>)
{
	return;
}

/*HttpBlock::HttpBlock(HttpBlock const & copy)
{
	
}
*/
HttpBlock::~HttpBlock(void)
{
	delete this->_error_pages;
	delete this->_index;
	delete this->_servers;
	return;
}

/*HttpBlock& operator=(HttpBlock const & rhs)
{

}
*/