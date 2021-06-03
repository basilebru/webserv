#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

# include "webserv.hpp"
# include "request_class.hpp"
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/socket.h>
# include <sys/un.h>

# define CGI_BUF_SIZE 1


class CgiHandler {

public:
	typedef std::map<std::string, std::string> stringMap;

private:
	char			**_envp;
	stringMap		_env_map;
	const Request&	_req;

	CgiHandler(void);
	void	initEnv(void);

public:
	CgiHandler(Request const& req);
	CgiHandler(CgiHandler const & copy);
	~CgiHandler(void);
	CgiHandler& operator=(CgiHandler const & rhs);

	std::vector<unsigned char>	execScript(std::string const& scriptName);
	void						fillEnvp(void);

};

std::ostream & operator<<(std::ostream & o, CgiHandler const & rhs);

#endif // CGI_HANDLER_HPP
