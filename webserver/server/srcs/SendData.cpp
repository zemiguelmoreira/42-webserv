/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SendData.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdias-mo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 09:20:09 by hmaciel-          #+#    #+#             */
/*   Updated: 2024/02/05 19:32:39 by jdias-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/SendData.hpp"

SendData::SendData()
{
	return ;
}
SendData::~SendData()
{
	return ;
}

int	SendData::sendData(std::string data, int &clientSocket) {
	if (data.empty()) {
		return (1);
	}
	size_t totalSent = 0;
	while (totalSent < data.length()) {
		size_t remaining = data.length() - totalSent;
		size_t currentChunkSize = std::min(static_cast<size_t>(SEND_CHUNK_SIZE), remaining);
		ssize_t sentBytes = send(clientSocket, data.c_str() + totalSent, currentChunkSize, MSG_NOSIGNAL);
		if (sentBytes == -1) {
			std::cerr << "Send return: -1 Sent: " << totalSent << "\n";
			return (1);
		}
		totalSent += static_cast<size_t>(sentBytes);
	}
	std::cout << "Sent: " << totalSent << "\n";
	return (0);
}

void	SendData::sendBinaryData( int &clientSocket, std::string mimeType , std::string requestURI, std::string const &workDir )
{
	// std::cout << "----------------------------------------" << std::endl;
	// std::cout << "Sending Binary Data: " << std::endl;
	std::ifstream		file;
	std::string			fileName = workDir + requestURI;
	std::string response;

	file.open( fileName.c_str(), std::ios::binary | std::ios::ate );
	if ( file.is_open() )
	{
		// Verifica o tamanho do arquivo em um ifstring
		std::streamsize size = file.tellg();
		file.seekg( 0, std::ios::beg );

		size_t	finalSize = (size_t)size;
		ssize_t totalSentBytes = 0;
		// Tamanho do chunk (porção)
		const std::streamsize chunkSize = 1;
		char chunk[chunkSize];
		response = "HTTP/1.1 200 OK\r\n" + mimeType + "\r\n" + "Content-Length: " + myToString( size ) + "\r\n\r\n";
		// std::cout << response << std::endl;
		if (send( clientSocket, response.c_str(), response.length(), MSG_NOSIGNAL ) == -1) {
			cerr << "Send failed\n";
		}
		while (size > 0)
		{
			std::streamsize bytesRead = file.read(chunk, chunkSize).gcount();
			if ( bytesRead > 0 )
			{
				ssize_t sentBytes;
				size_t remainingBytes = bytesRead;

				// Loop de tentativa de reenvio em caso de erro
				while ( remainingBytes > 0 ) {
					sentBytes = send( clientSocket, chunk, remainingBytes, MSG_NOSIGNAL );
					if (sentBytes < 0) {
						std::cerr << "Error: send: " << strerror(errno) << std::endl;
						break ;
					} else if (sentBytes == 0) {
						// std::cout << "A conexão foi interrompida pelo cliente, breaking loop" << std::endl;
						break; // Sai do loop se a conexão foi interrompida pelo cliente
					} else {
						totalSentBytes += sentBytes;
						remainingBytes -= sentBytes;
					}
				}
				if (remainingBytes > 0) {
					// Lidar com erro de envio final
					break;
				}
				size -= bytesRead; // Reduz o tamanho restante do arquivo
			}
			else
			{
				// std::cout << "nothing to read from file, breaking loop" << std::endl;
				break; // Não há mais dados para ler do arquivo
			}
		}
		file.close();

		//output debug
		// std::cout << "totalSize: " << finalSize << " - Total Enviado: " << totalSentBytes << std::endl;
		if ((size_t)totalSentBytes == finalSize)
		{
			// std::cout << "Arquivo enviado com sucesso!" << std::endl;
		}
		else
		{
			// std::cout << "Erro no envio do arquivo" << std::endl;
		}
	}
	else
	{
		std::ifstream		file;
		std::string 		response;
		std::stringstream 	buff;

		// std::cout << "404" << std::endl;
		file.open( "www/error_pages/404.html" );
		buff << file.rdbuf();
		response = "HTTP/1.1 404 NOT FOUND\r\n" + mimeType + "\r\n\r\n";

		// send( clientSocket, response.c_str(), response.length(), MSG_NOSIGNAL );
		//send( clientSocket, buff.str().c_str(), buff.str().length(), MSG_NOSIGNAL );
		if (send( clientSocket, response.c_str(), response.length(), MSG_NOSIGNAL ) == -1)
			cerr << "Erro send binary data, moving on..." << std::endl;
		if (send( clientSocket, buff.str().c_str(), buff.str().length(), MSG_NOSIGNAL ) == -1)
			cerr << "Erro send binary data, moving on..." << std::endl;

		file.close();

	}
	// std::cout << "----------------------------------------" << std::endl;
}

std::string	SendData::getResponseFromFolder( std::string folder, std::string const &workDir )
{
	DIR *dir;
	struct dirent *dp;
	( void )workDir;

	std::string	response;
	// std::cout << "Folder inside get dir response: " << folder << std::endl;
	std::string fullPath = workDir + folder;
	// std::cout << "full path inside get dir response: " << fullPath << std::endl;

	dir = opendir ( fullPath.c_str() );

	if ( folder[ folder.length() -1 ] == '/' )
		folder.erase( folder.size() - 1 );

	while ( (dp = readdir (dir) ) != NULL )
	{
		std::string link;
		std::string fileName( dp->d_name );
		std::string pathToFile = folder + "/" + fileName;
		// std::cout << "path to file: " << pathToFile << std::endl;
		link = "<a href=\"" + pathToFile  + "\">" + fileName + "</a>" + "<br />";
		response.append(link);
	}
	closedir( dir );

	// std::cout << "dir response: " << response << std::endl;
	return ( response );
}

void	SendData::sendDirData( int &clientSocket, std::string mimeType , std::string requestURI, std::string const &workDir )
{
	std::string	response;
	(void)workDir;
	response = "HTTP/1.1 200 OK\r\n" + mimeType + "\r\n" + "Content-type: text/html\r\n\r\n";
	if (send( clientSocket, response.c_str(), response.length(), MSG_NOSIGNAL ) == -1) {
		cerr << "Send failed\n";
	}
	std::string dirContent = getResponseFromFolder( requestURI, workDir );

	// std::cout << "dir content before send: " << dirContent << std::endl;
	if (send( clientSocket, dirContent.c_str(), dirContent.length(), MSG_NOSIGNAL ) == -1) {
		cerr << "Send failed\n";
	}
}

bool	SendData::checkIsDir( std::string folder, std::string const &workDir )
{
	// std::cout << "Identificando diretorio" << std::endl;
	DIR *dir;
	bool ret = false;
	std::string	fullPath;

	// std::cout << "folder: " << folder << std::endl;
	// std::cout << "workdir: " << workDir << std::endl;
	if ( folder.find( workDir ) == std::string::npos )
		fullPath = workDir + folder;
	else
		fullPath = folder;

	// std::cout << "fullpath: " << fullPath << std::endl;
	if ( (dir = opendir ( fullPath.c_str() ) ) != NULL )
	{
		ret = true;
	}
	closedir( dir );
	// std::cout << "Is Dir: " << ret << std::endl;
	return ( ret );
}

// bool		SendData::sendError(int &clientSocket, std::string requestURI, std::string code, Config *config)
// {
// 	;
// 		// std::cout << "contentL: " << _contentLength << "config lenght: " << _config->get_max_client_body_size() << std::endl;
// 		// if (_contentLength > _config->get_max_client_body_size())
// 		// {
// 		// 	std::cout << "identificado tamanho invalido" << std::endl;
// 		// 	std::string response = "HTTP/1.1 400 Morreu\r\n\r\nhahaha";
// 		// 	std::cout << response << std::endl;
// 		// 	send(_clientSocket, response.c_str(), response.size(), 0);
// }
// bool		SendData::sendRedirect(int &clientSocket, std::string requestURI, std::string code, Config *config)
// {
// 	if (requestURI == "/cgi.html")
// 	{
// 		std::cout << "Redirection localizado!" << std::endl;
// 		std::string response = "HTTP/1.1 301 Moved\r\nLocation: /team.html\r\n\r\n";
// 		std::cout << response << std::endl;
// 		send(clientSocket, response.c_str(), response.size(), 0);
// 		return ;
// 	}
// }
