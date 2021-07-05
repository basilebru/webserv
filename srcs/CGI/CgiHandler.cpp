#include "CgiHandler.hpp"

CgiHandler::CgiHandler(Request const& req, Response const& res) :
_envp(NULL), _req(req), _res(res), _hasCL(false), _hasCT(false), _hasRedir(false)
{
	// std::cout << "CGI CONSTRUCTOR" << std::endl;
	this->initEnv();
}

CgiHandler::CgiHandler(CgiHandler const & copy) :
_envp(copy._envp), _req(copy._req), _res(copy._res),
_headers(copy._headers), _body(copy._body), _hasCL(copy._hasCL),
_hasCT(copy._hasCT), _hasRedir(copy._hasRedir), _status(copy._status)
{}

CgiHandler::~CgiHandler(void)
{
	// std::cout << "CGI DESTRUCTOR" << std::endl;
	for (size_t i = 0; i < this->_env_map.size(); ++i)
		delete[] this->_envp[i];
	delete[] this->_envp;
}

CgiHandler& CgiHandler::operator=(CgiHandler const & rhs)
{
	this->_envp = rhs._envp;
	// this->_req = rhs._req;
	// this->_res = rhs._res;
	this->_headers = rhs._headers;
	this->_body = rhs._body;
	this->_hasCL = rhs._hasCL;
	this->_hasCT = rhs._hasCT;
	this->_hasRedir = rhs._hasRedir;
	this->_status = rhs._status;

	return *this;
}

void displayMap(std::map<std::string, std::string> const& map, char const separator = '\n')
{
	std::map<std::string, std::string>::const_iterator it = map.begin();
	while (it != map.end())
	{
		std::cout << it->first << " = " << it->second << separator;
		++it;
	}
	std::cout << std::endl;
}

void	CgiHandler::initEnv(void)
{
	/* Les variables d'environnement permettent au script d'accéder à des informations
	relatives au serveur, au client, à la requête. */

	std::map<std::string, std::string> headers = this->_req.get_headers();
	
	this->_env_map["REDIRECT_STATUS"]	=	"200"; //for php-cgi
	this->_env_map["CONTENT_LENGTH"]	=	iToString(this->_req.body_size);	// content-length de la requete
	this->_env_map["CONTENT_TYPE"]		=	headers["content-type"];	// content-type de la requete (POST)
	this->_env_map["GATEWAY_INTERFACE"]	=	"CGI/1.1";	// version du CGI qu'utilise le server
	this->_env_map["PATH_INFO"]			=	this->_req.req_line.target;	// derniere partie de l'URI apres le host
	this->_env_map["PATH_TRANSLATED"]	=	this->_res.getTarget();	// adresse reelle du script (idem PATH_INFO pour nous)
	this->_env_map["QUERY_STRING"]		=	this->_req.req_line.query_string;	// Contient tout ce qui suit le « ? » dans l'URL envoyée par le client.
	this->_env_map["REMOTE_ADDR"]		=	this->_req.host_uri;;	// adress ip du client
	this->_env_map["REQUEST_METHOD"]	=	this->_req.req_line.method;	// GET ou POST ou ...
	this->_env_map["REQUEST_URI"]		=	this->_req.req_line.target; // --> For the 42 tester
	this->_env_map["SCRIPT_NAME"]		=	this->_res.getTarget();	// full path du fichier de script
	this->_env_map["SERVER_NAME"]		=	this->_req.host_uri;	// DNS ou IP du server (hostname)
	this->_env_map["SERVER_PORT"]		=	this->_req.host_port;	// port ayant reçu la requête
	this->_env_map["SERVER_PROTOCOL"]	=	this->_req.req_line.version;;	// protocol HTTP (toujours HTTP/1.1 ?)
	this->_env_map["SERVER_SOFTWARE"]	=	"webserv";
	// this->_env_map["UPLOAD_DIR"]		=	this->_req.config.upload_dir;
 // 	this->_env_map["HTTP_CONTENT_TYPE"] =  "test/file";
	// this->_env_map["HTTP_ACCEPT_ENCODING"] =  "gzip";
 // 	this->_env_map["HTTP_HOST"] =  "127.0.0.1:9999";
 // 	this->_env_map["HTTP_TRANSFER_ENCODING"] =  "chunked";
 // 	this->_env_map["HTTP_USER_AGENT"] =  "Go-http-client/1.1";
 	this->_env_map["HTTP_X_SECRET_HEADER_FOR_TEST"] =  "1";

	// displayMap(this->_env_map);

}

void	CgiHandler::fillEnvp(void)
{
	this->_envp = new char*[this->_env_map.size() + 1];
	
	stringMap::const_iterator it = this->_env_map.begin();

	std::string str;
	size_t i = 0;
	while(i < this->_env_map.size())
	{
		str = it->first + '=' + it->second;
		this->_envp[i] = new char[str.size() + 1];
		strcpy(this->_envp[i], str.c_str());
		str.clear();
		++it;
		++i;
	}
	this->_envp[i] = NULL;
}

void	CgiHandler::storeBuffer(std::vector<unsigned char> &body, const char *buf, int len)
{
	// std::cout << "ENTER in STORE: " << std::endl;
	int i = 0;

	if (len < CGI_BUF_SIZE && len != 0)
		len++;
	// std::cout << "len: " << len << std::endl;
	while(i < len)
	{
		body.push_back(buf[i++]);
	}
	// std::cout << "OUT OFF STORE: " << std::endl;
}

void	CgiHandler::replaceLF(void)
{
	size_t pos = 0;

	while ((pos = this->_headers.find_first_of("\n", pos)) != std::string::npos)
	{
		this->_headers.replace(pos, 1, "\r\n");
		pos += 2;
	}
}

void	CgiHandler::flagHeaders(void)
{
	size_t pos = 0;
	std::string	upper;

	transform(this->_headers.begin(), this->_headers.end(), std::back_inserter(upper), toupper);
	// std::cerr << "UPPER: " << upper << std::endl;

	if (upper.find("CONTENT-LENGTH") != std::string::npos)
	{
		// std::cerr << "FOUND CONTENT LENGTH" << std::endl;
		this->_hasCL = true;
	}
	if (upper.find("CONTENT-TYPE") != std::string::npos)
	{
		// std::cerr << "FOUND CONTENT TYPE" << std::endl;
		this->_hasCT = true;
	}
	if (upper.find("LOCATION") != std::string::npos)
	{
		// std::cerr << "FOUND REDIRECTION" << std::endl;
		this->_hasRedir = true;
	}
	if ((pos = upper.find("STATUS")) != std::string::npos)
	{
		// std::cerr << "FOUND STATUS" << std::endl;
		this->_status = this->_headers.substr(pos + 7, upper.find("\n", pos) - pos - 7);
	}
}

void	CgiHandler::fillOutputs(std::vector<unsigned char>& buffer)
{
	// std::cout << "ENTER IN FILL OUTPUT" << std::endl;
	size_t		i = 0;
	int			count = 0;
	std::vector<unsigned char>::iterator it = buffer.begin();
	while(i < buffer.size())
	{
		this->_headers.push_back(buffer[i]);
		if (buffer[i] == '\n')
			++count;
		if (count == 2)
		{
			if (this->_headers.find("\r\n\r\n") != std::string::npos
				|| this->_headers.find("\n\n") != std::string::npos)
				break ;
			--count;
		}
		++i;
		++it;
	}
	// replaceLF();
	flagHeaders();
	if (it != buffer.end())
		this->_body.assign(++it, --buffer.end());

	std::cerr << "BDY-SIZE: " << this->_body.size() << std::endl;
	// std::cout << "this->headers: " << this->_headers << std::endl;
	// if (this->_body.size() < 1000000)
	// 	displayVec(this->_body);

}

/**
 * EXEC SCRIPT WITH COMMUNICATION BY PIPE + FILE
 *
 * @param       [std::string]
 * @return      [int]
 */

int	CgiHandler::execScript(std::string const& cgi_path)
{
	/* Le script prend des données en entrée et écrit son resultat dans STDOUT.
	Dans le cas de GET, les données d'entrées sont dans la var d'env QUERY_STRING,
	Dans le cas de POST, les données sont lues depuis STDIN (depuis le body de la requete).
	Comme le scrit écrit dans stdout, il faut lire stdout et l'enregistrer dans une variable,
	variable qui sera retournée par la fonction execScript() et utilsée pour contruire le bdy de la réponse.
	*/

	std::cout << "ENTER IN CGI HANDLER" << std::endl;

	std::vector<unsigned char>	body;
	char	buf[CGI_BUF_SIZE];
	int		ret = CGI_BUF_SIZE;
	int		status;
	int		cgi_fd;
	int		srvToCgi_fd[2]; // Pipe Server --> CGI

	this->fillEnvp();

	int srvToCgi_fd[2]; // Pipe Server --> CGI
	cgi_fd = open("/tmp/cgi_file", O_RDWR | O_CREAT | O_APPEND, 0666);
	if (cgi_fd == -1)
	{
		std::cout << "open error" << std::endl;
		return FAILURE;
	}
	if (pipe(srvToCgi_fd) == -1)
	{
		std::cerr << "pipe() srvToCgi failed, errno: " << errno << std::endl;
		return FAILURE;
	}

	int pid = fork();
	if (pid == -1)
	{
		std::cerr << "fork process failed" << std::endl;
		return FAILURE;
	}
	else if (pid == 0)
	{
		close(srvToCgi_fd[1]);  /* Ferme l'extrémité d'ecriture inutilisée */
		dup2(cgi_fd, STDOUT_FILENO);
		dup2(srvToCgi_fd[0], STDIN_FILENO);
		// dup2(cgi_fd, STDERR_FILENO); --> Not necessary
		close(cgi_fd);
		close(srvToCgi_fd[0]);   //Ferme l'extrémité de lecture après utilisation par le fils 

		char * argv[3] = {
			const_cast<char*>(cgi_path.c_str()),
			const_cast<char*>(this->_res.getTarget().c_str()),
			(char *)0
		};
		if (execve(argv[0], &argv[0], this->_envp) < 0) /* Le script écrit dans STDOUT */
		{
			std::cerr << "execve() failed, errno: " << errno << " - " << strerror(errno) << std::endl;
			close(srvToCgi_fd[0]);  /* Ferme l'extrémité de lecture après utilisation par le fils */
			_exit(1);
		}
	}
	else
	{
		close(srvToCgi_fd[0]);  /* Ferme l'extrémité de lecture inutilisée */

		if (!this->_req.body.empty())
		{
			// std::cout << "WRITE....." << std::endl;
			write(srvToCgi_fd[1], &this->_req.body[0], this->_req.body.size());
			// std::cout << "WRITE AFTER....." << std::endl;
		}

		close(srvToCgi_fd[1]);  /* Ferme l'extrémité d'éciture après utilisation par le père */

		if (waitpid(pid, &status, 0) == -1)
			return FAILURE;
		lseek(cgi_fd, 0, SEEK_SET); // reposition file offset at begining
		while (ret == CGI_BUF_SIZE)
		{
			// std::cout << "READ..." << std::endl;
			memset(buf, 0, CGI_BUF_SIZE);
			if ((ret = read(cgi_fd, buf, CGI_BUF_SIZE)) < 0)
				return FAILURE;
			// std::cout << "ret: " << ret << std::endl;
			this->storeBuffer(body, buf, ret);
		}
		if (!body.empty())
			fillOutputs(body);

		close(cgi_fd);

		if (WIFEXITED(status))
		{
			if (WEXITSTATUS(status) == 1)
				return FAILURE;
		}
	}
	std::cout << "OUT OF CGI HANDLER" << std::endl;
	return SUCCESS;
}

/* Getters */

std::string&                CgiHandler::getHeaders(void)
{
	return this->_headers;
}

std::vector<unsigned char>& CgiHandler::getBody(void)
{
	return this->_body;
}

bool&						CgiHandler::getHasContentLength(void)
{
	return this->_hasCL;
}

bool&						CgiHandler::getHasContentType(void)
{
	return this->_hasCT;
}

bool&						CgiHandler::getHasRedir(void)
{
	return this->_hasRedir;
}

std::string&				CgiHandler::getStatus(void)
{
	return this->_status;
}
