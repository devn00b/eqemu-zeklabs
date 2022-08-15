#include "../common/types.h"
#include "../common/EMuShareMem.h"
#include "../KingsBandits/KBStructs.h"

#define MAX_ZONES	10000

struct MMFZoneName_Struct {
	uint32		MaxZoneNameID;
	uint32		NextFreeIndex;
	uint32		nameCount;
	int32		nameIndex[MAX_ZONES+1];
	ZoneShortName_Struct	ZoneNames[0];
};