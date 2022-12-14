#ifndef LOGINSERVERLIST_H_
#define LOGINSERVERLIST_H_

#include "../common/servertalk.h"
#include "../common/linked_list.h"
#include "../common/timer.h"
#include "../common/queue.h"
#include "../common/eq_packet_structs.h"
#include "../common/Mutex.h"
#include "../common/EmuTCPConnection.h"

#ifdef WIN32
	void AutoInitLoginServer(void *tmp);
#else
	void *AutoInitLoginServer(void *tmp);
#endif

class LoginServer;

class LoginServerList{
public:
	LoginServerList();
	~LoginServerList();
	
	void	Add(const char*, int16, const char*, const char*, bool remoteServer);
	void	InitLoginServer();

	bool	Process();

	bool	SendInfo();
	bool	SendNewInfo();
	bool	SendStatus();

	bool	SendPacket(ServerPacket *pack);	
	bool	SendAccountUpdate(ServerPacket *pack);	
	bool	Connected();
	bool	AllConnected();
	bool	MiniLogin();
	bool	CanUpdate();
	
protected:
	LinkedList<LoginServer*> list;
};




#endif /*LOGINSERVERLIST_H_*/
