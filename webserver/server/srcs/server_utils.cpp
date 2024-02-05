/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdias-mo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2024/02/05 19:33:04 by jdias-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/server_utils.hpp"

void	setNonBlocking( int fd )
{
	if ( fd < 3 )
	{
		std::cerr << "Can't set non-blocking in this fd" <<std::endl;
	}
	int flags = fcntl( fd, F_GETFL, 0 );
	fcntl( fd, F_SETFL, flags | O_NONBLOCK );
}

bool	ends_with( std::string const & value, std::string const & ending )
{
	if ( ending.size() > value.size() )
		return false;
	return ( std::equal( ending.rbegin(), ending.rend(), value.rbegin() ) );
}

size_t	count_lines( std::string const &str )
{
	std::string			line;
	std::stringstream	buf;
	size_t 				lines_nb;

	lines_nb = 0;
	if ( str.size() > 0 )
	{
		while ( getline( buf, line ) )
			lines_nb++;
	}
	return ( lines_nb );
}

bool	has_forbidden(  std::string const &method, std::string uri, std::vector<Location> locations )
{
	std::vector<std::string> allows;
	// std::cout << "uri: " << uri << std::endl;
	// std::cout << "request: " << method << std::endl;
	bool	found = false;

	for ( size_t i = 0; i < locations.size(); i++ )
	{
		// std::cout << "locations path:" << locations[i].get_path() << std::endl;
		if (uri.find(locations[i].get_path()) != std::string::npos)
		{
			allows = locations[i].get_allow();
			found = true;
		}
	}

	std::vector<std::string>::iterator it_allows;

	if (!found && allows.size() <= 0 && method == "GET")
		return (false);

	if (allows.size() <= 0)
		return ( true );
	for ( it_allows = allows.begin(); it_allows != allows.end(); it_allows++ )
	{
		if ( method == *it_allows )
			return ( false );
	}
	return ( true );
}

bool	isExe(char *path) {
	if (!access(path, X_OK)) {
		return true;
	} else {
		if (errno == ENOENT) {
			cerr << "Error: File does not exist" << "\n";
		} else if (errno == EACCES) {
			cerr << "Error: Permission denied" << "\n";
		} else {
			cerr << "Error: Unable to determine execute permission" << "\n";
		}
		return (0);
	}
}

bool	isDir(const char *path) {
	DIR* dir = opendir(path);
	std::string dirStr(path);
	if (dir != NULL && dirStr != "/") {
		closedir(dir);
		cerr << "Error: Path is a directory\n";
		return (true);
	}
	return (false);
}

bool	fileExists(const char* filename) {
	return access(filename, F_OK) == 0;
}

std::string toString(int n) {
	std::ostringstream oss;
	oss << n;
	return oss.str();
}

char	fromHex(char c) {
	return isdigit(c) ? c - '0' : tolower(c) - 'a' + 10;
}

std::string	decode(const std::string& input) {
	std::ostringstream decoded;

	for (size_t i = 0; i < input.length(); ++i) {
		if (input[i] == '%' && i + 2 < input.length() && isxdigit(input[i + 1]) && isxdigit(input[i + 2])) {
			char high = fromHex(input[i + 1]);
			char low = fromHex(input[i + 2]);
			decoded.put((high << 4) | low);
			i += 2;
		} else if (input[i] == '+') {
			decoded.put(' ');
		} else {
			decoded.put(input[i]);
		}
	}
	return decoded.str();
}
