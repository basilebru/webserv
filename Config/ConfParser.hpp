/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:49:49 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/01 19:12:31 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_PARSER_HPP
# define CONF_PARSER_HPP

# include <iostream>
# include <unistd.h>
# include <fstream>
# include <sstream>
# include "HttpBlock.hpp"

class ConfParser {

private:
	const std::string		_configFile;
	HttpBlock				*_httpBlock; // will certainely be removed, ConfParser will be called from HttpBlock
	int						_block_type;
	size_t					_nbr_of_srv;
	size_t					_nbr_of_loc;

	void	check_block(std::string& token);
	int		parseHttp(std::string& token); 
	int		parseServer(std::string& token); 
	int		parseLocation(std::string& token);

public:
	ConfParser(void);
	ConfParser(std::string filename);
	ConfParser(ConfParser const & copy);
	~ConfParser(void);
	ConfParser& operator=(ConfParser const & rhs);

	void readConfFile(void);
	void setDirective(std::string &);
	void parseLine(std::string& line);
	void parseLine2(std::string& line);

};

std::ostream & operator<<(std::ostream & o, ConfParser const & rhs);

#endif // CONF_PARSER_HPP
