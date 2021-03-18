/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:49:49 by julnolle          #+#    #+#             */
/*   Updated: 2021/03/18 09:28:22 by julnolle         ###   ########.fr       */
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
	HttpBlock &				_httpBlock;
	std::string				_line;
	ConfParser(void);


public:
	ConfParser(HttpBlock & http_block);
	ConfParser(std::string filename, HttpBlock & http_block);
	ConfParser(ConfParser const & copy);
	~ConfParser(void);
	ConfParser& operator=(ConfParser const & rhs);

	void readConfFile(void);
	void parseLine(void);
};

std::ostream & operator<<(std::ostream & o, ConfParser const & rhs);

#endif // CONF_PARSER_HPP
