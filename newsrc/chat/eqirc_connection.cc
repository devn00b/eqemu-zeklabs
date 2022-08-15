#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "eqirc_connection.h"
#include "eqirc_bridge.h"
#include "eq_data.h"
#include "misc.h"

using namespace std;

void EQIRC_Connection::OnJOIN(string channel, string who)
{
EQIRC_Bridge *bridge=(EQIRC_Bridge *)Bridge;
IRC_Channel *chan=((EQIRC_Bridge *)Bridge)->GetChannel(channel);

	if (who==Me.GetIRCName()) {
		if (find(bridge->AdminChannels[Me.Server].begin(),bridge->AdminChannels[Me.Server].end(),chan->GetIRCName())!=bridge->AdminChannels[Me.Server].end())
			admin_channels.push_back(chan);
		else 
			channels.push_back(chan);
		Joining=true;
	} else {
		IRC_User u(who);
		chan->handle_JOIN(who);
		if (Announce)
			Message("","",u.GetEQName()+" has joined "+chan->GetEQName()+".");
	}
}

void EQIRC_Connection::SendChannelList()
{
eq_data eqdata;
string chanlist;
string chanliststr;
vector<IRC_Channel *>::iterator itr;
vector<string> users;
int i;
char temp[5];
	memset(&eqdata,0,sizeof(eq_data));
	eqdata.buffer[1]=0x02;
	for(i=1,itr=channels.begin();itr!=channels.end();i++,itr++) {
		users=(*itr)->GetMembersEQ();
		if (i!=1) {
			chanliststr+=",";
			chanlist+=",";
		}
		sprintf(temp,"%d",i);
		chanliststr+=temp;
		chanliststr+="=";
		sprintf(temp,"%d",users.size());
		chanliststr+=(*itr)->GetEQName()+"("+temp+")";
		chanlist+=(*itr)->GetEQName();
	}
	for(i,itr=admin_channels.begin();itr!=admin_channels.end();i++,itr++) {
		users=(*itr)->GetMembersEQ();
		if (i!=1) {
			chanlist+=",";
		}
		sprintf(temp,"%d",i);
		chanlist+=(*itr)->GetEQName();
	}
	memcpy(&eqdata.buffer[2],chanlist.c_str(),chanlist.length()+1);
	eqdata.length=chanlist.length()+3;
	EQ_Connection::Out.push_back(eqdata);
	Message("","",string("Channels: ")+chanliststr);
}

void EQIRC_Connection::OnSetOpStatus(string channel, string who, char opstatus)
{
	((EQIRC_Bridge *)Bridge)->GetChannel(channel)->handle_SetOpStatus(who,opstatus);;
}

void EQIRC_Connection::OnLIST(string channel, vector<string> memberlist)
{
	((EQIRC_Bridge *)Bridge)->GetChannel(channel)->handle_LIST(memberlist);
}

void EQIRC_Connection::OnPART(string channel, string who)
{
vector<IRC_Channel *>::iterator itr;
IRC_Channel *chan=((EQIRC_Bridge *)Bridge)->GetChannel(channel);
	chan->handle_PART(who);
	if (who==Me.GetIRCName()) {
		if ((itr=find(admin_channels.begin(),admin_channels.end(),chan))!=admin_channels.end())
			admin_channels.erase(itr);
		if ((itr=find(channels.begin(),channels.end(),chan))!=channels.end())
			channels.erase(itr);
		SendChannelList();
	} else {
		IRC_User u(who);
		if (Announce)
			Message("","",u.GetEQName()+" has left "+chan->GetEQName()+".");
	}
}

void EQIRC_Connection::OnQUIT(string who)
{
	((EQIRC_Bridge *)Bridge)->handle_QUIT(who);
}

void EQIRC_Connection::OnNICK(string from, string to)
{
	((EQIRC_Bridge *)Bridge)->handle_NICK(from,to);
}

void EQIRC_Connection::OnPRIVMSG(string from, string to, string message)
{
IRC_User u(from);
	if (message[0]!=0x01) {
		from=u.GetEQName();
		if (to[0]!='#') {
			IRC_User u(to);
			to=u.GetEQName();
			to=string("$")+to;
		} else {
			IRC_Channel c(to);
			to=c.GetEQName();
		}

		Message(from, to, message);
	}
}

void EQIRC_Connection::OnINVITE(string inviter, string invitee, string channel)
{
IRC_Channel c(channel);
string message;
	if (Me.GetIRCName()==inviter)
		message="You";
	else {
		IRC_User u(inviter);
		message=u.GetEQName(Me.Server);
	}
	message+=" invites ";
	if (Me.GetIRCName()==invitee)
		message+="you";
	else {
		IRC_User u(invitee);
		message+=u.GetEQName(Me.Server);
	}
	message+=" to join ";
	message+=c.GetEQName(Me.Server);
	Message("", "", message);
}

void EQIRC_Connection::OnKICK(string kicker, string kickee, string channel)
{
	OnPART(channel,kickee);
}

void EQIRC_Connection::OnNumericReply(int reply, IRC_Line line)
{
EQIRC_Bridge *bridge=(EQIRC_Bridge *)Bridge;
	switch (reply) {
		case ERR_NICKNAMEINUSE:
			{
				cout << timestamp() << "Name collision!" << Me.GetIRCName() << endl;
				Message("","",string("Nick collision for ")+Me.GetEQName()+", if you want chat channels, then wait a minute and try camping and returning. (I'm working on fixing it)");
				//IRC_Connection::Send(string("NICK ")+Me.GetIRCName());
				//PRIVMSG("NickServ",string("ghost ") + Me.GetIRCName() + " " + Password);
			}
			break;
		case 432:
			{
				cout << timestamp() << "Reserved nick!" << Me.GetIRCName() << endl;
				Message("","",string("Part of your nick (")+Me.GetEQName()+") is making IRC think its reserved.");
				SendDisconnect();
				close(FD);
				bridge->HasClosed(FD);
				last_eq_packet=1;
				FD=-1;
			}
		case RPL_ENDOFNAMES:
			SendChannelList();
			break;
		case ERR_INVITEONLYCHAN:
			{
				IRC_Channel c(line.Params[1]);
				Message("","",c.GetEQName()+" is invite only.");
			}
			break;
		case ERR_BADCHANNELKEY:
			{
				IRC_Channel c(line.Params[1]);
				Message("","",string("Invalid password for ")+c.GetEQName());
			}
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 265:
		case 266:
		case RPL_UNAWAY:
		case RPL_NOWAWAY:
		case RPL_LUSERCLIENT:
		case RPL_LUSEROP:
		case RPL_LUSERUNKNOWN:
		case RPL_LUSERCHANNELS:
		case RPL_LUSERME:
		case RPL_MOTD:
		case RPL_MOTDSTART:
		case RPL_ENDOFMOTD:
			break;
		default:
			IRC_Connection::OnNumericReply(reply,line);
			break;
	}
}

void EQIRC_Connection::OnERROR(string message)
{
EQIRC_Bridge *bridge=(EQIRC_Bridge *)Bridge;
	IRC_Connection::OnERROR(message);
	if (message.find("Closing")!=string::npos) {
		cout << timestamp() << "[" << EQ_Connection::Address << "] " << "Closing IRC connection for " << Me.GetIRCName() << endl;
		if (!Camped)
			Message("", "", "Your IRC connection timed out.  It should reconnect, but you may need to rejoin any channels you were in.");

		close(FD);
		bridge->HasClosed(FD);
		last_eq_packet=1;
		FD=-1;
	}
}

void EQIRC_Connection::OnNOTICE(string from, string to, string message)
{
	if (from=="NickServ") {
		if (to==Me.GetIRCName()+"ghost") {
			if (message=="Ghost with your nickname has been killed.") {
				Nick();
			}
		} else if (to==Me.GetIRCName()) {
			if (message.find("This nickname is registered and protected.")!=string::npos) {
				PRIVMSG("NickServ",string("IDENTIFY ")+Password);
			}
		} else if (to==Me.GetIRCName()) {
			if (message.find("Your nickname isn't registered.")!=string::npos) {
				cout << timestamp() << "**** Nick not registered!" << endl;
				//PRIVMSG("NickServ",string("REGISTER ")+Password);
			}
		}

	}
}

void EQIRC_Connection::OnClientLogin()
{
EQIRC_Bridge *bridge=(EQIRC_Bridge *)Bridge;
EQIRC_Connection *conn;
	EQ_Connection::OnClientLogin();
	Me.Server=Server;
	Me.Name=Name;
	Password="eqirc-conn"; 
	cout << "Checking for name being in use" << endl;
	conn=bridge->GetConnectionByName(Me.GetEQName());
	cout << " After return conn=" << hex << setw(8) << conn << dec << endl;
	if (conn!=NULL) {

		cout << timestamp() << "[" << EQ_Connection::Address << "] " << "User " << Me.GetEQName() << " has an active IRC connection.  Usurping it." << endl;
		IRC_Connection::Out=conn->IRC_Connection::Out;
		IRC_Connection::In=conn->IRC_Connection::In;
		IRC_Connection::InUnprocessed=conn->IRC_Connection::InUnprocessed;
		memcpy(IRC_Connection::ReadBuffer,conn->IRC_Connection::ReadBuffer,8192);
		IRC_Connection::seen_motd=conn->IRC_Connection::seen_motd;
		IRC_Connection::Debug=conn->IRC_Connection::Debug;
		FD=conn->FD;
		IRC_Connection::Password=conn->IRC_Connection::Password;
		conn->FD=-1;
		conn->last_eq_packet=1;
		conn->Me.Name+=":USURPED";
	}

	cout << "Adding connections_by_name[" << Me.GetEQName() << "] = 0x" << hex << setw(8) << &*this << dec << endl;
	cout << "Or should it be connections_by_name[" << Me.GetEQName() << "] = 0x" << hex << setw(8) << this << dec << endl;
	bridge->SetConnectionByName(Me.GetEQName(),&*this);
		
	//Connect(((EQIRC_Bridge *)Bridge)->IRCHost,((EQIRC_Bridge *)Bridge)->IRCPort);
	//Login();
}

void EQIRC_Connection::OnClientCommand(string command)
{
string cmd=pop_arg(command),arg,channame;
unsigned long chan_num;
IRC_Channel *chan;
EQIRC_Bridge *bridge=(EQIRC_Bridge *)Bridge;
	if ((chan_num=atoi(cmd.c_str()))>0) {
		if (chan_num<=channels.size()) {
			chan=channels[chan_num-1];
			PRIVMSG(chan->GetIRCName(),command);
			Message(Me.GetEQName(),chan->GetEQName(),command);
		} else {
			Message("", "", "Invalid channel number.");
		}
	} else if (cmd=="join") {
		while(command.length()) {
			arg=pop_arg(command," ,");
			channame=pop_arg(arg,":");
			if (channame.length()) {
				IRC_Channel c(channame);
				IRC_Connection::Send(string("JOIN ")+c.GetIRCName()+" "+arg);
			}
		}
	} else if (cmd=="tell") {
		IRC_User u(pop_arg(command));
		PRIVMSG(u.GetIRCName(),command);
		Message(Me.GetEQName(),string("$")+u.GetEQName(),command);
	} else if (cmd=="leaveall") {
		IRC_Connection::Send("JOIN");
	} else if (cmd=="raw") {
		IRC_Connection::Send(command);
	} else if (cmd=="leave") {
		while(command.length()) {
			arg=pop_arg(command," ,");
			if ((chan_num=atoi(arg.c_str()))>0) {
				if (chan_num<=channels.size()) {
					if (find(bridge->ForcedChannels[Me.Server].begin(),bridge->ForcedChannels[Me.Server].end(),channels[chan_num-1]->GetIRCName())==bridge->ForcedChannels[Me.Server].end())
						IRC_Connection::Send(string("PART ")+channels[chan_num-1]->GetIRCName());
					else
						Message("", "", "You may not leave that channel.");
				} else {
					Message("", "", "Invalid channel number.");
				}
			} else {
				IRC_Channel c(arg);
				if (find(bridge->AdminChannels[Me.Server].begin(),bridge->AdminChannels[Me.Server].end(),c.GetIRCName())==bridge->AdminChannels[Me.Server].end() &&
					find(bridge->ForcedChannels[Me.Server].begin(),bridge->ForcedChannels[Me.Server].end(),c.GetIRCName())==bridge->ForcedChannels[Me.Server].end()
				)
					IRC_Connection::Send(string("PART ")+c.GetIRCName());
				else
					Message("", "", "You may not leave that channel.");
			}
		}
	} else if (cmd=="announce") {
		/*
		while(command.length()) {
			arg=pop_arg(command," ,");
			if ((chan_num=atoi(arg.c_str()))>0) {
				if (chan_num<=channels.size()) {
					Announce[chan_num]=!Announce[chan_num];
				} else {
					Message("", "", "Invalid channel number.");
				}
			} else {
				vector<pair<IRC_Channel *,bool> >::iterator chanitr;
				IRC_Channel c(arg);
				for(i=0;chanint=channels.begin();chanitr!=channels.end();chanitr++,i++) {
					if (c.GetEQName()==(*chanitr)->GetEQName) {
						chanitr->second=!chanitr->second;
					}
				}
				if (chanitr==channels.end())
					Message("","",string("Joins/leaves will ")+(chanitr->second?"now":"no longer")+(" be announced on ")+c.GetEQName());
				else
					Message("","",string("You are not in ")+c.GetEQName());
			}
		}
		*/
		Announce=!Announce;
		Message("","",string("Joins/leaves will ")+(Announce?"now":"no longer")+" be announced");
	} else if (cmd=="afk") {
		if (pop_arg(command)=="on")
			IRC_Connection::Send("AWAY :EQ Player is AFK");
		else
			IRC_Connection::Send("AWAY");
	} else if (cmd=="list") {
		if (!command.length()) {
			SendChannelList();
		}
	}

}

void EQIRC_Connection::OnKeepalive()
{
	cout << timestamp() << "[" << EQ_Connection::Address << "] Got KeepAlive" << endl;
	time(&last_eq_packet);
}

void EQIRC_Connection::OnSessionDisconnect()
{
	IRC_Connection::Send(string("QUIT")+" :Player Camped");
	Camped=true;
}

void EQIRC_Connection::OnPacket(eq_data data)
{
EQIRC_Bridge *br;
	time(&last_eq_packet);
	if (FD<0 && Me.Server.length()>0) {
		br=((EQIRC_Bridge *)Bridge);
		pair<string,int> server=br->IRCServers[0];
		if (br->IRCServers.size()>1) {
			br->IRCServers.erase(br->IRCServers.begin());
			br->IRCServers.push_back(server);
		}
		cout << timestamp() << "[" << EQ_Connection::Address << "] Connecting to IRC for " << Me.GetEQName() << endl;
		Connect(server.first,server.second);
		if (FD>=0) {
			vector<string>::iterator itr;
			Login();
			for(itr=br->AdminChannels[Me.Server].begin();itr!=br->AdminChannels[Me.Server].end();itr++) {
				IRC_Connection::Send(string("JOIN ")+*itr);
			}
			for(itr=br->ForcedChannels[Me.Server].begin();itr!=br->ForcedChannels[Me.Server].end();itr++) {
				IRC_Connection::Send(string("JOIN ")+*itr);
			}
			for(itr=br->AutoChannels[Me.Server].begin();itr!=br->AutoChannels[Me.Server].end();itr++) {
				IRC_Connection::Send(string("JOIN ")+*itr);
			}
			br->Connected(FD);
		}
	}
}
