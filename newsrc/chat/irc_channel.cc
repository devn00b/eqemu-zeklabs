#include <string>
#include <map>
#include <vector>
#include "irc_user.h"
#include "irc_channel.h"
#include "misc.h"

using namespace std;

IRC_Channel::IRC_Channel(string channel_name)
{
	ParseChannel(channel_name,Server,Name);
}

void IRC_Channel::handle_LIST(vector<string> memberlist)
{
vector<string>::iterator itr;
	for(itr=memberlist.begin();itr!=memberlist.end();itr++) {
		if (itr->length()) {
			handle_JOIN(*itr);
		}
	}
}

void IRC_Channel::handle_JOIN(string who)
{
IRC_User u(who);
	members[u.GetIRCName()]=u;
}

void IRC_Channel::handle_PART(string who)
{
map<string,IRC_User>::iterator itr;

	if ((itr=members.find(who))!=members.end()) {
		members.erase(itr);
	}
}

void IRC_Channel::handle_SetOpStatus(string who, char opstatus)
{
map<string,IRC_User>::iterator itr;

	if ((itr=members.find(who))!=members.end()) {
		itr->second.OpStatus=opstatus;
		members.erase(itr);
	}
}

void IRC_Channel::handle_NICK(string from, string to)
{
map<string,IRC_User>::iterator itr;
IRC_User u;

	if ((itr=members.find(from))!=members.end()) {
		u=itr->first;
		u.NICK(to);
		members.erase(itr);
		members[to]=u;
	}
}

vector<string> IRC_Channel::GetMembersEQ()
{
map<string,IRC_User>::iterator itr;
vector<string> memberlist;

	for(itr=members.begin();itr!=members.end();itr++) {
		memberlist.push_back(string(&itr->second.OpStatus)+itr->second.GetEQName());
	}

	return memberlist;
}

vector<string> IRC_Channel::GetMembersIRC()
{
map<string,IRC_User>::iterator itr;
vector<string> memberlist;

	for(itr=members.begin();itr!=members.end();itr++) {
		memberlist.push_back(string(&itr->second.OpStatus)+itr->second.GetIRCName());
	}

	return memberlist;
}

void IRC_Channel::ParseChannel(string ircchannel, string &server, string &channel)
{
int ind;
	if (ircchannel.length()) {
		if (ircchannel[0]=='#') {
			if (ircchannel[1]=='[') {
				ind=ircchannel.find(']');
				server.assign(ircchannel,2,ind-2);
				channel.assign(ircchannel,ind+1,string::npos);
			} else {
				server="irc";
				channel.assign(ircchannel,1,string::npos);
			}
		} else { // EQ Name
			vector<string> parsed=split(ircchannel,".");
			if (parsed.size()==1) {
				server="";
				channel=parsed[0];
			} else {
				server=parsed[0];
				channel=parsed[1];
			}
		}
	}
}

string IRC_Channel::GetIRCName()
{
string name="#";
	if (Server.length() && Server!="irc") {
		name+="[";
		name+=Server;
		name+="]";
	}
	name+=Name;

	return name;
}

string IRC_Channel::GetEQName(string myserver)
{
string name;
	if (Server.length() && Server!=myserver) {
		name=Server+".";
	}
	name+=Name;

	return name;
}
