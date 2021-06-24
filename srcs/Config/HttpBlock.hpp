/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBlock.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 16:29:31 by julnolle          #+#    #+#             */
/*   Updated: 2021/06/24 20:26:43 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_BLOCK_HPP
# define HTTP_BLOCK_HPP

# include "webserv.hpp"
# include "utils.hpp"

	typedef size_t								size_type;
	typedef std::vector<std::string>			stringVec;
	typedef stringVec::iterator					strVecIterator;
	typedef std::map<int, std::string>			errorMap;
	typedef std::map<std::string, std::string>	stringMap;
	typedef std::pair<int, std::string>			redirPair;

class HttpBlock {

protected:

	std::string						_root;
	int								_autoindex; // on | off | not set
	std::vector<std::string>		_indexes;
	std::vector<std::string>		_limit_except; //ou allow_methods ?
	std::map<int, std::string>		_error_pages; /* num error, error file path */
	unsigned long					_client_max_body_size;
	int								_keepalive_timeout;
	int								_chunked_transfer_encoding; // on | off
	std::string						_auth_basic; // string | off
	std::string						_auth_basic_user_file; //httpasswd paths
	stringMap						_cgi_extensions;
	std::pair<int, std::string>		_return; //only for serv et loc blocks
	std::string						_upload_dir;

	/*For CGI*/
	std::vector<std::string>		_cgi_allowed_ext; // no need anymore

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
	int		setCgiExtensions(std::string const& ext, std::string const& cgi_path);
	int		setReturn(std::string const& code, std::string const& url);
	void	setUploadDir(std::string& path);

	// Getters
	const std::string&		getRoot(void) const;
	const int&				getAutoindex(void) const;
	const stringVec&		getIndexes(void) const;
	const stringVec&		getLimitExcept(void) const;
	const errorMap&			getErrorPages(void) const;
	const unsigned long&	getMaxBdySize(void) const;
	const int&				getKeepaliveTimeout(void) const;
	const int&				getChunkedEncoding(void) const;
	const std::string&		getAuthBasic(void) const;
	const std::string&		getAuthBasicFile(void) const;
	const stringVec&		getCgiAllowedExt(void) const;
	const redirPair&		getReturn(void) const;
	const stringMap&		getCgiExtensions(void) const;
	const std::string&		getUploadDir(void) const;

};

std::ostream & operator<<(std::ostream & o, HttpBlock const & rhs);

#endif // HTTP_BLOCK_HPP
