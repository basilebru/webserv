/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:49:49 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/06 17:36:58 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_PARSER_HPP
# define CONF_PARSER_HPP

# include "webserv.hpp"
# include "HttpBlock.hpp"

# define NB_BLOCKS		3
# define NB_HTTP_DIR		4
# define NB_SRV_DIR		8
# define NB_LOC_DIR		2

/*Blocks*/
# define HTTP		0
# define SERVER		1
# define LOCATION	2
// # define NOBLOCK	   -1

# define TRUE		0
# define FALSE		1
# define FAILIURE	-1


class ConfParser {

private:
	const std::string		_configFile;
	HttpBlock				*_httpBlock; // will certainely be removed, ConfParser will be called from HttpBlock
	int						_block_type;
	size_t					_line_nb;
	size_t					_nbr_of_srv;
	size_t					_nbr_of_loc;
	bool					_in_http_block;
	bool					_semi_col_not_found;
	int						_curr_dir;

	// void	identify_block(std::string& token);
	int		parseHttp(std::string& token);
	int		parseServer(std::string& token);
	int		parseLocation(std::string& token);
	int 	isHttpDirective(std::string& token) const;
	bool 	isSrvDirective(std::string& token) const;
	bool 	isLocDirective(std::string& token) const;

	
	static	std::string	http_dir[NB_HTTP_DIR];
	static	std::string	srv_dir[NB_SRV_DIR];
	static	std::string	loc_dir[NB_LOC_DIR];

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
