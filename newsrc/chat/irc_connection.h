#ifndef _IRC_CONNECTION_H

#define _IRC_CONNECTION_H

#include <string>
#include <vector>
#include <iostream>
#include <netinet/in.h>
#include "irc_user.h"
#include "irc_channel.h"
#include "irc_line.h"
#include "misc.h"

using namespace std;

class IRC_Connection {
	protected:
		IRC_User Me;
		vector<IRC_Channel> channels;
		vector<string> Out;
		vector<IRC_Line> In;
		string InUnprocessed;
		static char ReadBuffer[8192];
		bool seen_motd;
		int Debug;
	public:
		string Address;
		int FD;
		string Password;

		inline IRC_Connection() { FD=-1; seen_motd=false; Debug=0;}
		inline IRC_Connection(int fd) {FD=fd; seen_motd=false; Debug=0;}
		virtual ~IRC_Connection() { }
		virtual void OnJOIN(string channel, string who) { if (Debug) cout << timestamp() << Me.GetIRCName() << ": " << who << " joining " << channel << endl; }
		virtual void OnPART(string channel, string who) { if (Debug) cout << timestamp() << Me.GetIRCName() << ": " << who << " leaving " << channel << endl; }
		virtual void OnSetOpStatus(string channel, string who, char opstatus) { if (Debug) cout << who << " got " << opstatus << " in  " << channel << endl; }
		virtual void OnNICK(string from, string to) { if (Debug) cout << timestamp() << Me.GetIRCName() << ": " << from << " changed nick to " << to << endl; }
		virtual void OnQUIT(string who) { if (Debug) cout << timestamp() << Me.GetIRCName() << ": " << who << " quit." << endl; }
		virtual void OnLIST(string channel, vector<string> memberlist) { if (Debug) cout << timestamp() << Me.GetIRCName() << ": " << channel << " list: " << endl; }
		virtual void OnPRIVMSG(string from, string to, string message) { if (Debug) cout << timestamp() << Me.GetIRCName() << ": " << from << " said '" << message << "' to " << to << endl; }
		virtual void OnNOTICE(string from, string to, string message) { if (Debug) cout << timestamp() << Me.GetIRCName() << ": " << from << " noticed " << to << " of '" << message << endl; }
		virtual void OnINVITE(string inviter, string invitee, string channel) { if (Debug) cout << timestamp() << Me.GetIRCName() << ": " << inviter << " invited " << invitee << " to " << channel << endl; }
		virtual void OnKICK(string kicker, string kickee, string channel) { if (Debug) cout << timestamp() << Me.GetIRCName() << ": " << kicker << " kicked " << kickee << " from " << channel << endl; }
		virtual void OnPING(string message) { if (Debug) cout << timestamp() << "[" << Address << "] " << Me.GetIRCName() << ": " << "Got pinged with " << message << endl; };
		virtual void OnKILL(string killer, string killee) { if (Debug) cout << timestamp() << Me.GetIRCName() << ": " << killer << " killed " << killee << endl; }
		virtual void OnERROR(string message) { cout << timestamp() << "[" << Address << "] " << Me.GetIRCName() << ": " << "** ERROR: " << message << endl; }
		virtual void OnOther(IRC_Line line) { line.Dump(); }
		virtual void OnNumericReply(int reply, IRC_Line line) { if (Debug) { cout << timestamp() << "[" << Address << "] " << Me.GetIRCName() << ": " << "NumericReply: " << reply << endl; line.Dump(); } }
		void PreProcess(string buffer);
		void Process();
		void Send(string data);
		bool Connect(string host, int port);
		void Login();
		int Read();
		int Write();
		bool HasOutgoingData();
		inline void Nick(string nick="") { if (nick.length()) Me.Parse(nick); if (FD>=0) Send(string("NICK ")+Me.GetIRCName()); }
		inline void PRIVMSG(string to, string message) { Send(string("PRIVMSG ")+to+" :"+message); }
		string GetIRCName() { return Me.GetIRCName(); }
		string GetEQName() { return Me.GetEQName(); }
};

#endif
