#ifndef CONF_PARSER_HPP
# define CONF_PARSER_HPP

# include "webserv.hpp"
# include "utils.hpp"
# include "HttpBlock.hpp"
# include "ServerBlock.hpp"

/*Blocks*/
# define HTTP			'h'
# define SERVER			's'
# define LOCATION		'l'
# define NOBLOCK		'n'

class ConfParser {

typedef std::map<std::string, int (ConfParser::*)(void)>	dirMap;

private:
	std::string					_configFile;
	HttpBlock					_httpBlock; // will certainely be removed, ConfParser will be called from HttpBlock
	char						_block_type;
	size_t						_line_nb;
	std::vector<std::string>	_dir_line;
	LocationBlock				*_curr_location;
	std::vector<ServerBlock>	_servers;  /*Plusieurs ServerBlocks possibles dans l'HttpBlock */

	void	parseDirective(void);
	void	handleBlockIn(const std::string&);
	void	handleBlockOut(void);
	void	parseLine(std::string& line);
	void	sanitizeServers(void);
	void	checkDuplicateServer(std::vector<ServerBlock>::iterator);

	// Directive handling functions	
	int		setListen(void);
	int		setServerName(void);
	int		setRoot(void);
	int		setAutoIndex(void);
	int		setIndex(void);
	int		setErrorPage(void);
	int		setMaxBdySize(void);
	int		setKeepAlive(void);
	int		setAllowedMethods(void);
	int		setChunkEnc(void);
	int		setAuthBasic(void);
	int		setAuthBasicFile(void);
	int		setCgiAllowedExt(void);
	int		setCgiPath(void);
	int		setCgiExtensions(void);
	int		setReturn(void);
	int		setUploadDir(void);
	int		parseInclude(void);

	// Utility functions
	template <class Compare>
	void	checkNbrOfArgs(size_t expected_nbr, Compare comp);
	void	eraseComments(std::string& line);
	void	eraseSemiColon(void);


	// Static attributs to store allowed directives by config block
	static	dirMap	setHttpMap();
	static	dirMap	setSrvMap();
	static	dirMap	setLocMap();
	
	static	dirMap	http_map;
	static	dirMap	srv_map;
	static	dirMap	loc_map;

public:
	ConfParser(void);
	ConfParser(const std::string filename);
	ConfParser(ConfParser const & copy);
	~ConfParser(void);
	ConfParser& operator=(ConfParser const & rhs);

	void readConfFile(const std::string& confFile);

	// Getters
	HttpBlock&						getHttpBlock(void);
	const std::vector<ServerBlock>&	getServers(void) const;


	
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

	class FileOperationFail : public std::exception {

	private:
		std::string _msg;
	public:
		FileOperationFail(const std::string, ConfParser *);
		virtual ~FileOperationFail() throw() {};
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

	class DuplicateLocation : public std::exception {

	private:
		std::string _msg;
	public:
		DuplicateLocation(const std::string token, ConfParser *);
		virtual ~DuplicateLocation() throw() {};
		virtual const char* what() const throw();
	};

	class DuplicateDirective : public std::exception {

	private:
		std::string _msg;
	public:
		DuplicateDirective(const std::string token, ConfParser *);
		virtual ~DuplicateDirective() throw() {};
		virtual const char* what() const throw();
	};

};

std::ostream & operator<<(std::ostream & o, ConfParser const & rhs);

#endif // CONF_PARSER_HPP
