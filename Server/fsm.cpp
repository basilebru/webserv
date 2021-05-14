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


#include "Config/ConfParser.hpp"

LocationBlock match_loc(std::string target_uri, LocMap locations)
{
    LocationBlock target_config;
    int len(target_uri.length());
	int ret;

	// ex: target_uri = /path ; loc1 = /patrick ; loc2 = /lol
	// first iteration in while: [/path] vs [/patr] and vs [/lol]
	// 2nd iteration in while: [/pat] vs [/pat] and vs [/lo] --> STOP: loc1 selected

	// ex: target_uri = /path ; loc1 = /path ; loc2 = /pathfull
	// first iteration: [/path] vs [path/] and vs [/path] --> STOP: compare vs loc1 and loc2 return 0. Yet loc1 is compared first, as it comes first in map order --> loc1 selected 

    while (1)
    {
		// std::cout << "---" << std::endl;
		// std::cout << "len: " << len << std::endl;
		// std::cout << "sub: " << target_uri.substr(0, len) << std::endl;

        for (LocMap::const_iterator it = locations.begin(); it != locations.end(); it++)
        {
			// std::cout << "path: " << it->first;
            if ((ret = target_uri.compare(0, len, it->first, 0, len)) == 0)
            {
                target_config = it->second;
                return target_config;
            }
			// std::cout << " - ret: " << ret << std::endl;
        }
        len--;
    }
}

int fsm_config(t_fsm& machine, Request& req, conf& conf, std::string &response_buf, ServerBlock server_config, HttpBlock base_config)
{

    // server_config = server block corresponding to the server socket that accepted the connection
    
    (void)response_buf;
    
    LocationBlock location_config(match_loc(req.req_line.target, server_config.getLocations()));

    conf.allow_methods = location_config.getLimitExcept();
    if (conf.allow_methods.empty())
        conf.allow_methods = server_config.getLimitExcept();
    if (conf.allow_methods.empty())
        conf.allow_methods = base_config.getLimitExcept();

    conf.autoindex = location_config.getAutoindex();
    if (conf.autoindex == NOT_SET)
        conf.autoindex = server_config.getAutoindex();
    if (conf.autoindex == NOT_SET)
        conf.autoindex = base_config.getAutoindex();

    conf.error_pages = location_config.getErrorPages();
    if (conf.error_pages.empty())
        conf.error_pages = server_config.getErrorPages();
    if (conf.error_pages.empty())
        conf.error_pages = base_config.getErrorPages();
    
    conf.index = location_config.getIndexes();
    if (conf.index.empty())
        conf.index = server_config.getIndexes();
    if (conf.index.empty())
        conf.index = base_config.getErrorPages();
    
    conf.max_body_size = location_config.getMaxBdySize();
    if (conf.max_body_size == NOT_SET)
        conf.max_body_size = server_config.getMaxBdySize();
    if (conf.max_body_size == NOT_SET)
        conf.max_body_size = base_config.getMaxBdySize();

    conf.root = location_config.getRoot();
    if (conf.root.empty())
        conf.root = server_config.getRoot();
    if (conf.root.empty())
        conf.root = base_config.getRoot();

    // get conf from Request
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

// int fsm_config(t_fsm& machine, Request& req, conf& conf, std::string &response_buf)
// {
//     // get conf from Request
//     (void)req;
//     (void)response_buf;
//     conf.max_body_size = 0;
//     conf.allow_methods.push_back("GET");
//     conf.index.push_back("index");
//     conf.index.push_back("index.html");
//     conf.root = "html";
//     conf.autoindex = false;

//     // set req.target_uri
//     // (faire en sorte qu'il n'y ait pas de '/' a la fin de conf.root)
//     req.target_uri = conf.root + req.req_line.target;

//     machine.state = AUTH;
// }

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