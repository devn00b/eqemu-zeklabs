#include <string>
#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include "misc.h"
#include "command.h"

using namespace std;

void Command::Dispatch(string command,string root, string from, int level, vector<string> &responses)
{
string arg=pop_arg(command);
map<string,Command>::iterator cmditr;
	if (arg=="HELP")
		Help(root,from,level,responses);
	//else if (arg=="COMMANDS") 
		//Commands(root,from,responses);
	else if ((cmditr=Commands.find(arg))!=Commands.end()) {
		if (cmditr->second.Level<=level) {
			if (cmditr->second.Commands.size())
				cmditr->second.Dispatch(command,root+arg+" ",from,level,responses);
			else if (cmditr->second.Func)
				responses=cmditr->second.Func(command,from);
			else
				responses.push_back(string("Calling ")+cmditr->first+" function");
		} else
			responses.push_back(string("Access to ")+root+cmditr->first+" denied.");

	} else {
		responses.push_back(string("Invalid ")+root+"command");
	}
}

void Command::Help(string root, string from, int level, vector<string> &responses)
{
map<string,Command>::iterator cmditr;
string message;
	responses.push_back(string("Valid ")+root+"commands:");
	for(cmditr=Commands.begin();cmditr!=Commands.end();cmditr++) {
		if (cmditr->second.Level<=level) {
			string cmd=cmditr->first+" "+cmditr->second.Usage;
			cmd+=string("                    ").substr(0,15-cmd.length());
			message=string("     ")+cmd+"  -  "+cmditr->second.Description;
			if (cmditr->second.Commands.size())
				message+="  Send HELP for a list of commands";
			responses.push_back(message);
		}
	}
}

Command::Command()
{
	Level=0;
}

Command::Command(const Command &c)
{
	Func=c.Func;
	Usage=c.Usage;
	Description=c.Description;
	Commands=c.Commands;
	Level=c.Level;
}

Command::Command(boost::function<vector<string> (string,string)>  cfunc, string usage, string desc, int level)
{
	Func=cfunc;
	Usage=usage;
	Description=desc;
	Level=level;
}

Command::Command(string usage, string desc, int level)
{
	Usage=usage;
	Description=desc;
	Level=level;
}

