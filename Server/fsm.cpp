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

ServerBlock match_serv(Request req, std::vector<ServerBlock> servers)
{
    std::vector<ServerBlock> eligible_servers;
    // 1. evaluate IP and port
    // 1.a try exact match
    for (std::vector<ServerBlock>::iterator it = servers.begin(); it != servers.end(); it++)
    {
        if (it->getListenIP() == req.address.sin_addr.s_addr && it->getListenPort() == req.address.sin_port)
            eligible_servers.push_back(*it);
    }
    // 1.b if no exact match, try 0.0.0.0 match 
    if (eligible_servers.size() == 0)
    {
        for (std::vector<ServerBlock>::iterator it = servers.begin(); it != servers.end(); it++)
        {
            if (it->getListenIP() == 0 && it->getListenPort() == req.address.sin_port)
                eligible_servers.push_back(*it);
        }
    }
    // return chosen server if only one match
    if (eligible_servers.size() == 1)
        return eligible_servers[0];

    // 2. if multiple matchs, evaluate server_name
    // return first server_block that matches
    for (std::vector<ServerBlock>::iterator it = eligible_servers.begin(); it != eligible_servers.end(); it++)
    {
        if (std::find(it->getServerNames().begin(), it->getServerNames().end(), req.host_uri) != it->getServerNames().end())
            return *it;
    }
    // if no match, return first server_block on the list
    return eligible_servers[0];
}

LocationBlock match_loc(std::string target_uri, LocMap locations)
{
    while (target_uri.find('/') != std::string::npos)
    {
        target_uri = target_uri.substr(0, target_uri.find_last_of('/')); // cut target_uri at last '/'
        for (LocMap::const_iterator it = locations.begin(); it != locations.end(); it++)
        {
            if (target_uri.compare(it->first) == 0)
                return it->second;
        }
    }
    // if no match, return "empty" location block
    LocationBlock loc;
    return loc;
}

int fsm_config(t_fsm& machine, Request& req, conf& conf, std::string &response_buf, std::vector<ServerBlock> servers, HttpBlock base_config)
{

    // server_config = server block corresponding to the server socket that accepted the connection
    
    (void)response_buf;
    ServerBlock server_config(match_serv(req, servers));
    
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
        conf.index = base_config.getIndexes();
    
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