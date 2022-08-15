#include "KBFunctions.h"
#include "../common/MiscFunctions.h"

#ifdef ZONE
#include "../zone/zonedb.h"
extern ZoneDatabase database;
#endif
#ifdef WORLD
#include "../world/worlddb.h"
extern WorldDatabase database;
#endif
