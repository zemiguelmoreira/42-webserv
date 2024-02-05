/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdias-mo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 15:34:26 by jdias-mo          #+#    #+#             */
/*   Updated: 2024/02/03 17:54:03 by jdias-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "HandleData.hpp"
# include "server_utils.hpp"
# include <sys/types.h>
# include <unistd.h>
# include <sys/wait.h>
# include <cerrno>
# include <cstring>
# include <fstream>
# include <cstdio>

using std::string;
using std::map;
using std::cerr;
using std::cout;
using std::ifstream;
using std::ofstream;
using std::remove;

class Response
{
public:
	Response(map<string, string>&, string&, string);
	~Response();

	string&				getResponse();

	int					handle();
	int					upload();


private:
	Response();
	Response(Response const&);

	Response& 			operator=(Response const&);
	string				removeLines(string&);
	string				styleStr(string);

	map<string, string>	_envs;
	string				_respOutput;
	string				_body;
	string				_root;
};
