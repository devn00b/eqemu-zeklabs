#ifndef _IRC_LINE_H

#define _IRC_LINE_H

#include <string>
#include <vector>
#include "irc_prefix.h"

using namespace std;

class IRC_Line {
	public:
	IRC_Prefix Prefix;
	string Command;
	vector<string> Params;
	string Trailing;
	inline IRC_Line() { }
	inline IRC_Line(string line) { Parse(line); }
	void Parse(string line);
	string Get();
	void Dump();
};

#endif
