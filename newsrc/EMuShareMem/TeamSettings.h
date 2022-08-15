#include "../common/types.h"
#include "../common/EMuShareMem.h"
#include "../KingsBandits/KBStructs.h"
#include "Items.h"

#define MAX_TEAMS	5000

struct MMFTeamSettings_Struct {
	uint32		MaxTeamID;
	uint32		NextFreeIndex;
	uint32		teamCount;
	int32		teamIndex[MAX_TEAMS+1];
	bool		itemFound[MMF_EQMAX_ITEMS+1];
	PVPLeaderBoardEntry_Struct TopPVPKillEntries[100];
	PVPLeaderBoardEntry_Struct TopPVPPointEntries[100];
	PVPLeaderBoardEntry_Struct TopPVPInfamyEntries[100];
	TeamSettings_Struct	TeamSettings[0];
};