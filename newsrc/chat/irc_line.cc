#include <iostream>
#include <vector>
#include <string>
#include "irc_line.h"
#include "misc.h"

using namespace std;

void IRC_Line::Parse(string s)
{
string field;
string line=s;

	if (*(line.rbegin())=='\n')
		line.erase(line.length()-1,1);
	if (*(line.rbegin())=='\r')
		line.erase(line.length()-1,1);
	field=pop_arg(line," ");
	if (field[0]==':') {
		Prefix.Parse(field);
		field=pop_arg(line," ");
	}
	Command=field;
	while(line[0]!=':') {
		field=pop_arg(line," ");
		if (!field.length())
			break;
		Params.push_back(field);
	}
	if (line[0]==':') {
		Trailing=line.substr(1,string::npos);
	}

}

string IRC_Line::Get()
{
string s;
vector<string>::iterator itr;
	s=Prefix.Get();
	if (s.length())
		s+=" ";
	s+=Command;
	for(itr=Params.begin();itr!=Params.end();itr++) {
		s+=" ";
		s+=*itr;
	}

	if (Trailing.length()) {
		s+=" ";
		s+=Trailing;
	}

	return s;
}
	
void IRC_Line::Dump()
{
vector<string>::iterator itr;
	cout << "Prefix:" << endl;
	Prefix.Dump();
	cout << "Command: |" << Command << "|" << endl;
	cout << "Params:" << endl;
	for(itr=Params.begin();itr!=Params.end();itr++) {
		cout << "   |" << *itr << "|" << endl;
	}

	cout << "Trailing: |" << Trailing << "|" << endl;

}
	


