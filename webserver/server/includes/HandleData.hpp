/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdias-mo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 13:24:34 by hmaciel-          #+#    #+#             */
/*   Updated: 2024/02/02 18:40:53 by jdias-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# define REQUEST_BUFFER_SIZE 4096

# include "SendData.hpp"
# include "HandleCgi.hpp"
# include "Response.hpp"
# include "Config.hpp"
# include <iostream>
# include <map>
# include <vector>
# include <fstream>

using std::string;
using std::istringstream;
using std::vector;
using std::map;
using std::cerr;
using std::make_pair;

class Config;

class HandleData
{
public:
	HandleData();
	HandleData(int&, Config*);
	HandleData(const HandleData&);
	~HandleData();

	HandleData&					operator=(const HandleData&);
	bool						readFromSocket();
	size_t						get_contentLength();

private:
	void						dataRouter();
	bool						parseRequest(string&);
	string						getMimeType(string&);
	string						getRequestURI();
	bool						readyToSend();
	void						addToBody(string&);
	vector<string>				splitRequest(const string&, string);
	bool						headersToEnvs(vector<string>&);
	bool						addToEnvs(vector<string>);
	void						clearObj();
	bool						isRedirection();
	bool						isNotFound();
	bool						sendRedirection();

	int							_clientSocket;
	ssize_t						_readTotal;
	map<string, string>			_envs;
	size_t						_contentLength;
	string						_body;
	bool						_boundary;
	std::map<string, string>	_mime;
	bool						_isCgi;
	bool						_isDir;
	string						_workDir;
	SendData					_webSender;
	Config*						_config;
};

