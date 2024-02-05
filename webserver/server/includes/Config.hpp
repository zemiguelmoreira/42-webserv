/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdias-mo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by 2.fr>             #+#    #+#             */
/*   Updated: 2024/02/03 15:46:20 by jdias-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

#define MAX_BODY 409600

# include <vector>
# include <map>
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <cstdlib>
# include <fcntl.h>
# include "Location.hpp"
# include "WebServer.hpp"

class Location;
class WebServer;

class Config {

	private:

		std::vector<Location>					locations;
		std::vector<std::string>				server_names;
		std::vector<std::string>				redirectionPaths;
		std::map<std::string, std::string>		cgis;
		std::map<unsigned int, std::string>		error_pages; // parametrizadas podem ser mais ou não
		std::string								root; // root directory
		std::string								index; // file para "/"
		std::string								ipAddress;
		unsigned int							port;
		size_t									client_max_body_size;
		bool									autoIndex;
		std::map<unsigned int, std::string>		err_codes;

		Config(const Config &src);
		Config &operator=(const Config &opRight);

	public:
		Config();
		~Config();

		bool				checkServer(std::string &error);

		std::vector<std::string>					get_server_names(void) const;
		std::map<std::string, std::string>			get_cgis(void) const;
		std::map<unsigned int, std::string>			get_error_pages(void) const;
		std::string									get_ipAddress(void) const;
		unsigned int								get_port(void) const;
		std::string									get_root(void) const;
		std::string									get_index(void) const;
		size_t										get_max_client_body_size(void) const;
		std::vector<Location>						get_locations(void) const;
		bool										get_autoindex(void) const;
		std::vector<std::string>					get_redirectionPaths(void) const;
		std::map<unsigned int, std::string>			get_pages_paths(void) const;

		void				setServerNames(const std::string name);
		void				setCgi(const std::string ext, const std::string path);
		void				setErrorPage(const unsigned int code, const std::string path);
		void 				setIpAddress(std::string host);
		void				setPort(unsigned int listenPort);
		void				setRoot(std::string dir);
		void				setIndex(std::string ind);
		void				setClientBodySize(long long ind);
		void				setAutoIndex(bool value);
		void				setLocations(const Location &location);
		void				sendError(int clientSocket, int errCode);

		std::string				create_default_error_page(const unsigned int &error_code);
		int						file_size(const std::string pathFile);
		void					ft_make_default_pages(void);

		std::map<unsigned int, std::string>		create_map_return_codes(void);

};

#endif
