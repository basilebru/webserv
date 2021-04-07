/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:49:49 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/07 19:20:52 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_PARSER_HPP
# define CONF_PARSER_HPP

# include "webserv.hpp"
# include "HttpBlock.hpp"

# define NB_BLOCKS		3
# define NB_HTTP_DIR		4
# define NB_SRV_DIR		8
# define NB_LOC_DIR		3

/*Blocks*/
# define HTTP		0
# define SERVER		1
# define LOCATION	2
// # define NOBLOCK	   -1

# define TRUE		0
# define FALSE		1
# define FAILURE	-1

# define IN_HTTP_BLOCK      0x00000001
# define IN_SRV_BLOCK       0x00000002
# define IN_LOC_BLOCK       0x00000004
# define XXXXXXXXXXX3       0x00000008
# define XXXXXXXXXXX4       0x00000010
# define XXXXXXXXXXX5       0x00000020
# define XXXXXXXXXXX6       0x00000040
# define XXXXXXXXXXX7       0x00000080


class ConfParser {

public:
// typedef	int	(ConfParser::*t_parse)(std::string&);
typedef std::map<std::string, int (ConfParser::*)(std::string&)>	dirMap;

private:
	const std::string			_configFile;
	HttpBlock					*_httpBlock; // will certainely be removed, ConfParser will be called from HttpBlock
	int							_block_type;
	size_t						_line_nb;
	size_t						_nbr_of_srv;
	size_t						_nbr_of_loc;
	std::bitset<3>				_in_block;
	bool						_semi_col_not_found;
	std::string					_curr_dir;
	std::vector<std::string>	*_dir_line;

	int		parseHttp(void);
	int		parseServer(void);
	int		parseLocation(void);
	int 	isHttpDirective(std::string& token) const;
	bool 	isSrvDirective(std::string& token) const;
	bool 	isLocDirective(std::string& token) const;

	int		setListen(std::string& token);
	int		setServerName(std::string& token);
	int		setRoot(std::string& token);
	int		setErrorPage(std::string& token);
	int		setTimeout(std::string& token);
	int		setMaxBdySize(std::string& token);
	int		setCgiParam(std::string& token);
	int		setCgiPass(std::string& token);
	int		setAllowedMethods(std::string& token);
	int		setIndex(std::string& token);
	int		setAutoIndex(std::string& token);

	
	static	std::string	http_dir[NB_HTTP_DIR];
	static	std::string	srv_dir[NB_SRV_DIR];
	static	std::string	loc_dir[NB_LOC_DIR];

	static	dirMap	setHttpMap();
	static	dirMap	setSrvMap();
	static	dirMap	setLocMap();
	
	static	dirMap	http_map;
	static	dirMap	srv_map;
	static	dirMap	loc_map;

public:
	ConfParser(void);
	ConfParser(std::string filename);
	ConfParser(ConfParser const & copy);
	~ConfParser(void);
	ConfParser& operator=(ConfParser const & rhs);

	int	 readConfFile(void);
	int parseLine(std::string& line);
	void setDirective(std::string &);

	
	/*Excetpions*/
	
	class UnexpectedTocken : public std::exception {

	private:
		std::string _msg;
	public:
		UnexpectedTocken(const std::string token, const size_t line_nb);
		virtual ~UnexpectedTocken() throw() {};
		virtual const char* what() const throw();
	};

	class NoOpeningBracket : public std::exception {

	private:
		std::string _msg;
	public:
		NoOpeningBracket(const std::string token, const size_t line_nb);
		virtual ~NoOpeningBracket() throw() {};
		virtual const char* what() const throw();
	};

	class NoClosingBracket : public std::exception {

	private:
		std::string _msg;
	public:
		NoClosingBracket(const std::string token, const size_t line_nb);
		virtual ~NoClosingBracket() throw() {};
		virtual const char* what() const throw();
	};

	class UnknownDirective : public std::exception {

	private:
		std::string _msg;
	public:
		UnknownDirective(const std::string token, const size_t line_nb);
		virtual ~UnknownDirective() throw() {};
		virtual const char* what() const throw();
	};

};

std::ostream & operator<<(std::ostream & o, ConfParser const & rhs);

#endif // CONF_PARSER_HPP
