/*  EQEMu:  Everquest Server Emulator
	Copyright (C) 2001-2003  EQEMu Development Team (http://eqemulator.net)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY except by those people which sell it, which
	are required to give you total support for your newly bought product;
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef EQEMU_DATABASE_H
#define EQEMU_DATABASE_H

#define AUTHENTICATION_TIMEOUT	180
#define INVALID_ID				0xFFFFFFFF

#include "debug.h"
#include "types.h"
#include "dbcore.h"
#include "linked_list.h"
#include "eq_packet_structs.h"
#include "EQPacket.h"
/*#include "EQStream.h"
#include "guilds.h"
#include "MiscFunctions.h"
#include "Mutex.h"
#include "Item.h"
#include "extprofile.h"*/
#include <string>
#include <vector>
#include <map>
using namespace std;

//atoi is not int32 or uint32 safe!!!!
#define atoul(str) strtoul(str, NULL, 10)

//class Spawn;
class Corpse;
class Spawn2;
class NPC;
class SpawnGroupList;
class Petition;
class Client;
struct Combine_Struct;
//struct Faction;
//struct FactionMods;
//struct FactionValue;
struct ZonePoint;
struct NPCType;
class Inventory;
class ItemInst;

struct EventLogDetails_Struct {
	int32	id;
	char	accountname[64];
	int32	account_id;
	sint16	status;
	char	charactername[64];
	char	targetname[64];
	char	timestamp[64];
	char	descriptiontype[64];
	char	details[128];
};

struct ReplayList_Struct {
	int32	replayid;
	char	startTime[64];
	char	endTime[64];
};

struct CharacterEventLog_Struct {
int32	count;
int8	eventid;
EventLogDetails_Struct eld[255];
};


// Added By Hogie 
// INSERT into variables (varname,value) values('decaytime [minlevel] [maxlevel]','[number of seconds]');
// IE: decaytime 1 54 = Levels 1 through 54
//     decaytime 55 100 = Levels 55 through 100
// It will always put the LAST time for the level (I think) from the Database
struct npcDecayTimes_Struct {
	int16 minlvl;
	int16 maxlvl;
	int32 seconds;
};
// Added By Hogie -- End

struct VarCache_Struct {
	char varname[26];	// varname is char(25) in database
	char value[0];
};

struct PlayerProfile_Struct;
struct GuildRankLevel_Struct;
struct GuildRanks_Struct;
struct ExtendedProfile_Struct;
struct GuildMember_Struct;
class PTimerList;

class Database : public DBcore {
public:
	Database();
	Database(const char* host, const char* user, const char* passwd, const char* database,int32 port);
	bool Connect(const char* host, const char* user, const char* passwd, const char* database,int32 port);
	~Database();
	
	
//	void	ExtraOptions();
	

	/*
	 * General Character Related Stuff
	 */
	bool	UpdateName(const char* oldname, const char* newname);
	bool	SetHackerFlag(const char* accountname, const char* charactername, const char* hacked);
	bool	SetMQDetectionFlag(const char* accountname, const char* charactername, const char* hacked, const char* zone);
	bool	AddToNameFilter(const char* name);
	bool	ReserveName(int32 account_id, char* name);
	bool	CreateCharacter(uint32 account_id, char* name, int16 gender, int16 race, int16 class_, int8 str, int8 sta, int8 cha, int8 dex, int8 int_, int8 agi, int8 wis, int8 face);
	bool	DeleteCharacter(char* name);
	
	bool GetStartZone(PlayerProfile_Struct* in_pp, CharCreate_Struct* in_cc);
	bool GetStartZoneSoF(PlayerProfile_Struct* in_pp, CharCreate_Struct* in_cc);

	/*
	 * General Information Getting Queries
	 */
	bool	CheckNameFilter(const char* name, bool surname = false);
	bool	CheckUsedName(const char* name);
	int32	GetAccountIDByChar(const char* charname, int32* oCharID = 0);
	uint32	GetAccountIDByChar(uint32 char_id);
	int32	GetAccountIDByName(bool eqemuTable,const char* accname, sint16* status = 0, int32* lsid = 0);
	void	GetAccountName(int32 accountid, char* name, int32* oLSAccountID = 0);
	void	GetCharName(int32 char_id, char* name);
	uint8	GetCharLevel(int32 char_id);
	int32	GetCharacterID(const char *name);
	bool	CheckBannedIPs(const char* loginIP); //Lieka Edit:  Check incomming connection against banned IP table.
 	bool	AddBannedIP(char* bannedIP, const char* notes); //Lieka Edit:  Add IP address to the Banned_IPs table.
	bool	CheckGMIPs(const char* loginIP, int32 account_id);
	bool	AddGMIP(char* ip_address, char* name);
	void	LoginIP(int32 AccountID, const char* LoginIP);

	/*
	 * Instancing Stuff
	 */
	bool VerifyZoneInstance(int32 zone_id, int16 instance_id);
	bool VerifyInstanceAlive(int16 instance_id, int32 char_id);
	bool CharacterInInstanceGroup(int16 instance_id, int32 char_id);
	void DeleteInstance(uint16 instance_id);
	bool CheckInstanceExpired(uint16 instance_id);
	int32 ZoneIDFromInstanceID(uint16 instance_id);
	int32 VersionFromInstanceID(uint16 instance_id);
	int32 GetTimeRemainingInstance(uint16 instance_id, bool &is_perma);
	bool GetUnusedInstanceID(uint16 &instance_id);
	bool CreateInstance(uint16 instance_id, uint32 zone_id, uint32 version, uint32 duration);
	void PurgeExpiredInstances();
	bool AddClientToInstance(uint16 instance_id, uint32 char_id);
	bool RemoveClientFromInstance(uint16 instance_id, uint32 char_id);
	bool RemoveClientsFromInstance(uint16 instance_id);
	bool CheckInstanceExists(uint16 instance_id);
	void BuryCorpsesInInstance(uint16 instance_id);
	int16 GetInstanceVersion(uint16 instance_id);
	void SetInstanceDuration(int16 instance_id, int32 new_duration);
	bool GlobalInstance(uint16 instance_id);

	/*
	 * Adventure related.
	 */
	void UpdateAdventureStatsEntry(int32 char_id, int8 theme, bool win);
	bool GetAdventureStats(int32 char_id, int32 &guk_w, int32 &mir_w, int32 &mmc_w, int32 &ruj_w, int32 &tak_w, 
		int32 &guk_l, int32 &mir_l, int32 &mmc_l, int32 &ruj_l, int32 &tak_l);

	/*
	 * Account Related
	 */
	int32	GetMiniLoginAccount(char* ip);
	void	GetAccountFromID(int32 id, char* oAccountName, sint16* oStatus);
	int32	CheckLogin(const char* name, const char* password, sint16* oStatus = 0, bool* eqemuLogin=0);
	sint16	CheckStatus(int32 account_id);
	int32	CreateAccount(bool eqemuTable,const char* name, const char* password, sint16 status, int32 lsaccount_id = 0);
	bool	DeleteAccount(const char* name);
 	bool	SetAccountStatus(const char* name, sint16 status);
	bool	SetLocalPassword(uint32 accid, const char* password);

	// Kings & Bandits - Dual login support
	int32	GetAccountIDFromLSID(bool eqemuTable, int32 iLSID, char* oAccountName = 0, sint16* oStatus = 0);
	bool	UpdateLiveChar(char* charname,int32 lsaccount_id);
	bool	GetLiveChar(int32 account_id, char* cname);
	bool	GetAgreementFlag(int32 acctid, int8* agreementFlag=0, int32* syncAccID=0);
	void	SetAgreementFlag(int32 acctid);
	
	/*
	 * Groups
	 */
	int32	GetGroupID(const char* name);
	void	SetGroupID(const char* name, int32 id, int32 charid);
	void	ClearGroup(int32 gid = 0);
	char*	GetGroupLeaderForLogin(const char* name,char* leaderbuf);
	
	void	SetGroupLeaderName(int32 gid, const char* name);
	char*	GetGroupLeadershipInfo(int32 gid, char* leaderbuf, char* assist = NULL, char *marknpc = NULL,
				       GroupLeadershipAA_Struct* GLAA = NULL);
	void	ClearGroupLeader(int32 gid = 0);

	/*
	 * Raids
	 */
	void	ClearRaid(int32 rid = 0);
	void	ClearRaidDetails(int32 rid = 0);
	int32	GetRaidID(const char* name);
	const char *GetRaidLeaderName(int32 rid);

	/*
	 * Database Varaibles
	 */
	bool	GetVariable(const char* varname, char* varvalue, int16 varvalue_len);
	bool	SetVariable(const char* varname, const char* varvalue);
	bool	LoadVariables();
	int32	LoadVariables_MQ(char** query);
	bool	LoadVariables_result(MYSQL_RES* result);
	
	/*
	 * General Queries
	 */
	/*bool	LoadZoneNames();
	bool	QueryZoneNames(uint32* maxID);
	bool	extDBZoneNames(sint32 iZoneCount, int32 iMaxZoneID);
	bool	LoadZoneShortNames();*/

	void	DeleteFactionTable(uint32 charID);

	bool	GetZoneLongName(const char* short_name, char** long_name, char* file_name = 0, float* safe_x = 0, float* safe_y = 0, float* safe_z = 0, int32* graveyard_id = 0, int32* maxclients = 0, bool* disableGuildInviteRemove = 0);
	bool	GetZoneGraveyard(const int32 graveyard_id, int32* graveyard_zoneid = 0, float* graveyard_x = 0, float* graveyard_y = 0, float* graveyard_z = 0, float* graveyard_heading = 0);
	int32	GetZoneGraveyardID(int32 zone_id, int32 version);
	//int32	GetZoneID(const char* zonename);
	int8    GetPEQZone(int32 zoneID, int32 version);
	//const char*	GetZoneName(int32 zoneID, bool ErrorUnknown = false);
	int8	GetServerType();
	int8	GetSkillCap(int8 skillid, int8 in_race, int8 in_class, int16 in_level);
	int8	GetRaceSkill(int8 skillid, int8 in_race);
	bool	LoadPTimers(uint32 charid, PTimerList &into);
	void	ClearPTimers(uint32 charid);
	void	ClearMerchantTemp();
	void	SetLFP(int32 CharID, bool LFP); 
	void	SetLFG(int32 CharID, bool LFG);
	void	AddReport(std::string who, std::string against, std::string lines);
	
	
	
	bool	DeleteLoginStats(int32 id);
	bool	DeleteLoginWorldStats(int32 id);
	bool	UpdateLoginStats(int32 id, sint32 players, sint32 zones, sint32 servers);
	bool	InsertLoginStats(int32 id, char* loginName,char* loginDesc, char* loginAddr, sint32 players, sint32 zones, sint32 servers,int8 loginmode);
	bool	InsertWorldStats(int32 loginid, char* worldName, sint32 players, sint32 zones, int8 greenName=0);
	bool	UpdateWorldStats(int32 worldid, sint32 players, sint32 zones,int8 greenname);
	bool	DeleteAllStats();

	bool	AddVeteranReward(int32 account_id, int32 reward_id, int32 amount);
	
#ifdef LOGINCRYPTO

bool	SetLSAuthChange(int32 account_id, const char* ip);
	bool	UpdateLSAccountAuth(int32 account_id, int8* auth);
	int32	GetLSLoginInfo(char* iUsername, char* oPassword = 0, int8* lsadmin = 0, int* lsstatus = 0, bool* verified = 0, sint16* worldadmin = 0);
//	int32	GetLSLoginInfo(char* iUsername, char* oPassword = 0, char* md5pass = 0);
	int32	GetLSAuthentication(int8* auth);
	int32	GetLSAuthChange(const char* ip);
	bool	AddLoginAccount(char* stationname, char* password, char* chathandel, char* cdkey, char* email);
	int8	ChangeLSPassword(int32 accountid, const char* newpassword, const char* oldpassword = 0);
	bool	ClearLSAuthChange();
	bool	RemoveLSAuthChange(int32 account_id);
	bool	GetLSAccountInfo(int32 account_id, char* name, int8* lsadmin, int* status, bool* verified, sint16* worldadmin = 0);
	sint8	CheckWorldAuth(const char* account, const char* password, int32* account_id, int32* admin_id, int8* GreenName, bool* ShowDown);
	bool	UpdateWorldName(int32 accountid, const char* name);
	char*	GetBook(char txtfile[14]);
	void	LoadWorldList();

	int32	CheckEQLogin(const EQRawApplicationPacket* app, char* oUsername, int8* lsadmin, int* lsstatus, bool* verified, sint16* worldadmin,int32 ip);
int8	CheckWorldVerAuth(char* version);
	
#endif
	
protected:
	void	HandleMysqlError(int32 errnum);
	//bool	RunQuery(const char* query, int32 querylen, char* errbuf = 0, MYSQL_RES** result = 0, int32* affected_rows = 0, int32* errnum = 0, bool retry = true);
	
private:
	void DBInitVars();
	
	int32				max_zonename;
	char**				zonename_array;
	
	Mutex				Mvarcache;
	uint32				varcache_max;
	VarCache_Struct**	varcache_array;
	uint32				varcache_lastupdate;
};

bool	FetchRowMap(MYSQL_RES *result, map<string,string> &rowmap);
#endif
