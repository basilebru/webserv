#include "CgiHandler.hpp"

CgiHandler::CgiHandler(Request const& req) : _envp(NULL), _req(req)
{
	// std::cout << "CGI CTOR" << std::endl;
	this->initEnv();
}

/*CgiHandler::CgiHandler(CgiHandler const & copy)
{

}
*/

CgiHandler::~CgiHandler(void)
{
	// std::cout << "CGI DESTRUCTOR" << std::endl;
	for (size_t i = 0; i < this->_env_map.size(); ++i)
		delete[] this->_envp[i];
	delete[] this->_envp;
}

/*CgiHandler::CgiHandler& operator=(CgiHandler const & rhs)
{

}
*/


void	CgiHandler::initEnv(void)
{
	/* Les variables d'environnement permettent au script d'accéder à des informations
	relatives au serveur, au client, à la requête. */

	this->_env_map["AUTH_TYPE"]			=	"value";	// mode d'authentification, auth_basic ??
	this->_env_map["CONTENT_LENGTH"]	=	iToString(this->_req.body_size);	// content-length de la requete
	this->_env_map["CONTENT_TYPE"]		=	"value";	// content-type de la requete
	this->_env_map["GATEWAY_INTERFACE"]	=	"CGI/1.1";	// version du CGI qu'utilise le server
	this->_env_map["PATH_INFO"]			=	"value";	// derniere partie de l'URI apres le script name
	this->_env_map["PATH_TRANSLATED"]	=	"value";	// adresse reelle du script (idem PATH_INFO pour nous)
	this->_env_map["QUERY_STRING"]		=	"value";	// Contient tout ce qui suit le « ? » dans l'URL envoyée par le client.
	this->_env_map["REMOTE_ADDR"]		=	"value";	// adress ip du client
	this->_env_map["REMOTE_IDENT"]		=	"value";	// nom d'utilisateur du client
	this->_env_map["REMOTE_USER"]		=	"value";	// nom d'utilisateur (distant) du client
	this->_env_map["REQUEST_METHOD"]	=	"POST";	// GET ou POST ou ...
	this->_env_map["REQUEST_URI"]		=	"value";	// URI
	this->_env_map["SCRIPT_NAME"]		=	"value";	// full path du fichier de script
	this->_env_map["SERVER_NAME"]		=	"value";	// DNS ou IP du server (hostname)
	this->_env_map["SERVER_PORT"]		=	"value";	// port ayant reçu la requête
	this->_env_map["SERVER_PROTOCOL"]	=	"HTTP/1.1";	// protocol HTTP (toujours HTTP/1.1 ?)
	this->_env_map["SERVER_SOFTWARE"]	=	"webserv";

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


void	CgiHandler::storeBuffer(std::vector<unsigned char> &body, const char *buf)
{
	size_t i = 0;
	while(i < CGI_BUF_SIZE)
	{
		body.push_back(buf[i++]);
	}
}

void	CgiHandler::fillOutputs(std::vector<unsigned char>& buffer)
{
	size_t		i = 0;
	size_t		pos = 0;
	int			count = 0;
	std::string	upper;

	while(i < buffer.size())
	{
	    this->_headers.push_back(buffer[i]);
	    if (buffer[i] == '\n')
	    	++count;
	    if (count == 2)
	    {
	    	transform(this->_headers.begin(), this->_headers.end(), std::back_inserter(upper), toupper);
	    	if (upper.find("CONTENT-TYPE") != std::string::npos)
	    	{
	    		if ((pos = this->_headers.find("\n\n")) != std::string::npos)
	    		{
	    			this->_headers.replace(pos, 2, "\r\n\r\n");
	    			break ;
	    		}
	    		if (this->_headers.find("\r\n\r\n") != std::string::npos)
	    			break ;
	    		--count;
	    	}
	    	else
	    		--count;
	    }

	    ++i;
	}
	++i;
	std::cerr << "HEADERS: " << this->_headers << std::endl;
	while(i < buffer.size() - 1)
	{
	    this->_body.push_back(buffer[i]);
	    i++;
	}
	std::cerr << "BDY-SIZE: " << this->_body.size() << std::endl;

}

/**
 * EXEC SCRIPT WITH COMMUNICATION BY PIPE
 *
 * @param       [param1, param2, ...]
 * @return      [type]
 */

int	CgiHandler::execScript(std::string const& scriptName)
{
	/* Le script prend des données en entrée et écrit son resultat dans STDOUT.
	Dans le cas de GET, les données d'entrées sont dans la var d'env QUERY_STRING,
	Dans le cas de POST, les données sont lues depuis STDIN (depuis le body de la requete)

	Comme le scrit écrit dans stdout, il faut lire stdout et l'enregistrer dans une variable,
	variable qui sera retournée par la fonction execScript() et utilsée pour contruire le bdy de la réponse.

	*/
	std::vector<unsigned char> body;
	std::vector<unsigned char[CGI_BUF_SIZE]> test;
	char buf[CGI_BUF_SIZE];
	int ret = 1;


	this->fillEnvp();

	int cgiToSrv_fd[2]; // Pipe Server <-- CGI
	int srvToCgi_fd[2]; // Pipe Server --> CGI

	if (pipe(cgiToSrv_fd) == -1)
	{
		std::cerr << "pipe() cgiToSrv failed, errno: " << errno << std::endl;
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
		close(cgiToSrv_fd[0]);  /* Ferme l'extrémité de lecture inutilisée */
		close(srvToCgi_fd[1]);  /* Ferme l'extrémité d'ecriture inutilisée */
		dup2(cgiToSrv_fd[1], STDOUT_FILENO);
		dup2(srvToCgi_fd[0], STDIN_FILENO);

		char * argv[2] = { const_cast<char*>(scriptName.c_str()), NULL };
		if (execve(scriptName.c_str(), &argv[0], this->_envp) < 0) /* Le script écrit dans STDOUT */
		{
			std::cerr << scriptName.c_str() << std::endl;
			std::cerr << "execve() failed, errno: " << errno << " - " << strerror(errno) << std::endl;
			return FAILURE;
		}
		close(cgiToSrv_fd[1]);  /* Ferme l'extrémité d'éciture après utilisation par le fils */
		close(srvToCgi_fd[0]);  /* Ferme l'extrémité de lecture après utilisation par le fils */
	}
	else
	{
		close(cgiToSrv_fd[1]);  /* Ferme l'extrémité d'écriture inutilisée */
		close(srvToCgi_fd[0]);  /* Ferme l'extrémité de lecture inutilisée */		
		
		write(STDOUT_FILENO, this->_req.body.c_str(), this->_req.body.size()); // /!\ _req.body ne devrait pas etre un std::string

		while (ret > 0)
		{
			memset(buf, 0, CGI_BUF_SIZE);
			ret = read(cgiToSrv_fd[0], buf, CGI_BUF_SIZE);
			this->storeBuffer(body, buf);
		}

		fillOutputs(body);

		close(cgiToSrv_fd[0]);  /* Ferme l'extrémité de lecture après utilisation par le père */
		close(srvToCgi_fd[1]);  /* Ferme l'extrémité d'éciture après utilisation par le père */
		waitpid(pid, NULL, 0);
	}
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


/**
 * EXEC SCRIPT WITH COMMUNICATION BY SOCKET
 *
 * @param       [param1, param2, ...]
 * @return      [type]
 */

// std::vector<unsigned char>	CgiHandler::execScript(std::string const& scriptName)
// {

// 	std::vector<unsigned char> body;
// 	char buf;
// 	int ret = 1;


// 	this->fillEnvp();

	
// 	sockaddr_un sockaddr;
// 	memset((char *)&sockaddr, 0, sizeof(sockaddr)); 
// 	sockaddr.sun_family = AF_LOCAL;
// 	strncpy(sockaddr.sun_path, "socket", sizeof(sockaddr.sun_path)-1);

// 	int pid = fork();
// 	if (pid == -1)
// 	{
// 		std::cerr << "fork process failed" << std::endl;
// 		return body;
// 	}
// 	else if (pid == 0)
// 	{
// 		usleep(300000); // mainly for debug
// 		int newSocket = socket(AF_LOCAL, SOCK_STREAM, 0);

// 		if (connect(newSocket, (struct sockaddr *)&sockaddr, sizeof(sockaddr_un)) < 0)
// 		{
// 			std::cout << "Failed to connect socket. errno: " << errno << std::endl;
// 			close(newSocket);
// 			return body;
// 		}


// 		dup2(newSocket, STDOUT_FILENO);
// 		char * argv[2] = { const_cast<char*>(scriptName.c_str()), NULL };
// 		if (execve(scriptName.c_str(), argv, this->_envp) < 0)
// 		{
// 			std::cerr << "execve() failed, errno: " << errno << std::endl;
// 			return body;
// 		}
// 		close(newSocket);
// 	}
// 	else
// 	{

// 		unlink("socket");
// 		int newSocket = socket(AF_LOCAL, SOCK_STREAM, 0);
// 		if (bind(newSocket, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
// 		{
// 			close(newSocket);
// 			unlink("socket");
// 			if (errno != EADDRINUSE)
// 			{
// 				std::cout << "Failed to bind" << ". errno: " << errno << std::endl;
// 				return body;
// 			}
// 		}
// 		if (listen(newSocket, 10) < 0)
// 		{
// 			std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
// 			close(newSocket);
// 			unlink("socket");
// 			return body;
// 		}
// 		int connection;
// 		if((connection = accept(newSocket, (struct sockaddr*)NULL, NULL)) < 0) {
// 			std::cerr << "Server: Connection cannot be accepted" << std::endl;
// 			return body;
// 		}
		
// 		// write(connection, "JE SUIS LA LIGNE DE TEST D'ENVOI D'UN BODY EN CAS DE POST", 57);
		
// 		// usleep(500000);
// 		while (ret > 0)
// 		{
// 			memset(&buf, 0, CGI_BUF_SIZE);
// 			ret = read(connection, &buf, CGI_BUF_SIZE);
// 			body.push_back(buf);
// 		}
// 		close(newSocket);
// 		close(connection);
// 		unlink("socket");
// 		waitpid(pid, NULL, 0);
// 	}
// 	return body;
// }
