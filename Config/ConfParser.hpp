/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:49:49 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/15 15:43:37 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_PARSER_HPP
# define CONF_PARSER_HPP

# include "webserv.hpp"
# include "HttpBlock.hpp"
# include "utils.hpp"

# define NB_BLOCKS		3
# define NB_HTTP_DIR	4
# define NB_SRV_DIR		8
# define NB_LOC_DIR		3

/*Blocks*/
# define HTTP		0
# define SERVER		1
# define LOCATION	2

class ConfParser {

public:
// typedef	int	(ConfParser::*t_parse)(std::string&);
typedef std::map<std::string, int (ConfParser::*)(void)>	dirMap;

private:
	const std::string			_configFile;
	HttpBlock					_httpBlock; // will certainely be removed, ConfParser will be called from HttpBlock
	int							_block_type;
	size_t						_line_nb;
	size_t						_nbr_of_srv;
	size_t						_nbr_of_loc;
	std::bitset<3>				_in_block;
	bool						_semi_col_not_found;
	std::string					_curr_dir;
	std::vector<std::string>	_dir_line;

	void	parseDirective(void);
	void	handleBlockIn(const std::string&);
	void	handleBlockOut(void);

	// Directive handling functions	
	int		setListen();
	int		setServerName();
	int		setRoot();
	int		setAutoIndex();
	int		setIndex();
	int		setErrorPage();
	int		setMaxBdySize();
	int		setKeepAlive();
	int		setAllowedMethods();
	int		setChunkEnc();
	int		setAuthBasic();
	int		setAuthBasicFile();
	int		setCgiParam();
	int		setCgiPass();

	// Store the location URI	
	void	setLocationPath();

	template <class Compare>
	void checkNbrOfArgs(size_t expected_nbr, Compare comp);

	
	// static	std::string	http_dir[NB_HTTP_DIR];
	// static	std::string	srv_dir[NB_SRV_DIR];
	// static	std::string	loc_dir[NB_LOC_DIR];

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

	void readConfFile(void);
	void parseLine(std::string& line);

	// Getters
	HttpBlock&		getHttpBlock(void);


	
	/*Exceptions*/
	
	class UnexpectedTocken : public std::exception {

	private:
		std::string _msg;
	public:
		UnexpectedTocken(const std::string token, ConfParser *);
		virtual ~UnexpectedTocken() throw() {};
		virtual const char* what() const throw();
	};

	class NoOpeningBracket : public std::exception {

	private:
		std::string _msg;
	public:
		NoOpeningBracket(const std::string token, ConfParser *);
		virtual ~NoOpeningBracket() throw() {};
		virtual const char* what() const throw();
	};

	class NoClosingBracket : public std::exception {

	private:
		std::string _msg;
	public:
		NoClosingBracket(const std::string token, ConfParser *);
		virtual ~NoClosingBracket() throw() {};
		virtual const char* what() const throw();
	};

	class UnexpectedEOF : public std::exception {

	private:
		std::string _msg;
	public:
		UnexpectedEOF(const std::string token, ConfParser *);
		virtual ~UnexpectedEOF() throw() {};
		virtual const char* what() const throw();
	};

	class UnknownDirective : public std::exception {

	private:
		std::string _msg;
	public:
		UnknownDirective(const std::string token, ConfParser *);
		virtual ~UnknownDirective() throw() {};
		virtual const char* what() const throw();
	};

	class InvalidNbrOfArgs : public std::exception {

	private:
		std::string _msg;
	public:
		InvalidNbrOfArgs(const std::string token, ConfParser *);
		virtual ~InvalidNbrOfArgs() throw() {};
		virtual const char* what() const throw();
	};

	class InvalidValue : public std::exception {

	private:
		std::string _msg;
	public:
		InvalidValue(const std::string token, ConfParser *);
		virtual ~InvalidValue() throw() {};
		virtual const char* what() const throw();
	};

	class CannotOpenFile : public std::exception {

	private:
		std::string _msg;
	public:
		CannotOpenFile(ConfParser *);
		virtual ~CannotOpenFile() throw() {};
		virtual const char* what() const throw();
	};

	class UnknownHost : public std::exception {

	private:
		std::string _msg;
	public:
		UnknownHost(const std::string token, ConfParser *);
		virtual ~UnknownHost() throw() {};
		virtual const char* what() const throw();
	};

	class InvalidPort : public std::exception {

	private:
		std::string _msg;
	public:
		InvalidPort(const std::string token, ConfParser *);
		virtual ~InvalidPort() throw() {};
		virtual const char* what() const throw();
	};

};

std::ostream & operator<<(std::ostream & o, ConfParser const & rhs);

#endif // CONF_PARSER_HPP
