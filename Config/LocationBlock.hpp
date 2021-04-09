/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 15:59:09 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/09 12:07:37 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_BLOCK_HPP
# define LOCATION_BLOCK_HPP

# include <string>
# include <iostream>
# include <vector>
# include <map>

class LocationBlock {

	typedef size_t					size_type;

private:
	std::string						_path;
	std::string						_root;
	std::vector<std::string>		_limit_except; //ou allow_methods ?
	std::map<int, std::string>		_error_pages; /* num error, error file path */
	std::vector<std::string>		_index;
	bool							_chunked_transfer_encoding; // on | off
	size_type						_client_max_body_size;
	size_type						_keepalive_timeout;

	/*For CGI*/
	std::string							_cgi_pass;
	unsigned int						_cgi_port; //mixer en std::string avec _cgi_pass
	std::string							_cgi_index;
	std::map<std::string, std::string>	_cgi_params;

public:
	LocationBlock(void);
	LocationBlock(LocationBlock const & copy);
	~LocationBlock(void);
	LocationBlock& operator=(LocationBlock const & rhs);

	// Setters
	void	setMaxBdySize(size_type size);
	void	setKeepaliveTimeout(size_type timeout);

};

std::ostream & operator<<(std::ostream & o, LocationBlock const & rhs);

#endif // LOCATION_BLOCK_HPP
