#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

# include "webserv.hpp"
# include "Response.hpp"
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/socket.h>
# include <sys/un.h>

# define CGI_BUF_SIZE 65536


class CgiHandler {

public:
	typedef std::map<std::string, std::string> stringMap;

private:
	char						**_envp;
	stringMap					_env_map;
	const Request&				_req;
	const Response&				_res;
	std::string					_headers;
	std::vector<unsigned char>	_body;
	bool						_hasCL;
	bool						_hasCT;
	bool						_hasRedir;
	std::string					_status;

	CgiHandler(void);
	void	initEnv(void);
	void	fillEnvp(void);
	void	storeBuffer(std::vector<unsigned char>&, const char *buf, int len);
	void	fillOutputs(std::vector<unsigned char>&);
	void	replaceLF(void);
	void	flagHeaders(void);



public:
	CgiHandler(Request const& req, Response const& res);
	CgiHandler(CgiHandler const & copy);
	~CgiHandler(void);
	CgiHandler& operator=(CgiHandler const & rhs);

	int	execScript(std::string const& extension);
	
	/* Getters */
	std::string&				getHeaders(void);
	std::vector<unsigned char>&	getBody(void);
	bool&						getHasContentLength(void);
	bool&						getHasContentType(void);
	bool&						getHasRedir(void);
	std::string&				getStatus(void);

};

std::ostream & operator<<(std::ostream & o, CgiHandler const & rhs);

#endif // CGI_HANDLER_HPP
