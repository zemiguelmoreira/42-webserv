/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_utils.hpp                                   :+:      :+:    :+:   */
/*   config_utils.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmaciel- <hmaciel-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/03 14:15:57 by nmoreira          #+#    #+#             */
/*   Updated: 2024/01/28 17:26:08 by hmaciel-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CONFIG_UTILS_HPP
# define CONFIG_UTILS_HPP

# include "Config.hpp"
# include "Location.hpp"

class Config;

bool verifyServerConflict(const std::string &newIpAddress, const unsigned int newPort, const std::vector<std::string> &newNames, std::string &msg, const std::vector<Config*> &config);
void ft_print(std::vector<Config*> &config);
void config(const std::string &configFile, std::vector<Config*> &config);

#endif