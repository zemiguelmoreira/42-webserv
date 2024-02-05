/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmaciel- <hmaciel-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/24 11:59:06 by hmaciel-          #+#    #+#             */
/*   Updated: 2023/10/24 13:47:27 by hmaciel-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "server_utils.hpp"
# include "SendData.hpp"
# include "HandleData.hpp"
# include "parse.hpp"
# include "config_utils.hpp"
# include "colors.hpp"
# include "Config.hpp"
# include <sstream>
# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <unistd.h>
# include <fcntl.h>
# include <sys/epoll.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <errno.h>
# include <stdlib.h>
# include <algorithm>
# include <fstream>
# include <sys/wait.h>

# define MAX_EVENTS 100
# define MAX_CLIENTS 100
# define SERVER_TIMEOUT 10

class Config;
class HandleData;

class WebServer {

	public:
		WebServer( Config *config, int epollFd );
		~WebServer();

		void						run(int fd);
		void						removeSocketFromEpoll(int clientSocket);
		std::map<int, time_t>		getTimeouts();

	private:
		std::vector<int>			sockets;
		std::map<int, HandleData>	webHandler;
		std::map<int, time_t>		timeouts;
		Config						*config;
		int 						port;
		int 						serverSocket;
		int 						epollFd;
		char						**envsToExecve;
		struct epoll_event			server_ev, events[MAX_EVENTS];

		WebServer();
		WebServer &operator=(const WebServer &opRight);
		WebServer(const WebServer &src);

		//int							createNewConnection(int port);
		int							createServerSocket();
		void						addSocketToEpoll(int fd, struct epoll_event ev);
		void						handleNewConnection();
		void						handleClientRequest(int &clientSocket);
};
