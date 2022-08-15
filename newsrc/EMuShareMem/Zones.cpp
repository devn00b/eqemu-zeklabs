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
#include "Zones.h"
#include "../common/timer.h"
#include "MMF.h"

MMF ZoneNamesMMF;
const MMFZoneName_Struct* MMFZoneNameData = 0;
MMFZoneName_Struct* MMFZoneNameData_Writable = 0;

DLLFUNC bool AddZoneName(uint32 id, const ZoneShortName_Struct* rld) {
	if (!MMFZoneNameData_Writable) {
		return false;
	}
	if (id > MAX_ZONES || MMFZoneNameData_Writable->NextFreeIndex >= MMFZoneNameData_Writable->nameCount) {
		return false;
	}
	if (MMFZoneNameData_Writable->nameIndex[id] != 0xFFFF) {
		return false;
	}
	
	uint32 nextid = MMFZoneNameData_Writable->NextFreeIndex++;
	MMFZoneNameData_Writable->nameIndex[id] = nextid;
	memcpy(&MMFZoneNameData_Writable->ZoneNames[nextid], rld, sizeof(ZoneShortName_Struct));

	return true;
}

DLLFUNC bool DLLLoadZoneNames(CALLBACK_DBLoadZoneNames cbDBLoadZoneNames, int32 structSize, sint32* lootnameCount, int32* iMaxDropID) {
	if (structSize != sizeof(ZoneShortName_Struct)) {
		cout << "Error: EMuShareMem: DLLLoadRandomZoneNames: structSize != sizeof(ZoneShortName_Struct)" << endl;
		cout << "ZoneShortName_Struct has changed, EMuShareMem.dll needs to be recompiled." << endl;
		return false;
	}
	if (*iMaxDropID > MAX_ZONES) {
		cout << "Error: EMuShareMem: DLLLoadRandomZoneNames: iMaxDropID > MAX_ZONES" << endl;
		cout << "You need to increase the define in RandomLootDrop.h." << endl;
		return false;
	}
	
	MMFZoneNameData_Writable = 0;
	//Allocate the shared memory for the item structures
	int32 ZoneNamestructSize = sizeof(ZoneShortName_Struct);
	int32 mmfStructSize = sizeof(MMFZoneName_Struct);

	int32 tmpMemSize = sizeof(MMFZoneName_Struct) + 256 + (ZoneNamestructSize * (*lootnameCount));
	cout << "ZoneNamestruct Size: " << ZoneNamestructSize << ", MMF Size: " << mmfStructSize << endl;

	if (ZoneNamesMMF.Open("EQEMuZoneNames", tmpMemSize)) {
		if (ZoneNamesMMF.CanWrite()) {
			MMFZoneNameData_Writable = (MMFZoneName_Struct*) ZoneNamesMMF.GetWriteableHandle();
			if (!MMFZoneNameData_Writable) {
				cout << "Error: EMuShareMem: DLLLoadRandomZoneNames: !MMFZoneNameData_Writable" << endl;
				return false;
			}

			memset(MMFZoneNameData_Writable, 0, tmpMemSize);
			for(int i=0; i<MAX_ZONES; i++)
				MMFZoneNameData_Writable->nameIndex[i] = 0xFFFF;
			MMFZoneNameData_Writable->MaxZoneNameID = *iMaxDropID;
			MMFZoneNameData_Writable->nameCount = *lootnameCount;
			//the writable handle has been created, do the load below after we have the 
			//serialization handle as well.
		} else {
			if (!ZoneNamesMMF.IsLoaded()) {
				Timer::SetCurrentTime();
				int32 starttime = Timer::GetCurrentTime();
				while ((!ZoneNamesMMF.IsLoaded()) && ((Timer::GetCurrentTime() - starttime) < 300000)) {
					Sleep(10);
					Timer::SetCurrentTime();
				}
				if (!ZoneNamesMMF.IsLoaded()) {
					cout << "Error: EMuShareMem: DLLLoadRandomZoneNames: !ZoneNamesMMF.IsLoaded() (timeout)" << endl;
					return false;
				}
			}
			MMFZoneNameData = (const MMFZoneName_Struct*) ZoneNamesMMF.GetHandle();
			if (!MMFZoneNameData) {
				cout << "Error: EMuShareMem: DLLLoadRandomZoneNames: !MMFZoneNameData (CanWrite=false)" << endl;
				return false;
			}
			*iMaxDropID = MMFZoneNameData->MaxZoneNameID;
			*lootnameCount = MMFZoneNameData->nameCount;

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
	if (!cbDBLoadZoneNames(*lootnameCount, *iMaxDropID)) {
		cout << "Error: EMuShareMem: DLLLoadRandomZoneNames: !cbDBLoadRandomZoneNames" << endl;
		return false;
	}
	
	
	//Now, Disable the write handle and get the read handle.
	//do this for both item struct and serialization data
	
	MMFZoneNameData = 0;
	ZoneNamesMMF.SetLoaded();
	MMFZoneNameData = (const MMFZoneName_Struct*) ZoneNamesMMF.GetHandle();
	if (!MMFZoneNameData) {
		cout << "Error: EMuShareMem: DLLLoadRandomZoneNames: !MMFZoneNameData (CanWrite=true)" << endl;
		return false;
	}
	
	return true;
}

DLLFUNC const ZoneShortName_Struct* GetZoneName(int32 id) {
	if (MMFZoneNameData == 0 || (!ZoneNamesMMF.IsLoaded()) || id > MAX_ZONES || MMFZoneNameData->nameIndex[id] == 0xFFFF)
		return 0;

	return &MMFZoneNameData->ZoneNames[MMFZoneNameData->nameIndex[id]];
}

DLLFUNC const ZoneShortName_Struct* IterateZoneNames(uint32* NextIndex) {
	if (MMFZoneNameData == 0 || (!ZoneNamesMMF.IsLoaded()) || (*NextIndex) > MAX_ZONES)
		return 0;
	do {
		if (MMFZoneNameData->nameIndex[*NextIndex] != 0xFFFF)
			return &MMFZoneNameData->ZoneNames[MMFZoneNameData->nameIndex[(*NextIndex)++]];
	} while (++(*NextIndex) < MAX_ZONES);
	
	return 0;
}
