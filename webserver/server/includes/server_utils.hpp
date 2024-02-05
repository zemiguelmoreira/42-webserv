/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_utils.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdias-mo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/24 12:26:15 by hmaciel-          #+#    #+#             */
/*   Updated: 2024/02/03 12:13:35 by jdias-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <sstream>
# include <fcntl.h>
# include <algorithm>
# include <map>
# include <vector>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <dirent.h>
# include <cerrno>
# include "Location.hpp"

using std::cerr;

template <typename T>
std::string	myToString( T n )
{
	std::ostringstream oss;
	oss << n;
	return ( oss.str() );
}

bool		checkType( std::string type );
bool		endswith( std::string const & value, std::string const & ending );
void		setNonBlocking( int fd );
size_t		count_lines( std::string const &str );
bool		is_https( std::string const &request );
bool		has_forbidden( std::string const &request, std::string uri, std::vector<Location> locations );
bool		isExe(char *path);
bool		isDir(const char *path);
bool		fileExists(const char* filename);
std::string	toString(int n);
char		fromHex(char c);
std::string	decode(const std::string&);
