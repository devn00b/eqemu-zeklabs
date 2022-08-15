#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "eqirc_bridge.h"
#include "eqirc_connection.h"
#include "irc_channel.h"
#include "eq_data.h"
#include "misc.h"
#include <sqlplus.hh>

using namespace std;

EQIRC_Bridge::EQIRC_Bridge(unsigned short eqport, string irchost, unsigned short ircport, string cont)
{
pair<string,int> server(irchost,ircport);
	EQPort=eqport;
	IRCServers.push_back(server);

	eq_fd=EQ_Connection::Listen(eqport);
	FD_ZERO(&read_master);
	FD_SET(eq_fd,&read_master);
	max_fd=eq_fd;
	ControllerName=cont;
	Debug=1;

	controller.Bridge=(void *)(&*this);
}

int EQIRC_Bridge::LoadServerChannels()
{
Result::iterator resitr;

	ForcedChannels.clear();
	cout << "Loading forced channels" << endl;
	try {

		Query q=db.query();	
		
		q << "select * from forced_channels";

		Result r=q.store();

		for(resitr=r.begin();resitr!=r.end();resitr++) {
			Row row=*resitr;
			ForcedChannels[(string)row["server"]].push_back((string)row["channel"]);
		}
	} catch (BadQuery er) {
		cerr << "Error: " << er.error << endl;
	} catch (BadConversion er) { // handle bad conversions
		cerr << "Error: Tried to convert \"" << er.data << "\" to a \"" << er.type_name << "\"." << endl;
	}

	AdminChannels.clear();
	cout << "Loading admin channels" << endl;
	try {
		Query q=db.query();	
		
		q << "select * from admin_channels";

		Result r=q.store();

		for(resitr=r.begin();resitr!=r.end();resitr++) {
			Row row=*resitr;
			AdminChannels[(string)row["server"]].push_back((string)row["channel"]);
		}
	} catch (BadQuery er) {
		cerr << "Error: " << er.error << endl;
	} catch (BadConversion er) { // handle bad conversions
		cerr << "Error: Tried to convert \"" << er.data << "\" to a \"" << er.type_name << "\"." << endl;
	}

	AutoChannels.clear();
	cout << "Loading auto channels" << endl;
	try {
		Query q=db.query();	
		
		q << "select * from auto_channels";

		Result r=q.store();

		for(resitr=r.begin();resitr!=r.end();resitr++) {
			Row row=*resitr;
			AutoChannels[(string)row["server"]].push_back((string)row["channel"]);
		}
	} catch (BadQuery er) {
		cerr << "Error: " << er.error << endl;
	} catch (BadConversion er) { // handle bad conversions
		cerr << "Error: Tried to convert \"" << er.data << "\" to a \"" << er.type_name << "\"." << endl;
	}

}

EQIRC_Connection *EQIRC_Bridge::GetConnectionByName(string name)
{
map<string,EQIRC_Connection *>::iterator itr;
EQIRC_Connection *conn=NULL;
	cout << "There are " << connections_by_name.size() << "/" << connections.size() << " records.  Looking for " << name << endl;
	if ((itr=connections_by_name.find(name))!=connections_by_name.end()) {
		conn=itr->second;
		cout << "Found it! conn=" << hex << setw(8) << conn << dec << endl;
	}

	return conn;
}

bool EQIRC_Bridge::Ghost(string who, int not_fd)
{
map<string,EQIRC_Connection *>::iterator itr;
bool found=true;
	for(itr=connections.begin();itr!=connections.end();itr++) {
		if (itr->second->GetIRCName()==who && itr->second->FD!=not_fd) {
			if (itr->second->FD >= 0) {
				FD_CLR(itr->second->FD,&read_master);
				close(itr->second->FD);
			}
			itr->second->FD=-1;
			itr->second->last_eq_packet=1;
			found=true;
			break;
		}
	}
	return found;
}

IRC_Channel *EQIRC_Bridge::GetChannel(string channel)
{
map<string,IRC_Channel *>::iterator itr;
	if ((itr=channels.find(channel))!=channels.end()) {
		return itr->second;
	}
	channels[channel]=new IRC_Channel(channel);
	if ((itr=channels.find(channel))==channels.end()) {
		cerr << timestamp() << "Could not find the channel we just added! This should not happen!" << endl;
		exit(-1);
	}
	return itr->second;
}

int EQIRC_Bridge::Process()
{
map<string,EQIRC_Connection *>::iterator itr,itr2,itr3;
fd_set write_set,read_set;
bool haseqdata=false;
int num,ircfd;
timeval select_sleep;
time_t now;
	time(&now);
	FD_ZERO(&write_set);
	for(itr=connections.begin();itr!=connections.end();) {
		if ((ircfd=itr->second->FD)>=0) {
			FD_SET(ircfd,&read_master);
			if (ircfd>max_fd)
				max_fd=ircfd;

		} 
		if (itr->second->Camped && ircfd<0) {
			cout << timestamp() << "*** Cleaning up camped connection for " << itr->second->GetEQName() << endl;
			itr2=itr;
			itr++;
			if ((itr3=connections_by_name.find(itr2->second->GetEQName()))==connections_by_name.end()) {
				cout << timestamp() << "*** Didn't find connection in connections_by_name via name" << endl;
				for(itr3=connections_by_name.begin();itr3!=connections_by_name.end();itr3++) {
					if (itr3->second==itr2->second) {
						cout << timestamp() << "*** Whew! Did find connection in connections_by_name via name via pointer" << endl;
						break;
					}
				}
			}
			if (itr3==connections_by_name.end())
				cout << timestamp() << "*** Didn't find connection in connections_by_name" << endl;
			else 
				connections_by_name.erase(itr3);
			delete itr2->second;
			connections.erase(itr2);
			continue;
		}

		if (itr->second->FD<0 && itr->second->last_eq_packet && now-itr->second->last_eq_packet > 60) {
			cout << timestamp() << "*** Cleaning up dead connection for " << itr->second->GetEQName() << endl;
			itr2=itr;
			itr++;
			if ((itr3=connections_by_name.find(itr2->second->GetEQName()))==connections_by_name.end()) {
				cout << timestamp() << "*** Didn't find connection in connections_by_name via name" << endl;
				for(itr3=connections_by_name.begin();itr3!=connections_by_name.end();itr3++) {
					if (itr3->second==itr2->second) {
						cout << timestamp() << "*** Whew! Did find connection in connections_by_name via name via pointer" << endl;
						break;
					}
				}
			}
			if (itr3==connections_by_name.end())
				cout << timestamp() << "*** Didn't find connection in connections_by_name" << endl;
			else 
				connections_by_name.erase(itr3);
			delete itr2->second;
			connections.erase(itr2);
			continue;
		}

		if (now-itr->second->last_eq_packet > 120) {
			if (ircfd>=0) {
				if (itr->second->last_eq_packet) {
					cerr << timestamp() << "*** EQ (" << itr->second->GetEQName() << ") client ping timeout!" << endl;
					itr->second->IRC_Connection::Send("QUIT :EQ Client ping timeout");
					itr->second->last_eq_packet=0;
				}
			}
		}

		if (ircfd>=0 && itr->second->HasOutgoingIRCData()) {
			if (Debug>1)
				cout << timestamp() << "IRCFD " << ircfd << " has data to send" << endl;
			FD_SET(ircfd,&write_set);
		}
		if (itr->second->HasOutgoingEQData())
			haseqdata=true;

		itr++;
	}
	if (controller.FD<0) {
		pair<string,int> server=IRCServers[0];
		if (IRCServers.size()>1) {
			IRCServers.erase(IRCServers.begin());
			IRCServers.push_back(server);
		}
		controller.Nick(string("[emuop]")+ControllerName);
		controller.Password="ircbridge";
		controller.Connect(server.first,server.second);
		controller.Login();
	}

	if (controller.FD>=0) {
		if (controller.FD>max_fd)
			max_fd=controller.FD;
		FD_SET(controller.FD,&read_master);
		if (controller.HasOutgoingData()) {
			FD_SET(controller.FD,&write_set);
		}
	}


	if (haseqdata)
		FD_SET(eq_fd,&write_set);

	memcpy(&read_set,&read_master,sizeof(fd_set));

	select_sleep.tv_sec=30;
	select_sleep.tv_usec=0;
	if ((num=select(max_fd+1,&read_set,&write_set,NULL,&select_sleep))<=0) {
		// Error
	}

	for(itr=connections.begin();itr!=connections.end();itr++) {
		if (FD_ISSET(eq_fd,&write_set) && itr->second->HasOutgoingEQData())
			itr->second->EQ_Connection::Write(eq_fd);
		if ((ircfd=itr->second->FD)>=0) {
			if (FD_ISSET(itr->second->FD,&write_set)) {
				if (itr->second->IRC_Connection::Write()<=0) {
					close(ircfd);
					FD_CLR(ircfd,&read_master);
					itr->second->FD=-1;
					continue;
				}
			}
			if (FD_ISSET(itr->second->FD,&read_set)) {
				if (itr->second->IRC_Connection::Read()<=0) {
					close(ircfd);
					FD_CLR(ircfd,&read_master);
					itr->second->FD=-1;
				} else {
					itr->second->IRC_Connection::Process();
				}
			}
		}
	}

	// Handle controller connection
	if (FD_ISSET(controller.FD,&write_set)) {
		controller.Write();
	}

	if (FD_ISSET(controller.FD,&read_set)) {
		if (controller.Read()<0) {
			close(controller.FD);
			FD_CLR(controller.FD,&read_set);
			controller.FD=-1;
		} else {
			controller.Process();
		}
	}

	if (FD_ISSET(eq_fd,&read_set))
		EQ_Read();

	return 1;
}

int EQIRC_Bridge::EQ_Read()
{
char temp[25];
struct sockaddr_in from;
map<string,EQIRC_Connection *>::iterator itr;
eq_data data;

	while(EQ_Connection::Read(eq_fd,&from,data)>=0) {
		unsigned long ip=from.sin_addr.s_addr;
		sprintf(temp,"%d.%d.%d.%d:%d",
			*(unsigned char *)&ip,
			*((unsigned char *)&ip+1),
			*((unsigned char *)&ip+2),
			*((unsigned char *)&ip+3),
			ntohs(from.sin_port));
		if (Debug>1)
			cout << timestamp() << "Data from " << temp;
		if ((itr=connections.find(temp))!=connections.end()) {
			if (Debug>1)
				cout << " (" << itr->second->GetEQName() << ")" <<endl;
			itr->second->EQ_Connection::Process(data);
		} else {
			if (Debug>1)
				cout  <<endl;
			if (data.buffer[1]==OP_SessionInit) {
				EQIRC_Connection *conn=new EQIRC_Connection(&*this,from);
				conn->EQ_Connection::Address=temp;
				conn->IRC_Connection::Address=temp;
				connections[temp]=conn;
				if ((itr=connections.find(temp))==connections.end()) {
					cerr << timestamp() << "Could not find the connection we just added! This should not happen!" << endl;
					exit(-1);
				}
				itr->second->EQ_Connection::Process(data);
			} else {
				if (data.buffer[1]!=OP_KeepAlive) {
					cout << timestamp() << "[" << temp << "] *** Got a non-init packet for a non-existing connection, dropping it" << endl;
					dump_message(data.buffer,data.length,timestamp());
				}
				if (data.buffer[1]==0x1e) {
					data.buffer[1]=0x05;
					data.length=8;
				} else {
					memset(data.buffer,0,512);
					data.buffer[1]=0x1d;	//Disconnect
					data.length=8;
				}
				SendTo(eq_fd,(char *)data.buffer,data.length,from);
			}
		}
	}
	return 1;
}
