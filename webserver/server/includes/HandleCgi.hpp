/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleCgi.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmoreira <nmoreira@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 15:34:28 by jdias-mo          #+#    #+#             */
/*   Updated: 2024/02/04 17:00:41 by nmoreira         ###   ########.fr       */
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
# include <cstdlib>
# include <ctime>

# define CGI_BUFFER_SIZE 4096
# define READ 0
# define WRITE 1
# define CGI_TIMEOUT 10

using std::string;
using std::map;
using std::cerr;
using std::cout;
using std::ifstream;

class HandleCgi
{
public:
	HandleCgi(map<string, string>&, string&, string);
	~HandleCgi();

	void				envsToCgi();
	char**				mapToCArray();
	int					executeCgi();
	string&				getCgi();
	void				deleteCArray(char **arr);

private:
	HandleCgi();
	HandleCgi(HandleCgi const&);

	HandleCgi&			operator=(HandleCgi const&);

	std::time_t			_timeout;
	pid_t				_cgiPid;
	int					_toCgi[2];
	int					_fromCgi[2];
	char				**_cgiEnvsChar;
	map<string, string>	_cgiEnvs;
	string				_cgiOutput;
	string				_scriptPath;
	map<string, string>	_envs;
	string				_body;
	string				_root;
};

