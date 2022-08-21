#include "user.hpp"

User::User() :   _passwordPassed(0), _nickNamePassed(0), _userPassed(0)  { };
User::User(int fd) :  _sockfd(fd), _passwordPassed(0), _nickNamePassed(0), _userPassed(0)  {}

void		User::setFd(int fd) { _sockfd = fd; }
void		User::setUsername(string username) { _username = username; }
void		User::setNickname(string nickname) { _nickname = nickname; }
void		User::setPasswordPassed() { _passwordPassed = 1; }
void		User::setNicknamePassed() { _nickNamePassed = 1; }
void		User::setUserPassed() { _userPassed = 1; }
int			User::getFd() { return(_sockfd); }
int			User::getPasswordPassed() { return(this->_passwordPassed); }
int			User::getUserPassed() { return(this->_userPassed); }
int			User::getNickNamePassed() { return(this->_nickNamePassed); }
bool		User::getAllPrepArguments() { return _passwordPassed && _nickNamePassed && _userPassed ? true : false; }
string		User::getUsername() { return(_username); }
string		User::getNickname() { return(_nickname); }

int			User::parsCommand(Server &server, string message, int i, struct pollfd fds[]){
	bool allPrepIsDone = server.getUser(i).getAllPrepArguments();

	if (!allPrepIsDone)
		return server.getUser(i).preparationCommands(server, message, i);
	vector<User> newVector = server.getVectorOfUsers();
	Command command(message, server.getUser(i).getFd(), server.getUser(i).getNickname(), server.getUser(i).getUsername(), newVector);
	return command.commandStart(server, fds);
}

string		getFirstWord(string message){
	string firstWord;
	if (!message.empty()){	
		std::istringstream stringToSplit(message.c_str());
		string stringSplitted;
	
		while (getline(stringToSplit, stringSplitted, ' ' ) && stringSplitted != " "){
			firstWord = stringSplitted;
			break ;
		}
		firstWord.erase(std::remove(firstWord.begin(), firstWord.end(), '\r'), firstWord.end());
		firstWord.erase(std::remove(firstWord.begin(), firstWord.end(), '\n'), firstWord.end());
	}
	return firstWord;
}

vector<string>		getParametrs(string message){
	vector<string> parametrs;
	if (!message.empty()){	
		std::istringstream stringToSplit(message.c_str());
		string stringSplitted;
	
		while (getline(stringToSplit, stringSplitted, ' ' ) && stringSplitted != " ")
			parametrs.push_back(stringSplitted);
		parametrs.erase(parametrs.begin());
		
		for (vector<string>::iterator it = parametrs.begin(); it != parametrs.end(); it++)
			(*it).erase(std::remove((*it).begin(), (*it).end(), '\r'), (*it).end());
		
		for (vector<string>::iterator it = parametrs.begin(); it != parametrs.end(); it++)
			(*it).erase(std::remove((*it).begin(), (*it).end(), '\n'), (*it).end());
	}
	return parametrs;
}

int			User::preparationCommands(Server &server, string message, int i){
    static int onlyOnce = 0;
	vector<string> parametrs = getParametrs(message);
	string firstWord = getFirstWord(message);

	if (!onlyOnce && parametrs.size() > 0){
		if (parametrs[0] == "LSPING") {
            string pong = "PONG " + parametrs[1] + "\r\n";
            send(_sockfd, pong.c_str(), pong.length() + 1, 0);
        }
        ++onlyOnce;
    }
	
	if (server.getUser(i).getPasswordPassed() == 0 && firstWord == "PASS"){
		server.getUser(i).checkUserPassword(server, message, i);
		return (1);
	} else if (server.getUser(i).getPasswordPassed() == 0 && firstWord != "PASS") {
		sendError(ERR_NOTREGISTERED);
		return (1);
	}

	bool NickUserPassed = server.getUser(i).getUserPassed() && server.getUser(i).getNickNamePassed() ? true : false;
	if (!NickUserPassed){
		if (firstWord == "NICK\n" || firstWord == "NICK")
			return(server.getUser(i).parsNickCommand(server, message, i));
		else if (firstWord == "USER\n" || firstWord == "USER")
			return(server.getUser(i).parsUserCommand(server, message, i));
		else{
			sendError(ERR_NOTREGISTERED);
			return (1);
		}
	} 
	return (0);
}

int				checkIdentity(Server &server, vector<string> arguments, int fd){
	string newNick = arguments[0];
	vector<User> tmpVector = server.getVectorOfUsers();

	if (newNick.length() <= 0 || newNick.length() >= 9) {
		string err = ERR_ERRONEUSNICKNAME(newNick);
		send(fd, err.c_str(), err.length() + 1, 0);
		return 1;
	}
	
	for (vector<User>::iterator it = tmpVector.begin(); it != tmpVector.end(); it++){
		if ((*it).getNickname() == newNick){
			string InUse = ERR_NICKNAMEINUSE(newNick);
			send(fd, InUse.c_str(), InUse.length() + 1, 0);
			return 1;
		}
	}

	return 0;
}	

int				User::parsNickCommand(Server &server, string message, int i){
	vector<string>	parametrs = getParametrs(message);
	if (parametrs.size() == 0){
		sendError(ERR_NEEDMOREPARAMS(string("NICK")));
		return (1);
	}
	int error = checkIdentity(server, parametrs, _sockfd);
	if (error)
		return 1;
	
	server.setNicknameByUser(parametrs[0], i);
	server.setNicknamePassedByUser(i);
	if (GET_USER_PASSED){
		send(_sockfd, NEW_USER(_nickname, _username).c_str(), NEW_USER(_nickname, _username).length() + 1, 0);
		SEND_ABOUT_NEW_USER;
	}
	return (1);
}

int				User::parsUserCommand(Server &server, string message, int i){
	vector<string>	parametrs = getParametrs(message);
	
	if (parametrs.size() == 0){
		sendError(ERR_NEEDMOREPARAMS(string("USER")));
		return (1);
	}
	server.setUsernameByUser(parametrs[0], i);
	server.setUserPassedByUser(i);
	if (GET_NICK_PASSED){
		send(_sockfd, NEW_USER(_nickname, _username).c_str(), NEW_USER(_nickname, _username).length() + 1, 0);
		SEND_ABOUT_NEW_USER;
	}
	return (1);
}

void			User::checkUserPassword(Server &server, string message, int i){
	vector<string>	parametrs = getParametrs(message);
	if (parametrs.size() == 0){
		sendError(ERR_NEEDMOREPARAMS(string("PASS")));
		return ;
	}

	if (parametrs[0] == std::string(server.getPassword()))
		server.setPasswordPassedByUser(i);
	else
		sendError(ERR_PASSWDMISMATCH);
}

void	User::sendError(std::string err) { send(_sockfd, err.c_str(), err.length() + 1, 0); }

User::~User() { };