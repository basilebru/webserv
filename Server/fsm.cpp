#include "fsm.hpp"

int			lexer(Request &req)
{
	t_fsm			machine;
    conf            conf;
    std::string     response_buf;
	static t_func	func[NB_STATE] = {fsm_config, fsm_auth, fsm_generate, fsm_upstream, fsm_error, fsm_writter};
	int				ret;

    machine.response_sent = false;
	while (machine.response_sent == false)
	{
		// chose_state(buf, &machine, i);
		ret = func[machine.state](machine, req, conf, response_buf);
		if (ret == FAILURE)
			return (FAILURE);
	}
	return (SUCCESS);
}

int fsm_config(t_fsm& machine, Request& req, conf& conf, std::string &response_buf)
{
    // get conf from Request
    (void)req;
    (void)response_buf;
    conf.max_body_size = 0;
    conf.allow_methods.push_back("GET");
    conf.index.push_back("index");
    conf.index.push_back("index.html");
    conf.root = "html";
    conf.autoindex = false;

    // set req.target_uri
    // (faire en sorte qu'il n'y ait pas de '/' a la fin de conf.root)
    req.target_uri = conf.root + req.req_line.target;

    machine.state = AUTH;
}

int fsm_auth(t_fsm& machine, Request& req, conf& conf, std::string &response_buf)
{
    (void)req;
    (void)conf;
    (void)response_buf;
    // check conf authentification attribute to see if auth needed. Do authentification if needed

    machine.state = UPSTREAM;
}

int fsm_upstream(t_fsm& machine, Request& req, conf& conf, std::string &response_buf)
{
    (void)req;
    (void)conf;
    (void)response_buf;
    // check conf cgi attribute -> send content to cgi and receive content back

    machine.state = GENERATE;
}

bool trailing_slash(std::string uri)
{
    if (uri[uri.size() - 1] == '/')
        return true;
    return false;
}

int try_index(Request &req, conf &conf)
{
    struct stat buf;
    
    for (std::vector<std::string>::iterator it = conf.index.begin(); it != conf.index.end(); it++)
    {
        // check if file exist
        if (stat(it->c_str(), &buf) == 0)
        {
            // if exists, internal redirect to this file
            req.req_line.target = *it;
            return SUCCESS;
        }
        return FAILURE;
    }
}

int fsm_generate(t_fsm& machine, Request& req, conf& conf, std::string &response_buf)
{
    int ret;

    if (trailing_slash(req.target_uri))
    {
        if (!conf.index.empty())
        {
            ret = try_index(req, conf);
            if (ret == SUCCESS)
            {
                machine.state = CONFIG;
                return SUCCESS;
            }
        if (conf.autoindex)
        {
            std::cout << "generating autoindex" << std::endl;
            // generate auto-index
            machine.state = WRITTER;
            return SUCCESS;
        }
        req.error_code = 404;
        machine.state = ERROR;
        return SUCCESS;
    }    

    // "generate" target_uri based on HTTP method

}

int fsm_error(t_fsm& machine, Request& req, conf& conf, std::string &response_buf)
{
    // internal redirect if error_page corresponding to error
    std::map<int, std::string>::iterator it;
    if (it = conf.error_pages.find(req.error_code)) != conf.error_pages.end())
    {
        req.req_line.target = it->second;
        // if other method that GET/HEAD, change method to GET
        machine.state = CONFIG;
        return SUCCESS;
    }

    std::cout << "generating error page" << std::endl;
    // generate default error response


}