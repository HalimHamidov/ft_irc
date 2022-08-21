#pragma once

#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sstream>

using std::string;
using std::vector;

#define BUFFER_SIZE 4096

#define ERR_NOTREGISTERED "451 *  :You have not registered\r\n"
#define ERR_UNKNOWNCOMMAND(command) ("421 *  " + command + " :Unknown command")
#define ERR_NEEDMOREPARAMS(command) ("461 *  " + command + " :Not enough parameters\n")
#define ERR_PASSWDMISMATCH "464 *  :Password incorrect\n"
#define ERR_ALREADYREGISTRED "462 *  :You may not reregister\n"
#define ERR_ERRONEUSNICKNAME(nick) ("432 *  " + nick + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(nick) ("433 *  " + nick + " :Nickname is already in use\r\n")
#define ERR_NORECIPIENT(command) ("411 * :No recipient given " + command + "\n")
#define ERR_NOTEXTTOSEND "412 *  :No text to send\n"
#define ERR_CANNOTSENDTOCHAN(channel) ("404 *  " + channel " :Cannot send to channel\n")
#define ERR_NOSUCHNICK(nick) ("401 *  " + nick + " :No such nick/channel\n")
#define ERR_CHANNELISFULL(channel) ("471 *  " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_TOOMANYCHANNELS(channel) ("405 *  " + channel + " :You have joined too many channels\r\n")
#define ERR_BADCHANNELKEY(channel) ("475 * " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_NOSUCHCHANNEL(channel) ("403 *  " + channel + " :No such channel\n")
#define ERR_NOTONCHANNEL(channel) ("442 *  " + channel + " :You're not on that channel\r\n")
#define ERR_CHANOPRIVSNEEDED(channel) ("482 *  " + channel " :You're not channel operator\n")
#define	ERR_USER_NOT_IN_CHANNEL send(fd, "441 ERR_USERNOTINCHANNEL \r\n", 28, 0)
#define NEW_USER(nickname, username) ("Dear "+ nickname + "!" + username +"!" + " 001 RPL_WELCOME * Welcome to the IRC!\r\n")
#define GET_USER_PASSED server.getUser(i).getUserPassed()
#define GET_NICK_PASSED server.getUser(i).getNickNamePassed()
#define SEND_ABOUT_NEW_USER std::cout << "New User has connected! Nickname: " << "is " << server.getUser(i).getNickname() << "!" << " Username: "<< "is " << server.getUser(i).getUsername() << "!" << " fd number " << "= " << server.getUser(i).getFd() << "!" << std::endl;

void	MessageDeliveryFormat(int fd, string nickname, string username, string message);
void	error(string error);