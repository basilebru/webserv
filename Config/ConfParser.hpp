/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:49:49 by julnolle          #+#    #+#             */
/*   Updated: 2021/03/22 17:50:26 by julnolle         ###   ########.fr       */
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
	HttpBlock				_httpBlock;
	// std::string				_line;
	void parseHttp(char *line); 
	void paseServer(char *line); 
	void ParseAutoindex(char *line); 
	void ParseListen(char *line);

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
