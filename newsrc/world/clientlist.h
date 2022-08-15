#ifndef CLIENTLIST_H_
#define CLIENTLIST_H_

#include "../common/eq_packet_structs.h"
#include "../common/linked_list.h"
#include "../common/timer.h"
#include "../common/rulesys.h"
#include "../common/servertalk.h"
#include <vector>
#include <string>
#include <map>

class Client;
class ZoneServer;
class WorldTCPConnection;
class ClientListEntry;
class ServerPacket;
struct ServerClientList_Struct;

class ClientList {
public:
	ClientList();
	~ClientList();
	
	void Process();
	
	//from old ClientList
	void	Add(Client* client);
	Client*	Get(int32 ip, int16 port);
	Client* FindByAccountID(int32 account_id);
	Client* FindByName(char* charname);
	Client* FindByCharID(int32 charid);

	// Kings & Bandits, counts pvp total values
	void	CountPVPTotals(uint8 serverFlag, uint32* neutralPlayers, uint32* teamOne, uint32* teamTwo, uint32* teamThree, uint32* teamFour, uint32* teamFive);

	bool	DetermineXPBonus(uint32 teamID, int pctValue);

	void	ZoneBootup(ZoneServer* zs);
	void	RemoveCLEReferances(ClientListEntry* cle);
	
	
	//from ZSList
	
	void	SendWhoAll(int32 fromid,const char* to, sint16 admin, Who_All_Struct* whom, WorldTCPConnection* connection, int32 charid);
	void	SendFriendsWho(ServerFriendsWho_Struct *FriendsWho, WorldTCPConnection* connection);
	void	SendOnlineGuildMembers(uint32 FromID, uint32 GuildID);
	void	SendClientVersionSummary(const char *Name);
	void	SendLFGMatches(ServerLFGMatchesRequest_Struct *LFGMatchesRequest);
	void    ConsoleSendWhoAll(const char* to, sint16 admin, Who_All_Struct* whom, WorldTCPConnection* connection);
	void	SendCLEList(const sint16& admin, const char* to, WorldTCPConnection* connection, const char* iName = 0);

	bool	SendPacket(const char* to, ServerPacket* pack);
	void	SendGuildPacket(int32 guild_id, ServerPacket* pack);
	
	void	ClientUpdate(ZoneServer* zoneserver, ServerClientList_Struct* scl);
	void	CLERemoveZSRef(ZoneServer* iZS);
	ClientListEntry* CheckAuth(int32 iLSID, const char* iKey);
	ClientListEntry* CheckAuth(const char* iName, const char* iPassword);
	ClientListEntry* CheckAuth(int32 id, const char* iKey, int32 ip);
	ClientListEntry* FindCharacter(const char* name);
	ClientListEntry* FindCLEByAccountID(int32 iAccID);
	ClientListEntry* FindCLEByCharacterID(int32 iCharID);
	ClientListEntry* GetCLE(int32 iID);
	int		GetCLEIP(int32 iIP, int32 charID,int32 accountID, ClientListEntry* origEntry, bool countCharacterID=false, bool byAccountID=false);
	void	EnforceSessionLimit(int32 iLSAccountID);
	void	CLCheckStale();
	void	RemoveCLE(ClientListEntry* cle);
	void	CLEKeepAlive(int32 numupdates, int32* wid);
	void	CLEAdd(int32 iLSID, const char* iLoginName, const char* iLoginKey, sint16 iWorldAdmin = 0, int32 ip = 0, uint8 local=0, bool localLoginServer=false);
	void	UpdateClientGuild(int32 char_id, int32 guild_id);
	
	int GetClientCount();
	void GetClients(const char *zone_name, std::vector<ClientListEntry *> &into);

	int32 GetNeutralCount() { return mNeutralCount; }
	int32 GetTeamOneCount() { return mTeamOneCount; }
	int32 GetTeamTwoCount() { return mTeamTwoCount; }
	int32 GetTeamThreeCount() { return mTeamThreeCount; }
	int32 GetTeamFourCount() { return mTeamFourCount; }
	int32 GetTeamFiveCount() { return mTeamFiveCount; }

protected:
	inline int32 GetNextCLEID() { return NextCLEID++; }
	
	//this is the list of people actively connected to zone
	LinkedList<Client*> list;
	
	//this is the list of people in any zone, not nescesarily connected to world
	Timer	CLStale_timer;
	int32 NextCLEID;
	LinkedList<ClientListEntry *> clientlist;
	std::map<int32,int32> whotimestamps;

	uint32 mNeutralCount;
	uint32 mTeamOneCount;
	uint32 mTeamTwoCount;
	uint32 mTeamThreeCount;
	uint32 mTeamFourCount;
	uint32 mTeamFiveCount;

};




#endif /*CLIENTLIST_H_*/
