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
#include <sys/time.h>
#endif

#include <memory.h>
#include <iostream>
using namespace std;
#include "TeamSettings.h"
#include "../common/timer.h"
#include "MMF.h"

MMF TeamSettingsMMF;
MMFTeamSettings_Struct* MMFTeamSettingsData = 0;
MMFTeamSettings_Struct* MMFTeamSettingsData_Writable = 0;

DLLFUNC bool AddTeamSettings(uint32 id, TeamSettings_Struct* tss) {
	if ( !MMFTeamSettingsData_Writable && TeamSettingsMMF.IsLoaded())
	{
		MMFTeamSettingsData = (MMFTeamSettings_Struct*) TeamSettingsMMF.GetHandle();
		tss->upkeepPaid = true; // this is true by default since we dont save this in the DB.

		tss->totalGuardsCount = 0;
		uint32 nextid = MMFTeamSettingsData->NextFreeIndex++;

		if ( MMFTeamSettingsData->teamCount <= nextid )
			MMFTeamSettingsData->teamCount = nextid++;

		MMFTeamSettingsData->teamIndex[id] = nextid;
		memcpy(&MMFTeamSettingsData->TeamSettings[nextid], tss, sizeof(TeamSettings_Struct));
		return true;
	}
	if (!MMFTeamSettingsData_Writable) {
		return false;
	}
	if (id > MAX_TEAMS) {
		return false;
	}
	if (MMFTeamSettingsData_Writable->teamIndex[id] != 0xFFFF) {
		return false;
	}

	tss->upkeepPaid = true; // this is true by default since we dont save this in the DB.

	tss->totalGuardsCount = 0;
	uint32 nextid = MMFTeamSettingsData_Writable->NextFreeIndex++;

	if ( MMFTeamSettingsData_Writable->teamCount <= nextid )
		MMFTeamSettingsData_Writable->teamCount = nextid++;

	MMFTeamSettingsData_Writable->teamIndex[id] = nextid;
	memcpy(&MMFTeamSettingsData_Writable->TeamSettings[nextid], tss, sizeof(TeamSettings_Struct));

	return true;
}

DLLFUNC bool UpdateTeamSettings(uint32 id, const TeamSettings_Struct* tss) {
	if ( !MMFTeamSettingsData_Writable && TeamSettingsMMF.IsLoaded())
	{
		MMFTeamSettingsData = (MMFTeamSettings_Struct*) TeamSettingsMMF.GetHandle();
		int32 indxID = MMFTeamSettingsData->teamIndex[id];
		if ( indxID != 0xFFFF )
			memcpy(&MMFTeamSettingsData->TeamSettings[indxID], tss, sizeof(TeamSettings_Struct));
		return true;
	}
	if (!MMFTeamSettingsData_Writable) {
		return false;
	}
	if (id > MAX_TEAMS) {
		return false;
	}

	int32 indxID = MMFTeamSettingsData_Writable->teamIndex[id];
	if (indxID == 0xFFFF) {
		return false;
	}

	memcpy(&MMFTeamSettingsData_Writable->TeamSettings[indxID], tss, sizeof(TeamSettings_Struct));

	return true;
}

DLLFUNC bool UpdatePVPLeaderboard(const PVPLeaderBoardEntry_Struct* lbe) {

	int bestPosPointReplaceName = -1;
	int bestPosPointReplaceValue = -1;

	int bestPosKillReplaceName = -1;
	int bestPosKillReplaceValue = -1;

	int bestPosInfamyReplaceName = -1;
	int bestPosInfamyReplaceValue = -1;
	int pos = 0;

	if ( !MMFTeamSettingsData_Writable && TeamSettingsMMF.IsLoaded())
	{
		MMFTeamSettingsData = (MMFTeamSettings_Struct*) TeamSettingsMMF.GetHandle();
		for (pos=0;pos<100;pos++)
		{
			// Do Point checks first since they are strange
			if ( bestPosPointReplaceName < 0 && MMFTeamSettingsData->TopPVPPointEntries[pos].Name[0] == 0 || !strcasecmp(MMFTeamSettingsData->TopPVPPointEntries[pos].Name, lbe->Name) )
			{
				bestPosPointReplaceName = pos;
			}
			else if ( bestPosPointReplaceName < 0 && bestPosPointReplaceValue < 0 && lbe->TotalPoints > MMFTeamSettingsData->TopPVPPointEntries[pos].TotalPoints )
			{
				bestPosPointReplaceValue = pos;
			}

			if ( bestPosKillReplaceName < 0 && MMFTeamSettingsData->TopPVPKillEntries[pos].Name[0] == 0 || !strcasecmp(MMFTeamSettingsData->TopPVPKillEntries[pos].Name, lbe->Name)  ) // null option field use this and break
			{
				bestPosKillReplaceName = pos;
			}
			else if ( bestPosKillReplaceName < 0 && bestPosKillReplaceValue < 0 && lbe->Kills > MMFTeamSettingsData->TopPVPKillEntries[pos].Kills )
			{
				bestPosKillReplaceValue = pos;
			}

			if ( bestPosInfamyReplaceName < 0 && MMFTeamSettingsData->TopPVPInfamyEntries[pos].Name[0] == 0 || !strcasecmp(MMFTeamSettingsData->TopPVPInfamyEntries[pos].Name, lbe->Name) )
			{
				bestPosInfamyReplaceName = pos;
			}
			else if ( bestPosInfamyReplaceName < 0 && bestPosInfamyReplaceValue < 0 && lbe->Infamy > MMFTeamSettingsData->TopPVPInfamyEntries[pos].Infamy )
			{
				bestPosInfamyReplaceValue = pos;
			}
		}

		/*if ( bestPosPointReplaceName < 0 )
		{
		for(;pos<100;pos++)
		{
		// Doing any additional checks just in case we missed out!
		if ( MMFTeamSettingsData->TopPVPPointEntries[pos].Name[0] == 0 || !strcasecmp(MMFTeamSettingsData->TopPVPPointEntries[pos].Name, lbe->Name) )
		{
		bestPosPointReplaceName = pos;
		break;
		}
		else if ( bestPosPointReplaceValue < 0 && lbe->TotalPoints > MMFTeamSettingsData->TopPVPPointEntries[pos].TotalPoints )
		{
		bestPosPointReplaceValue = pos;
		}
		}
		}*/

		// Set Points Field
		if ( bestPosPointReplaceName > -1  && bestPosPointReplaceValue < 0 )
		{
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceName].Deaths = lbe->Deaths;
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceName].Infamy = lbe->Infamy;
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceName].Kills = lbe->Kills;
			strcpy(MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceName].Name,lbe->Name);
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceName].TotalPoints = lbe->TotalPoints;
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceName].Infamy = lbe->Infamy;
		}
		else if ( bestPosPointReplaceName < 0 && bestPosPointReplaceValue > -1 )
		{
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].Deaths = lbe->Deaths;
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].Infamy = lbe->Infamy;
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].Kills = lbe->Kills;
			strcpy(MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].Name,lbe->Name);
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].TotalPoints = lbe->TotalPoints;
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].Infamy = lbe->Infamy;
		}
		else if ( bestPosPointReplaceName > -1 && bestPosPointReplaceValue > -1 )
		{
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceName].Deaths = MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].Deaths;
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceName].Infamy = MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].Infamy;
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceName].Kills = MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].Kills;
			strcpy(MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceName].Name,MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].Name);
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceName].TotalPoints = MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].TotalPoints;
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceName].Infamy = MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].Infamy;

			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].Deaths = lbe->Deaths;
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].Infamy = lbe->Infamy;
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].Kills = lbe->Kills;
			strcpy(MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].Name,lbe->Name);
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].TotalPoints = lbe->TotalPoints;
			MMFTeamSettingsData->TopPVPPointEntries[bestPosPointReplaceValue].Infamy = lbe->Infamy;
		}

		// Set Kills Field
		if ( bestPosKillReplaceName > -1  && bestPosKillReplaceValue < 0 )
		{
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceName].Deaths = lbe->Deaths;
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceName].Infamy = lbe->Infamy;
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceName].Kills = lbe->Kills;
			strcpy(MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceName].Name,lbe->Name);
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceName].TotalPoints = lbe->TotalPoints;
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceName].Infamy = lbe->Infamy;
		}
		else if ( bestPosKillReplaceName < 0 && bestPosKillReplaceValue > -1 )
		{
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].Deaths = lbe->Deaths;
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].Infamy = lbe->Infamy;
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].Kills = lbe->Kills;
			strcpy(MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].Name,lbe->Name);
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].TotalPoints = lbe->TotalPoints;
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].Infamy = lbe->Infamy;
		}
		else if ( bestPosKillReplaceName > -1 && bestPosKillReplaceValue > -1 )
		{
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceName].Deaths = MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].Deaths;
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceName].Infamy = MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].Infamy;
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceName].Kills = MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].Kills;
			strcpy(MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceName].Name,MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].Name);
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceName].TotalPoints = MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].TotalPoints;
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceName].Infamy = MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].Infamy;

			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].Deaths = lbe->Deaths;
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].Infamy = lbe->Infamy;
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].Kills = lbe->Kills;
			strcpy(MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].Name,lbe->Name);
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].TotalPoints = lbe->TotalPoints;
			MMFTeamSettingsData->TopPVPKillEntries[bestPosKillReplaceValue].Infamy = lbe->Infamy;
		}

		// Set Infamy Field
		if ( bestPosInfamyReplaceName > -1  && bestPosInfamyReplaceValue < 0 )
		{
			int replacePosition = -1;
			for(int i=bestPosInfamyReplaceName+1;i<100;i++)
			{
				if ( MMFTeamSettingsData->TopPVPInfamyEntries[i].Name[0] == 0 )
					break;
				if ( MMFTeamSettingsData->TopPVPInfamyEntries[i].Infamy > lbe->Infamy )
				{
					MMFTeamSettingsData->TopPVPInfamyEntries[i-1].Deaths = MMFTeamSettingsData->TopPVPInfamyEntries[i].Deaths;
					MMFTeamSettingsData->TopPVPInfamyEntries[i-1].Infamy = MMFTeamSettingsData->TopPVPInfamyEntries[i].Infamy;
					MMFTeamSettingsData->TopPVPInfamyEntries[i-1].Kills = MMFTeamSettingsData->TopPVPInfamyEntries[i].Kills;
					strcpy(MMFTeamSettingsData->TopPVPInfamyEntries[i-1].Name,MMFTeamSettingsData->TopPVPInfamyEntries[i].Name);
					MMFTeamSettingsData->TopPVPInfamyEntries[i-1].TotalPoints = MMFTeamSettingsData->TopPVPInfamyEntries[i].TotalPoints;
					MMFTeamSettingsData->TopPVPInfamyEntries[i-1].Infamy = MMFTeamSettingsData->TopPVPInfamyEntries[i].Infamy;
					replacePosition = i;
				}
				else
					break;
			}

			if ( replacePosition > -1 )
				bestPosInfamyReplaceName = replacePosition;

			MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceName].Deaths = lbe->Deaths;
			MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceName].Infamy = lbe->Infamy;
			MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceName].Kills = lbe->Kills;
			strcpy(MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceName].Name,lbe->Name);
			MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceName].TotalPoints = lbe->TotalPoints;
			MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceName].Infamy = lbe->Infamy;
		}
		else if ( bestPosInfamyReplaceName < 0 && bestPosInfamyReplaceValue > -1 )
		{
			MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Deaths = lbe->Deaths;
			MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy = lbe->Infamy;
			MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Kills = lbe->Kills;
			strcpy(MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Name,lbe->Name);
			MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].TotalPoints = lbe->TotalPoints;
			MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy = lbe->Infamy;
		}
		else if ( bestPosInfamyReplaceName > -1 && bestPosInfamyReplaceValue > -1 )
		{

			if ( (bestPosInfamyReplaceName - bestPosInfamyReplaceValue) > 1 )
			{
				for (int i=bestPosInfamyReplaceName;i>1;i--)
				{
					if ( MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy > MMFTeamSettingsData->TopPVPInfamyEntries[i-1].Infamy )
					{
						MMFTeamSettingsData->TopPVPInfamyEntries[i].Deaths = MMFTeamSettingsData->TopPVPInfamyEntries[i-1].Deaths;
						MMFTeamSettingsData->TopPVPInfamyEntries[i].Infamy = MMFTeamSettingsData->TopPVPInfamyEntries[i-1].Infamy;
						MMFTeamSettingsData->TopPVPInfamyEntries[i].Kills = MMFTeamSettingsData->TopPVPInfamyEntries[i-1].Kills;
						strcpy(MMFTeamSettingsData->TopPVPInfamyEntries[i].Name,MMFTeamSettingsData->TopPVPInfamyEntries[i-1].Name);
						MMFTeamSettingsData->TopPVPInfamyEntries[i].TotalPoints = MMFTeamSettingsData->TopPVPInfamyEntries[i-1].TotalPoints;
						MMFTeamSettingsData->TopPVPInfamyEntries[i].Infamy = MMFTeamSettingsData->TopPVPInfamyEntries[i-1].Infamy;

						MMFTeamSettingsData->TopPVPInfamyEntries[i-1].Deaths = MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Deaths;
						MMFTeamSettingsData->TopPVPInfamyEntries[i-1].Infamy = MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy;
						MMFTeamSettingsData->TopPVPInfamyEntries[i-1].Kills = MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Kills;
						strcpy(MMFTeamSettingsData->TopPVPInfamyEntries[i-1].Name,MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Name);
						MMFTeamSettingsData->TopPVPInfamyEntries[i-1].TotalPoints = MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].TotalPoints;
						MMFTeamSettingsData->TopPVPInfamyEntries[i-1].Infamy = MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy;
					}
				}
			}
			else
			{
				MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceName].Deaths = MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Deaths;
				MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceName].Infamy = MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy;
				MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceName].Kills = MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Kills;
				strcpy(MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceName].Name,MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Name);
				MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceName].TotalPoints = MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].TotalPoints;
				MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceName].Infamy = MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy;
			}

			MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Deaths = lbe->Deaths;
			MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy = lbe->Infamy;
			MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Kills = lbe->Kills;
			strcpy(MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Name,lbe->Name);
			MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].TotalPoints = lbe->TotalPoints;
			MMFTeamSettingsData->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy = lbe->Infamy;
		}

		return true;
	}
	if (!MMFTeamSettingsData_Writable) {
		return false;
	}
	for (pos=0;pos<100;pos++)
	{
		// Do Point checks first since they are strange
		if ( bestPosPointReplaceName < 0 && MMFTeamSettingsData_Writable->TopPVPPointEntries[pos].Name[0] == 0 || !strcasecmp(MMFTeamSettingsData_Writable->TopPVPPointEntries[pos].Name, lbe->Name) )
		{
			bestPosPointReplaceName = pos;
		}
		else if ( bestPosPointReplaceName < 0 && bestPosPointReplaceValue < 0 && lbe->TotalPoints > MMFTeamSettingsData_Writable->TopPVPPointEntries[pos].TotalPoints )
		{
			bestPosPointReplaceValue = pos;
		}

		if ( bestPosKillReplaceName < 0 && MMFTeamSettingsData_Writable->TopPVPKillEntries[pos].Name[0] == 0 || !strcasecmp(MMFTeamSettingsData_Writable->TopPVPKillEntries[pos].Name, lbe->Name)  ) // null option field use this and break
		{
			bestPosKillReplaceName = pos;
		}
		else if ( bestPosKillReplaceName < 0 && bestPosKillReplaceValue < 0 && lbe->Kills > MMFTeamSettingsData_Writable->TopPVPKillEntries[pos].Kills )
		{
			bestPosKillReplaceValue = pos;
		}

		if ( bestPosInfamyReplaceName < 0 && MMFTeamSettingsData_Writable->TopPVPInfamyEntries[pos].Name[0] == 0 || !strcasecmp(MMFTeamSettingsData_Writable->TopPVPInfamyEntries[pos].Name, lbe->Name)  ) // null option field use this and break
		{
			bestPosInfamyReplaceName = pos;
		}
		else if ( bestPosInfamyReplaceName < 0 && bestPosInfamyReplaceValue < 0 && lbe->Infamy > MMFTeamSettingsData_Writable->TopPVPInfamyEntries[pos].Infamy )
		{
			bestPosInfamyReplaceValue = pos;
		}
	}

	/*if ( bestPosPointReplaceName < 0 )
	{
	for(;pos<100;pos++)
	{
	// Doing any additional checks just in case we missed out!
	if ( MMFTeamSettingsData_Writable->TopPVPPointEntries[pos].Name[0] == 0 || !strcasecmp(MMFTeamSettingsData_Writable->TopPVPPointEntries[pos].Name, lbe->Name) )
	{
	bestPosPointReplaceName = pos;
	break;
	}
	else if ( bestPosPointReplaceValue < 0 && lbe->TotalPoints > MMFTeamSettingsData_Writable->TopPVPPointEntries[pos].TotalPoints )
	{
	bestPosPointReplaceValue = pos;
	}
	}
	}*/

	// Set Points Field
	if ( bestPosPointReplaceName > -1  && bestPosPointReplaceValue < 0 )
	{
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceName].Deaths = lbe->Deaths;
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceName].Infamy = lbe->Infamy;
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceName].Kills = lbe->Kills;
		strcpy(MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceName].Name,lbe->Name);
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceName].TotalPoints = lbe->TotalPoints;
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceName].Infamy = lbe->Infamy;
	}
	else if ( bestPosPointReplaceName < 0 && bestPosPointReplaceValue > -1 )
	{
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].Deaths = lbe->Deaths;
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].Infamy = lbe->Infamy;
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].Kills = lbe->Kills;
		strcpy(MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].Name,lbe->Name);
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].TotalPoints = lbe->TotalPoints;
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].Infamy = lbe->Infamy;
	}
	else if ( bestPosPointReplaceName > -1 && bestPosPointReplaceValue > -1 )
	{
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceName].Deaths = MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].Deaths;
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceName].Infamy = MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].Infamy;
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceName].Kills = MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].Kills;
		strcpy(MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceName].Name,MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].Name);
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceName].TotalPoints = MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].TotalPoints;
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceName].Infamy = MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].Infamy;

		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].Deaths = lbe->Deaths;
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].Infamy = lbe->Infamy;
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].Kills = lbe->Kills;
		strcpy(MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].Name,lbe->Name);
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].TotalPoints = lbe->TotalPoints;
		MMFTeamSettingsData_Writable->TopPVPPointEntries[bestPosPointReplaceValue].Infamy = lbe->Infamy;
	}

	// Set Kills Field
	if ( bestPosKillReplaceName > -1  && bestPosKillReplaceValue < 0 )
	{
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceName].Deaths = lbe->Deaths;
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceName].Infamy = lbe->Infamy;
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceName].Kills = lbe->Kills;
		strcpy(MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceName].Name,lbe->Name);
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceName].TotalPoints = lbe->TotalPoints;
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceName].Infamy = lbe->Infamy;
	}
	else if ( bestPosKillReplaceName < 0 && bestPosKillReplaceValue > -1 )
	{
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].Deaths = lbe->Deaths;
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].Infamy = lbe->Infamy;
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].Kills = lbe->Kills;
		strcpy(MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].Name,lbe->Name);
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].TotalPoints = lbe->TotalPoints;
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].Infamy = lbe->Infamy;
	}
	else if ( bestPosKillReplaceName > -1 && bestPosKillReplaceValue > -1 )
	{
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceName].Deaths = MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].Deaths;
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceName].Infamy = MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].Infamy;
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceName].Kills = MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].Kills;
		strcpy(MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceName].Name,MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].Name);
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceName].TotalPoints = MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].TotalPoints;
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceName].Infamy = MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].Infamy;

		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].Deaths = lbe->Deaths;
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].Infamy = lbe->Infamy;
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].Kills = lbe->Kills;
		strcpy(MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].Name,lbe->Name);
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].TotalPoints = lbe->TotalPoints;
		MMFTeamSettingsData_Writable->TopPVPKillEntries[bestPosKillReplaceValue].Infamy = lbe->Infamy;
	}

	// Set Infamy Field
	if ( bestPosInfamyReplaceName > -1  && bestPosInfamyReplaceValue < 0 )
	{
		int replacePosition = -1;
		for(int i=bestPosInfamyReplaceName+1;i<100;i++)
		{
			if ( MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i].Name[0] == 0 )
				break;
			if ( MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i].Infamy > lbe->Infamy )
			{
				MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].Deaths = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i].Deaths;
				MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].Infamy = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i].Infamy;
				MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].Kills = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i].Kills;
				strcpy(MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].Name,MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i].Name);
				MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].TotalPoints = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i].TotalPoints;
				MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].Infamy = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i].Infamy;
				replacePosition = i;
			}
			else
				break;
		}

		if ( replacePosition > -1 )
			bestPosInfamyReplaceName = replacePosition;

		MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceName].Deaths = lbe->Deaths;
		MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceName].Infamy = lbe->Infamy;
		MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceName].Kills = lbe->Kills;
		strcpy(MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceName].Name,lbe->Name);
		MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceName].TotalPoints = lbe->TotalPoints;
		MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceName].Infamy = lbe->Infamy;
	}
	else if ( bestPosInfamyReplaceName < 0 && bestPosInfamyReplaceValue > -1 )
	{
		MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Deaths = lbe->Deaths;
		MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy = lbe->Infamy;
		MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Kills = lbe->Kills;
		strcpy(MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Name,lbe->Name);
		MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].TotalPoints = lbe->TotalPoints;
		MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy = lbe->Infamy;
	}
	else if ( bestPosInfamyReplaceName > -1 && bestPosInfamyReplaceValue > -1 )
	{

		if ( (bestPosInfamyReplaceName - bestPosInfamyReplaceValue) > 1 )
		{
			for (int i=bestPosInfamyReplaceName;i>1;i--)
			{
				if ( MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy > MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].Infamy )
				{
					MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i].Deaths = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].Deaths;
					MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i].Infamy = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].Infamy;
					MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i].Kills = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].Kills;
					strcpy(MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i].Name,MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].Name);
					MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i].TotalPoints = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].TotalPoints;
					MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i].Infamy = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].Infamy;

					MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].Deaths = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Deaths;
					MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].Infamy = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy;
					MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].Kills = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Kills;
					strcpy(MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].Name,MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Name);
					MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].TotalPoints = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].TotalPoints;
					MMFTeamSettingsData_Writable->TopPVPInfamyEntries[i-1].Infamy = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy;
				}
			}
		}
		else
		{

			MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceName].Deaths = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Deaths;
			MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceName].Infamy = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy;
			MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceName].Kills = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Kills;
			strcpy(MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceName].Name,MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Name);
			MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceName].TotalPoints = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].TotalPoints;
			MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceName].Infamy = MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy;

		}

		MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Deaths = lbe->Deaths;
		MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy = lbe->Infamy;
		MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Kills = lbe->Kills;
		strcpy(MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Name,lbe->Name);
		MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].TotalPoints = lbe->TotalPoints;
		MMFTeamSettingsData_Writable->TopPVPInfamyEntries[bestPosInfamyReplaceValue].Infamy = lbe->Infamy;
	}

	return true;
}


DLLFUNC bool UpdateGuardSettings(const GuardSettings_Struct* gss) {
	int i=0;
	int bestReplaceID = -1;
	bool replaceField = false;
	uint32 teamID = 0;
	if ( !MMFTeamSettingsData_Writable && TeamSettingsMMF.IsLoaded())
	{
		MMFTeamSettingsData = (MMFTeamSettings_Struct*) TeamSettingsMMF.GetHandle();

		teamID = MMFTeamSettingsData->teamIndex[gss->teamID];
		
		for(int i=0;i<MMFTeamSettingsData->TeamSettings[teamID].totalGuardsCount;i++)
		{
			if ( MMFTeamSettingsData->TeamSettings[teamID].TeamGuards[i].guardNPCTypeID == 0 && 
				MMFTeamSettingsData->TeamSettings[teamID].TeamGuards[i].didMobSpawn == false )
			{
				bestReplaceID = i;
				break;
			}
		}

		if ( bestReplaceID == -1 && MMFTeamSettingsData->TeamSettings[teamID].totalGuardsCount > (GUARDLIMIT-2) )
			return true; // max of GUARDLIMIT-2

		if ( bestReplaceID == -1 )
		{
			bestReplaceID = MMFTeamSettingsData->TeamSettings[teamID].totalGuardsCount;
			MMFTeamSettingsData->TeamSettings[teamID].totalGuardsCount+=1;
		}

		memcpy(&MMFTeamSettingsData->TeamSettings[teamID].TeamGuards[bestReplaceID],gss,sizeof(GuardSettings_Struct));

		return true;
	}

	if ( !MMFTeamSettingsData_Writable )
		return false;

	teamID = MMFTeamSettingsData_Writable->teamIndex[gss->teamID];
			
	for(int i=0;i<MMFTeamSettingsData_Writable->TeamSettings[teamID].totalGuardsCount;i++)
	{
		if ( MMFTeamSettingsData_Writable->TeamSettings[teamID].TeamGuards[i].guardNPCTypeID == 0 && 
			MMFTeamSettingsData_Writable->TeamSettings[teamID].TeamGuards[i].didMobSpawn == false )
		{
			bestReplaceID = i;
			break;
		}
	}

	if ( bestReplaceID == -1 && MMFTeamSettingsData_Writable->TeamSettings[teamID].totalGuardsCount > (GUARDLIMIT-2) )
		return true; // max of GUARDLIMIT-2

	if ( bestReplaceID == -1 )
	{
		bestReplaceID = MMFTeamSettingsData_Writable->TeamSettings[teamID].totalGuardsCount;
		MMFTeamSettingsData_Writable->TeamSettings[teamID].totalGuardsCount+=1;
	}

	memcpy(&MMFTeamSettingsData_Writable->TeamSettings[teamID].TeamGuards[bestReplaceID],gss,sizeof(GuardSettings_Struct));

	return true;
}


DLLFUNC bool UpdateGuildHateList(uint32 teamid, GuildHate_Struct* ghs, bool purgeAll) {
	uint32 teamID = 0;
	if ( !MMFTeamSettingsData_Writable && TeamSettingsMMF.IsLoaded())
	{
		MMFTeamSettingsData = (MMFTeamSettings_Struct*) TeamSettingsMMF.GetHandle();

		teamID = MMFTeamSettingsData->teamIndex[teamid];

		if ( teamID >= MMFTeamSettingsData->teamCount )
			return true;

		int idToUse = -1;
		for(int i=0;i<GUILDHATELIMIT;i++)
		{
			if ( purgeAll )
			{
				MMFTeamSettingsData->TeamSettings[teamID].guildhate[i].taggedguild = 0;
				MMFTeamSettingsData->TeamSettings[teamID].guildhate[i].taggedtimestamp = 0;
			}
			else if ( MMFTeamSettingsData->TeamSettings[teamID].guildhate[i].taggedguild == ghs->taggedguild )
			{
				idToUse = i;
				break;
			}
			else if ( idToUse == -1 && MMFTeamSettingsData->TeamSettings[teamID].guildhate[i].taggedguild < 1 )
			{
				idToUse = i;
			}
		}

		if ( idToUse > -1 )
		{
			memcpy(&MMFTeamSettingsData->TeamSettings[teamID].guildhate[idToUse],ghs,sizeof(GuildHate_Struct));
		}

		return true;
	}

	if ( !MMFTeamSettingsData_Writable )
		return false;

	teamID = MMFTeamSettingsData_Writable->teamIndex[teamid];

	if ( teamID >= MMFTeamSettingsData_Writable->teamCount )
		return true;


	
		int idToUse = -1;
		for(int i=0;i<GUILDHATELIMIT;i++)
		{
			if ( purgeAll )
			{
				MMFTeamSettingsData_Writable->TeamSettings[teamID].guildhate[i].taggedguild = 0;
				MMFTeamSettingsData_Writable->TeamSettings[teamID].guildhate[i].taggedtimestamp = 0;
			}
			else if ( MMFTeamSettingsData_Writable->TeamSettings[teamID].guildhate[i].taggedguild == ghs->taggedguild )
			{
				idToUse = i;
				break;
			}
			else if ( idToUse == -1 && MMFTeamSettingsData_Writable->TeamSettings[teamID].guildhate[i].taggedguild < 1 )
			{
				idToUse = i;
			}
		}

		if ( idToUse > -1 )
		{
			memcpy(&MMFTeamSettingsData_Writable->TeamSettings[teamID].guildhate[idToUse],ghs,sizeof(GuildHate_Struct));

		}

	return true;
}

DLLFUNC bool DLLLoadTeamSettings(CALLBACK_DBLoadTeamSettings cbDBLoadTeamSettings, int32 structSize, sint32* count, int32* maxTeamID) {
	if (structSize != sizeof(TeamSettings_Struct)) {
		cout << "Error: EMuShareMem: DLLLoadTeamSettings: structSize != sizeof(TeamSettings_Struct)" << endl;
		cout << "TeamSettings_Struct has changed, EMuShareMem.dll needs to be recompiled." << endl;
		return false;
	}
	if (*maxTeamID > MAX_TEAMS) {
		cout << "Error: EMuShareMem: DLLLoadTeamSettings: iMaxDropID > MAX_TEAMS" << endl;
		cout << "You need to increase the define in TeamSettings.h." << endl;
		return false;
	}

	MMFTeamSettingsData_Writable = 0;
	//Allocate the shared memory for the item structures
	int32 ZoneNamestructSize = sizeof(TeamSettings_Struct);
	int32 mmfStructSize = sizeof(MMFTeamSettings_Struct);

	int32 tmpMemSize = sizeof(MMFTeamSettings_Struct) + 256 + (ZoneNamestructSize * MAX_TEAMS);
	cout << "TeamSettingsStruct Size: " << ZoneNamestructSize << ", MMF Size: " << mmfStructSize << endl;

	if (TeamSettingsMMF.Open("EQEMuTeams", tmpMemSize)) {
		if (TeamSettingsMMF.CanWrite()) {
			MMFTeamSettingsData_Writable = (MMFTeamSettings_Struct*) TeamSettingsMMF.GetWriteableHandle();
			if (!MMFTeamSettingsData_Writable) {
				cout << "Error: EMuShareMem: DLLLoadRandomZoneNames: !MMFTeamSettingsData_Writable" << endl;
				return false;
			}

			memset(MMFTeamSettingsData_Writable, 0, tmpMemSize);
			for(int i=0; i<MAX_TEAMS; i++)
				MMFTeamSettingsData_Writable->teamIndex[i] = 0xFFFF;
			MMFTeamSettingsData_Writable->MaxTeamID = *maxTeamID;
			MMFTeamSettingsData_Writable->teamCount = *count;
			//the writable handle has been created, do the load below after we have the 
			//serialization handle as well.
		} else {
			if (!TeamSettingsMMF.IsLoaded()) {
				Timer::SetCurrentTime();
				int32 starttime = Timer::GetCurrentTime();
				while ((!TeamSettingsMMF.IsLoaded()) && ((Timer::GetCurrentTime() - starttime) < 300000)) {
					Sleep(10);
					Timer::SetCurrentTime();
				}
				if (!TeamSettingsMMF.IsLoaded()) {
					cout << "Error: EMuShareMem: DLLLoadTeamSettings: !TeamSettingsMMF.IsLoaded() (timeout)" << endl;
					return false;
				}
			}
			MMFTeamSettingsData = (MMFTeamSettings_Struct*) TeamSettingsMMF.GetHandle();
			if (!MMFTeamSettingsData) {
				cout << "Error: EMuShareMem: DLLLoadTeamSettings: !MMFTeamSettingsData (CanWrite=false)" << endl;
				return false;
			}
			*maxTeamID = MMFTeamSettingsData->MaxTeamID;
			*count = MMFTeamSettingsData->teamCount;

			return true;
		}
	} else {
		cout << "Error Loading Items: TeamSettings.cpp: pDLLLoadTeamSettings: Open() == false" << endl;
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
	if (!cbDBLoadTeamSettings(*count, *maxTeamID)) {
		cout << "Error: EMuShareMem: DLLLoadRandomZoneNames: !cbDBLoadRandomZoneNames" << endl;
		return false;
	}


	//Now, Disable the write handle and get the read handle.
	//do this for both item struct and serialization data

	MMFTeamSettingsData = 0;
	TeamSettingsMMF.SetLoaded();
	MMFTeamSettingsData = (MMFTeamSettings_Struct*) TeamSettingsMMF.GetHandle();
	if (!MMFTeamSettingsData) {
		cout << "Error: EMuShareMem: DLLLoadRandomZoneNames: !MMFTeamSettingsData (CanWrite=true)" << endl;
		return false;
	}

	return true;
}

DLLFUNC TeamSettings_Struct* GetTeamSettings(int32 id) {
	if (MMFTeamSettingsData == 0 || (!TeamSettingsMMF.IsLoaded()) || id > MAX_TEAMS || MMFTeamSettingsData->teamIndex[id] == 0xFFFF)
		return 0;

	return &MMFTeamSettingsData->TeamSettings[MMFTeamSettingsData->teamIndex[id]];
}

DLLFUNC GuardSettings_Struct* GetGuardSettings(int32 teamid, int32 npctypeid, int32 spawngroupid) {
	if (MMFTeamSettingsData == 0 || (!TeamSettingsMMF.IsLoaded()) || teamid > MAX_TEAMS || MMFTeamSettingsData->teamIndex[teamid] == 0xFFFF)
		return 0;

	uint32 teamidx = MMFTeamSettingsData->teamIndex[teamid];
	for (uint32 fieldid=0;fieldid<MMFTeamSettingsData->TeamSettings[teamidx].totalGuardsCount;fieldid++)
	{
		if ( MMFTeamSettingsData->TeamSettings[teamidx].TeamGuards[fieldid].guardNPCTypeID == npctypeid && 
			( MMFTeamSettingsData->TeamSettings[teamidx].TeamGuards[fieldid].spawngroupid == 0 || MMFTeamSettingsData->TeamSettings[teamidx].TeamGuards[fieldid].spawngroupid == spawngroupid ) )
			return &MMFTeamSettingsData->TeamSettings[teamidx].TeamGuards[fieldid];
	}
	return 0;
}

DLLFUNC GuildHate_Struct* GetGuildHateList(int32 teamid, int32 targetguildid) {
	if (MMFTeamSettingsData == 0 || (!TeamSettingsMMF.IsLoaded()) || teamid > MAX_TEAMS || MMFTeamSettingsData->teamIndex[teamid] == 0xFFFF)
		return 0;
	
	timeval tv;
	gettimeofday(&tv, NULL);

	uint32 teamidx = MMFTeamSettingsData->teamIndex[teamid];
	for (uint32 fieldid=0;fieldid<GUILDHATELIMIT;fieldid++)
	{
		if ( MMFTeamSettingsData->TeamSettings[teamidx].guildhate[fieldid].taggedtimestamp > 0 &&
			( tv.tv_sec - MMFTeamSettingsData->TeamSettings[teamidx].guildhate[fieldid].taggedtimestamp ) > 86400 ) // 24 hours in seconds
		{
			if ( !MMFTeamSettingsData_Writable && TeamSettingsMMF.IsLoaded())
			{
				MMFTeamSettingsData = (MMFTeamSettings_Struct*) TeamSettingsMMF.GetHandle();
				MMFTeamSettingsData->TeamSettings[teamidx].guildhate[fieldid].taggedguild = 0;
				MMFTeamSettingsData->TeamSettings[teamidx].guildhate[fieldid].taggedtimestamp = 0;
			}
			else if ( MMFTeamSettingsData_Writable )
			{
				MMFTeamSettingsData_Writable->TeamSettings[teamidx].guildhate[fieldid].taggedguild = 0;
				MMFTeamSettingsData_Writable->TeamSettings[teamidx].guildhate[fieldid].taggedtimestamp = 0;
			}
		}

		if ( MMFTeamSettingsData->TeamSettings[teamidx].guildhate[fieldid].taggedguild == targetguildid )
			return &MMFTeamSettingsData->TeamSettings[teamidx].guildhate[fieldid];
	}
	return 0;
}

DLLFUNC PVPLeaderBoardEntry_Struct* GetPVPLeadershipBoardByInfamy() {
	if (MMFTeamSettingsData == 0 || (!TeamSettingsMMF.IsLoaded()))
		return 0;

	return MMFTeamSettingsData->TopPVPInfamyEntries;
}

DLLFUNC PVPLeaderBoardEntry_Struct* GetPVPLeadershipBoardByPoints() {
	if (MMFTeamSettingsData == 0 || (!TeamSettingsMMF.IsLoaded()))
		return 0;

	return MMFTeamSettingsData->TopPVPPointEntries;
}

DLLFUNC PVPLeaderBoardEntry_Struct* GetPVPLeadershipBoardByKills() {
	if (MMFTeamSettingsData == 0 || (!TeamSettingsMMF.IsLoaded()))
		return 0;

	return MMFTeamSettingsData->TopPVPKillEntries;
}

DLLFUNC bool* GetIsItemFound(int32 id) {
	if (MMFTeamSettingsData == 0 || (!TeamSettingsMMF.IsLoaded()) || id > MMF_EQMAX_ITEMS)
		return 0;

	return &MMFTeamSettingsData->itemFound[id];
}