#ifndef _EQIRC_BRIDGE_H

#define _EQIRC_BRIDGE_H

#include <string>
#include <map>
#include <mysql.h>
#include "eqirc_connection.h"
#include "irc_channel.h"
#include "controller.h"

using namespace std;

class EQIRC_Bridge {
	friend class Controller;
	private:
		fd_set read_master;
		int eq_fd;
		int max_fd;
		map<string,IRC_Channel *> channels;
		map<string,EQIRC_Connection *> connections;
		map<string,EQIRC_Connection *> connections_by_name;
		Controller controller;
		string ControllerName;
		int Debug;
	public:
		vector<pair<string,int> > IRCServers;
		map<string,vector<string> > AdminChannels;
		map<string,vector<string> > ForcedChannels;
		map<string,vector<string> > AutoChannels;
		unsigned short EQPort;
		string IRCPassword;
		Connection db;
		EQIRC_Bridge(unsigned short eqport, string irchost, unsigned short ircport, string cont="ChatController");
		IRC_Channel *GetChannel(string channel);
		void handle_QUIT(string who) { }
		void handle_NICK(string from, string to) { }
		int Process();
		int EQ_Read();
		inline void Connected(int fd) { FD_SET(fd,&read_master); if (fd>max_fd) max_fd=fd; }
		inline void AddAltIRCServer(string host, int port) { pair<string,int> server(host,port); IRCServers.push_back(server); }
		bool Ghost(string who, int not_fd);
		EQIRC_Connection *GetConnectionByName(string name);
		void SetConnectionByName(string name, EQIRC_Connection *conn) { connections_by_name[name]=conn; }
		void HasClosed(int FD) { if (FD>=0) FD_CLR(FD,&read_master); }
		int LoadServerChannels();

};

#endif
