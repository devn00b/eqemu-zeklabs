
#ifndef KBSTRUCTS_H
#define KBSTRUCTS_H

/*
** Compiler override to ensure
** byte aligned structures
*/
#pragma pack(1)

#include "../common/types.h"
#include "../common/item_struct.h"
#include "../common/eq_packet_structs.h"

/* Random Name Creation Structures */
/* Process: Call table RandomNameGroups and query for fields (group)templateid,startgroupid,middlegroupid,connectgroupid,endgroupid
** Use the group id results to query a count of existing in table RandomNames by templateid and populate the structure + count fields.

MemShare Load (called for each groupid, start, middle, connect, end):

	select t1.templateid,t1.startgroupid,count(t2.templateid),t2.templateid from randomnamegroups as t1 INNER JOIN randomnames as t2 ON t2.templateid = t1.startgroupid;

take t2.templateid result to generate the table through the next query:

	select name,weight from randomnames where templateid=t2.templateid
*/

/* Random Item Drop Structures */
namespace RandomItemField {
	enum {
#define F(x) x,
#include "../KingsBandits/KBItemStructs.h"
#undef F
		misc // add any misc after here
	};
};

struct RandomItemTemplate_Struct {
	uint32 itemid;
	uint8 itemclass; // common=0,container=1,book=2
	uint8 minstatus;
	char  idfile[30];
	uint8 weight;
	uint8 norent;
	uint8 nodrop; // No Drop: 0=nodrop, 255=not nodrop - nodrop
	uint8 size;
	uint32 slots;
	
	// min,max price in copper
	uint32 minprice;
	uint32 maxprice;

	uint32 icon;
	uint32 loregroup;
	bool loreflag;
	bool pendingloreflag;
	bool artifactflag;
	
	// like eq2 status?
	uint32 minfavor;
	uint32 maxfavor;

	uint32 minguildfavor;
	uint32 maxguildfavor;

	uint32 pointtype; // this relates to the favor? I am not sure!

	uint8 bagtype; // 0:Small Bag, 1:Large Bag, 2:Quiver, 3:Belt Pouch ... there are 50 types - bagtype

	// we will focus on only giving out bag slots 8-10
	uint8 bagslots;// Number of slots: can only be 2, 4, 6, 8, or 10 - bagslots

	uint8 bagsize; // Size: 0=tiny, 1=small, 2=medium, 3=large, 4=giant - size
	
	// 0-100 % of weight reduction (primary focus on bags)
	uint8 minbagwr;
	uint8 maxbagwr;

	bool benefitflag;
	bool tradeskills; // flag for tradeskill based item
	
	/* Resistance min/max */
	sint8 mincr;
	sint8 maxcr;
	sint8 mindr;
	sint8 maxdr;
	sint8 minpr;
	sint8 maxpr;
	sint8 minmr;
	sint8 maxmr;
	sint8 minfr;
	sint8 maxfr;
	
	sint8 minstr;
	sint8 maxstr;
	sint8 minsta;
	sint8 maxsta;
	sint8 minagi;
	sint8 maxagi;
	sint8 mindex;
	sint8 maxdex;
	sint8 mincha;
	sint8 maxcha;
	sint8 minint;
	sint8 maxint;
	sint8 minwis;
	sint8 maxwis;

	sint32 minhp;
	sint32 maxhp;

	sint32 minmana;
	sint32 maxmana;

	sint32 minac;
	sint32 maxac;

	uint32 deity;
	
	sint32 minskillmodvalue;
	sint32 maxskillmodvalue;
	
	uint32 skillmodtype;
	
	// based dmg %
	uint32 minbanedmgamt;
	uint32 maxbanedmgamt;

	bool magic;

	sint32 mincasttime_;
	sint32 maxcasttime_;
	
	// required player level
	uint8 minreqlevel;
	uint8 maxreqlevel;

	// required skill level
	uint8 minreclevel;
	uint8 maxreclevel;

	uint8 recskill;

	uint8 minrange;
	uint8 maxrange;

	uint8 mindamage;
	uint8 maxdamage;

	uint32 mincolor;
	uint32 maxcolor;

	uint32 classes;
	uint32 races;

	// charges on item min/max
	sint16 minmaxcharges;
	sint16 maxmaxcharges;

	uint8 itemtype;

	uint8 material;

	float minsellrate;
	float maxsellrate;

	// cast time 'clicky' effects
	sint16 mincasttime; // min cast time
	sint16 maxcasttime; // max cast time

	uint32 elitematerial;
	sint32 procrate;
	uint32 charmfileid;
	char charmfile[32];
	uint32 augtype;
	uint8 augslot1type;
	uint8 augslot2type;
	uint8 augslot3type;
	uint8 augslot4type;
	uint8 augslot5type;
	uint8 augslots1visible;
	uint8 augslots2visible;
	uint8 augslots3visible;
	uint8 augslots4visible;
	uint8 augslots5visible;

	uint32 minbanedmgraceamt;
	uint32 maxbanedmgraceamt;

	uint32 banedmgrace;

	uint32 augrestrict;

	uint32 minendurance;
	uint32 maxendurance;

	uint32 dotshielding;
	
	uint32 minattack;
	uint32 maxattack;
	
	uint8 mindelay;
	uint8 maxdelay;

	uint32 minregen;
	uint32 maxregen;

	uint32 minmanaregen;
	uint32 maxmanaregen;

	uint32 minenduranceregen;
	uint32 maxenduranceregen;

	uint32 minhaste;
	uint32 maxhaste;

	uint32 damageshield;

	uint32 recastdelay;

	uint32 recasttype;

	uint32 augdistiller;
	
	uint32 bardtype;
	sint32 bardvalue;

	bool attuneable;
	bool nopet;
	bool potionbelt;
	bool stackable;
	bool notransfer;
	uint8 stacksize;
	uint8 potionbeltslots;

	ItemEffect_Struct Click, Proc, Worn, Focus, Scroll, Bard;

	uint8 book;
	uint32 booktype;

	char filename[33]; // book data file name

	sint32 minhealamt;
	sint32 maxhealamt;

	sint32 minspelldmg;
	sint32 maxspelldmg;

	uint32 scriptfileid;
	uint16 expendablearrow;
	
	uint32 clairvoyance;
	char	clickname[65];
	char	procname[65];
	char	wornname[65];
	char	focusname[65];
	char	scrollname[65];

	uint32 itemNameGroup;
	uint32 itemRandCount; // the max per group of items we can generate, used in GetItemHash

};

struct RandomNameGroup_Struct {
	uint32 templateID; // id in the db

	// Template reference in randomnames
	uint32 startGroupID;
	uint32  startGroupWeight;
	uint32 middleGroupID;
	uint32  middleGroupWeight;
	uint32 connectGroupID;
	uint32  connectGroupWeight;
	uint32 endGroupID;
	uint32  endGroupWeight;
};

struct RandomDropRate_Struct {
	uint32 dropRateID; // id in the db

	uint32 baseDropChance; // base chance used in the formula

	// Existing drop chances
	uint32 extremeRareDropChance;
	uint32 rareDropChance;
	uint32 specialDropChance;
	uint32 commonDropChance;

	// new drop chance

	uint32 newExtremeRareDropChance;
	uint32 newRareDropChance;
	uint32 newSpecialDropChance;
	uint32 newCommonDropChance;
};

#define MAXRANDOMITEMTEMPLATE 20

struct RandomLootDrop_Struct {
	uint32 lootDropID; // id reference to generate an item or list of items
	uint32 itemGroupID; // references the items that are grouped in RandomItemTemplate to be generated for use here
	uint32 itemGroupCount; // group if items assigned to this random loot drop
	uint32 dropRateID; // references the RandomDropRates group id to use on handling chance of a certain item drop
	uint8  minItemDrop; // minimum items to drop
	uint8  maxItemDrop; // maximum items to drop
	uint32 minCoinDrop; // minimum coin (copper) to drop
	uint32 maxCoinDrop; // maximum coin (copper) to drop

	/*Matching/Requirement Variables for drop*/
	uint8 minLevel; // player or avg group must be above(or equal) to min level (if not 0)
	uint8 maxLevel; // player or avg group must be below(or equal) to max level (if not 0)

	uint32 zoneID; // if zone id is not 0, all npc's in this zone will drop this random loot table
	uint32 npcID; // if npc id is not 0, we will load this into a specific npc to use
	uint32 npcGroupID; // reference a group of npcs (spawngroup - spawngroupid)
	struct RandomItemTemplate_Struct itemTemplates[MAXRANDOMITEMTEMPLATE];
};
struct ItemDropResponse_Struct {
	uint32 itemID;
	bool isNewItem;
};
struct RandomLootDropResult_Struct {
uint32 coinDropValue;
uint8 itemDropCount;
ItemDropResponse_Struct items[50]; // result item ids from generated/selected items
};

struct ZoneShortName_Struct {
	int32 zoneID;
	int32 dbID;
	int32 instanceID;
	char zoneShortName[32];
};

struct GuardSettings_Struct {
	uint32 teamID; // team id for guard
	uint32 guardNPCTypeID; // guard npc type id
	uint8 classID; // class to represent the guard, this will also set its spells - map to item class
	uint32 hpBonus; // map to item hp
	uint32 manaBonus; // map to item mana
	uint32 hpRegenBonus; // map to regen
	uint32 manaRegenBonus; // map to mana regen
	uint32 AC; // map to ac
	uint8 level; // map to level requirement
	int32 lastkilledtimestamp; // last killed timestamp
	int32 lastkilledguildid; // last guild id of character who killed this mob
	int32 lastkilledcharid; // last char id of character who killed this mob
	int32 lastkilledteamid;
	int32 lastkilledlevel;
	bool didMobSpawn;
	int32 spawngroupid;
};

struct GuildHate_Struct {
	int32 taggedguild;
	int32 taggedtimestamp;
};

#define GUARDLIMIT 200
#define GUILDHATELIMIT 20
struct TeamSettings_Struct {
	uint32 teamID;
	char kingPCName[64];
	uint32 kingNPCTypeID;
	sint32 bankcopper; // reserve in copper inside bank
	sint32 banksilver; // reserve in copper inside bank
	sint32 bankgold; // reserve in copper inside bank
	sint32 bankplatinum; // reserve in copper inside bank

	// placeholder for city owner coin (this will be updated when they zone in and replaced when they zone out)
	sint32 placeholdercopper;
	sint32 placeholdersilver;
	sint32 placeholdergold;
	sint32 placeholderplatinum;

	uint32 upkeepRate; // cost in copper per eq day for upkeep
	bool upkeepPaid; // set after timer triggers in world server - after server is brought up this will always be true for the first 72 min.
	uint8 newbieTaxRate;
	uint8 citizenTaxRate;

	int32 bankzoneid; // zone which will contain the alternate 'banker' for the city owner
	// team xp settings
	uint8 teamXPBonus;

	// Guard Settings for each team, track and update between the db fields
	uint8 totalGuardsCount;

	// guild owner id (Added April 29 2013)
	int32 guildid; // 0 = no owner, > 0 guildid owner
	int32 charid; // 0 = no owner, >0 charid of owner
	int32 ownerteamid; // team id owner
	int32 ownerlastvisit; //timestamp of owners last visit (unix timestamp)

	uint8 citymode; // 0 = hostile, 1 = friendly, 2 = restrictive

	float x;
	float y;
	float z;
	float radius;
	int32 zoneid;
	GuardSettings_Struct TeamGuards[GUARDLIMIT];
	
	GuildHate_Struct guildhate[GUILDHATELIMIT];
};

enum CITYMODE
{
HOSTILE=0,
FRIENDLY=1,
RESTRICTIVE=2,
CAMPCONSTRUCTED=96,
CAMPDESTROYED=97,
CITYENABLED=98, // can't be taken, but is useable by the team that owns it
CITYDISABLED=99 // can't be taken isn't useable
};

// Restore structure packing to default
#pragma pack()

#endif
