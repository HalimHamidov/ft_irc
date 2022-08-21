#include "channel.hpp"
#include "command.hpp"

Channel::Channel() { }
Channel::Channel(string channelName, int fd) : _channelName(channelName), _fdAdmin(fd) { _fds.push_back(fd); }

vector<int>			Channel::getFdVector(){ return(_fds); }
int					Channel::getFdAdmin(){ return(_fdAdmin); }
string				Channel::getChannelName(){ return(_channelName); }
void				Channel::fdsPushBack(int fd) { _fds.push_back(fd); }
void				Channel::setChannelName(string channelName) { _channelName = channelName; }
void				Channel::setFdVector(vector<int> &fds) { _fds = fds; }
void				Channel::setFdAdmin(int fd) { _fdAdmin = fd; }
void				Channel::setNewVector(vector<int> &newVector) { _fds = newVector; }

void	NewUserConnect(Server &server, int fd, string nickname, int id, string channelName){
	Channel tmpChannel = server.getChannel(id);
	vector<int> tmpFdVector = tmpChannel.getFdVector();
	string userJoined = ":127.0.0.1 " + nickname + " " + "JOIN " + channelName + "\r\n";
	
	for (size_t i = 0; i < tmpFdVector.size(); i++){
		if (tmpFdVector[i] != fd)
			send(tmpFdVector[i], userJoined.c_str(), userJoined.length() + 1, 0);
	}
	
	string beginMessage = string(":ircserv 331 " + nickname + " " + channelName + ": No topic is set\r\n"); 
	send(fd, beginMessage.c_str(), beginMessage.length() + 1, 0);	
}

bool	checkChannelNameExist(vector<Channel> &tmpVector, string channelName){
	for (vector<Channel>::iterator it = tmpVector.begin(); it != tmpVector.end(); it++){
		if ((*it).getChannelName() == channelName)
			return true;
	}
	return false;
}

bool Channel::checkUserInChannel(int fd) {
	for (vector<int>::iterator it = _fds.begin(); it != _fds.end(); it++){
		if ((*it) == fd)
			return true;
	}
	return false;
}

void	Channel::doChannelPrivmsg(int fd, string message, string nickname, string username){
	if (checkUserInChannel(fd)) {
		for (vector<int>::iterator it = _fds.begin(); it != _fds.end(); it++) {
			if ((*it) != fd)
				MessageDeliveryFormat((*it), nickname, username, message);
		}
	} else {
        send(fd, "404 ERR_CANNOTSENDTOCHAN: \r\n", 28, 0);
		std::cout << "NOT.IN.CHANNEL!\n";
	}
}

void	Command::createNewChannel(Server &server){
	Channel *channel = new Channel(_arguments[0], _fd);
	int	_channelID = server.getChannelID();
	server.channelsPushBack(channel);
	delete channel;
	
	NewUserConnect(server, _fd, _nickname, _channelID, _arguments[0]);
	std::cout << "NEW CHANNEL! " << _arguments[0] << " ADMIN IS " << _nickname << std::endl;
	server.setChannelID(1);
}

int	checkChannelErrors(vector<string> _arguments, int _fd){
	if (_arguments.size() == 0){
		string err = ERR_NEEDMOREPARAMS((string)"JOIN");
		send(_fd, err.c_str(), err.length() + 1, 0);
		return 1;
	}
	if (_arguments.size() > 0){
		if (_arguments[0][0] != '#'){
			string err = ERR_BADCHANNELKEY((string)"JOIN");
			send(_fd, err.c_str(), err.length() + 1, 0);
			return 1;
		}
	}
	return 0;
}

void	Command::doJoinCommand(Server &server){
	int	_channelID = server.getChannelID();
	bool channelNameExist = false;
	vector<Channel> tmpVector = server.getVectorOfChannels();
	channelNameExist = checkChannelNameExist(tmpVector, _arguments[0]);
	int ifChannelError = checkChannelErrors(_arguments, _fd);
	if (ifChannelError)
		return ;
	if (!channelNameExist)
		createNewChannel(server);
	else {
		for (vector<Channel>::iterator it = tmpVector.begin(); it != tmpVector.end(); it++){
			if ((*it).getChannelName() == _arguments[0]){
				if ((*it).getFdAdmin() != _fd){
					vector<int>::iterator it2;
					vector<int> tmpFd = (*it).getFdVector();
					for (it2= tmpFd.begin(); it2 != tmpFd.end(); it2++){
						if ((*it2) == _fd)
							break;
					}
					if (it2 == tmpFd.end()){
                        if(tmpFd.size() == 0)
                            (*it).setFdAdmin(_fd);
                        (*it).fdsPushBack(_fd);
                        server.channelVectorSetNew(tmpVector);
						NewUserConnect(server, _fd, _nickname, _channelID - 1, _arguments[0]);
						std::cout << "New user at " << server.getChannel(_channelID - 1).getChannelName() << " by fd " << _fd << " " << _nickname << std::endl;
					}
				}
			}
		}
	}
}

void Command::doKickCommand(Server &server) {
	if(_arguments.size() < 2){
        string err = "461 *  KICK :Not enough parameters\r\n";
        send(_fd, err.c_str(), err.length() + 1, 0);
		return;
	}

	vector<User> tmpVectorOfUsers = server.getVectorOfUsers();
	bool userExist = false;
	int userFd;
	for (vector<User>::iterator it = tmpVectorOfUsers.begin(); it != tmpVectorOfUsers.end(); it++){
		if ((*it).getNickname() == _arguments[1]){
			userExist = true;
			userFd = (*it).getFd();
			break;
		}
	}
	
	if (userExist){
		Channel tmpChannel;
		vector<Channel> tmpVector = server.getVectorOfChannelsRef();
        vector<Channel>::iterator it;
		for ( it = tmpVector.begin(); it != tmpVector.end(); it++){
			if ((*it).getChannelName() == _arguments[0]){
				if((*it).doKickFromChannel(_fd, userFd, _arguments[1])) {
                    std::cout << _arguments[1] << " WAS KICKED FROM " << _arguments[0] << std::endl;
                    string err = "YOU WAS KICKED FROM " + _arguments[0] + "\r\n";
                    send(userFd, err.c_str(), err.length() + 1, 0);
                    server.channelVectorSetNew(tmpVector);
                    break;
                }
			}
		}
        if (it == tmpVector.end()){
            string channelDoesNotExist = ERR_NOSUCHCHANNEL(_arguments[0]);
            send(_fd, channelDoesNotExist.c_str(), channelDoesNotExist.length() + 1, 0);
        }
	}
    else {
        string userDoesNotExist = "441 * " + _arguments[1] + " " + _arguments[0] + " :They aren't on that channel\r\n";
        send(_fd, userDoesNotExist.c_str(), userDoesNotExist.length() + 1, 0);
    }
}

bool Channel::doKickFromChannel(int fd, int userFd, string userName){
    if (fd == userFd) {
        string err = "You can't KICK yourself\r\n";
        send(fd, err.c_str(), err.length() + 1, 0);
        return false;
    }
	if (fd == _fdAdmin){
		vector<int>::iterator itb = _fds.begin();
		vector<int>::iterator ite = _fds.end();
		vector<int>::iterator it;
		for (it = itb; it != ite; it++){
			if ((*it) == userFd){
				_fds.erase(it);
				return true;
			}
		}
	if (it == ite) {
		string err = "441 * " + userName +  _channelName + " :They aren't on that channel\n";
		send(fd, err.c_str(), err.length() + 1, 0);
		return false;
	}
	} else {
		string err = "482 *  " + _channelName + " :You're not channel operator\n";
		send(fd, err.c_str(), err.length() + 1, 0);
		return false;
	}
	return false;
}
void Channel::printFds() {
	vector<int>::iterator itb = _fds.begin();
	vector<int>::iterator ite = _fds.end();
    std::cout << "Admin fd: " << _fdAdmin << "\n";
	for (vector<int>::iterator it = itb; it != ite; it++){
		std::cout << "fd" << (*it) << "\n";
	}
}
Channel::~Channel() { }