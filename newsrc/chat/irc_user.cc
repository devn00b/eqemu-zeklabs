#include <string>
#include <vector>
#include "irc_user.h"
#include "misc.h"

using namespace std;

IRC_User::IRC_User(string server, string name, char opstatus, bool away)
{
	Init(server,name,opstatus,away);
}

void IRC_User::Init(string server, string name, char opstatus, bool away)
{
	Server=server;
	Name=name;
	OpStatus=opstatus;
	Away=away;
}

IRC_User::IRC_User(string nick, char opstatus, bool away)
{
	Parse(nick);
	OpStatus=opstatus;
	Away=away;
}

string IRC_User::GetIRCName()
{
string name="";
	if (Server.length() && Server!="irc")
		name=string("[")+Server+"]";
	name+=Name;

	return name;
}

string IRC_User::GetEQName(string myserver)
{
string name="";
	if (Server.length()) {
		if (Server!=myserver)
			name=Server+".";
	} else
		name="irc.";
	name+=Name;

	return name;
}

void IRC_User::Parse(string nick)
{
int ind;
	if (strchr("@%+",nick[0])!=NULL) {
		OpStatus=nick[0];
		nick.erase(0,1);
	} else
		OpStatus='\0';

	if (nick[0]=='[') {
		ind=nick.find(']');
		Server.assign(nick,1,ind-1);
		Name.assign(nick,ind+1,string::npos);
	} else {
		vector<string> parsed=split(nick,".");
		if (parsed.size()==1) {
			Server="irc";
			Name=parsed[0];
		} else {
			Server=parsed[0];
			Name=parsed[1];
		}
	}
}
