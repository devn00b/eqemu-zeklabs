#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include "irc_connection.h"
#include "irc_replies.h"
#include "eqirc_bridge.h"
#include "eqirc_connection.h"
#include "controller.h"
#include "misc.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "command.h"
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

using namespace std;

Controller *current=NULL;

void Controller::OnNumericReply(int reply, IRC_Line line)
{
	switch (reply) {
		case ERR_NICKNAMEINUSE:
			{
				cout << "** Trying to ghost myself!" << endl;
				Send(string("NICK ")+Me.GetIRCName()+"ghost");
				PRIVMSG("NickServ",string("ghost ") + Me.GetIRCName() + " " + Password);
			}
			break;
		default:
			//IRC_Connection::OnNumericReply(reply,line);
			break;
	}
}

void Controller::OnNOTICE(string from, string to, string message)
{
	//IRC_Connection::OnNOTICE(from,to,message);
	if (from=="NickServ") {
		if (to==Me.GetIRCName()+"ghost") {
			if (message=="Ghost with your nickname has been killed.") {
				Nick();
			}
		} else if (to==Me.GetIRCName()) {
			if (message.find("This nickname is registered and protected.")!=string::npos) {
				PRIVMSG("NickServ",string("IDENTIFY ")+Password);
			}
		}

	}
}

void Controller::OnPRIVMSG(string from, string to, string message)
{
string arg,respondto;
vector<string>::iterator itr;
string fromstr;
	arg=pop_arg(message);
	if (to==Me.GetIRCName() || (to[0]=='#' && arg==Me.GetIRCName())) {
		if (arg==Me.GetIRCName()) {
			arg=pop_arg(message);
			respondto=to;
			fromstr=from+": ";
		} else {
			respondto=from;
		}
		if (from.find("Serv")==string::npos) {
			vector<string> responses;
			current=&*this;
			int level=GetAuthLevel(from);
			CommandList.Dispatch(arg+" "+message,"",from,level,responses);

			if (responses.size()) {
				for(itr=responses.begin();itr!=responses.end();itr++) {
					PRIVMSG(respondto,fromstr+(*itr));
				}
			} else {
				PRIVMSG(respondto,fromstr+"Okay");
			}
		}
	}
}


vector<string> Controller::handlePING(string command, string from)
{
vector<string> responses;
	responses.push_back("I am here.");

	return responses;
}


vector<string> Controller::handleSUICIDE(string command, string from)
{
vector<string> responses;
	cerr << timestamp() << from << " asked me to SUICIDE" << endl;
	exit(0);

	return responses;
}

vector<string> Controller::handleDO(string command, string from)
{
vector<string> responses;
	cerr << timestamp() << from << " asked me to DO: " << command  << endl;
	Send(command);

	return responses;
}

vector<string> Controller::handleSTATUS(string command, string from)
{
vector<string> responses;
ostringstream msg;
EQIRC_Bridge *bridge;
	bridge=(EQIRC_Bridge *)Bridge;
	msg << "I have " << bridge->connections.size();
	msg << " user(s) on " << bridge->channels.size() << " channel(s)";

	responses.push_back(msg.str());

	return responses;
}

vector<string> Controller::handleBROADCAST(string command, string from)
{
vector<string> responses;
EQIRC_Bridge *bridge;
map<string,EQIRC_Connection *>::iterator connitr;
IRC_User u(from);
string message;
	
	bridge=(EQIRC_Bridge *)Bridge;
	message=u.GetEQName()+" broadcasts: " + command;
	for(connitr=bridge->connections.begin();connitr!=bridge->connections.end();connitr++) {
		connitr->second->Message("","",message);
	}

	return responses;
}

vector<string> Controller::handleCHANNEL(string command, string from)
{
vector<string> responses;
vector<string>::iterator itr;
ostringstream msg;
EQIRC_Bridge *bridge;
unsigned int max_msg_length=500-from.length()-from.length();
string s;
map<string,IRC_Channel *>::iterator chanitr;
	bridge=(EQIRC_Bridge *)Bridge;
	if (command.length() && command!="LIST") {
		if ((chanitr=bridge->channels.find(command))!=bridge->channels.end()) {
			msg << "Channel " << command << ": ";
			vector<string> members=chanitr->second->GetMembersIRC();
			for(itr=members.begin();itr!=members.end();itr++) {
				if ((msg.str().length()+itr->length())>max_msg_length) {
					responses.push_back(msg.str());
					msg.str("");
				}
				msg << (*itr) +" ";
			}
			responses.push_back(msg.str());
		} else {
			responses.push_back(string("No such channel '")+command+"'");
		}
	} else {
		msg << "Channles in use: ";
		for(chanitr=bridge->channels.begin();chanitr!=bridge->channels.end();chanitr++) {
			s=chanitr->second->GetIRCName();
			if ((msg.str().length()+s.length())>max_msg_length) {
				responses.push_back(msg.str());
				msg.str("");
			}
			msg << s +" ";
		}
		responses.push_back(msg.str());
	}

	return responses;
}

vector<string> Controller::handleWHO(string command, string from)
{
vector<string> responses;
map<string,EQIRC_Connection *>::iterator connitr;
ostringstream msg;
string arg;
EQIRC_Bridge *bridge;
string ircname,eqname;
	bridge=(EQIRC_Bridge *)Bridge;
	if ((arg=pop_arg(command))!="") {
		for(connitr=bridge->connections.begin();connitr!=bridge->connections.end();connitr++) {
			ircname=connitr->second->Me.GetIRCName();
			eqname=connitr->second->Me.GetEQName();
			if (ircname==arg || eqname==arg) {
				unsigned long ip=connitr->second->address.sin_addr.s_addr;
				msg << eqname << ":";
				msg << " From:" << dec;
				msg << (int)*(unsigned char *)&ip;
				msg << "." << (int)*((unsigned char *)&ip+1);
				msg << "." << (int)*((unsigned char *)&ip+2);
				msg << "." << (int)*((unsigned char *)&ip+3);
				msg << "," << (int)ntohs(connitr->second->address.sin_port);
				msg << ", Session:" << hex << setw(8) << setfill('0') << connitr->second->Session << dec;
				msg << ", Key:" << hex << setw(8) << setfill('0') << connitr->second->Key << dec;
				msg << ", Seq(In/Out):" << connitr->second->NextInSeq << "/" << connitr->second->NextOutSeq;
				msg << ", Chan count:" << connitr->second->channels.size();
				responses.push_back(msg.str());
				break;
			}
		}

		if (connitr==bridge->connections.end())
			msg << "No such user: " << arg;
	} else {
		unsigned int max_msg_length=500-from.length()-from.length();
		string s;
		msg << "Connected users: ";
		for(connitr=bridge->connections_by_name.begin();connitr!=bridge->connections_by_name.end();connitr++) {
			s=connitr->second->GetIRCName();
			if ((msg.str().length()+s.length())>max_msg_length) {
				responses.push_back(msg.str());
				msg.str("");
			}
			msg << connitr->second->Me.GetIRCName()+" ";
		}
		responses.push_back(msg.str());
	}

	return responses;
}

vector<string> Controller::handleAUTHPASS(string command, string from)
{
vector<string> responses;
	if(pop_arg(command)==Password) {
		AuthList.push_back(from);
	} else {
		responses.push_back("Invalid password");
	}

	return responses;
}

vector<string> Controller::handleAUTHLIST(string command, string from)
{
vector<string> responses;
string msg="I will repond to messages from: ";
vector<string>::iterator itr;
	for(itr=AuthList.begin();itr!=AuthList.end();itr++)
		msg+=*itr+" ";

	responses.push_back(msg);

	return responses;
}

vector<string> Controller::handleAUTHADD(string command, string from)
{
vector<string> responses;
string arg;
	while(command !="") {
		arg=pop_arg(command);
		if (arg.length())
			AuthList.push_back(arg);
	}

	return responses;
}

vector<string> Controller::handleAUTHDEL(string command, string from)
{
vector<string> responses;
vector<string>::iterator itr;
string arg;
	while(command !="") {
		arg=pop_arg(command);
		if (arg.length()) {
			while((itr=find(AuthList.begin(),AuthList.end(),arg))!=AuthList.end())
				AuthList.erase(itr);
		}
	}

	return responses;
}

vector<string> Controller::handleCONFIGRELOAD(string command, string from)
{
vector<string> responses;
EQIRC_Bridge *bridge;
	bridge->LoadServerChannels();
	return responses;
}

vector<string> Controller::handleCONFIGFORCED(string command, string from)
{
map<string,vector<string> >::iterator serveritr;
vector<string> responses;
EQIRC_Bridge *bridge;
	bridge=(EQIRC_Bridge *)Bridge;

	for(serveritr=bridge->ForcedChannels.begin();serveritr!=bridge->ForcedChannels.end();serveritr++) {
		responses.push_back(serveritr->first+"=>("+join(serveritr->second.begin(),serveritr->second.end()));
	}
	
	return responses;
}

vector<string> Controller::handleCONFIGADMIN(string command, string from)
{
map<string,vector<string> >::iterator serveritr;
vector<string> responses;
EQIRC_Bridge *bridge;
	bridge=(EQIRC_Bridge *)Bridge;

	for(serveritr=bridge->AdminChannels.begin();serveritr!=bridge->AdminChannels.end();serveritr++) {
		responses.push_back(serveritr->first+"=>("+join(serveritr->second.begin(),serveritr->second.end()));
	}
	
	return responses;
}

vector<string> Controller::handleCONFIGAUTO(string command, string from)
{
map<string,vector<string> >::iterator serveritr;
vector<string> responses;
EQIRC_Bridge *bridge;
	bridge=(EQIRC_Bridge *)Bridge;

	for(serveritr=bridge->AutoChannels.begin();serveritr!=bridge->AutoChannels.end();serveritr++) {
		responses.push_back(serveritr->first+"=>("+join(serveritr->second.begin(),serveritr->second.end()));
	}
	
	return responses;
}

void Controller::InitCommands()
{
	CommandList.Commands["DO"]=Command(boost::bind(boost::mem_fn(&Controller::handleDO),this,_1,_2),"command","Executes ccommand. i.e. DO PRIVMSG #eqemucoders : Hi!",1);
	CommandList.Commands["CHANNEL"]=Command(boost::bind(boost::mem_fn(&Controller::handleCHANNEL),this,_1,_2),"","Displays the list of channels that bridge users are in",1);
	CommandList.Commands["WHO"]=Command(boost::bind(boost::mem_fn(&Controller::handleWHO),this,_1,_2),"[user]","Lists current users or details of a specific user.",1);
	CommandList.Commands["BROADCAST"]=Command(boost::bind(boost::mem_fn(&Controller::handleBROADCAST),this,_1,_2),"message","Sends a global message to all connected bridge users",1);
	CommandList.Commands["STATUS"]=Command(boost::bind(boost::mem_fn(&Controller::handleSTATUS),this,_1,_2),"","Brief bridge status.",1);
	CommandList.Commands["PING"]=Command(boost::bind(boost::mem_fn(&Controller::handlePING),this,_1,_2),"","Make sure I'm alive.",1);
	CommandList.Commands["SUICIDE"]=Command(boost::bind(boost::mem_fn(&Controller::handleSUICIDE),this,_1,_2),"","Quit immediately with no cleanup, just exit()",1);

	Command cmd("command","Authorization level commands.",0);
	cmd.Commands["PASS"]=Command(boost::bind(boost::mem_fn(&Controller::handleAUTHPASS),this,_1,_2),"password","Authenticates you with password.",0);
	cmd.Commands["LIST"]=Command(boost::bind(boost::mem_fn(&Controller::handleAUTHLIST),this,_1,_2),"","Lists authenticated users.",1);
	cmd.Commands["ADD"]=Command(boost::bind(boost::mem_fn(&Controller::handleAUTHADD),this,_1,_2),"user ...","Adds the listed users as auctenticated.",1);
	cmd.Commands["DEL"]=Command(boost::bind(boost::mem_fn(&Controller::handleAUTHDEL),this,_1,_2),"user ...","Deletes the listed users as auctenticated.",1);
	CommandList.Commands["AUTH"]=cmd;

	Command cmd2("command","Configuration level commands.",1);
	cmd2.Commands["RELOAD"]=Command(boost::bind(boost::mem_fn(&Controller::handleCONFIGRELOAD),this,_1,_2),"","Reloads forced/auto/admin channel lists.",1);
	cmd2.Commands["FORCED"]=Command(boost::bind(boost::mem_fn(&Controller::handleCONFIGFORCED),this,_1,_2),"","List forced channels.",1);
	cmd2.Commands["ADMIN"]=Command(boost::bind(boost::mem_fn(&Controller::handleCONFIGADMIN),this,_1,_2),"","List admin channels.",1);
	cmd2.Commands["AUTO"]=Command(boost::bind(boost::mem_fn(&Controller::handleCONFIGAUTO),this,_1,_2),"","List auto channels.",1);
	CommandList.Commands["CONFIG"]=cmd2;
}
