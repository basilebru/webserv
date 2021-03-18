/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:55:26 by julnolle          #+#    #+#             */
/*   Updated: 2021/03/18 09:27:42 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"
#include "HttpBlock.hpp"

int main()
{
	HttpBlock http_block;
	ConfParser parser("../old/nginx.conf", http_block);

	parser.readConfFile();
	

	return 0;
}