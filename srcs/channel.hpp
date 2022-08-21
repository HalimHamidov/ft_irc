#pragma once
#include "rfc2812rpl.hpp"


class Channel{
	private:
	string				_channelName;
	vector<int>			_fds;
	int					_fdAdmin;
	public:
	Channel();
	Channel(string channelName, int fd);
	~Channel();

	string				getChannelName();
	vector<int>			getFdVector();
	int					getFdAdmin();
	void				fdsPushBack(int fd);
	bool 				doKickFromChannel(int fd, int userFd, string userName);
	void				doChannelPrivmsg(int fd, string message, string nickname, string username);
	void				setChannelName(string channelName);
	void				setFdVector(vector<int> &fds);
	void				setFdAdmin(int fd);	
	void				setNewVector(vector<int> &newVector);
	bool				checkUserInChannel(int fd);
	void				printFds();

};

