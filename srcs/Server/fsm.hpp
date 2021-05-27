#include "Request/request_class.hpp"
#include "Config/ConfParser.hpp"
#include <map>

#include <sys/types.h> // fstat
#include <sys/stat.h> // fstat
#include <unistd.h> // fstat



#define NB_STATE 6
#define FAILURE -1
#define SUCCESS 0
#define DONE 1
#define NOT_SET -1

struct conf
{
        size_t max_body_size;
        std::vector<std::string> allow_methods;
        std::string root;
        std::vector<std::string> index;
        int autoindex;
        errorMap error_pages;

        conf(void): max_body_size(NOT_SET), autoindex(NOT_SET)
        {};
        // authentification attribute
        // cgi attributes
};


enum	e_state
{
	CONFIG,
	AUTH,
	GENERATE,
	UPSTREAM,
	ERROR,
    WRITTER
};

typedef struct	s_fsm
{
	enum e_state	state;
	bool			response_sent; // flag
	bool			flag_def; // flag
}				t_fsm;

typedef	int	(*t_func)(t_fsm&, Request&, conf&, std::string &response_buf);

int fsm_config(t_fsm&, Request&, conf&, std::string&);
int fsm_auth(t_fsm&, Request&, conf&, std::string&);
int fsm_generate(t_fsm&, Request&, conf&, std::string&);
int fsm_upstream(t_fsm&, Request&, conf&, std::string&);
int fsm_error(t_fsm&, Request&, conf&, std::string&);
int fsm_writter(t_fsm&, Request&, conf&, std::string&);