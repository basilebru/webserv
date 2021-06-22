/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBlock.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 16:29:31 by julnolle          #+#    #+#             */
/*   Updated: 2021/06/22 12:11:27 by julnolle         ###   ########.fr       */
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
	typedef std::pair<int, std::string>	redirPair;

class HttpBlock {

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
	std::pair<int, std::string>		_return; //only for serv et loc blocks
	std::string						_upload_dir;

	/*For CGI*/
	std::vector<std::string>		_cgi_allowed_ext; //File types that are allowed as CGI scripts

public:
	HttpBlock(void);
	HttpBlock(HttpBlock const & copy);
	~HttpBlock(void);
	HttpBlock& operator=(HttpBlock const & rhs);

	// Setters
	void	setRoot(std::string const& path);
	void	setAutoIndex(std::string const& state);
	void	setIndexes(strVecIterator first, strVecIterator last);
	void	setLimitExcept(strVecIterator first, strVecIterator last);
	int		setErrorPages(strVecIterator first, strVecIterator last, std::string const& val);
	void	setMaxBdySize(size_type const& size);
	void	setKeepaliveTimeout(size_type const& timeout);
	void	setChunkedEncoding(std::string const& state);
	void	setAuthBasic(std::string const& path);
	void	setAuthBasicFile(std::string const& path);
	void	setCgiAllowedExt(strVecIterator first, strVecIterator last);
	int		setReturn(std::string const& code, std::string const& url);
	void	setUploadDir(std::string& path);

	// Getters
	const std::string&		getRoot(void) const;
	const int&				getAutoindex(void) const;
	const stringVec&		getIndexes(void) const;
	const stringVec&		getLimitExcept(void) const;
	const errorMap&			getErrorPages(void) const;
	const long int&			getMaxBdySize(void) const;
	const long int&			getKeepaliveTimeout(void) const;
	const int&				getChunkedEncoding(void) const;
	const std::string&		getAuthBasic(void) const;
	const std::string&		getAuthBasicFile(void) const;
	const stringVec&		getCgiAllowedExt(void) const;
	const redirPair&		getReturn(void) const;
	const std::string&		getUploadDir(void) const;

};

std::ostream & operator<<(std::ostream & o, HttpBlock const & rhs);

#endif // HTTP_BLOCK_HPP
