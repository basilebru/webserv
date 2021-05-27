#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

# include "webserv.hpp"

class CgiHandler {

public:
	typedef std::map<std::string, std::string> stringMap;

private:
	char		**_envp;
	stringMap	_env_map;

	void	initEnv(void);

public:
	CgiHandler(void);
	// CgiHandler(/* overload */);
	// CgiHandler(/* overload */);
	CgiHandler(CgiHandler const & copy);
	~CgiHandler(void);
	CgiHandler& operator=(CgiHandler const & rhs);

	std::string	execScript(std::string const& scriptName);
	void		fillEnvp(void);

};

std::ostream & operator<<(std::ostream & o, CgiHandler const & rhs);

#endif // CGI_HANDLER_HPP
