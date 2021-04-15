/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBlock.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 16:29:31 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/15 16:47:32 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_BLOCK_HPP
# define HTTP_BLOCK_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>
# include "ServerBlock.hpp"
# include "webserv.hpp"
# include "utils.hpp"

class HttpBlock {

	typedef size_t								size_type;
	typedef std::vector<std::string>::iterator	strVecIterator;

private:
	std::string						_root;
	bool							_autoindex; // on | off
	std::vector<std::string>		_indexes;
	std::vector<std::string>		_limit_except; //ou allow_methods ?
	std::map<int, std::string>		_error_pages; /* num error, error file path */
	size_type						_client_max_body_size;
	size_type						_keepalive_timeout;
	bool							_chunked_transfer_encoding; // on | off
	std::string						_auth_basic; // string | off
	std::string						_auth_basic_user_file; //httpasswd paths
	std::vector<ServerBlock>		_servers;  /*Plusieurs ServerBlocks possibles dans l'HttpBlock */

public:
	HttpBlock(void);
	HttpBlock(HttpBlock const & copy);
	~HttpBlock(void);
	HttpBlock& operator=(HttpBlock const & rhs);

	// Setters
	void	setRoot(std::string path);
	void	setAutoIndex(std::string& state);
	void	setIndexes(strVecIterator first, strVecIterator last);
	void	setLimitExcept(strVecIterator first, strVecIterator last);
	void	setErrorPages(strVecIterator first, strVecIterator last, std::string& val);
	void	setMaxBdySize(size_type size);
	void	setKeepaliveTimeout(size_type timeout);
	void	setChunkedEncoding(std::string& state);
	void	setAuthBasic(std::string path);
	void	setAuthBasicFile(std::string path);
	// void	setServers(std::string index);

	// Getters
	const std::string&		getRoot(void) const;
	const bool&				getAutoindex(void) const;
	const stringVec&		getIndexes(void) const;
	const stringVec&		getLimitExcept(void) const;
	const errorMap&			getErrorPages(void) const;
	const size_type&		getMaxBdySize(void) const;
	const size_type&		getKeepaliveTime(void) const;
	const bool&				getChunkedEncoding(void) const;
	const std::string&		getAuthBasic(void) const;
	const std::string&		getAuthBasicFile(void) const;

	const std::vector<ServerBlock>& getServers() const;
	ServerBlock&	getLastServer(void);

	
	void	addServer();
	

	ServerBlock& selectServer(std::string ip, int port, std::string host);


};

std::ostream & operator<<(std::ostream & o, HttpBlock const & rhs);

#endif // HTTP_BLOCK_HPP
