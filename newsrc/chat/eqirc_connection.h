#ifndef _EQIRC_CONNECTION_H

#define _EQIRC_CONNECTION_H

#include <string>
#include <vector>
#include <iostream>
#include <netinet/in.h>
#include "irc_user.h"
#include "irc_channel.h"
#include "irc_connection.h"
#include "eq_connection.h"
#include "eq_data.h"
#include "controller.h"

using namespace std;

class EQIRC_Connection : public IRC_Connection, public EQ_Connection {
	friend class Controller;
	private:
		vector<IRC_Channel *> admin_channels;
		vector<IRC_Channel *> channels;
		void *Bridge;
		//Controller control;
	public:
		bool Joining;
		bool Announce;
		time_t last_eq_packet;
		bool Camped;

		inline EQIRC_Connection() : IRC_Connection (), EQ_Connection () { Joining=false; last_eq_packet=0; Camped=false; Announce=false; }
		inline EQIRC_Connection(void *bridge, sockaddr_in from) : IRC_Connection(), EQ_Connection(from) { Bridge=bridge; EQ_Connection::Debug=1; IRC_Connection::Debug=1; Joining=false; last_eq_packet=0; Camped=false; Announce=false; }

		void SendChannelList();

		// Overridding IRC_Connection methods
		void OnJOIN(string channel, string who);
		void OnPART(string channel, string who);
		void OnSetOpStatus(string channel, string who, char opstatus);
		void OnNICK(string from, string to);
		void OnQUIT(string who);
		void OnERROR(string message);
		void OnLIST(string channel, vector<string> memberlist);
		void OnPRIVMSG(string from, string to, string message);
		void OnINVITE(string inviter, string invitee, string channel);
		void OnKICK(string kicker, string kickee, string channel);
		void OnNumericReply(int reply, IRC_Line line);
		void OnNOTICE(string from, string to, string message);
		void OnOther(IRC_Line line) { }

		// Overriding EQ_Connection methods.
		void OnClientLogin();
		void OnClientCommand(string command);
		void OnSessionDisconnect();
		void OnKeepalive();
		void OnPacket(eq_data data);

		// Functions Bridge wants that are ambiguous here
		inline bool HasOutgoingIRCData() { return IRC_Connection::HasOutgoingData(); }
		inline bool HasOutgoingEQData() { return EQ_Connection::HasOutgoingData(); }
};

#endif
