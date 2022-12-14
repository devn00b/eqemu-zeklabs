/*  EQEMu:  Everquest Server Emulator
    Copyright (C) 2001-2002  EQEMu Development Team (http://eqemu.org)

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
#ifndef ZONE_H
#define ZONE_H

#include "../common/Mutex.h"
#include "../common/linked_list.h"
#include "../common/types.h"
#include "../common/eqtime.h"
#include "../common/servertalk.h"
#include "../common/rulesys.h"
#include "../common/eq_packet_structs.h"
#include "features.h"
#include "spawngroup.h"
//#include "mob.h"
#include "zonedump.h"
#include "spawn2.h"
#include "tasks.h"
#include "pathing.h"
#include "QGlobals.h"

class Map;
class WaterMap;

class KBAchievement
{
public:
	KBAchievement(int32 id, int32 intimestamp, int32 incooldown_seconds, int8 incooldown, int32 zoneid)
	{
		achievementid = id;
		timestamp = intimestamp;
		cooldown_seconds = incooldown_seconds;
		cooldown = incooldown;
		zoneid = zoneid;
	}

	KBAchievement(int32 id, char* aname, char* desc, int32 rkills, int32 rdeaths, int32 rkillstreak, int32 rpoints, int32 rlevel, int32 ritemid,
		int32 pts, int32 coin, int32 bcast, int32 eitemid, int32 zreqkills, int32 zreqdeaths, int8 rteamid, int32 rqtime, int32 rqtimezonedeath, int32 repeatable, int32 incooldown_seconds)
	{
		achievementid = id;
		strncpy(name,aname,32);
		strncpy(description,desc,128);
		reqkills = rkills;
		reqdeaths =  rdeaths;
		reqkillstreak = rkillstreak;
		reqpoints = rpoints;
		reqlevel = rlevel;
		reqitemid = ritemid;

		earnpoints = pts;
		earncoin = coin;
		earnitemid = eitemid;
		broadcast = bcast;

		reqzonekills = zreqkills;
		reqzonedeaths = zreqdeaths;

		reqteamid = rteamid;
		
		reqtime = rqtime;
		reqtimezonedeath = rqtimezonedeath;
		isrepeatable = repeatable;
		
		cooldown_seconds = incooldown_seconds;
		cooldown = ( incooldown_seconds > 0 ) ? 1 : 0;
		zoneid = 0;
	}
	int32 achievementid;
	char name[32];
	char description[128];
	int32 reqkills;
	int32 reqdeaths;
	int32 reqkillstreak;
	int32 reqpoints;
	int32 reqlevel;
	int32 reqitemid;
	
	int32 reqzonekills;
	int32 reqzonedeaths;
	
	int8 reqteamid;

	int32 earnpoints;
	int32 earncoin;
	int32 earnitemid;

	int32 broadcast;
	
	int32 reqtime;
	int32 reqtimezonedeath;

	int32 isrepeatable; // 0 = no, 1 = yes
	
	int32 timestamp;
	int32 cooldown_seconds;
	int8 cooldown;
	int32 zoneid;
};

struct ZonePoint 
{
	float x;
	float y;
	float z;
	float heading;
	int16 number;
	float target_x;
	float target_y;
	float target_z;
	float target_heading;
	int16 target_zone_id;
	sint32 target_zone_instance;
	int32 client_version_mask;
	float radius;
};
struct ZoneClientAuth_Struct {
	int32	ip;			// client's IP address
	int32	wid;		// client's WorldID#
	int32	accid;
	sint16	admin;
	int32	charid;
	bool	tellsoff;
	char	charname[64];
	char	lskey[30];
	bool	stale;
	bool	officialClient;
};

extern EntityList entity_list;
class database;
class PathManager;
struct SendAA_Struct;

class database;

class Zone
{
public:
	static bool Bootup(int32 iZoneID, int32 iInstanceID, bool iStaticZone = false);
	static void Shutdown(bool quite = false);
	
	Zone(int32 in_zoneid, int32 in_instanceid, const char* in_short_name);
	~Zone();
	bool	Init(bool iStaticZone);
	bool	LoadZoneCFG(const char* filename, uint16 instance_id, bool DontLoadDefault = false);
	bool	SaveZoneCFG();
	bool	IsLoaded();
	bool	IsPVPZone() { return pvpzone; }
	inline const char*	GetLongName()	{ return long_name; }
	inline const char*	GetFileName()	{ return file_name; }
	inline const char*	GetShortName()	{ return short_name; }
	inline const int32	GetZoneID() const { return zoneid; }
	inline const int32	GetInstanceID() const { return instanceid; }
	inline const int16	GetInstanceVersion() const { return instanceversion; }

	static void	ClearZonePoints(LinkedList<ZonePoint*>* zone_points);

	inline Timer* GetInstanceTimer() { return Instance_Timer; }

	inline const float&	safe_x()		{ return psafe_x; }
	inline const float&	safe_y()		{ return psafe_y; }
	inline const float&	safe_z()		{ return psafe_z; }
	inline const int32& graveyard_zoneid()	{ return pgraveyard_zoneid; }
	inline const float& graveyard_x()	{ return pgraveyard_x; }
	inline const float& graveyard_y()	{ return pgraveyard_y; }
	inline const float& graveyard_z()	{ return pgraveyard_z; }
	inline const float& graveyard_heading() { return pgraveyard_heading; }
	inline const int32& graveyard_id()	{ return pgraveyard_id; }

	inline const int32& GetMaxClients() { return pMaxClients; }

	void	LoadAAs();
	int		GetTotalAAs() { return totalAAs; }
	SendAA_Struct*	GetAABySequence(int32 seq) { return aas[seq]; }
	SendAA_Struct*	FindAA(int32 id);
	uint8	GetTotalAALevels(int32 skill_id);
	void	LoadZoneDoors(const char* zone, int16 version);
	bool	LoadZoneObjects();
	bool	LoadGroundSpawns();
	void	ReloadStaticData();
	
	int32	CountSpawn2();
	ZonePoint* GetClosestZonePoint(float x, float y, float z, const char* to_name, Client *client, float max_distance = 40000.0f);
	ZonePoint* GetClosestZonePoint(float x, float y, float z, int32	to, Client *client, float max_distance = 40000.0f);
	ZonePoint* GetClosestZonePointWithoutZone(float x, float y, float z, Client *client, float max_distance = 40000.0f);
	SpawnGroupList spawn_group_list;

	bool RemoveSpawnEntry(uint32 spawnid);
	bool RemoveSpawnGroup(uint32 in_id);
	
	bool	Process();
	void	DumpAllSpawn2(ZSDump_Spawn2* spawn2dump, int32* spawn2index);
	int32	DumpSpawn2(ZSDump_Spawn2* spawn2dump, int32* spawn2index, Spawn2* spawn2);

	bool	Depop(bool StartSpawnTimer = true);
	void	Repop(int32 delay = 0);
	void	SpawnStatus(Mob* client);
	void	ShowEnabledSpawnStatus(Mob* client);
	void    ShowDisabledSpawnStatus(Mob* client);
	void    ShowSpawnStatusByID(Mob* client, uint32 spawnid);
	void	StartShutdownTimer(int32 set_time = (RuleI(Zone, AutoShutdownDelay)));
	void	AddAuth(ServerZoneIncommingClient_Struct* szic);
	void	RemoveAuth(const char* iCharName);
	void	ResetAuth();
	bool	GetAuth(int32 iIP, const char* iCharName, int32* oWID = 0, int32* oAccID = 0, int32* oCharID = 0, sint16* oStatus = 0, char* oLSKey = 0, bool* oTellsOff = 0, bool* officialClient = 0);
	int32	CountAuth();

	void		AddAggroMob()			{ aggroedmobs++; }
	void		DelAggroMob()			{ aggroedmobs--; }
	bool		AggroLimitReached()		{ return (aggroedmobs>10)?true:false; } // change this value, to allow more NPCs to autoaggro
	sint32		MobsAggroCount()		{ return aggroedmobs; }
	inline bool InstantGrids()			{ return(!initgrids_timer.Enabled()); }
	void		SetStaticZone(bool sz)	{ staticzone = sz; }
	inline bool	IsStaticZone()			{ return staticzone; }
	inline void	GotCurTime(bool time)	{ gottime = time; }
	void DBAWComplete(int8 workpt_b1, DBAsyncWork* dbaw);
	
	void	SpawnConditionChanged(const SpawnCondition &c, sint16 old_value);
		
	void	GetMerchantDataForZoneLoad();
	void	LoadNewMerchantData(uint32 merchantid);
	void	LoadTempMerchantData();
	sint32	GetTempMerchantQuantity(int32 NPCID, uint32 Slot);
	void	LoadTempMerchantData_result(MYSQL_RES* result);
	void	LoadMerchantData_result(MYSQL_RES* result);
	int		SaveTempItem(int32 merchantid, int32 npcid, int32 item, sint32 charges, bool sold=false);

	void SetInstanceTimer(int32 new_duration);
	void LoadLDoNTraps();
	void LoadLDoNTrapEntries();
	void LoadAdventureFlavor();
	
	map<uint32,NPCType *> npctable;
	map<uint32,EQApplicationPacket*> merchantbuffer;
	map<uint32,std::list<MerchantList> > merchanttable;
	map<uint32,std::list<TempMerchantList> > tmpmerchanttable;
	map<uint32,std::string> adventure_entry_list_flavor;
	map<uint32,LDoNTrapTemplate*> ldon_trap_list;
	map<uint32,std::list<LDoNTrapTemplate*> > ldon_trap_entry_list;
	list<InternalVeteranReward> VeteranRewards;
	char *adv_data;
	bool did_adventure_actions;

	void	DoAdventureCountIncrease();
	void	DoAdventureAssassinationCountIncrease();
	void	DoAdventureActions();
	void	LoadVeteranRewards();

	Map*	zonemap;
	WaterMap* watermap;
	PathManager *pathing;
	NewZone_Struct	newzone_data;
	int8	zone_weather;

	SpawnConditionManager spawn_conditions;
	
	EQTime	zone_time;
	void	GetTimeSync();
	void	SetDate(int16 year, int8 month, int8 day, int8 hour, int8 minute);
	void	SetTime(int8 hour, int8 minute);

	void	weatherSend(Client* sendTo=NULL);
	bool	CanBind() const { return(can_bind); }
	bool	IsCity() const { return(is_city); }
	bool	CanDoCombat() const { return(can_combat); }
	bool	CanLevitate() const {return(can_levitate); } // Magoth78
	bool    CanCastOutdoor() const {return(can_castoutdoor);} //qadar
	// Kings & Bandits addon can_pvp flag, 0 = no, 1 = yes (team), 2 = yes (ffa)
	uint8	CanPVPValue() { return can_pvp; }
	void	SetCanPVPValue(uint8 value) { can_pvp = value; }
	
	uint8	CanHateSummonFull() { return canhatesummonfull; }
	void	SetHateSummonFull(uint8 value) { canhatesummonfull = value; }
	
	void	SetIsHotZone(bool value ) { is_hotzone = value; }
	bool	IsHotzone() const { return(is_hotzone); }
	
	void	SetIsPVPHotZone(int32 value ) { is_pvphotzone = value; }
	int32	IsPVPHotzone() { return(is_pvphotzone); }
	inline	bool BuffTimersSuspended() const { return newzone_data.SuspendBuffs; };
	
	float	GetNoLOSPetDistance() { return no_pet_los_distance; }
	
	bool	UseDeathMods() const {return(use_death_mods); }

	time_t	weather_timer;
	int8	weather_type;
	
	int8 loglevelvar;
	int8 merchantvar;
	int8 tradevar;
	int8 lootvar;

	bool	HasGraveyard();
	void	SetGraveyard(int32 zoneid, int32 x, int32 y, int32 z, int32 heading);
	
	// Kings & Bandits Achievements
	void		LoadAchievements(int32 zoneid);

	void		LoadBlockedSpells(int32 zoneid);
	void		ClearBlockedSpells();
	bool		IsSpellBlocked(int32 spell_id, float nx, float ny, float nz);
	const char *GetSpellBlockedMessage(int32 spell_id, float nx, float ny, float nz);
	int			GetTotalBlockedSpells() { return totalBS; }
	inline bool HasMap() { return zonemap != NULL; }
	inline bool HasWaterMap() { return watermap != NULL; }

	QGlobalCache *GetQGlobals() { return qGlobals; }
	QGlobalCache *CreateQGlobals() { qGlobals = new QGlobalCache(); return qGlobals; }
	void	UpdateQGlobal(uint32 qid, QGlobal newGlobal);
	void	DeleteQGlobal(std::string name, uint32 npcID, uint32 charID, uint32 zoneID);

	LinkedList<Spawn2*> spawn2_list;
	LinkedList<ZonePoint*> zone_point_list;
	int32	numzonepoints;

	LinkedList<KBAchievement*> achievements;
	KBAchievement* GetAchievementID(int32 achievementid);

	bool IsGuildManagementDisabled() { return guildmanagedisabled; }

	void SetServerPlayerCounts(uint32 neutralAmt, uint32 teamOneAmt, uint32 teamTwoAmt, uint32 teamThreeAmt, uint32 teamFourAmt, uint32 teamFiveAmt)
	{
		neutralCount = neutralAmt;
		teamOneCount = teamOneAmt;
		teamTwoCount = teamTwoAmt;
		teamThreeCount = teamThreeAmt;
		teamFourCount = teamFourAmt;
		teamFiveCount = teamFiveAmt;
	}

	uint32 GetPlayerCount(uint32 teamid)
	{
		switch(teamid){
			case 0:
				return neutralCount;
			break;
			case 1:
				return teamOneCount;
			break;
			case 2:
				return teamTwoCount;
			break;
			case 3:
				return teamThreeCount;
			break;
			case 4:
				return teamFourCount;
			break;
			case 5:
				return teamFiveCount;
			break;
		}

		return 0;
	}
private:
	int32	zoneid;
	int32	instanceid;
	int16	instanceversion;
	char*	short_name;
	char	file_name[16];
	char*	long_name;
	char*	map_name;
	bool pvpzone;
	bool guildmanagedisabled;
	float	psafe_x, psafe_y, psafe_z;
	int32	pMaxClients;
	bool	can_bind;
	bool	is_city;
	bool	can_combat;
	bool    can_castoutdoor;
	bool	can_levitate;
	// use the modified respawn / loot values
	bool	use_death_mods;
	// Kings & Bandits addon can_pvp flag, 0 = no, 1 = yes (team), 2 = yes (ffa)
	uint8	can_pvp;
	uint8	canhatesummonfull;
	bool	is_hotzone;
	int32	is_pvphotzone;
	int32	pgraveyard_id, pgraveyard_zoneid;
	float	pgraveyard_x, pgraveyard_y, pgraveyard_z, pgraveyard_heading;
	int     default_ruleset;
	float	no_pet_los_distance;

	int	totalBS;
	ZoneSpellsBlocked *blocked_spells;
	
	int		totalAAs;
	SendAA_Struct **aas;	//array of AA structs
	
	uint32 neutralCount,teamOneCount,teamTwoCount,teamThreeCount,teamFourCount,teamFiveCount;

	/*
		Spawn related things
	*/
	sint32	aggroedmobs;
	Timer initgrids_timer;	//delayed loading of initial grids.


	bool	staticzone;
	bool	gottime;
	
	int32 pQueuedMerchantsWorkID;
	int32 pQueuedTempMerchantsWorkID;

	Timer	autoshutdown_timer;
	Timer	clientauth_timer;
	Timer	spawn2_timer;
	Timer	qglobal_purge_timer;
	Timer*  Weather_Timer;
	Timer*	Instance_Timer;
	Timer*	Instance_Shutdown_Timer;
	Timer*  Instance_Warning_timer;
	LinkedList<ZoneClientAuth_Struct*> client_auth_list;
	QGlobalCache *qGlobals;
	// Kings & Bandits zone timers
	// hourly hot zone timer
	Timer*  Hour_Timer;

	Mutex	MZoneLock;
};

#endif

