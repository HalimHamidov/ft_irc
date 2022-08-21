#include "command.hpp"

Command::Command() { }

Command::Command(string message, int fd, string nickname, string username, vector<User> &users) : _fd(fd), _message(message),  _username(username), _nickname(nickname),  _users(users) {
	if (!message.empty()){	
		std::istringstream stringToSplit(message.c_str());
		string stringSplitted;
	
		while (getline(stringToSplit, stringSplitted, ' ' ) && stringSplitted != " ")
			_arguments.push_back(stringSplitted);
		_command = _arguments[0];
		_command.erase(std::remove(_command.begin(), _command.end(), '\r'), _command.end());
		_command.erase(std::remove(_command.begin(), _command.end(), '\n'), _command.end());
		_arguments.erase(_arguments.begin());

		for (vector<string>::iterator it = _arguments.begin(); it != _arguments.end(); it++)
			(*it).erase(std::remove((*it).begin(), (*it).end(), '\r'), (*it).end());

		for (vector<string>::iterator it = _arguments.begin(); it != _arguments.end(); it++)
			(*it).erase(std::remove((*it).begin(), (*it).end(), '\n'), (*it).end());
	}
}

vector<User>		Command::getVectorOfUsers() { return (_users); }

int		Command::commandStart(Server &server, struct pollfd fds[]){
	string	commands[] = {"NICK", "QUIT", "PRIVMSG", "NOTICE", "HELP", "JOIN", "KICK", "BOT"};
	if (std::find(std::begin(commands), std::end(commands), _command) != std::end(commands)){
		checkCommand(server, fds);
		return (1);
	}

	return (0);
}

void	Command::checkCommand(Server &server, struct pollfd fds[]){
	
	if (_command == "QUIT") doQuitCommand(server, fds);
	else if (_command == "NICK") doNickCommand(server);
	else if (_command == "PRIVMSG") doPrivmsgCommand(server);
	else if (_command == "NOTICE") doNoticeCommand(server);
	else if (_command == "JOIN") doJoinCommand(server);
	else if (_command == "KICK") doKickCommand(server);

	if (_command == "BOT" && _arguments[0] == "HELP") doHelpCommand();
	if (_command == "BOT" && _arguments[0] == "INFO") doInfoCommand();
	if (_command == "BOT" && _arguments[0] == "SHOWUSER") doShowuserCommand(server);
	if (_command == "BOT" && _arguments[0] == "SHOWTIME") doShowtimeCommand();
}

void	Command::doNoticeCommand(Server &server){
	if (_arguments[0] == "*")
		return ;
	doPrivmsgCommand(server); 
}

void	Command::doQuitCommand(Server &server, struct pollfd fds[]){
	std::cout << fds[server.getId() + 1].fd << "  disconnected" << std::endl;
	fds[server.getId() + 1].fd = -1;
	
	vector<User> tmpVector = server.getVectorOfUsers();
	tmpVector.erase(tmpVector.begin() + server.getId());
	server.usersVectorSetNew(tmpVector);
	server.setCountConnects(-1);
	
	close(_fd);
}

void	Command::doNickCommand(Server &server){
	string newNick = _arguments[0];
	newNick.erase(std::remove(newNick.begin(), newNick.end(), '\n'), newNick.end());
	vector<User> tmpVector = server.getVectorOfUsers();
	
	for (vector<User>::iterator it = tmpVector.begin(); it != tmpVector.end(); it++){
		if ((*it).getNickname() == newNick){
			string InUse = ERR_NICKNAMEINUSE(newNick);
			send(_fd, InUse.c_str(), InUse.length() + 1, 0);
			return ;
		}
	}
	server.setNicknameByUser(newNick, server.getId());
}

void	Command::doPrivmsgCommand(Server &server){

	if (_arguments.size() < 2){ 
		send(_fd, ERR_NEEDMOREPARAMS(string("PRIVMSG")).c_str(), ERR_NEEDMOREPARAMS(string("PRIVMSG")).length() + 1, 0);
		return ; 
	}

	bool userExist = false;
	int fdToPm;
	for (vector<User>::iterator it = _users.begin(); it != _users.end(); it++){
		if ((*it).getNickname() == _arguments[0]){
			userExist = true;
			fdToPm = (*it).getFd();
			break;
		}
	}
	
	bool channelExist = false;
	Channel tmpChannel;
	vector<Channel> tmpVector = server.getVectorOfChannels();
	for(vector<Channel>::iterator it = tmpVector.begin(); it != tmpVector.end(); it++){
		if ((*it).getChannelName() == _arguments[0]){
			channelExist = true;
			tmpChannel = (*it);
			break;
		}
	}

	if (userExist)
		MessageDeliveryFormat(fdToPm, _nickname, _username, _message);
	else if (channelExist) 
		tmpChannel.doChannelPrivmsg(_fd, _message, _nickname, _username);
	else {
		send(_fd, ERR_NORECIPIENT(string("PRIVMSG")).c_str(), ERR_NORECIPIENT(string("PRIVMSG")).length() + 1, 0);
		return ;
	}
}

Command::~Command() { }
