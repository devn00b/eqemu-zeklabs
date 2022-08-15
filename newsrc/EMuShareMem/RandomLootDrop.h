#include "../common/types.h"
#include "../common/EMuShareMem.h"
#include "../KingsBandits/KBStructs.h"

// Need a value to set memory limit
#define MAX_RANDLOOTDROPS		20000
#define MAX_DROPRATES			5000
#define MAX_RANDOMNAMEGROUPS	10000

struct MMFRandLootDrop_Struct {
	uint32		MaxLootDropID;
	uint32		NextFreeIndex;
	uint32		dropCount;
	uint32		itemCount;
	uint32		dropIndex[MAX_RANDLOOTDROPS+1];
	RandomLootDrop_Struct	LootDrops[0];
};

struct MMFRandDropRate_Struct {
	uint32		MaxDropRateID;
	uint32		NextFreeIndex;
	uint32		dropRateCount;
	uint32		dropIndex[MAX_DROPRATES+1];
	RandomDropRate_Struct	DropRates[0];
};

struct MMFRandNameGroup_Struct {
	uint32		MaxRandNameID;
	uint32		NextFreeIndex;
	uint32		randNameGroupCount;
	uint32		nameIndex[MAX_RANDOMNAMEGROUPS+1];
	RandomNameGroup_Struct	NameGroups[0];
};

//#define MMF_MAX_ITEMS_MEMSIZE	sizeof(MMFItems_Struct) + 256



