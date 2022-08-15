#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "irc_connection.h"
#include "misc.h"

using namespace std;

char IRC_Connection::ReadBuffer[8192];

int IRC_Connection::Read()
{
string buffer;
int length=0;
	if (FD>=0) {
		if ((length=read(FD,IRC_Connection::ReadBuffer,sizeof(IRC_Connection::ReadBuffer)-1))<=0) {
			if (errno==EAGAIN)
				length=0;
			else
				length=-1;
		} else {
			IRC_Connection::ReadBuffer[length]=0;
			PreProcess(IRC_Connection::ReadBuffer);
		}
	}

	return length;
}

void IRC_Connection::PreProcess(string buffer)
{
vector<string> lines;
vector<string>::iterator itr;
	buffer.insert(0,InUnprocessed);
	lines=splitonstr(buffer,"\r\n");
	InUnprocessed=*(lines.rbegin());
	lines.pop_back();

	for(itr=lines.begin();itr!=lines.end();itr++)
		In.push_back(IRC_Line(*itr));
}

void IRC_Connection::Process()
{
int cmdval;
string cmd;
IRC_Line line;

	
	while(In.size()) {
		// Pop one off
		line=In[0];
		In.erase(In.begin());

		//line.Dump();
		if ((cmdval=atoi(line.Command.c_str()))!=0) {
			switch(cmdval) {
				case 353:
					OnLIST(line.Params[2],split(line.Trailing));
					break;
				default:
					if (cmdval==376)
						seen_motd=true;
					OnNumericReply(cmdval,line);
					break;
			}
		} else if (line.Command=="PRIVMSG") {
			OnPRIVMSG(line.Prefix.Nick,line.Params[0],line.Trailing);
		} else if (line.Command=="JOIN") {
			OnJOIN(line.Trailing,line.Prefix.Nick);
		} else if (line.Command=="NOTICE") {
			OnNOTICE(line.Prefix.Nick,line.Params[0],line.Trailing);
		} else if (line.Command=="PART") {
			OnPART(line.Params[0],line.Prefix.Nick);
		} else if (line.Command=="INVITE") {
			OnINVITE(line.Prefix.Nick,line.Params[0],line.Trailing);
		} else if (line.Command=="KICK") {
			OnKICK(line.Prefix.Nick,line.Params[1],line.Params[0]);
		} else if (line.Command=="PING") {
			Out.insert(Out.begin(),string("PONG "+line.Trailing+"\r\n"));
			//OnPING(line.Trailing);
		} else if (line.Command=="ERROR") {
			OnERROR(line.Trailing);
		} else if (line.Command=="KILL") {
			OnKILL(line.Prefix.Nick,line.Params[0]);
		} else if (line.Command=="QUIT") {
			OnQUIT(line.Prefix.Nick);
		} else if (line.Command=="MODE" && line.Params[0][0]=='#') {
			/*
			if ((ind=line.Params[1].find('o'))==string::npos)
				if ((ind=line.Params[1].find('h'))==string::npos)
					ind=line.Params[1].find('v');
			*/
		} else {
			OnOther(line);
		}
	}
}

void IRC_Connection::Send(string data)
{
	Out.push_back(data+"\r\n");
}

int IRC_Connection::Write()
{
int length=0;
string message;

	if (FD>=0) {
		message=Out[0];
		Out.erase(Out.begin());
		if (Debug>1)
			cout << timestamp() << "Write to IRC:" << message;
		if ((length=write(FD,message.c_str(),message.length()))<=0)
			length=-1;
	}
	return length;
}

bool IRC_Connection::Connect(string host, int port)
{
	if (Debug)
		cout << timestamp()  << "*** Connecting IRC to " << host << ":" << port << endl;
	FD=SocketConnect(host,port);

	return FD >= 0;
}

void IRC_Connection::Login()
{
	Debug=1;
	if (Address.length()) {
		string ip=Address;
		unsigned long i;
		if ((i=ip.find(':'))!=string::npos) {
			ip.erase(i,string::npos);
		}
		cout << "Setting IP to " << ip << endl;
		Send(string("SETIP ")+ip);
	}
	Send(string("PASS ")+Password);
	Send(string("NICK ")+Me.GetIRCName());
	Send(string("USER ")+Me.Name+" 0 * "+Me.Name);
}


bool IRC_Connection::HasOutgoingData()
{
bool result=false;
	if (Out.size()>0) {
		result=true;
		//cout << "I have data to send: " << Out[0] << endl;
		if (!seen_motd) {
			//cout << "I have not seen motd yet" << endl;
			result=false;
			string s=Out[0];
			s=pop_arg(s);

			if (s=="NICK" || s=="USER" || s=="PASS" || s=="PONG" || s=="PING" || s=="SETIP") {
				//cout << "Thats okay since I'm sending: " << s << endl;
				result=true;
			}else {
				//cout << "I guess I'll have to wait: " << s << endl;
			}
		}
	}

	return result;
}
