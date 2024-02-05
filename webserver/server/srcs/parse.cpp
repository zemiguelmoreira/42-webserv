/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdias-mo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 15:07:46 by nmoreira          #+#    #+#             */
/*   Updated: 2024/02/05 19:31:59 by jdias-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/parse.hpp"

bool ft_digit(const std::string &str)
{
	for (std::size_t i = 0; i < str.length(); ++i)
	{
		if (!std::isdigit(str[i]))
		{
			return false;
		}
	}
	return true;
}

// int	is_dir(const char *path) // função em c - minishell
// {
// 	DIR		*dir;

// 	dir = opendir(path);
// 	if (dir)
// 	{
// 		closedir(dir);
// 		return (1);
// 	}
// 	return (0);
// }

int is_dir(const std::string &path)
{
    DIR *dir = opendir(path.c_str());
    if (dir)
    {
        closedir(dir);
        return 1;
    }
    return 0;
}

void ft_pop_back(std::string &s)
{
	size_t pos = s.find_last_not_of('/');
	size_t pos1 = s.find_first_not_of('/');

	if (pos != std::string::npos)
		s.erase(pos + 1);
	if (pos1 != std::string::npos)
	{
		std::string temp = s.substr(pos1);
		s = temp;
	}
	else
		s.clear();

	return ;
}

void ft_pop_back2(std::string &s)
{
	size_t pos = s.find_last_not_of('/');
	size_t pos1 = s.find_first_not_of('/');

	if (pos != std::string::npos)
		s.erase(pos + 1);
	if (pos1 != std::string::npos)
	{
		std::string temp = s.substr(pos1 - 1);
		s = temp;
	}
	else
		s.clear();

	return ;
}

int lastLine(const std::vector<std::string> fileConfig)
{
	std::vector<std::string>::const_reverse_iterator rIter = fileConfig.rbegin();

	std::string str;
	str = (*rIter);
	size_t pos = str.find(';');
	if (pos != std::string::npos && pos == str.length() - 1)
		str.erase(pos);

	if (str.size() != 1)
		return 1;
	if (str != "}")
		return 1;
	return 0;
}

/*verificar endereço IP*/
bool isValidIP(const std::string& ip_str) {
    std::vector<int> nums;
    std::istringstream iss(ip_str);
    std::string num;

    while (std::getline(iss, num, '.'))
	{
        try {

			if (num.length() > 3)
				return false;

            int value = std::atoi(num.c_str());

            if (value < 0 || value > 255)
                return false;  // Valor fora da faixa permitida

            nums.push_back(value);
        } catch (std::invalid_argument&) {
            return false;  // Não é um número válido
   		}
	}
    return nums.size() == 4;  // Deve haver exatamente 4
}

bool verifyBraces(std::vector<std::string> fileConfig)
{
	std::stack<char> braceStack;
	std::stack<char> braceStackServer;
	bool insideLocationBlock = false;
	bool insideServerBlock = false;

	for (std::vector<std::string>::iterator it = fileConfig.begin(); it != fileConfig.end(); ++it)
	{
		std::istringstream iss(*it);
		std::string word;

		while (iss >> word)
		{
			if (word == "#")
			{
				break ;
			}
			else if (word == "{")
			{
				if (insideLocationBlock)
					braceStack.push('{');
				else if (insideServerBlock)
					braceStackServer.push('{');
			}
			else if (word == "}")
			{
				if (!braceStack.empty() && braceStack.top() == '{')
				{
					braceStack.pop();
					if (braceStack.empty() && insideLocationBlock)
						insideLocationBlock = false;
					continue;
				}
				else if (!braceStackServer.empty() && braceStackServer.top() == '{')
				{
					braceStackServer.pop();
					if (braceStackServer.empty() && insideServerBlock)
						insideServerBlock = false;
				}
				else
					return false;

			}
			else if (word == "location")
			{
				insideLocationBlock = true;
			}
			else if (word == "server")
			{
				insideServerBlock = true;
			}
		}
	}
	if (!braceStack.empty() || !braceStackServer.empty())
		return false;

	return true;
}

Location  ft_location(const std::vector<std::string> fileConfig, std::vector<std::string> tokens, size_t &line_count)
{
	Location location;
	std::string temp = tokens[1];

	if (temp.size() > 0 && temp[0] != '/')
		temp = "/" + temp;
	// if (temp.size() > 1 && temp[temp.length() - 1] == '/')
	// 	ft_pop_back2(temp);
	if (temp.size() > 1)
		ft_pop_back2(temp);
	// std::cout << temp << std::endl;
	location.setPath(temp);
	line_count++;

	std::vector<std::string>::const_iterator it = fileConfig.begin() + line_count;
	while (it != fileConfig.end())
	{
		// std::cout << fileConfig[line_count] << std::endl;
		std::istringstream iss1(fileConfig[line_count]);
		std::vector<std::string> tokens1; // por linha
		std::string word;

		while (iss1 >> word)
		{
			tokens1.push_back(word);
		}
		if (tokens1.empty() || tokens1[0][0] == '#')
		{
			it++;
			line_count++;
			continue;
		}
		if (tokens1[0] == "}")
			break;
		if (tokens1[0] == "allow")
		{
			if (tokens1.size() < 2)
			{
				// std::cout << "Error: 'location' Allow." << std::endl;
				it++;
				line_count++;
				continue;
			}
			for (std::vector<std::string>::const_iterator it1 = tokens1.begin() + 1; it1 != tokens1.end(); ++it1)
			{
				if (*it1 == "GET" || *it1 == "POST" || *it1 == "DELETE")
					location.setAllow(*it1);
			}
		} else if (tokens1[0] == "return")
		{
			if (tokens1.size() != 3 || location.get_redirectionCode() != 0)
			{
				std::cerr << "Error: 'location' Return." << std::endl;
				it++;
				line_count++;
				continue;
			}
			int rCode = atoi(tokens1[1].c_str());
			if (!(rCode == 300 || rCode == 301 || rCode == 302 || rCode == 303 || rCode == 304 || rCode == 307 || rCode == 308))
			{
				std::cerr << "Error: 'location' Return Code Invalid." << std::endl;
				++it;
				++line_count;
				continue ;
			}
			location.setRedirectionCode(rCode);
			location.setRedirectionPath(tokens1[2]); // deve ser verificado aquando do response
		} else if (tokens1[0] == "root")
		{
			if (tokens1.size() != 2)
			{
				std::cerr << "Error: 'location' Root." << std::endl;
				it++;
				line_count++;
				continue;
			}
			std::string temp1 = tokens1[1];
			if (!temp1.empty() && temp1[0] != '/')
				temp1 = "/" + temp1;
			if (!temp1.empty())
				ft_pop_back2(temp1);

			location.setRootPath(temp1);
			// std::cout << "rootPath location " << temp1 << std::endl;

		} else if (tokens1[0] == "index")
		{
			if (tokens1.size() != 2)
			{
				std::cerr << "Error: 'location' Index." << std::endl;
				it++;
				line_count++;
				continue;
			}
			std::string temp2 = tokens1[1];
			if (!temp2.empty())
				ft_pop_back(temp2);

			location.setIndex(temp2);

		} else if (tokens1[0] == "autoindex")
		{
			if (tokens1.size() != 2)
			{
				std::cerr << "Error: 'location' AutoIndex." << std::endl;
				it++;
				line_count++;
				continue;
			}
			location.setAutoindex(tokens1[1] == "on");
		}
		// std::cout << tokens[0] << std::endl;
		++it;
		++line_count;
	}
	return location;
}

Config *parseConfig(std::vector<std::string> fileConfig, size_t &line_count)
{
	std::vector<std::string>::const_iterator it = fileConfig.begin() + line_count;
	if (it == fileConfig.end())
		return NULL;
	Config *server = new Config();

	++it;
	++line_count;
	while (it != fileConfig.end())
	{
		// std::cout << fileConfig[line_count] << std::endl;
		size_t posSegundoHash = fileConfig[line_count].find('#');
		if (posSegundoHash == 0 )
		{
			posSegundoHash = fileConfig[line_count].find('#', fileConfig[line_count].find('#') + 1);
			if (posSegundoHash != std::string::npos)
				fileConfig[line_count] = fileConfig[line_count].substr(0, posSegundoHash);
		} else if (posSegundoHash != std::string::npos)
			fileConfig[line_count] = fileConfig[line_count].substr(0, posSegundoHash);
		// std::cout << fileConfig[line_count] << std::endl;
		std::istringstream iss1(fileConfig[line_count]);
		std::vector<std::string> tokens; // por linha
		std::string word;

		while (iss1 >> word)
		{

			size_t pos = word.find(';');
			// Verifica se o ponto e vírgula está no final da string
			if (pos != std::string::npos && pos == word.length() - 1)
			{
				// Remove o ponto e vírgula do final da string
				word.erase(pos);
			}

			if (word.length())
				tokens.push_back(word);
		}
		if (tokens.empty() || tokens[0][0] == '#')
		{
			it++;
			line_count++;
			continue;
		}
		if (tokens[0] == "}") // verificar esta saída
		 	break;
		if (tokens[0] == "server")
		{
			line_count -= 1; // para ler a linha server
			delete server;
			return NULL;
		}
		else if (tokens[0] == "location")
		{
			if (tokens.size() != 3)
			{
				std::cerr << "Error: 'location'" << std::endl;
				delete server;
				return NULL;
			}
			if (tokens[2] != "{") { // em principio isto não será necessário
				std::cerr << "Error: After 'location' directory put the '{' !" << std::endl;
				delete server;
				return NULL;
			}

			size_t temp_count = line_count;
			server->setLocations(ft_location(fileConfig, tokens, line_count));
			it += line_count - temp_count;
			if (it == fileConfig.end()) // verificar pode dar erro
			{
				std::cerr << "Error: 'location' Unexpected end of file." << std::endl;
				delete server;
				return NULL;
			}
			// std::cout << fileConfig[line_count] << std::endl;
			// std::cout << "saída" << *it << std::endl;
		}
		else if (tokens[0] == "server_name")
		{
			if (tokens.size() < 2)
			{
				// std::cout << "Error: server name" << std::endl;
				// delete server;
				// return NULL;
				server->setServerNames("localhost");
			}
			for (std::vector<std::string>::iterator serv = tokens.begin() + 1; serv != tokens.end(); ++serv)
				server->setServerNames(*serv);
		}
		else if (tokens[0] == "cgi")
		{
			if (tokens.size() != 3)
			{
				std::cerr << "Error: cgi" << std::endl;
				delete server;
				return NULL;
			}
			server->setCgi(tokens[1], tokens[2]);
		}
		else if (tokens[0] == "error_page")
		{
			if (tokens.size() != 3)
			{
				std::cerr << "Error: error_page" << std::endl;
				delete server;
				return NULL;
			}
			unsigned int code = static_cast<unsigned int>(std::atoi(tokens[1].c_str()));
			std::string path = tokens[2];
			if (!path.empty())
				ft_pop_back2(path);
			server->setErrorPage(code, path);
		}
		else if (tokens[0] == "listen")
		{
			if (tokens.size() != 2)
			{
				std::cerr << "Error: listen" << std::endl;
				delete server;
				return NULL;
			}

			size_t pos1 = tokens[1].find(':');
			std::string port;
			std::string host;
			if (pos1 == std::string::npos)
			{
				// port = static_cast<unsigned int>(std::atoi(tokens[1].c_str()));
				port = tokens[1];
				// std::cout << "Porta1: " << port << std::endl;
				host = "localhost";
			}
			else
			{

				std::vector<std::string> words;
				std::istringstream wordStream(tokens[1]);
				std::string word1;
				while (std::getline(wordStream, word1, ':'))
					words.push_back(word1);

				if (words.size() != 2)
				{
					std::cerr << "Error listen - address:port" << std::endl;
					delete server;
					return NULL;
				}
				if (words[0] == "localhost" || words[0] == "127.0.0.1")
					host = "localhost";
				else if (isValidIP(words[0])) // validar expressão
					host = words[0];

				port = words[1];
			}
			if (ft_digit(port))
				server->setPort(static_cast<unsigned int>(std::atoi(port.c_str())));
			server->setIpAddress(host);
			// std::cout << "Porta: " << port << std::endl;
			// std::cout << "IPadress: " << host << std::endl;
		}
		else if (tokens[0] == "root")
		{
			if (tokens.size() != 2)
			{
				std::cerr << "Error: root" << std::endl;
				delete server;
				return NULL;
			}
			std::string temp = tokens[1];
			// if (!temp.empty() && temp[0] != '/')
			// 	temp = "/" + temp;
			if (!temp.empty())
				ft_pop_back(temp);
			// std::cout << temp << std::endl;
			// std::cout << temp << std::endl;
			// std::cout << is_dir(temp) << std::endl;
        	if (is_dir(temp))
			{
				// std::cout << "teste root existe" << std::endl;
				server->setRoot(temp);
			}
		}
		else if (tokens[0] == "index")
		{
			if (tokens.size() != 2)
			{
				std::cerr << "Error: index" << std::endl;
				delete server;
				return NULL;
			}
			std::string ind = tokens[1];
			if (!ind.empty())
				ft_pop_back(ind);

			// std::cout << ind << std::endl;
			server->setIndex(ind);
		}
		else if (tokens[0] == "client_max_body_size")
		{
			if (tokens.size() != 2)
			{
				std::cerr << "Error: max client body size" << std::endl;
				delete server;
				return NULL;
			}
			if (!ft_digit(tokens[1]))
			{
				std::cerr << "Error: max client body size: not a digit" << std::endl;
				delete server;
				return NULL;
			}
			server->setClientBodySize(std::atoll(tokens[1].c_str()));
		}
		else if (tokens[0] == "autoindex")
		{
			if (tokens.size() != 2)
			{
				std::cerr << "Error autoindex" << std::endl;
				delete server;
				return NULL;
			}
			server->setAutoIndex(tokens[1] == "on"); // uso uma expressão boleana
		}
		++it;
		++line_count;
	}
	// std::cout << *it << std::endl;
	std::string error;
	// std::cout << server->checkServer(error) << std::endl;
	// std::cout << error << std::endl;
	if (server->checkServer(error))
	{
		std::cerr << error << std::endl;
		delete server;
		return NULL;
	}
	return server;
}
