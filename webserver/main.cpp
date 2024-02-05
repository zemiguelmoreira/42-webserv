/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmoreira <nmoreira@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/04 21:21:15 by hmaciel-          #+#    #+#             */
/*   Updated: 2024/02/04 16:29:55 by nmoreira         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "./server/includes/WebServer.hpp"

int	stop_server = false;

void	handle_signals( int signal )
{
	if ( signal == SIGINT || signal == SIGQUIT )
		stop_server = true;
}

bool ft_isspace(std::string s)
{
    for (int i = 0; i < (int)s.length(); i++)
    {
        if (!std::isspace(s[i]))
            return false;
    }
    return true;
}

bool checkFile(std::string str)
{
	size_t pos = str.find_last_of(".");

	if (pos == std::string::npos)
	{
		std::cout << "Error: Invalid name file - '.'!" << std::endl;
		return false;
	}

	size_t pos1 = str.find_last_of("/");
	std::string name;
	if (pos1 != std::string::npos)
		name = str.substr(pos1 + 1, pos - (pos1 + 1));
	else
		name = str.substr(0, pos);

	std::string ext = str.substr(pos + 1, str.length());

	if (name.empty())
	{
		std::cout << "Error: Invalid name file!" << std::endl;
		return false;
	}
	if (ext != "conf")
	{
		std::cout << "Error: Invalid extension file!" << std::endl;
		return false;
	}
	return true;
}

int main(int argc, char const *argv[])
{
	std::vector<Config*> 					configs;
	std::vector<WebServer*> 				servers;
	int				 						epollFd = 0;
	struct epoll_event						events[MAX_EVENTS];
	
	if (argc != 2)
	{
		std::cerr << "Error: Invalid number of arguments" << std::endl;
		return 1;
	}
	else if (ft_isspace(std::string(argv[1])))
	{
		std::cerr << "Error: Invalid name of destination file - spaces!" << std::endl;
		return 1;
	}

	std::string file(argv[1]);

	if (!checkFile(file))
		return 1;

	try {

		config(file, configs);

	} catch (std::exception &e) {

		std::cout << e.what() << std::endl;
		return 1;
	}

	if (!configs.size())
	{
		std::cout << "No server configurated!!" << std::endl;
		return 1;
	} else {
		for (std::vector<Config*>::iterator it = configs.begin(); it != configs.end(); ++it)
		{
			(*it)->ft_make_default_pages();
		}
	}


	epollFd = epoll_create1(0);
	if (epollFd == -1) {
        perror("Erro ao criar epoll");
        exit(EXIT_FAILURE);
    }
	// std::cout << BWHT << "epoll created, fd" << epollFd << RESET << std::endl;


	for (std::vector<Config*>::iterator it = configs.begin(); it != configs.end(); ++it)
	{
		servers.push_back( new WebServer( *it, epollFd ));
	}

	signal( SIGINT, handle_signals );
	signal( SIGQUIT, handle_signals );
	//main loop servers
	while(!stop_server)
	{
		int numEvents = epoll_wait( epollFd, events, MAX_EVENTS, 0 );
		
		for ( int i = 0; i < numEvents; ++i )
		{
			int fd = events[i].data.fd;
			for (size_t i = 0; i < servers.size(); i++)
			{
				servers[i]->run(fd);
			}
		}

		//timeout
		for ( size_t s = 0; s < servers.size(); s++ )
		{
			std::map<int, time_t>	current = servers[s]->getTimeouts();
			// std::cout << "timeouts size: " << current.size() << std::endl;
			for ( std::map<int, time_t>::iterator it = current.begin(); it != current.end();)
			{
				// std::cout << "Timeout: " << (time(NULL) - it->second) << std::endl;
				if ( (time(NULL) - it->second) > SERVER_TIMEOUT )
				{
					// std::cout << it->first << "=>" << it->second << std::endl;//
					// std::cout << "Bye " << it->first << std::endl;//
					std::map<int, time_t>::iterator tmp = it;
					// std::cout << it->first << std::endl;//
					++it;
					servers[s]->removeSocketFromEpoll(tmp->first);
				}
				else
				{
					++it;
				}
			}
		}
	}


	for (std::vector<Config*>::iterator it = configs.begin(); it != configs.end(); ++it)
	{
		if ((*it))
			delete (*it);
	}
	for (std::vector<WebServer*>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		if ((*it))
			delete (*it);
	}
	configs.clear();
	servers.clear();
	if (epollFd)
		close(epollFd);

	return 0;
}
