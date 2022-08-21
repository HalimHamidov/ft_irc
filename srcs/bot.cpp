#include "command.hpp"
#include <chrono>
#include <time.h>

void	Command::doHelpCommand(){
	if (_arguments.size() == 1)
		send(_fd, "BOT COMMANDS: HELP, SHOWTIME, INFO, SHOWUSER\r\n", 46, 0);
	else
		send(_fd, "BOT ERROR Wrong syntax\r\n", 25, 0);
}

void	Command::doInfoCommand(){
	string showInfo = ": BOT NICKNAME is [" + _nickname + "]\r\n:BOT USERNAME IS [" + _username + "]\r\n";
	send(_fd, showInfo.c_str(), showInfo.length() + 1, 0);
}

void	Command::doShowuserCommand(Server &server){
	if (_arguments.size() != 2){
		send(_fd, ":BOT Syntax is: BOT SHOWUSER <id>\r\n", 36, 0);
		return ;
	}
	
	try{
		int id = stoi(_arguments[1]);
		vector<User> tmpVectorOfUsers = server.getVectorOfUsers();

		string infoToSend;
		int numberOfUsers = tmpVectorOfUsers.size();
		string stringNumberOfUsers = ":BOT SHOWUSERS NUMBER OF USERS [" + std::to_string(numberOfUsers) + "]\r\n";
		if (id > numberOfUsers - 1 || id < 0){
			send(_fd, ":BOT Out of range!\r\n", 21, 0);
			throw std::invalid_argument("Out of range");
		}

		string infoAboutUserID = ":BOT USER ID [" + std::to_string(id) + "]\r\n";
		string infoAboutUserUsername = ":BOT USER USERNAME [" + tmpVectorOfUsers[id].getUsername() + "]\r\n";
		string infoAboutUserNickname = ":BOT USER NICKNAME [" + tmpVectorOfUsers[id].getNickname() + "]\r\n";
		infoToSend = stringNumberOfUsers + infoAboutUserID + infoAboutUserUsername + infoAboutUserNickname;
		send(_fd, infoToSend.c_str(), infoToSend.length() + 1, 0);
	} catch(std::invalid_argument& e) { send(_fd, "BOT Send integer!\r\n", 20, 0); }
	
}

void	Command::doShowtimeCommand(){
	time_t givemetime = time(NULL);
	string timeNow = ":BOT Time is " + string(ctime(&givemetime));
	send(_fd, timeNow.c_str(), timeNow.length() + 1, 0);
}