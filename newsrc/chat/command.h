#ifndef _COMMAND_H

#define _COMMAND_H

#include <string>
#include <map>
#include <vector>
#include <stdio.h>
#include <boost/function.hpp>

using namespace std;

class Command {
	public:
		boost::function<vector<string> (string,string)> Func;
		string Usage;
		string Description;
		map<string,Command> Commands;
		void *pass_data;
		int Level;

		Command();
		Command(const Command &c);
		Command(boost::function<vector<string> (string,string)> cfunc, string usage, string desc, int level);
		Command(string usage, string desc, int level);
		void Dispatch(string command,string root, string from, int level, vector<string> &responses);
		void Help(string root, string from, int level, vector<string> &responses);
};

#endif
