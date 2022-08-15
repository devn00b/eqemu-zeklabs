#ifndef SHAREDDB_H_
#define SHAREDDB_H_

#define MAX_ITEM_ID				125000

#include "database.h"
#include "skills.h"
#include "../zone/spdat.h"
#include "Item.h"

#include <list>

/*Kings & Bandits Header Addition shareddb.h*/
#include "../KingsBandits/KBStructs.h"

struct Item_Struct;
struct NPCFactionList;
struct Faction;
struct LootTable_Struct;
struct LootDrop_Struct;

	enum RNDITEM_DROP_TYPE
	{
		NONE=0,
		EXTREMERARE=1,
		RARE=2,
		SPECIAL=3,
		COMMON=4,
		NEWEXTREMERARE=5,
		NEWRARE=6,
		NEWSPECIAL=7,
		NEWCOMMON=8
	};

/*
 * This object is inherited by world and zone's DB object,
 * and is mainly here to facilitate shared memory, and other
 * things which only world and zone need.
 */
class SharedDatabase : public Database {
public:
	SharedDatabase();
	SharedDatabase(const char* host, const char* user, const char* passwd, const char* database,int32 port);
	virtual ~SharedDatabase();

	/*
	 * General Character Related Stuff
	 */
	bool	SetGMSpeed(int32 account_id, int8 gmspeed);
	int8	GetGMSpeed(int32 account_id);
	bool	SetHideMe(int32 account_id, int8 hideme);
	bool	SetGMToggle(int32 account_id, int8 tellsoff);
	bool	GetPlayerProfile(uint32 account_id, char* name, PlayerProfile_Struct* pp, Inventory* inv, ExtendedProfile_Struct *ext, char* current_zone = 0, uint32 *current_instance = 0);
	bool	SetPlayerProfile(uint32 account_id, uint32 charid, PlayerProfile_Struct* pp, Inventory* inv, ExtendedProfile_Struct *ext, uint32 current_zone = 0, uint32 current_instance = 0);
	int32	SetPlayerProfile_MQ(char** query, uint32 account_id, uint32 charid, PlayerProfile_Struct* pp, Inventory* inv, ExtendedProfile_Struct *ext, uint32 current_zone = 0, uint32 current_instance = 0);
	sint32	DeleteStalePlayerCorpses();
	sint32	DeleteStalePlayerBackups();

	bool	UpdatePlayerProfile(PlayerProfile_Struct* pp, ExtendedProfile_Struct* ext);

	int32	GetTotalTimeEntitledOnAccount(uint32 AccountID);

	/*
	 * Character Inventory
	 */
	static bool IsBankSlot(uint32 slot, bool excludeShared=false);
	bool	SaveCursor(int32 char_id, std::list<ItemInst*>::const_iterator &start, std::list<ItemInst*>::const_iterator &end);
	bool	SaveInventory(uint32 char_id, const ItemInst* inst, sint16 slot_id, Inventory* inv=0, int32 zone_id=0);
	bool	VerifyInventory(uint32 account_id, sint16 slot_id, const ItemInst* inst);
	bool	GetSharedBank(uint32 id, Inventory* inv, bool is_charid);
	sint32	GetSharedPlatinum(int32 account_id);
	bool	SetSharedPlatinum(int32 account_id, sint32 amount_to_add);
	bool	LoadItemFromRow(int32 char_id, Inventory* inv, sint16 slot_id, MYSQL_ROW row, int32 zoneid, bool teamBank=false);
	bool	GetInventory(uint32 char_id, Inventory* inv, int32 zone_id);
	bool	GetInventory(uint32 account_id, char* name, Inventory* inv, int32 zone_id);
	bool    SetStartingItems(PlayerProfile_Struct* pp, Inventory* inv, uint32 si_race, uint32 si_class, uint32 si_deity, uint32 si_current_zone, char* si_name, int admin);
	bool	LoadCityBank(int32 teamid, Inventory* inv, int32 zone_id);
	
	
	string	GetBook(const char *txtfile);
	
	/*
	 * Item Methods
	 */
	ItemInst* CreateItem(uint32 item_id);
	ItemInst* CreateItem(uint32 item_id, sint16 charges, uint32 aug1=0, uint32 aug2=0, uint32 aug3=0, uint32 aug4=0, uint32 aug5=0);
	ItemInst* CreateItem(const Item_Struct* item, sint16 charges=0, uint32 aug1=0, uint32 aug2=0, uint32 aug3=0, uint32 aug4=0, uint32 aug5=0);
	ItemInst* CreateBaseItem(const Item_Struct* item);
	ItemInst* CreateBaseItem(const Item_Struct* item, sint16 charges);
	
	/*
	 * Shared Memory crap
	 */
	inline const int32	GetMaxItem()			{ return max_item; }
	inline const int32	GetMaxLootTableID()		{ return loottable_max; }
	inline const int32	GetMaxLootDropID()		{ return lootdrop_max; }
	inline const int32	GetMaxNPCType()			{ return max_npc_type; }
	inline const int32  GetMaxNPCFactionList()	{ return npcfactionlist_max; }
	const Item_Struct*	GetItem(uint32 id);
	Item_Struct*	GetItemNoConst(uint32 id);
	const EvolveInfo*	GetEvolveInfo(uint32 loregroup);
	const NPCFactionList*	GetNPCFactionEntry(uint32 id);
	int16	GetSkillCap(int8 Class_, SkillType Skill, int8 Level);
	int8	GetTrainLevel(int8 Class_, SkillType Skill, int8 Level);
//	const Door*				GetDoor(int8 door_id, const char* zone_name);
//	const Door*				GetDoorDBID(uint32 db_id);
	const	LootTable_Struct* GetLootTable(int32 loottable_id);
	const	LootDrop_Struct* GetLootDrop(int32 lootdrop_id);
	bool	LoadItems();
	bool	LoadLoot();
	bool	LoadNPCFactionLists();
	bool	LoadSkillCaps();
	bool	GetCommandSettings(map<string,uint8> &commands);
	const Item_Struct* IterateItems(uint32* NextIndex);
	bool	DBLoadItems(sint32 iItemCount, uint32 iMaxItemID);
	bool	DBLoadZoneNames(sint32 iZoneCount, uint32 iMaxZoneID);


	bool	DBLoadNPCTypes(sint32 iNPCTypeCount, uint32 iMaxNPCTypeID);
	bool	DBLoadNPCFactionLists(sint32 iNPCFactionListCount, uint32 iMaxNPCFactionListID);
	bool	DBLoadLoot();
	bool	DBLoadSkillCaps();
	void	DBLoadDamageShieldTypes(SPDat_Spell_Struct* sp, sint32 iMaxSpellID);


	// Kings & Bandits - DB Additions
	
	bool	DBLoadTeamSettings(sint32 iItemCount, uint32 iMaxItemID);
	bool	DBLoadPVPLeaderBoard(char* query);
	bool	DBLoadGuardTeamValues();
	TeamSettings_Struct*	GetTeamSettings(uint8 id);
	PVPLeaderBoardEntry_Struct*	GetPVPLeaderBoardEntriesByKills();
	PVPLeaderBoardEntry_Struct*	GetPVPLeaderBoardEntriesByPoints();
	PVPLeaderBoardEntry_Struct*	GetPVPLeaderBoardEntriesByInfamy();
	GuardSettings_Struct*	GetGuardSettings(uint32 kingdomid, uint32 npctypeid, uint32 spawngroupid=0);
	GuildHate_Struct*	GetGuildHateList(uint32 kingdomid, uint32 guildid);
	bool*	GetIsItemFound(uint32 id);

	// Main Database On-Load Calls
	
	// Loads the random loot drop table which populates the base of RandomLootDrop_Struct, including newing space for items list through the count
	bool	LoadRandomLootDrops();
	bool	LoadRandomLootDropsTable();
	
	// Loads the random drop chance tables, referenced by id in loot drop tables
	bool	LoadRandomDropRates();
	bool	LoadRandomDropRatesTable();

	// Loads the random name groups and their weight
	bool	LoadRandomNameGroups();
	bool	LoadRandomNameGroupsTable();

	// Each RandomLootDrop loaded from LoadRandomLootDrops will call this table to load the randomitemtemplates into the structure
	bool	PopulateLootDropItemList(RandomLootDrop_Struct* rld);

	// Calls DB adds new item & retrieves item id, adds item to mem share returns item ID
	bool	FixItem(Item_Struct* item, uint32 itemId);
	bool	AddNewItem(Item_Struct* item, uint32 templateID, char* hash, uint32* itemID=0);
	
	sint32	GetRandomLootDropCount(int32* oMaxID = 0);
	
	sint32	GetRandomDropRateCount(int32* oMaxID = 0);
	
	sint32	GetRandomNameGroupCount(int32* oMaxID = 0);
	
	bool	GetRandomNameMaxWeight(uint32 templateID, uint32* oMaxID = 0);
	bool	GetRandomName(uint32 templateID, uint32 weight,string* data);

	const RandomLootDrop_Struct* GetRandomLootDrop(uint32 id);
	const RandomDropRate_Struct* GetRandomDropRate(uint32 id);
	const RandomNameGroup_Struct* GetRandomNameGroup(uint32 id);
	
	string GenerateRandomName(uint32 nameGroupID);

	uint32 GetItemHash(float dbValue,const RandomItemTemplate_Struct* rndItem);
	bool GenerateRandomLootGroupResult(RandomLootDropResult_Struct* rldr, uint32 groupid, char* npcName="UNKNOWN", char* pcName="UNKNOWN", int32 providedCRC=0);
	uint32 CalculateBaseFormula(float baseValue,float dbValue, uint32 min, uint32 max);
	uint32 CalculateOppositeBaseFormula(float baseValue,float dbValue, uint32 min, uint32 max);
	sint32 CalculateBaseFormula(float baseValue,float dbValue, sint32 min, sint32 max);
	sint16 CalculateBaseFormula(float baseValue,float dbValue, sint16 min, sint16 max);
	uint8 CalculateBaseFormula(float baseValue,float dbValue, uint8 min, uint8 max);
	float CalculateBaseFormula(float baseValue,float dbValue, float min, float max);
	void GetRandomPercentage(float* floatValue, uint32* intValue);
	bool GenerateRandItemStruct(float baseValue, float dbValue, Item_Struct* itmStruct, const RandomItemTemplate_Struct* rndItem, RNDITEM_DROP_TYPE dropType, char* npcNameStr="UNKNOWN", char* pcNameStr="UNKNOWN");
	bool GetExistingRandomDropID(uint8 typeID, uint32 checksum, uint32 itemTemplateID, uint32* itemID, bool randomize=false);
	
	void InsertCharacterStats(uint32 charid);
	void UpdateCharacterPVPStats(uint32 charid, uint32 pvpkills, uint32 pvpdeaths, uint32 pvpcurpts, uint32 pvpcareerpts, uint32 bestkillstreak, 
		uint32 worstdeathstreak, uint32 curkillstreak, uint32 killsinday, char* lastkillname, char* lastdeathname);
	void AddCharacterPVPKill(uint32 charidattacker, uint32 charidkilled, uint32 ptsearned, char* attackerName, char* killedName, char* zoneShortName, uint8 attackerLevel, uint8 killedLevel, int32 attackerguildid, int32 killedguildid, int32 attackerteamid, int32 killedteamid, bool instanced=false);
	void UpdateCharacterStats(uint32 charid, uint32 astr, uint32 asta, uint32 adex, uint32 aagi, uint32 aint, uint32 awis, uint32 acha, uint32 hp, uint32 maxhp, 
		uint32 mana, uint32 maxmana, uint32 endurance, uint32 maxendurance, uint32 timeplayed, uint32 infamy, bool bountySet, uint8 ischaronline=0);
	bool GetLeaderBoardStructForChar(char* name, PVPLeaderBoardDetailsReply_Struct* boardPVP);
	
	bool UpdateItemDiscovery(uint32 itemID, char* name, bool isGM); // return true if it is an item discovery, otherwise false
	bool IsZoneUp(char* shortname);
	int8 TeamPVPFlag(uint32 charID);
	int8 GuardTeamFlag(uint32 npctypeid, uint32 spawngroupid);
	uint32 GetCharacterGuildID(uint32 charID);
	bool GetPlayerZoneKills(uint32 charID, char* zoneShortName, uint32* kills=0, uint32 secondsElapsed=0);
	bool GetPlayerZoneDeaths(uint32 charID, char* zoneShortName, uint32* deaths=0, uint32 secondsElapsed=0, bool skipInstanced=false);

	int32 GetSyncAccountByName(char* accountName, char* key);
	void UpdateAccountSetSyncID(uint32 acctID, uint32 syncAcctID);
	int8 GetSyncCharacterCount(uint32 accountID, uint32 syncAcctID);
	int32 GetSyncAccountID(uint32 accountID, bool* iseqemuacct=0);
	
	void DeleteGuardSettings(uint32 teamid, uint32 guardnpcid, uint32 spawngroupid);
	void UpdateGuardSettings(uint32 teamid, uint32 guardnpcid, uint8 classid, uint32 hpBonus, uint32 manaBonus, uint32 hpRegen, uint32 manaRegen, uint32 AC, uint8 level, uint32 spawngroupid);
	void AddGuardSettings(uint32 teamid, uint32 guardnpcid, uint8 classid, uint32 hpBonus, uint32 manaBonus, uint32 hpRegen, uint32 manaRegen, uint32 AC, uint8 level, uint32 spawngroupid);


	void AddGuardSpellOmitList(uint32 teamid, uint32 guardnpcid, uint32 effect_type, uint32 spell_id, uint32 spawngroupid);
	void DeleteGuardSpellOmitList(uint32 teamID, uint32 guardnpcid, uint32 spawngroupid);

	void AddGuardSpellList(uint32 teamid, uint32 guardnpcid, uint32 npc_spell_list, uint32 spawngroupid);
	void DeleteGuardSpellList(uint32 teamid, uint32 guardnpcid, uint32 spawngroupid);
	
	void AddGuardItem(uint32 teamid, uint32 guardnpcid, uint32 zoneid, uint32 spawngroupid, uint32 itemid, sint16 charges);
	void DeleteGuardItems(uint32 teamid, uint32 guardnpcid, uint32 zoneid, uint32 spawngroupid);
	
	void UpdateGuildHateList(uint32 teamid, uint32 guildid, uint32 timestamp, bool purgeAll=false);

	uint32 AddTeam(uint32 guildid, uint32 charid, uint8 citymode, uint32 ownerteamid, float x, float y, float z, float radius, uint32 zoneid);
	/*
	 * General Queries
	 */
	bool	LoadZoneNames();
	bool	QueryZoneNames(sint32 count, uint32* maxID);
	bool	LoadZoneShortNames();
	int32	GetZoneID(const char* zonename);
	const char*	GetZoneName(int32 zoneID, int32 instanceID=0, bool ErrorUnknown = false, bool ReturnStaticZone=false );
	const ZoneShortName_Struct* IterateZoneNames(uint32* NextIndex);
	const RandomLootDrop_Struct* IterateRandomLootDrops(uint32* NextIndex);
	int32 GetZoneIDByDBID(int32 dbID);
	const ZoneShortName_Struct* GetZoneInfo(const char* zonename);
	
	bool	GetSafePoints(const char* short_name, int32 version, float* safe_x = 0, float* safe_y = 0, float* safe_z = 0, sint16* minstatus = 0, int8* minlevel = 0, char *flag_needed = NULL);
	bool	GetSafePoints(int32 zoneID, int32 version, float* safe_x = 0, float* safe_y = 0, float* safe_z = 0, sint16* minstatus = 0, int8* minlevel = 0, char *flag_needed = NULL) { return GetSafePoints(GetZoneName(zoneID), version, safe_x, safe_y, safe_z, minstatus, minlevel, flag_needed); }

	// Kings & Bandits - modified to return a char id if needed
	bool	StoreCharacter(uint32 account_id, PlayerProfile_Struct* pp, Inventory* inv, ExtendedProfile_Struct *ext, uint32* charid=0);
	int8	CopyCharacter(const char* oldname, const char* newname, int32 acctid);
	
	bool	MoveCharacterToZone(const char* charname, const char* zonename,int32 zoneid);
	bool	MoveCharacterToZone(const char* charname, const char* zonename);
	bool	MoveCharacterToZone(int32 iCharID, const char* iZonename);
	// Kings & Bandits - world server pvpflag to track users
	int32	GetCharacterInfo(const char* iName, int32* oAccID = 0, int32* oZoneID = 0, int32* oInstanceID = 0,float* oX = 0, float* oY = 0, float* oZ = 0, uint8* pvpFlag=0, uint8* level=0);
	int16 GetInstanceID(const char* zone, int32 charid, int16 version);
	int16 GetInstanceID(int32 zone, int32 charid, int16 version);
	bool    logevents(const char* accountname,int32 accountid,int8 status,const char* charname,const char* target, const char* descriptiontype, const char* description,int event_nid,
	float charXPos=0.0f, float charYPos=0.0f, float charZPos=0.0f, char* charZoneName="", float targXPos=0.0f, float targYPos=0.0f, float targZPos=0.0f);
	bool    logchatevents(const char* accountname,int32 accountid,int8 status,const char* charname,const char* target, const char* chattext,int chatgroup_id,
	float charXPos=0.0f, float charYPos=0.0f, float charZPos=0.0f, char* charZoneName="", float targXPos=0.0f, float targYPos=0.0f, float targZPos=0.0f);
	bool    logcombatevents(const char* accountname,int32 accountid,int8 status,const char* charname,const char* target, const char* actiontext,int actiongroup_id,
	float charXPos=0.0f, float charYPos=0.0f, float charZPos=0.0f, char* charZoneName="", float targXPos=0.0f, float targYPos=0.0f, float targZPos=0.0f);
	
	bool	LoadTeamSettings();
	void	UpdateTeamSettings(TeamSettings_Struct* tss);
	void	UpdatePVPLeaderBoard(PVPLeaderBoardEntry_Struct* lbe);
	bool	UpdateDatabaseQuery(const char* query);

	void AssignGroupToInstance(int32 gid, int32 instance_id);
	void AssignRaidToInstance(int32 rid, int32 instance_id);
	void FlagInstanceByGroupLeader(int32 zone, int16 version, int32 charid, int32 gid);
	void FlagInstanceByRaidLeader(int32 zone, int16 version, int32 charid, int32 rid);

	
	// Image KaB/Red69/Zek 6/15 - Additions for replay on characters
	uint32 CreateCharacterReplay(uint32 charID, uint32 zoneID);
	void UpdateEndReplay(uint32 replayID);
	void AddReplayFrame(uint32 replayID, uint32 frameID, float x, float y, float z, float heading, float speed, float recordedspeed, uint32 animation, uint8 levitate, 
		float targetx, float targety, float targetz, float targetheading, float targetspeed, char* targetName);
	bool GetReplayList(int32 charid, int32 zoneid, LinkedList<ReplayList_Struct*>* list);
	bool GetReplayFrame(uint32 replayID, uint32 frameID, float* x, float* y, float* z, float* heading, float* speed, float* recordedspeed, uint32* animation, uint8* levitate, 
		float* targetx, float* targety, float* targetz, float* targetheading, float* targetspeed, char* targetName);
	
	// Image: tell/group queue
	bool InsertMessageQueue(char* toName, char* fromName, int16 channelID, int8 language, int16 langskill, char* message, 
		int32 petitionID=0, bool bypassMessageLimit=false, uint32 command=0, uint32 commandValue=0, uint32 guildid=0, char* lastAttacker="", int32 fromgroupid=0, char* accountName="", int32 optionalfield1=0, int32 optionalfield2=0);
	bool IsMaxedMessages(char* toName);
	uint32 GetCharacterStatsUnsignedInt(uint32 charID, char* fieldName);
	void SetCharacterStatsUnsignedInt(uint32 charID, char* fieldName, uint32 value);
	void SetCharacterStatsStatic(uint32 charID, char* fieldName, char* str);
protected:
	void SDBInitVars();
	
	/*
	 * Private shared mem stuff
	 */
	sint32	GetItemsCount(int32* oMaxID = 0);
	sint32	GetNPCTypesCount(int32* oMaxID = 0);
	sint32	GetNPCFactionListsCount(int32* oMaxID = 0);
	static bool extDBLoadItems(sint32 iItemCount, int32 iMaxItemID);
	static bool extDBLoadNPCFactionLists(sint32 iNPCFactionListCount, int32 iMaxNPCFactionListID);
	static bool extDBLoadLoot();
	static bool extDBLoadSkillCaps();
	
	/* Kings & Bandits */
	// additional memshare calls
	static bool extDBLoadTeamSettings(sint32 iItemCount, int32 iMaxItemID);
	static bool extDBLoadRandomLootDrops(sint32 count, int32 maxid);
	static bool extDBLoadRandomDropRates(sint32 count, int32 maxid);
	static bool extDBLoadRandomNameGroups(sint32 count, int32 maxid);
	static bool	extDBZoneNames(sint32 iZoneCount, int32 iMaxZoneID);
	
	uint32				max_item;
	uint32				max_zonename;
	uint32				max_npc_type;
	uint32				max_door_type;
	uint32				npcfactionlist_max;
	uint32				loottable_max;
	uint32				lootdrop_max;
	
	int32				npc_spells_maxid;
	
	/* Kings & Bandits */
	// additional memshare value
	uint32				max_randomlootdropid;
	uint32				max_randomdroprateid;
	uint32				max_randomnamegroupid;
private:
	static SharedDatabase *s_usedb;
};





#endif /*SHAREDDB_H_*/



















