/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBlock.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 16:29:31 by julnolle          #+#    #+#             */
/*   Updated: 2021/05/25 17:48:49 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_BLOCK_HPP
# define HTTP_BLOCK_HPP

# include "webserv.hpp"
# include "utils.hpp"

	typedef size_t						size_type;
	typedef std::vector<std::string>	stringVec;
	typedef stringVec::iterator			strVecIterator;
	typedef std::map<int, std::string>	errorMap;

class HttpBlock {

public:
		typedef std::map<std::string, std::string>		mapCgiParams;

protected:

	std::string						_root;
	int								_autoindex; // on | off | not set
	std::vector<std::string>		_indexes;
	std::vector<std::string>		_limit_except; //ou allow_methods ?
	std::map<int, std::string>		_error_pages; /* num error, error file path */
	long int						_client_max_body_size;
	long int						_keepalive_timeout;
	int								_chunked_transfer_encoding; // on | off
	std::string						_auth_basic; // string | off
	std::string						_auth_basic_user_file; //httpasswd paths

	/*For CGI*/
	std::string						_cgi_pass;
	size_type						_cgi_port; //mixer en std::string avec _cgi_pass ?
	std::string						_cgi_index;
	mapCgiParams					_cgi_params;


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
	int		setErrorPages(strVecIterator first, strVecIterator last, std::string& val);
	void	setMaxBdySize(size_type size);
	void	setKeepaliveTimeout(size_type timeout);
	void	setChunkedEncoding(std::string& state);
	void	setAuthBasic(std::string path);
	void	setAuthBasicFile(std::string path);
	void	setCgiPass(std::string value);
	void	setCgiPort(size_type port);
	void	setCgiIndex(std::string index);
	void	setCgiParams(mapCgiParams params);

	// Getters
	const std::string&		getRoot(void) const;
	const int&				getAutoindex(void) const;
	const stringVec&		getIndexes(void) const;
	const stringVec&		getLimitExcept(void) const;
	const errorMap&			getErrorPages(void) const;
	const long int&			getMaxBdySize(void) const;
	const long int&			getKeepaliveTime(void) const;
	const int&				getChunkedEncoding(void) const;
	const std::string&		getAuthBasic(void) const;
	const std::string&		getAuthBasicFile(void) const;

};

std::ostream & operator<<(std::ostream & o, HttpBlock const & rhs);

#endif // HTTP_BLOCK_HPP
