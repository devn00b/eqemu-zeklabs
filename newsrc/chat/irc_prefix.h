#ifndef _IRC_PREFIX_H

#define _IRC_PREFIX_H

#include <string>

using namespace std;
class IRC_Prefix
{
	public:
	string Nick;
	string User;
	string Host;
	inline IRC_Prefix() { }
	inline IRC_Prefix(string prefix) { Parse(prefix); }
	inline IRC_Prefix(string nick, string user, string host) { Nick=nick; User=user; Host=host; }
	string Get();
	void Parse(string prefix);
	void Dump();
};

#endif
