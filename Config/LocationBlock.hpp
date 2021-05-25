/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 15:59:09 by julnolle          #+#    #+#             */
/*   Updated: 2021/05/25 17:51:33 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_BLOCK_HPP
# define LOCATION_BLOCK_HPP

# include "HttpBlock.hpp"

class LocationBlock : public HttpBlock {

private:
	std::string		_path; //Optional since locations are stored in map


public:
	LocationBlock(void);
	LocationBlock(LocationBlock const & copy);
	~LocationBlock(void);
	LocationBlock& operator=(LocationBlock const & rhs);

	// Setters
	void	setPath(std::string path);

	// Getters
	const std::string&		getPath(void) const;
};

std::ostream & operator<<(std::ostream & o, LocationBlock const & rhs);

#endif // LOCATION_BLOCK_HPP
