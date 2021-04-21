/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 15:59:09 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/21 11:35:52 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_BLOCK_HPP
# define LOCATION_BLOCK_HPP

# include "HttpBlock.hpp"

class LocationBlock : public HttpBlock {

	typedef std::map<std::string, std::string>		mapCgiParams;

private:
	std::string		_path; //Optional since locations are stored in map

	/*For CGI*/
	std::string		_cgi_pass;
	size_type		_cgi_port; //mixer en std::string avec _cgi_pass ?
	std::string		_cgi_index;
	mapCgiParams	_cgi_params;

public:
	LocationBlock(void);
	LocationBlock(LocationBlock const & copy);
	~LocationBlock(void);
	LocationBlock& operator=(LocationBlock const & rhs);

	// Setters
	void	setPath(std::string path);

	void	setCgiPass(std::string value);
	void	setCgiPort(size_type port);
	void	setCgiIndex(std::string index);
	void	setCgiParams(mapCgiParams params);

	// Getters
	const std::string&		getPath(void) const;
};

std::ostream & operator<<(std::ostream & o, LocationBlock const & rhs);

#endif // LOCATION_BLOCK_HPP
