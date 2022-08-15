#ifndef _IRC_USER_H

#define _IRC_USER_H

#include <string>

using namespace std;

class IRC_User {
	public:
		string Server;
		string Name;
		char OpStatus;
		bool Away;

		IRC_User(string server, string name, char opstatus=0, bool away=false);
		IRC_User(string nick, char opstatus=0, bool away=false);
		IRC_User() { Server=""; Name=""; OpStatus=0; Away=false; }
		inline void Init(string server, string name, char operstatus=0, bool away=false);
		inline void NICK(string name) { Name=name; }
		string GetIRCName();
		string GetEQName(string myserver="");

		void Parse(string nick);
};

#endif
