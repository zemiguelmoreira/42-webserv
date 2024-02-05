/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmaciel- <hmaciel-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/24 12:03:29 by hmaciel-          #+#    #+#             */
/*   Updated: 2023/10/24 14:27:49 by hmaciel-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WebServer.hpp"
#include <csignal>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <algorithm>

/* CONSTRUCTORS AND DETRUCTOR */

/**
 * @brief WebServer constructor.
 *
 * Initializes a WebServer object with the provided Server configuration.
 * Creates and configures the server socket, sets it to non-blocking mode,
 * and adds it to the epoll event loop for monitoring incoming connections.
 *
 * @param config Pointer to the Server configuration for the web server.
 */
WebServer::WebServer( Config *config, int epollFd ) : config( config ), epollFd(epollFd)
{
	std::memset(&server_ev, 0, sizeof(struct sockaddr_in));
	serverSocket = createServerSocket( );
}

WebServer::WebServer() {
	return;
}

// Copy constructor
WebServer::WebServer(const WebServer &src) {
	*this = src;
	return;
}

// Copy overload
WebServer &WebServer::operator=(const WebServer &opRight) {
	if (this != &opRight)
		return *this;
	return *this;
}

// Destructor
WebServer::~WebServer()
{
	close(serverSocket);

	return ;
}

void	WebServer::run( int fd )
{
	if ( fd == serverSocket )
	{
		// Handle new incoming connection
		handleNewConnection( );
	}
	else
	{
		if ( std::find(sockets.begin(), sockets.end(), fd) != sockets.end())
			handleClientRequest( fd );
	}
}

/** @brief Create and configure the server socket for the web server.
 *
 * This function creates a socket, sets it up with the specified port and address,
 * enables the SO_REUSEADDR option, binds it to the specified address, and starts
 * listening for incoming connections. Additionally, it prints information about
 * the server creation to the console.
 *
 * @param port The port number to bind the server socket to.
 * @return The file descriptor of the created server socket.
 */
int WebServer::createServerSocket( )
{
	// Create a socket
	int serverSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if (serverSocket == -1)
	{
		perror( "Error creating server socket" );
		std::exit(1);
	}

	// Configure server address structure
	struct sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(struct sockaddr_in));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons( config->get_port() );

	// Enable the SO_REUSEADDR option to allow immediate reuse of the address
	const int trueFlag = 1;
	setsockopt( serverSocket, SOL_SOCKET, SO_REUSEADDR, &trueFlag, sizeof(int) );

	// Bind the socket to the specified address and port
	if ( bind( serverSocket, ( struct sockaddr* )&serverAddr, sizeof( serverAddr ) ) == -1 )
	{
		perror( "Error binding server socket" );
		std::exit( 1 );
	}

	// Start listening for incoming connections with a maximum backlog of clients
	if ( listen( serverSocket, MAX_CLIENTS ) == -1 )
	{
		perror( "Error listening on server socket" );
		std::exit( 1 );
	}

	server_ev.events = EPOLLIN;
    server_ev.data.fd = serverSocket;

	// std::cout << "epollFd na criacao do server: " << epollFd << std::endl;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocket, &server_ev) == -1) {
			perror("epoll_ctl: listen_sock");
			std::exit(EXIT_FAILURE);
    }
	std::cout << "Server created at: " << BGRN << config->get_server_names()[0] << ":" << config->get_port() << " - fd: " << serverSocket << RESET << std::endl;

	return serverSocket;
}

/** @brief Add a socket to the epoll event loop for monitoring specific events.
 *
 * This function adds the specified socket to the epoll event loop for monitoring
 * events specified by the `events` parameter. It creates an epoll event structure,
 * associates the socket with the desired events, and adds it to the epoll file descriptor.
 *
 * @param fd The file descriptor of the socket to be added to the epoll event loop.
 * @param events The events to be monitored for the specified socket (EPOLLIN, EPOLLOUT, etc.).
 */
void	WebServer::addSocketToEpoll( int fd, struct epoll_event ev )
{
	// Add the socket to the epoll event loop
	if ( epoll_ctl( epollFd, EPOLL_CTL_ADD, fd, &ev) == -1 )
	{
		perror( "Error adding socket to epoll" );
		//exit( 1 );
	}
}

void	WebServer::removeSocketFromEpoll( int clientSocket)
{
	if (epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSocket, NULL) == -1) {
    	perror("epoll_ctl");
		return ;
        //exit(EXIT_FAILURE);
	}
	// if ( epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSocket, &ev) == -1 )
	// 	std::cout << BRED << "Error removing client socket from epoll" << std::endl;
	timeouts.erase(clientSocket);
	sockets.erase( std::remove( sockets.begin(), sockets.end(), clientSocket ), sockets.end() );
	webHandler.erase(clientSocket);
	if ( close( clientSocket ) == -1 )
	{
		std::cerr << BRED << "Error to close fd" << std::endl;
	}
	// else {
	// 	std::cout << "--- CONNECTION ENDED ---\n";
	// 	std::cout << "Socket FD: " << clientSocket << "\n";
	// }
}

/** @brief Handle a new incoming connection.
 *
 * This function is called when a new connection is accepted by the server.
 * It accepts the incoming connection, sets the new socket to non-blocking mode,
 * adds it to the epoll event loop for monitoring read events, and adds it to
 * the list of client sockets.
 */
void WebServer::handleNewConnection( )
{

	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof( clientAddr );

	// Accept the incoming connection
	int newSocket = accept( serverSocket, ( struct sockaddr* )&clientAddr, &clientAddrLen );
	// std::cout << "newSocket: " << newSocket << std::endl;

	if (newSocket != -1)
	{
		// std::cout << "--- NEW CONNECTION ---\n";
		// std::cout << "Socket FD: " << newSocket << "\n";
		setNonBlocking( newSocket );

		struct epoll_event			client_ev;
		std::memset(&client_ev, 0, sizeof(struct sockaddr_in));

		client_ev.events = EPOLLIN;// | EPOLLOUT | EPOLLET;
		client_ev.data.fd = newSocket;
		timeouts[newSocket] = time(NULL);
		addSocketToEpoll( newSocket, client_ev );
		sockets.push_back( newSocket );//
	} else {
		perror("Accept");
	}
}

/** @brief Handle a client request on an existing connection.
 *
 * This function reads data from the client socket in a non-blocking manner,
 * appending it to the request data until a complete request is received.
 * It then parses the request using the web handler and performs further
 * processing based on the received data. Finally, it closes the client socket
 * after processing the request.
 *
 * @param clientSocket The file descriptor of the client socket.
 */
void WebServer::handleClientRequest(int &clientSocket)
{
	if (webHandler.find(clientSocket) == webHandler.end()) {
		webHandler.insert(std::make_pair(clientSocket, HandleData(clientSocket, config)));
	}
	if (webHandler[clientSocket].readFromSocket()) {
		removeSocketFromEpoll(clientSocket);
	}
	else
	{
		timeouts[clientSocket] = time(NULL);
	}
}

std::map<int, time_t>		WebServer::getTimeouts()
{
	return ( this->timeouts );
}
