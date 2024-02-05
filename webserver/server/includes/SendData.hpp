/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SendData.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdias-mo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 09:14:17 by hmaciel-          #+#    #+#             */
/*   Updated: 2024/02/01 13:31:05 by jdias-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# define SEND_CHUNK_SIZE 1472

# include <dirent.h>
# include <iostream>
# include <sstream>
# include <fstream>
# include <sys/socket.h>
# include "server_utils.hpp"

class SendData
{
	private:
		std::string	getResponseFromFolder( std::string folder, std::string const &workDir);

	public:
		SendData();
		~SendData();

		void		sendBinaryData( int &clientSocket, std::string mimeType , std::string requestURI, std::string const &workDir );
		void		sendDirData( int &clientSocket, std::string mimeType , std::string requestURI, std::string const &workDir );
		bool		checkIsDir( std::string folder, std::string const &workDir );
		int		sendData(std::string data, int &clientSocket);
		// bool		sendError(int &clientSocket, std::string requestURI, std::string code, Config *config);
		// bool		sendRedirect(int &clientSocket, std::string requestURI, std::string code, Config *config);

};
