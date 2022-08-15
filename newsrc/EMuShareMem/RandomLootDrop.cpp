/*
  Note: Do NOT change this to load items on an as-needed basis. Since this memory is
  accessed from multiple threads, you'd need mutex's all over the place if it was
  ever to be modified/updated/added to. The overhead of the mutexes would be alot more
  in the long run than the delay in loading.

  -Quagmire
*/

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include "../common/unix.h"
#endif

#include <memory.h>
#include <iostream>
using namespace std;
#include "RandomLootDrop.h"
#include "../common/timer.h"
#include "MMF.h"

MMF RandomLootDropMMF;
const MMFRandLootDrop_Struct* MMFRandomLootDropData = 0;
MMFRandLootDrop_Struct* MMFRandomLootDropData_Writable = 0;

MMF RandomDropRateMMF;
const MMFRandDropRate_Struct* MMFRandomDropRateData = 0;
MMFRandDropRate_Struct* MMFRandomDropRateData_Writable = 0;

MMF RandomNameGroupMMF;
const MMFRandNameGroup_Struct* MMFRandomNameGroupData = 0;
MMFRandNameGroup_Struct* MMFRandomNameGroupData_Writable = 0;

DLLFUNC bool AddRandomLootDrop(uint32 id, const RandomLootDrop_Struct* rld) {
	if (!MMFRandomLootDropData_Writable) {
		return false;
	}
	if (id > MAX_RANDLOOTDROPS || MMFRandomLootDropData_Writable->NextFreeIndex >= MMFRandomLootDropData_Writable->dropCount) {
		return false;
	}
	if (MMFRandomLootDropData_Writable->dropIndex[id] != 0xFFFF) {
		return false;
	}
	
	uint32 nextid = MMFRandomLootDropData_Writable->NextFreeIndex++;
	MMFRandomLootDropData_Writable->dropIndex[id] = nextid;
	memcpy(&MMFRandomLootDropData_Writable->LootDrops[nextid], rld, sizeof(RandomLootDrop_Struct));

	return true;
}

DLLFUNC bool AddRandomDropRate(uint32 id, const RandomDropRate_Struct* rdr) {
	if (!MMFRandomDropRateData_Writable) {
		return false;
	}
	if (id > MAX_DROPRATES || MMFRandomDropRateData_Writable->NextFreeIndex >= MMFRandomDropRateData_Writable->dropRateCount) {
		return false;
	}
	if (MMFRandomDropRateData_Writable->dropIndex[id] != 0xFFFF) {
		return false;
	}
	
	uint32 nextid = MMFRandomDropRateData_Writable->NextFreeIndex++;
	MMFRandomDropRateData_Writable->dropIndex[id] = nextid;
	memcpy(&MMFRandomDropRateData_Writable->DropRates[nextid], rdr, sizeof(RandomDropRate_Struct));

	return true;
}

DLLFUNC bool AddRandomNameGroup(uint32 id, const RandomNameGroup_Struct* rng) {
	if (!MMFRandomNameGroupData_Writable) {
		return false;
	}
	if (id > MAX_RANDOMNAMEGROUPS || MMFRandomNameGroupData_Writable->NextFreeIndex >= MMFRandomNameGroupData_Writable->randNameGroupCount) {
		return false;
	}
	if (MMFRandomNameGroupData_Writable->nameIndex[id] != 0xFFFF) {
		return false;
	}
	
	uint32 nextid = MMFRandomNameGroupData_Writable->NextFreeIndex++;
	MMFRandomNameGroupData_Writable->nameIndex[id] = nextid;
	memcpy(&MMFRandomNameGroupData_Writable->NameGroups[nextid], rng, sizeof(RandomNameGroup_Struct));

	return true;
}

DLLFUNC bool DLLLoadRandomLootDrops(CALLBACK_DBLoadRandomLootDrops cbDBLoadRandomLootDrops, int32 structSize, sint32* lootDropCount, int32* iMaxDropID) {
	if (structSize != sizeof(RandomLootDrop_Struct)) {
		cout << "Error: EMuShareMem: DLLLoadRandomLootDrops: structSize != sizeof(RandomLootDrop_Struct)" << endl;
		cout << "RandomLootDrop_Struct has changed, EMuShareMem.dll needs to be recompiled." << endl;
		return false;
	}
	if (*iMaxDropID > MAX_RANDLOOTDROPS) {
		cout << "Error: EMuShareMem: DLLLoadRandomLootDrops: iMaxDropID > MAX_RANDLOOTDROPS" << endl;
		cout << "You need to increase the define in RandomLootDrop.h." << endl;
		return false;
	}
	
	MMFRandomLootDropData_Writable = 0;
	//Allocate the shared memory for the item structures
	int32 lootDropStructSize = sizeof(RandomLootDrop_Struct);
	int32 mmfStructSize = sizeof(MMFRandLootDrop_Struct);

	int32 tmpMemSize = sizeof(MMFRandLootDrop_Struct) + 256 + (lootDropStructSize * (*lootDropCount));
	cout << "LootDropStruct Size: " << lootDropStructSize << ", MMF Size: " << mmfStructSize << endl;

	if (RandomLootDropMMF.Open("EQEMuALootDrops", tmpMemSize)) {
		if (RandomLootDropMMF.CanWrite()) {
			MMFRandomLootDropData_Writable = (MMFRandLootDrop_Struct*) RandomLootDropMMF.GetWriteableHandle();
			if (!MMFRandomLootDropData_Writable) {
				cout << "Error: EMuShareMem: DLLLoadRandomLootDrops: !MMFRandomLootDropData_Writable" << endl;
				return false;
			}

			memset(MMFRandomLootDropData_Writable, 0, tmpMemSize);
			for(int i=0; i<MAX_RANDLOOTDROPS; i++)
				MMFRandomLootDropData_Writable->dropIndex[i] = 0xFFFF;
			MMFRandomLootDropData_Writable->MaxLootDropID = *iMaxDropID;
			MMFRandomLootDropData_Writable->dropCount = *lootDropCount;
			//the writable handle has been created, do the load below after we have the 
			//serialization handle as well.
		} else {
			if (!RandomLootDropMMF.IsLoaded()) {
				Timer::SetCurrentTime();
				int32 starttime = Timer::GetCurrentTime();
				while ((!RandomLootDropMMF.IsLoaded()) && ((Timer::GetCurrentTime() - starttime) < 300000)) {
					Sleep(10);
					Timer::SetCurrentTime();
				}
				if (!RandomLootDropMMF.IsLoaded()) {
					cout << "Error: EMuShareMem: DLLLoadRandomLootDrops: !RandomLootDropMMF.IsLoaded() (timeout)" << endl;
					return false;
				}
			}
			MMFRandomLootDropData = (const MMFRandLootDrop_Struct*) RandomLootDropMMF.GetHandle();
			if (!MMFRandomLootDropData) {
				cout << "Error: EMuShareMem: DLLLoadRandomLootDrops: !RandomLootDropMMF (CanWrite=false)" << endl;
				return false;
			}
			*iMaxDropID = MMFRandomLootDropData->MaxLootDropID;
			*lootDropCount = MMFRandomLootDropData->dropCount;

			return true;
		}
	} else {
		cout << "Error Loading Items: RandomLootDrop.cpp: pDLLLoadItems: Open() == false" << endl;
		return false;
	}
	/*
	
			// use a callback so the DB functions are done in the main exe
			// this way the DLL doesnt have to open a connection to mysql
			if (!cbDBLoadItems(*iItemCount, *iMaxItemID)) {
				cout << "Error: EMuShareMem: DLLLoadItems: !cbDBLoadItems" << endl;
				return false;
			}

	*/
	
	// use a callback so the DB functions are done in the main exe
	// this way the DLL doesnt have to open a connection to mysql
	if (!cbDBLoadRandomLootDrops(*lootDropCount, *iMaxDropID)) {
		cout << "Error: EMuShareMem: DLLLoadRandomLootDrops: !cbDBLoadRandomLootDrops" << endl;
		return false;
	}
	
	
	//Now, Disable the write handle and get the read handle.
	//do this for both item struct and serialization data
	
	MMFRandomLootDropData = 0;
	RandomLootDropMMF.SetLoaded();
	MMFRandomLootDropData = (const MMFRandLootDrop_Struct*) RandomLootDropMMF.GetHandle();
	if (!MMFRandomLootDropData) {
		cout << "Error: EMuShareMem: DLLLoadRandomLootDrops: !MMFRandomLootDropData (CanWrite=true)" << endl;
		return false;
	}
	
	return true;
}


DLLFUNC bool DLLLoadRandomDropRates(CALLBACK_DBLoadRandomDropRates cbDBLoadRandomDropRates, int32 structSize, sint32* lootDropCount, int32* iMaxDropID) {
	if (structSize != sizeof(RandomDropRate_Struct)) {
		cout << "Error: EMuShareMem: DLLLoadRandomDropRates: structSize != sizeof(RandomDropRate_Struct)" << endl;
		cout << "RandomDropRate_Struct has changed, EMuShareMem.dll needs to be recompiled." << endl;
		return false;
	}
	if (*iMaxDropID > MAX_DROPRATES) {
		cout << "Error: EMuShareMem: DLLLoadRandomDropRates: iMaxDropID > MAX_DROPRATES" << endl;
		cout << "You need to increase the define in RandomLootDrop.h." << endl;
		return false;
	}
	
	MMFRandomDropRateData_Writable = 0;
	//Allocate the shared memory for the item structures
	int32 lootDropStructSize = sizeof(RandomDropRate_Struct);
	int32 mmfStructSize = sizeof(MMFRandDropRate_Struct);

	int32 tmpMemSize = sizeof(MMFRandDropRate_Struct) + 256 + (lootDropStructSize * (*lootDropCount));
	cout << "DropRateStruct Size: " << lootDropStructSize << ", MMF Size: " << mmfStructSize << endl;

	if (RandomDropRateMMF.Open("EQEMuBDropRates", tmpMemSize)) {
		if (RandomDropRateMMF.CanWrite()) {
			MMFRandomDropRateData_Writable = (MMFRandDropRate_Struct*) RandomDropRateMMF.GetWriteableHandle();
			if (!MMFRandomDropRateData_Writable) {
				cout << "Error: EMuShareMem: DLLLoadRandomDropRates: !MMFRandomDropRateData_Writable" << endl;
				return false;
			}

			memset(MMFRandomDropRateData_Writable, 0, tmpMemSize);
			for(int i=0; i<MAX_DROPRATES; i++)
				MMFRandomDropRateData_Writable->dropIndex[i] = 0xFFFF;
			MMFRandomDropRateData_Writable->MaxDropRateID = *iMaxDropID;
			MMFRandomDropRateData_Writable->dropRateCount = *lootDropCount;
			//the writable handle has been created, do the load below after we have the 
			//serialization handle as well.
		} else {
			if (!RandomDropRateMMF.IsLoaded()) {
				Timer::SetCurrentTime();
				int32 starttime = Timer::GetCurrentTime();
				while ((!RandomDropRateMMF.IsLoaded()) && ((Timer::GetCurrentTime() - starttime) < 300000)) {
					Sleep(10);
					Timer::SetCurrentTime();
				}
				if (!RandomDropRateMMF.IsLoaded()) {
					cout << "Error: EMuShareMem: DLLLoadRandomDropRates: !RandomDropRateMMF.IsLoaded() (timeout)" << endl;
					return false;
				}
			}
			MMFRandomDropRateData = (const MMFRandDropRate_Struct*) RandomDropRateMMF.GetHandle();
			if (!MMFRandomDropRateData) {
				cout << "Error: EMuShareMem: DLLLoadRandomDropRates: !RandomDropRateMMF (CanWrite=false)" << endl;
				return false;
			}
			*iMaxDropID = MMFRandomDropRateData->MaxDropRateID;
			*lootDropCount = MMFRandomDropRateData->dropRateCount;

			return true;
		}
	} else {
		cout << "Error Loading Items: RandomLootDrop.cpp: DLLLoadRandomDropRates: Open() == false" << endl;
		return false;
	}
	/*
	
			// use a callback so the DB functions are done in the main exe
			// this way the DLL doesnt have to open a connection to mysql
			if (!cbDBLoadItems(*iItemCount, *iMaxItemID)) {
				cout << "Error: EMuShareMem: DLLLoadItems: !cbDBLoadItems" << endl;
				return false;
			}

	*/
	
	// use a callback so the DB functions are done in the main exe
	// this way the DLL doesnt have to open a connection to mysql
	if (!cbDBLoadRandomDropRates(*lootDropCount, *iMaxDropID)) {
		cout << "Error: EMuShareMem: DLLLoadRandomLootDrops: !cbDBLoadRandomDropRates" << endl;
		return false;
	}
	
	
	//Now, Disable the write handle and get the read handle.
	//do this for both item struct and serialization data
	
	MMFRandomDropRateData = 0;
	RandomDropRateMMF.SetLoaded();
	MMFRandomDropRateData = (const MMFRandDropRate_Struct*) RandomDropRateMMF.GetHandle();
	if (!MMFRandomDropRateData) {
		cout << "Error: EMuShareMem: DLLLoadRandomDropRates: !MMFRandomDropRateData (CanWrite=true)" << endl;
		return false;
	}
	
	return true;
}


DLLFUNC bool DLLLoadRandomNameGroups(CALLBACK_DBLoadRandomNameGroups cbDBLoadRandomNameGroups, int32 structSize, sint32* nameGroupCount, int32* iMaxGroupID) {
	if (structSize != sizeof(RandomNameGroup_Struct)) {
		cout << "Error: EMuShareMem: DLLLoadRandomNameGroups: structSize != sizeof(RandomNameGroup_Struct)" << endl;
		cout << "RandomNameGroup_Struct has changed, EMuShareMem.dll needs to be recompiled." << endl;
		return false;
	}
	if (*iMaxGroupID > MAX_RANDOMNAMEGROUPS) {
		cout << "Error: EMuShareMem: DLLLoadRandomNameGroups: iMaxDropID > MAX_RANDOMNAMEGROUPS" << endl;
		cout << "You need to increase the define in RandomLootDrop.h." << endl;
		return false;
	}
	
	MMFRandomNameGroupData_Writable = 0;
	//Allocate the shared memory for the item structures
	int32 tmpStructSize = sizeof(RandomNameGroup_Struct);
	int32 mmfStructSize = sizeof(MMFRandNameGroup_Struct);

	int32 tmpMemSize = mmfStructSize + 256 + (tmpStructSize * (*nameGroupCount));
	cout << "NameGroupStruct Size: " << structSize << ", MMF Size: " << mmfStructSize << endl;

	if (RandomNameGroupMMF.Open("EQEMuRandomNameGroups", tmpMemSize)) {
		if (RandomNameGroupMMF.CanWrite()) {
			MMFRandomNameGroupData_Writable = (MMFRandNameGroup_Struct*) RandomNameGroupMMF.GetWriteableHandle();
			if (!MMFRandomNameGroupData_Writable) {
				cout << "Error: EMuShareMem: DLLLoadRandomNameGroups: !MMFRandomNameGroupData_Writable" << endl;
				return false;
			}

			memset(MMFRandomNameGroupData_Writable, 0, tmpMemSize);
			for(int i=0; i<MAX_RANDOMNAMEGROUPS; i++)
				MMFRandomNameGroupData_Writable->nameIndex[i] = 0xFFFF;
			MMFRandomNameGroupData_Writable->MaxRandNameID = *iMaxGroupID;
			MMFRandomNameGroupData_Writable->randNameGroupCount = *nameGroupCount;
			//the writable handle has been created, do the load below after we have the 
			//serialization handle as well.
		} else {
			if (!RandomNameGroupMMF.IsLoaded()) {
				Timer::SetCurrentTime();
				int32 starttime = Timer::GetCurrentTime();
				while ((!RandomNameGroupMMF.IsLoaded()) && ((Timer::GetCurrentTime() - starttime) < 300000)) {
					Sleep(10);
					Timer::SetCurrentTime();
				}
				if (!RandomNameGroupMMF.IsLoaded()) {
					cout << "Error: EMuShareMem: DLLLoadRandomNameGroups: !RandomNameGroupMMF.IsLoaded() (timeout)" << endl;
					return false;
				}
			}
			MMFRandomNameGroupData = (const MMFRandNameGroup_Struct*) RandomNameGroupMMF.GetHandle();
			if (!MMFRandomNameGroupData) {
				cout << "Error: EMuShareMem: DLLLoadRandomNameGroups: !RandomNameGroupMMF (CanWrite=false)" << endl;
				return false;
			}
			*iMaxGroupID = MMFRandomNameGroupData->MaxRandNameID;
			*nameGroupCount = MMFRandomNameGroupData->randNameGroupCount;

			return true;
		}
	} else {
		cout << "Error Loading Items: RandomLootDrop.cpp: DLLLoadRandomNameGroups: Open() == false" << endl;
		return false;
	}
	/*
	
			// use a callback so the DB functions are done in the main exe
			// this way the DLL doesnt have to open a connection to mysql
			if (!cbDBLoadItems(*iItemCount, *iMaxItemID)) {
				cout << "Error: EMuShareMem: DLLLoadItems: !cbDBLoadItems" << endl;
				return false;
			}

	*/
	
	// use a callback so the DB functions are done in the main exe
	// this way the DLL doesnt have to open a connection to mysql
	if (!cbDBLoadRandomNameGroups(*nameGroupCount, *iMaxGroupID)) {
		cout << "Error: EMuShareMem: DLLLoadRandomNameGroups: !cbDBLoadRandomNameGroups" << endl;
		return false;
	}
	
	
	//Now, Disable the write handle and get the read handle.
	//do this for both item struct and serialization data
	
	MMFRandomNameGroupData = 0;
	RandomNameGroupMMF.SetLoaded();
	MMFRandomNameGroupData = (const MMFRandNameGroup_Struct*) RandomNameGroupMMF.GetHandle();
	if (!MMFRandomNameGroupData) {
		cout << "Error: EMuShareMem: DLLLoadRandomNameGroups: !MMFRandomNameGroupData (CanWrite=true)" << endl;
		return false;
	}
	
	return true;
}

DLLFUNC const RandomLootDrop_Struct* GetRandomLootDrop(uint32 id) {
	if (MMFRandomLootDropData == 0 || (!RandomLootDropMMF.IsLoaded()) || id > MAX_RANDLOOTDROPS || MMFRandomLootDropData->dropIndex[id] == 0xFFFF)
		return 0;

	return &MMFRandomLootDropData->LootDrops[MMFRandomLootDropData->dropIndex[id]];
}

DLLFUNC const RandomDropRate_Struct* GetRandomDropRate(uint32 id) {
	if (MMFRandomDropRateData == 0 || (!RandomDropRateMMF.IsLoaded()) || id > MAX_DROPRATES || MMFRandomDropRateData->dropIndex[id] == 0xFFFF)
		return 0;

	return &MMFRandomDropRateData->DropRates[MMFRandomDropRateData->dropIndex[id]];
}

DLLFUNC const RandomNameGroup_Struct* GetRandomNameGroup(uint32 id) {
	if (MMFRandomNameGroupData == 0 || (!RandomNameGroupMMF.IsLoaded()) || id > MAX_RANDOMNAMEGROUPS || MMFRandomNameGroupData->nameIndex[id] == 0xFFFF)
		return 0;

	return &MMFRandomNameGroupData->NameGroups[MMFRandomNameGroupData->nameIndex[id]];
}

DLLFUNC const RandomLootDrop_Struct* IterateLootDrops(uint32* NextIndex) {
	if (MMFRandomLootDropData == 0 || (!RandomLootDropMMF.IsLoaded()) || (*NextIndex) > MAX_RANDLOOTDROPS)
		return 0;
	do {
		if (MMFRandomLootDropData->dropIndex[*NextIndex] != 0xFFFF)
			return &MMFRandomLootDropData->LootDrops[MMFRandomLootDropData->dropIndex[(*NextIndex)++]];
	} while (++(*NextIndex) < MAX_RANDLOOTDROPS);
	
	return 0;
}
