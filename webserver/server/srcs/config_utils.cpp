/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdias-mo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/03 14:15:24 by nmoreira          #+#    #+#             */
/*   Updated: 2024/02/03 15:16:52 by jdias-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


# include "../includes/config_utils.hpp"


bool verifyServerConflict(const std::string &newIpAddress, const unsigned int newPort, const std::vector<std::string> &newNames, std::string &msg, const std::vector<Config*> &server)
{
	for (std::vector<Config*>::const_iterator serverIt = server.begin(); serverIt != server.end(); ++serverIt)
	{
		if ((*serverIt)->get_ipAddress() == newIpAddress && (*serverIt)->get_port() == newPort)
		{
			msg = "Error: IPadress or port conflict!";
			return true;
		}
		else
		{
			const std::vector<std::string> server_names = (*serverIt)->get_server_names();
			if (newNames.empty())
			{
				msg = "Error: No names!";
				return true;
			}
			for (std::vector<std::string>::const_iterator newName = newNames.begin(); newName != newNames.end(); ++newName)
			{
				if (find(server_names.begin(), server_names.end(), *newName) != server_names.end())
				{
					msg = "Error: Equal server names!";
					return true;
				}
			}
		}
	}
	return false;
}

void ft_print(std::vector<Config*> &server) {

	for (std::vector<Config*>::iterator it = server.begin(); it != server.end(); ++it)
	{
		std::cout << "==============Server=============" << std::endl;
		std::cout << "Root: " << (*it)->get_root() << std::endl;
		std::cout << "Index: " << (*it)->get_index() << std::endl;
		std::cout << "IpAdress: " << (*it)->get_ipAddress() << std::endl;
		std::cout << "Port: " << (*it)->get_port() << std::endl;
		std::cout << "Max Client Body Size: " << (*it)->get_max_client_body_size() << std::endl;
		std::cout << "Autoindex: " << std::boolalpha << (*it)->get_autoindex() << std::endl;
		std::vector<std::string> names = (*it)->get_server_names();
		for (std::vector<std::string>::iterator it1 = names.begin(); it1 != names.end(); ++it1)
			std::cout << "Server Name: " << *it1 << std::endl;
		std::map<std::string, std::string> cgiMap = (*it)->get_cgis();
		std::map<std::string, std::string>::iterator it2;
		for (it2 = cgiMap.begin(); it2 != cgiMap.end(); ++it2) {
			std::cout << "Extension: " << it2->first << "; Path: " << it2->second << std::endl;
		}
		std::map<unsigned int, std::string> errPages = (*it)->get_error_pages();
		std::map<unsigned int, std::string>::iterator it3;
		for (it3 = errPages.begin(); it3 != errPages.end(); ++it3) {
			std::cout << "Code: " << it3->first << "; Error Page Path: " << it3->second << std::endl;
		}
		std::vector<Location> locs = (*it)->get_locations();
		std::vector<Location>::iterator it4;
		for (it4 = locs.begin(); it4 != locs.end(); ++it4) {
			std::cout << "==============Location=============" << std::endl;
			std::cout << "Path: " << it4->get_path() << std::endl;
			std::cout << "Redirection Code: " << it4->get_redirectionCode() << std::endl;
			std::cout << "Redirection Path: " << it4->get_redirectionPath() << std::endl;
			std::cout << "RootPath: " << it4->get_rootPath() << std::endl;
			std::cout << "Index: " << it4->get_index() << std::endl;
			std::cout << "Autoindex: " << it4->get_autoindex() << std::endl;
			std::vector<std::string> allows = it4->get_allow();
			std::vector<std::string>::iterator it5;
			std::cout << "Allows: ";
			for (it5 = allows.begin(); it5 != allows.end(); ++it5)
				std::cout << *it5 << " ";
			std::cout << std::endl;
			std::cout << "============End Location===========" << std::endl;
		}
		std::cout << "==============End Server=============" << std::endl;
	}
	return ;
}

void config(const std::string &configFile, std::vector<Config*> &server) {

	std::string line;
	std::string temp;
	size_t line_count = 0;
	std::vector<std::string> fileConfig;

	if (configFile.empty()) {
        // std::cout << "Error: No argument provided for config file" << std::endl;
        // return;
		throw std::invalid_argument("Error: No argument provided for config file");
    }

	std::ifstream file(configFile.c_str(), std::ios::in);
	std::ifstream tempFile(configFile.c_str(), std::ios::in); // usar este para teste porque se usar o outro o getline fica a apontar para o fim

	if (!file.is_open()) {
		// std::cout<< "Error: Unable to open config file" << std::endl;
		// return ;
		throw std::runtime_error("Error: Unable to open config file");
    }

	std::getline(tempFile, temp, '\0');
	if (temp.empty())
	{
		// std::cout << "Error: Empty file!" << std::endl;
		tempFile.close();
		throw std::invalid_argument("Error: Empty file");
		// return ;
	}
	tempFile.close(); // introduzido no final.

	while (std::getline(file, line))
		fileConfig.push_back(line);

	// std::cout << fileConfig.size() << std::endl; //teste tamanho

	if (!verifyBraces(fileConfig)) // controla os braces não preciso de detectar no parse
	{
		// std::cout << "Error: Config file wrong format - braces '{}'" << std::endl;
		file.close();
		throw std::invalid_argument("Error: Config file wrong format - braces '{}'");
		// return ;
	}

	while (line_count < fileConfig.size()) {

        std::istringstream iss(fileConfig[line_count]);
        std::vector<std::string> tokens;
		std::string word;

        while (iss >> word) {
            tokens.push_back(word);
        }

		/*teste token*/
		// std::vector<std::string>::iterator ite1 = tokens.end();
		// for (std::vector<std::string>::iterator it1 = tokens.begin(); it1 != ite1; ++it1)
		// {
		// 	std::cout << *it1 << " ";
		// }
		// std::cout << std::endl;

        if (tokens.size() > 1 && (tokens[0] == "server" && tokens[1] == "{"))
		{
			std::string msg;
            Config *tmpServer = parseConfig(fileConfig, line_count);
            if (tmpServer && verifyServerConflict(tmpServer->get_ipAddress(), tmpServer->get_port(), tmpServer->get_server_names(), msg, server))
			{
                std::cout << "Error: Conflict with another server" << std::endl;
                std::cout << msg << std::endl;
				delete (tmpServer);
				continue ;
            }
			if (tmpServer)
			{
            	server.push_back(tmpServer);
				// std::cout << tmpServer->get_port() << std::endl;
			}
        }
        line_count++;
    }
	/*testes*/
	// std::vector<Location> teste = server[0]->get_locations();
	// std::cout << server[0]->get_port() << std::endl;
	// std::cout << server[1]->get_port() << std::endl;
	// std::cout << server[2]->get_port() << std::endl;
	// std::cout << teste[1].get_path() << std::endl;

	// ft_print(server);
    file.close();
	return ;
}

// std::string	erroResponseMaker(int erroCode, std::string erroPage)
// {
// 	std::string	response = "HTTP/1.1 "
// 							+ create_map_return_codes()[erroCode]
// 							+ "Content-type: text/html\r\n\r\n"
// 							+ erroPage;
// 	return ( response );
// }
