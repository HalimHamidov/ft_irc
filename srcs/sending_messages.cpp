#include "rfc2812rpl.hpp"

// message output  <nick>!<user>@<host> PRIVMSG <userx> :Hi\r\n”

void	MessageDeliveryFormat(int fd, string nickname, string username, string message){
	string messageToSend = ":" + nickname + "!" + username + "@127.0.0.1 " + message + "\r\n";
	send(fd, messageToSend.c_str(), messageToSend.length() + 1, 0);
}

bool	contains(string array[], string message){
	for (size_t i = 0; i < array->length(); i++){
		if (array[i] == message) {
			std::cout << message << std::endl;
			return (true);
		}
	}
	return (false);
}

void	error(string error){
	std::cout << error << std::endl;
	exit(EXIT_FAILURE);
}