#pragma once
#include "rfc2812rpl.hpp"
#include "user.hpp"
#include "server.hpp"
#include "channel.hpp"

class User;
class Server;
class Channel;

class Command{
	private:
	int				_fd;
	int				_i;
	string			_message;
	string			_username;
	string			_nickname;
	string			_command;

	vector<User>	_users;
	vector<string>	_arguments;

	public:
	Command();
	Command(string message, int fd, string nickname, string username, vector<User> &users);
	~Command();

	vector<User>	getVectorOfUsers();

	int		commandStart(Server &server, struct pollfd fds[]);
	void	checkCommand(Server &server, struct pollfd fds[]);
	void	doQuitCommand(Server &server, struct pollfd fds[]);
	void	doNickCommand(Server &server);
	void	doPrivmsgCommand(Server &server);
	void	doNoticeCommand(Server &server);
	void	doJoinCommand(Server &server);
	void	doKickCommand(Server &server);
	void	doHelpCommand();
	void	doInfoCommand();
	void	doShowuserCommand(Server &server);
	void	doShowtimeCommand();
	void	createNewChannel(Server &server);
	bool	checkUserInChannel(Channel &channel);
};


