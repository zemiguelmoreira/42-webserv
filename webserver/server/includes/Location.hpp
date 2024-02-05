/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmaciel- <hmaciel-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/12 18:47:19 by nmoreira          #+#    #+#             */
/*   Updated: 2024/01/18 10:27:19 by hmaciel-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <vector>

class Location
{
	private:
	
		std::string					path;
		std::vector<std::string>	allow;
		unsigned int				redirectionCode;
		std::string					redirectionPath;
		std::string					rootPath;
		std::string					index;
		bool						autoindex;

	public:
		Location();
		~Location();
		Location(const Location &src);
		Location &operator=(const Location &opRight);

		bool checkLocation() const;

		std::vector<std::string>	get_allow(void) const;
		std::string					get_path(void) const;
		unsigned int				get_redirectionCode(void) const;
		std::string					get_redirectionPath(void) const;
		std::string					get_rootPath(void) const;
		std::string					get_index(void) const;
		bool						get_autoindex(void) const;

		void			setAllow(const std::string &allowStr);
		void			setPath(const std::string &location);
		void			setRedirectionCode(const unsigned int code);
		void			setRedirectionPath(const std::string rePath);
		void			setRootPath(const std::string root);
		void			setIndex(const std::string ind);
		void			setAutoindex(const bool aindex);
};

#endif
