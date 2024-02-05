/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdias-mo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2024/02/05 19:29:50 by jdias-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "../includes/Config.hpp"

Config::Config() : port(0), client_max_body_size(MAX_BODY), autoIndex(false) {
	return ;
}

Config::~Config() {
	// std::cout << "teste" << std::endl;
}

bool Config::checkServer(std::string &error) {

	if (get_ipAddress() == "")
		return (error = "IPaddress not set", 1);
	if (get_port() == 0)
		return (error = "port not set", 1);
	if (get_root() == "")
		return (error = "root not set", 1);
	if (get_index() == "")
		return (error = "index not set", 1);
	if (get_max_client_body_size() == 0)
		return (error = "max_client_body_size is not set", 1);
	std::map<std::string, std::string>	cgis1 = get_cgis();
	for (std::map<std::string, std::string>::iterator cgi = cgis1.begin(); cgi != cgis1.end(); ++cgi) {
        std::ifstream file(cgi->second.c_str());
        if (!file.is_open()) {
            return (error = "cgi " + cgi->second + " is unavailable", 1);
        }
		file.close();
	}

	std::vector<Location> loc = this->get_locations();
	if (!loc.size())
		return (error = "No locations!", 1);
	for (std::vector<Location>::iterator locIt = loc.begin(); locIt != loc.end(); ++locIt)
	{
		if (!(*locIt).checkLocation())
			return (error = "Error: Invalid locations!", 1);
	}
	for (std::vector<Location>::iterator locIt = loc.begin(); locIt != loc.end(); ++locIt)
	{
		if ((*locIt).get_redirectionCode())
			this->redirectionPaths.push_back((*locIt).get_path());
	}
	return 0;
}

void	Config::sendError(int clientSocket, int errCode)
{
	std::string 		response;
	std::stringstream	body;

	std::ifstream	file;
	file.open( get_pages_paths()[errCode].c_str() );
	if ( file.is_open() )
	{
		body << file.rdbuf();
		//std::cout << "Body: " << body << std::endl;
	}
	else
	{
		body << "Error: " + create_map_return_codes()[errCode];
	}

	response = "HTTP/1.1 "
				+ create_map_return_codes()[errCode] + "\r\nContent-Length: " + toString(body.str().size()) + "\r\n\r\n" + body.str();
	if (send(clientSocket, response.c_str(), response.size(), 0)) {
		cout << "Sent error " << errCode << "\n";
	} else {
		cerr << "Failed sending error " << errCode << "\n";
	}
}

void	Config::setServerNames(const std::string name) {
		this->server_names.push_back(name);
		return ;
}

void 	Config::setCgi(const std::string ext, const std::string path) {
		this->cgis[ext] = path;
		return ;
}

void	Config::setErrorPage(const unsigned int code, const std::string path) {
		this->error_pages[code] = path;
		return ;
}

void 	Config::setIpAddress(std::string host) {
		this->ipAddress = host;
}

void	Config::setPort(unsigned int listenPort) {
		this->port = listenPort;
}

void	Config::setRoot(std::string dir) {
		this->root = dir;
}

void	Config::setIndex(std::string ind) {
		this->index = ind;
}

void	Config::setClientBodySize(long long size) {
		this->client_max_body_size = size;
}

void	Config::setAutoIndex(bool value) {
		this->autoIndex = value;
}

void	Config::setLocations(const Location &location) {
	this->locations.push_back(location);
	return ;
}

unsigned int	Config::get_port(void) const {
	return this->port;
}

std::vector<std::string>	Config::get_server_names(void) const {
	return this->server_names;
}

std::map<std::string, std::string>	Config::get_cgis(void) const {
	return this->cgis;
}

std::map<unsigned int, std::string>	Config::get_error_pages(void) const {
	return this->error_pages;
}

std::map<unsigned int, std::string>	Config::get_pages_paths(void) const {
	return this->err_codes;
}


std::string		Config::get_ipAddress(void) const {
	return this->ipAddress;
}

std::string		Config::get_root(void) const {
	return this->root;
}

std::string		Config::get_index(void) const {
	return this->index;
}

size_t		Config::get_max_client_body_size(void) const {
	return this->client_max_body_size;
}

std::vector<Location>	Config::get_locations(void) const {
	return this->locations;
}

bool	Config::get_autoindex(void) const {
	return this->autoIndex;
}

std::vector<std::string>	Config::get_redirectionPaths(void) const {
	return this->redirectionPaths;
}

std::string Config::create_default_error_page(const unsigned int &error_code)
{
	std::string str1;
	std::string line;
	std::string tempFile;
	std::stringstream number;
	number << error_code;
	std::string code = number.str();

	std::string message = create_map_return_codes()[error_code];
	std::string path = "default_error_pages/default.html";
	std::string filename = "default_" + code;
	std::string path1 = "default_error_pages/" + filename + ".html";
	std::ifstream file(path.c_str(), std::ios::in);
	std::ofstream file1(path1.c_str(), std::ios::out);

	if (!file.is_open() || !file1.is_open())
		return "";

	std::getline(file, tempFile, '\0');
	size_t pos = tempFile.find("message");
	while (pos != std::string::npos)
	{
		str1 = tempFile.substr(0, pos);
		str1 += message;
		str1 += tempFile.substr(pos + 7, tempFile.length());
		tempFile = str1;
		pos = tempFile.find("message");
	}
	file1 << tempFile;
	file1.close();
	file.close();

	return path1;
}

int Config::file_size(const std::string pathFile) {

    std::string filename = pathFile;

    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo! Não é possivel saber o size!" << std::endl;
        return 1;
    }
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();

    file.close();

    return fileSize;
}

std::map<unsigned int, std::string> Config::create_map_return_codes(void)
{
	std::map<unsigned int, std::string> codes;
	codes[100] = "100 Continue";
	codes[101] = "101 Switching Protocols";
	codes[200] = "200 OK";
	codes[201] = "201 Created";
	codes[202] = "202 Accepted";
	codes[203] = "203 Non-Authoritative Information";
	codes[204] = "204 No Content";
	codes[205] = "205 Reset Content";
	codes[300] = "300 Multiple Choices";
	codes[301] = "301 Moved Permanently";
	codes[302] = "302 Found";
	codes[303] = "303 See Other";
	codes[307] = "307 Temporary Redirect";
	codes[308] = "308 Permanent Redirect";
	codes[400] = "400 Bad Request";
	codes[403] = "403 Forbidden";
	codes[404] = "404 Not Found";
	codes[405] = "405 Method Not allow";
	codes[406] = "406 Not Acceptable";
	codes[408] = "408 Request Timeout";
	codes[409] = "409 Conflict";
	codes[410] = "410 Gone";
	codes[411] = "411 Length Required";
	codes[413] = "413 Payload Too Large";
	codes[414] = "414 URI Too Long";
	codes[415] = "415 Unsupported Media Type";
	codes[426] = "426 Upgrade Required";
	codes[500] = "500 Internal Server Error";
	codes[501] = "501 Not Implemented";
	codes[502] = "502 Bad Gateway";
	codes[503] = "503 Service Unavailable";
	codes[504] = "504 Gateway Timeout";
	codes[505] = "505 HTTP Version Not Supported";
	codes[508] = "508 Loop Detected";
	return codes;
}

void Config::ft_make_default_pages(void) {
	std::string filePath;
	std::map<unsigned int, std::string> codes = create_map_return_codes();
	this->err_codes = codes;
	for (std::map<unsigned int, std::string>::iterator it = codes.begin(); it != codes.end(); ++it)
	{
		if (it->first >= 400)
		{
			filePath = create_default_error_page(it->first);
			if (error_pages.size() > 0)
			{
				for (std::map<unsigned int, std::string>::iterator it1 = error_pages.begin(); it1 != error_pages.end(); ++it1)
				{
					if (it1->first == it->first)
					{
						std::string file_teste = "www/error_pages/" + it1->second;
						std::ifstream file2(file_teste.c_str());
						while (file_teste.find("//") != string::npos)
							file_teste.replace(file_teste.find("//"), 2, "/");
						// std::cout << "file: " << file2.is_open() << std::endl;
						if (file2.is_open())
							err_codes[it->first] = file_teste;
						else
							err_codes[it->first] = filePath;
					}
					else
						err_codes[it->first] = filePath;
				}
			}
			else {
				err_codes[it->first] = filePath;
			}
		}
	}
	// for (std::map<unsigned int, std::string>::iterator it1 = err_codes.begin(); it1 != err_codes.end(); ++it1)
	// {
	//  std::cout << it1->first << ": " << it1->second << std::endl;
	// }
	// return;
}
