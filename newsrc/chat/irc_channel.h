#ifndef _IRC_CHANNEL_H

#define _IRC_CHANNEL_H

#include <string>
#include <map>
#include <vector>
#include "irc_user.h"

using namespace std;

class IRC_Channel {
	private:
		string Server;
		string Name;
		map<string,IRC_User> members;
	public:
		IRC_Channel(string channel_name="");
		void handle_PART(string who);
		void handle_JOIN(string who);
		void handle_SetOpStatus(string who, char optstatus);
		void handle_NICK(string from, string to);
		void handle_LIST(vector<string> memberlist);
		vector<string> GetMembers();
		string GetIRCName();
		string GetEQName(string myserver="");
		vector<string> GetMembersEQ();
		vector<string> GetMembersIRC();
		static void ParseChannel(string ircchannel, string &server, string &channel);
};

#endif
