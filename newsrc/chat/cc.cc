#include <stdlib.h>
#include <signal.h>
#include <sqlplus.hh>
#include "eqirc_bridge.h"
#include <string>

void sigpipe_handler(int sig)
{
	cerr << "Got SIGPIPE!" << endl;
}

int main(int argc, char **argv)
{
EQIRC_Bridge b(atoi(argv[1]),"localhost",7654, argv[2]);
	try {
		b.db.connect("test");
	} catch (BadQuery er) {
		cerr << "Error: " << er.error << endl;
		return 1;
	} catch (BadConversion er) { // handle bad conversions
		cerr << "Error: Tried to convert \"" << er.data << "\" to a \"" << er.type_name << "\"." << endl;
		return 1;
	}
	b.LoadServerChannels();
	//b.AdminChannels["Shawn"].push_back("#announcements ircstuff");
	//b.AutoChannels["Shawn"].push_back("#channelbugs ircstuff");
	//b.ForcedChannels["Rog"].push_back("#rairc");
	//b.ForcedChannels["RaidAddicts"].push_back("#rairc");
	//b.ForcedChannels["Scorpious2k"].push_back("#scorpious2k");
	//b.ForcedChannels["Scorpious2k"].push_back("#scorpious2k");
	//b.ForcedChannels["GreatEscape"].push_back("#greatescape");
	//b.AddAltIRCServer("localhost",7654);
	b.AddAltIRCServer("192.168.0.200",7654);
	signal(SIGPIPE,sigpipe_handler);
	while(1)
		b.Process();

	return 0;
		
}

