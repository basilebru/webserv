/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 15:23:44 by julnolle          #+#    #+#             */
/*   Updated: 2021/03/17 17:14:04 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_BLOCK_HPP
# define SERVER_BLOCK_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>
# include <LocationBlock.hpp>

class ServerBlocks {

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
	ServerBlocks(void);
	ServerBlocks(ServerBlocks const & copy);
	~ServerBlocks(void);
	ServerBlocks& operator=(ServerBlocks const & rhs);
	
};

std::ostream & operator<<(std::ostream & o, ServerBlocks const & rhs);

#endif // SERVER_BLOCK_HPP
