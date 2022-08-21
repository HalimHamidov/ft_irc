#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include "server.hpp"
#include "user.hpp"

void	Setup(Server &server, struct pollfd fds[], int count)
{
	server.setCountConnects(1);
	for (size_t i = 0; i < (size_t)count; i++)
		fds[i].fd = -1;
	fds[0].events = POLLIN;
	fds[0].revents = 0;
}

int		main(int argc, char **argv){
	if (argc != 3) { error("Your executable will be run as follows: ./ircserv <port> <password>"); } 
	
	Server	server(atoi(argv[1]), std::string(argv[2]));
	struct pollfd fds[8]; 
	Setup(server, fds, 8);
	server.createSocket(server);
	server.bindSocket(server);
	server.listenSocket(server, fds);
	server.mainLoop(server, fds);
}