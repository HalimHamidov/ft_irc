#ifndef USER_HPP
# define USER_HPP
#include "rfc2812rpl.hpp"
#include "server.hpp"
#include "command.hpp"

class Server;

class User {

	private:
	string	_nickname;
	string	_username;
	int		_sockfd;
	
	int		_passwordPassed;
	int		_nickNamePassed;
	int		_userPassed;

	void checkUserPassword(Server &server, string message, int i);

	public:
	User();
	User(int fd);
	~User();
	
	void	setUsername(string username);
	void	setNickname(string nickname);
	void	setFd(int fd);

	void	setPasswordPassed();
	void	setNicknamePassed();
	void	setUserPassed();
	
	int		getFd();
	int		getPasswordPassed();
	int		getNickNamePassed();
	int		getUserPassed();

	string	getUsername();
	string	getNickname();
	bool	getAllPrepArguments();

	int		preparationCommands(Server &server, string message, int i);
	int		parsNickCommand(Server &server, string message, int i);
	int		parsUserCommand(Server &server, string message, int i);
	int		parsCommand(Server &server, string message, int i, struct pollfd fds[]); 

	void sendError(string err);
};

#endif