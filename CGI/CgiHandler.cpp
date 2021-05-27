#include "CgiHandler.hpp"
#include <sys/types.h>
#include <sys/wait.h>

#define BUF_SIZE 32

CgiHandler::CgiHandler(void) : _envp(NULL)
{
	std::cout << "CGI CTOR" << std::endl;
	this->initEnv();
}

/*CgiHandler::CgiHandler(CgiHandler const & copy)
{

}
*/

CgiHandler::~CgiHandler(void)
{
	std::cout << "CGI DESTRUCTOR" << std::endl;
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
	this->_env_map["CONTENT_LENGTH"]	=	"value";	// content-length de la requete
	this->_env_map["CONTENT_TYPE"]		=	"value";	// content-type de la requete
	this->_env_map["GATEWAY_INTERFACE"]	=	"CGI/1.1";	// version du CGI qu'utilise le server
	this->_env_map["PATH_INFO"]			=	"value";	// derniere partie de l'URI apres le script name
	this->_env_map["PATH_TRANSLATED"]	=	"value";	// adresse reelle du script (idem PATH_INFO pour nous)
	this->_env_map["QUERY_STRING"]		=	"value";	// Contient tout ce qui suit le « ? » dans l'URL envoyée par le client.
	this->_env_map["REMOTE_ADDR"]		=	"value";	// adress ip du client
	this->_env_map["REMOTE_IDENT"]		=	"value";	// nom d'utilisateur du client
	this->_env_map["REMOTE_USER"]		=	"value";	// nom d'utilisateur (distant) du client
	this->_env_map["REQUEST_METHOD"]	=	"value";	// GET ou POST ou ...
	this->_env_map["REQUEST_URI"]		=	"value";	// URI
	this->_env_map["SCRIPT_NAME"]		=	"value";	// full path du fichier de script
	this->_env_map["SERVER_NAME"]		=	"value";	// DNS ou IP du server (hostname)
	this->_env_map["SERVER_PORT"]		=	"value";	// port ayant reçu la requête
	this->_env_map["SERVER_PROTOCOL"]	=	"HTTP/1.1";	// protocol HTTP (toujours HTTP/1.1 ?)
	this->_env_map["SERVER_SOFTWARE"]	=	"webserv";

}


void	CgiHandler::fillEnvp(void)
{
	this->_envp = new char*[this->_env_map.size()];
	
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
}

std::string	CgiHandler::execScript(std::string const& scriptName)
{
	/* Le script prend des données en entrée et écrit son resultat dans STDOUT.
	Dans le cas de GET, les données d'entrées sont dans la var d'env QUERY_STRING,
	Dans le cas de POST, les données sont lues depuis STDIN (depuis le body de la requete)

	Comme le scrit écrit dans stdout, il faut lire stdout et l'enregistrer dans une variable,
	variable qui sera retournée par la fonction execScript() et utilsée pour contruire le bdy de la réponse.
	
	utilisation de dup, dup2, waitpid, lseek ?
	*/

	std::string body;
	char buf[BUF_SIZE];
	int ret = 1;


	this->fillEnvp();

	int pipefd[2];

	if (pipe(pipefd) == -1)
	{
		std::cerr << "pipe() failed, errno: " << errno << std::endl;
		return "";
	}

	int pid = fork();
	if (pid == -1)
	{
		std::cerr << "fork process failed" << std::endl;
		return "";
	}
	else if (pid == 0)
	{
		close(pipefd[0]);  /* Ferme l'extrémité de lecture inutilisée */
		dup2(pipefd[1], STDOUT_FILENO);
		
		char **argv = NULL; /* Le script écrit dans STDOUT */
		if (execve(scriptName.c_str(), argv, this->_envp) < 0)
		{
			std::cerr << "execve() failed, errno: " << errno << std::endl;
			return "";
		}
		close(pipefd[1]);  /* Ferme l'extrémité d'éciture après utilisation par le fils */
	}
	else
	{
		close(pipefd[1]);  /* Ferme l'extrémité d'écriture inutilisée */
		dup2(pipefd[0], STDIN_FILENO);
		while (ret > 0)
		{
			memset(buf, 0, BUF_SIZE);
			ret = read(STDIN_FILENO, buf, BUF_SIZE - 1);
			body += buf;
		}
		close(pipefd[0]);  /* Ferme l'extrémité de lecture après utilisation par le père */
		waitpid(pid, NULL, 0);
	}
	// return the body red from stdout
	return body;
}
