/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 15:23:44 by julnolle          #+#    #+#             */
/*   Updated: 2021/03/18 09:13:47 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_BLOCK_HPP
# define SERVER_BLOCK_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>
# include "LocationBlock.hpp"

class ServerBlock {

	typedef size_t					size_type;

private:
	std::string						_listenIP;
	unsigned int					_listenPort;
	std::vector<std::string>		_server_names;
	bool							_autoindex;
	std::string						_root;
	std::vector<LocationBlock>		_locations; /*Plusieurs LocationBlocks possibles dans un ServerBlock */
	std::map<int, std::string>		_error_pages; /* num error, error file path */
	size_type						_client_max_body_size;
	std::vector<std::string>		_index;
	bool							_chunked_transfer_encoding; // on | off

public:
	ServerBlock(void);
	ServerBlock(ServerBlock const & copy);
	~ServerBlock(void);
	ServerBlock& operator=(ServerBlock const & rhs);
	
};

std::ostream & operator<<(std::ostream & o, ServerBlock const & rhs);

#endif // SERVER_BLOCK_HPP
