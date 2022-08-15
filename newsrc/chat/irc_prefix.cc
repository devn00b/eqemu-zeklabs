#include <string>
#include <vector>
#include <iostream>
#include "irc_prefix.h"
#include "misc.h"

using namespace std;

void IRC_Prefix::Parse(string prefix)
{
vector<string> chunks;
int count;
	if (prefix[0]==':') {
		prefix.erase(0,1);
		chunks=split(prefix,"@!");
		count=chunks.size();
		Host=chunks[count-1];
		if (count>1) {
			Nick=chunks[0];
			if (count>2)
				User=chunks[1];
			else
				User="";
		} else {
			Nick="";
			User="";
		}
	}
}

string IRC_Prefix::Get()
{
string prefix;
	if (Nick.length() || Host.length()) {
		prefix=":";
		if (Nick.length())
			prefix+=Nick;
		if (Host.length()) {
			if (User.length()) {
				prefix+="!";
				prefix+=User;
			}
			if (prefix.length()!=1)
				prefix+="@";
			prefix+=Host;
		}
	}
	return prefix;
}

void IRC_Prefix::Dump()
{
	cout << "Nick: |" << Nick << "|" << endl;
	cout << "User: |" << User << "|" << endl;
	cout << "Host: |" << Host << "|" << endl;

}
