#ifndef _CONTROLLER_H

#define _CONTROLLER_H

#include <vector>
#include <string>
#include "irc_connection.h"
#include "irc_replies.h"
#include "command.h"

class Controller : public IRC_Connection {
	Command CommandList;
	void InitCommands();
	
	public:
	vector<string> AuthList;
	Controller() { InitCommands(); }
	void *Bridge;
	void OnNumericReply(int reply, IRC_Line line);
	void OnNOTICE(string from, string to, string message);
	void OnPRIVMSG(string from, string to, string message);
	void OnOther(IRC_Line line) { }
	inline int GetAuthLevel(string who) { return (find(AuthList.begin(),AuthList.end(),who)==AuthList.end()) ? 0 : 1; }
	vector<string> handlePING(string command, string from);
	vector<string> handleSUICIDE(string command, string from);
	vector<string> handleDO(string command, string from);
	vector<string> handleSTATUS(string command, string from);
	vector<string> handleWHO(string command, string from);
	vector<string> handleBROADCAST(string command, string from);
	vector<string> handleCHANNEL(string command, string from);
	vector<string> handleAUTHPASS(string command, string from);
	vector<string> handleAUTHLIST(string command, string from);
	vector<string> handleAUTHADD(string command, string from);
	vector<string> handleAUTHDEL(string command, string from);
	vector<string> handleCONFIGRELOAD(string command, string from);
	vector<string> handleCONFIGFORCED(string command, string from);
	vector<string> handleCONFIGADMIN(string command, string from);
	vector<string> handleCONFIGAUTO(string command, string from);
};

#endif

