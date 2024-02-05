/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdias-mo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 18:33:33 by nmoreira          #+#    #+#             */
/*   Updated: 2024/02/03 16:31:16 by jdias-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_HPP
# define PARSE_HPP

# include <stack>
# include <algorithm>
# include <sys/types.h>
# include <dirent.h>
# include "Config.hpp"
# include "Location.hpp"

class Location;

class Config;

bool 		ft_digit(const std::string& str);
int 		is_dir(const std::string &path);
void		ft_pop_back2(std::string& s);
bool 		verifyBraces(std::vector<std::string> fileConfig);
int			lastLine(const std::vector<std::string> fileConfig);
bool 		isValidIP(const std::string& ip_str);
Location 	ft_location(const std::vector<std::string> fileConfig, std::vector<std::string> tokens, size_t &line_count);
Config		*parseConfig(std::vector<std::string> fileConfig, size_t &line_count);


#endif
