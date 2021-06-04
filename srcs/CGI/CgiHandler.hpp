#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

# include "webserv.hpp"
# include "request_class.hpp"
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/socket.h>
# include <sys/un.h>

# define CGI_BUF_SIZE 32


class CgiHandler {

public:
	typedef std::map<std::string, std::string> stringMap;

private:
	char						**_envp;
	stringMap					_env_map;
	const Request&				_req;
	std::string					_headers;
	std::vector<unsigned char>	_body;

	CgiHandler(void);
	void	initEnv(void);
	void	fillEnvp(void);
	void	storeBuffer(std::vector<unsigned char>&, const char *buf);
	void	fillOutputs(std::vector<unsigned char>&);

public:
	CgiHandler(Request const& req);
	CgiHandler(CgiHandler const & copy);
	~CgiHandler(void);
	CgiHandler& operator=(CgiHandler const & rhs);

	int	execScript(std::string const& scriptName);
	
	/* Getters */
	std::string&				getHeaders(void);
	std::vector<unsigned char>&	getBody(void);

};

std::ostream & operator<<(std::ostream & o, CgiHandler const & rhs);

#endif // CGI_HANDLER_HPP
