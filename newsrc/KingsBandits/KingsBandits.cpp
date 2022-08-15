
#include "KingsBandits.h"
#include "KBFunctions.h"

#ifdef ZONE
#include "../zone/command.h"
#include "../zone/client.h"
#include "../zone/zone.h"
#include "../zone/worldserver.h"
#include "../zone/masterentity.h"
#include "../zone/mob.h"
#include "../zone/zone.h"
#endif

#ifdef ZONE
extern ZoneDatabase database;
extern Zone* zone;

void command_summonlootgroup(Client *c, const Seperator *sep)
{
	if (!sep->IsNumber(1))
		c->Message(0, "Usage: #summonlootgroup [group id]");
	else {
		int32 groupid = atoi(sep->arg[1]);
		RandomLootDropResult_Struct rld;
		memset(&rld,0,sizeof(RandomLootDropResult_Struct));

		bool generatedResp = database.GenerateRandomLootGroupResult(&rld, groupid, "UNKNOWN", (char*)c->GetName(), c->CharacterID());
		for(int i=0;i<rld.itemDropCount;i++)
		{
			if ( rld.items[i].itemID == 0 )
				continue;

			c->SummonItem(rld.items[i].itemID);
		}

		c->AddMoneyToPP(rld.coinDropValue,true);

	}
}

void command_setpvpflag(Client *c, const Seperator *sep)
{
	if (!sep->IsNumber(1))
		c->Message(0, "Usage: #setpvpflag [value] (with selected target)");
	else {
		uint8 pvpflag = (uint8)atoul(sep->arg[1]);
		if ( c->GetTarget() == NULL || !c->GetTarget()->IsClient() )
			c->Message(0, "No target or not a client.");
		else
		{
			c->Message(0, "Set PVP flag on player to %i.",pvpflag);
			c->GetTarget()->CastToClient()->SetPVP(pvpflag);
			c->Message(0, "Final flag set to %i (may have been overriden if 30+).",c->GetTarget()->CastToClient()->GetPVPValue());
		}
	}
}

void command_setzonepvpflag(Client *c, const Seperator *sep)
{
	if (!sep->IsNumber(1))
		c->Message(0, "Usage: #setzonepvpflag [value] 0=nopvp,1=teampvp,2=ffapvp - sets current zone pvp flag.");
	else {
		uint8 pvpflag = (uint8)atoul(sep->arg[1]);
		zone->SetCanPVPValue(pvpflag);
		c->Message(0, "Set PVP zone flag temporarily to %i.",pvpflag);
	}
}

void command_setisking(Client *c, const Seperator *sep)
{
	if (!sep->IsNumber(1))
		c->Message(0, "Usage: #setisking [value] 0=notking,1=king - sets king player flag.");
	else {
		uint8 pvpflag = (uint8)atoul(sep->arg[1]);
		c->SetIsKing((bool)pvpflag);
		c->Message(0, "Set King Flag on %s to %i.",c->GetName(),c->IsKing());
	}
}

void command_syncacct(Client *c, const Seperator *sep)
{
	if (strlen(sep->arg[1]) < 1 || strlen(sep->arg[2]) < 1)
		c->Message(0, "Usage: #syncacct [accountname] [synckey].");
	else {
		bool isEQEmuAcct = false;
		int32 syncAcctID = database.GetSyncAccountID(c->AccountID(),&isEQEmuAcct);
		if ( !isEQEmuAcct ) { c->Message(13,"Error! You cannot sync a eqpvp.com login account with another.  This is used only on a EQEmulator.net account to sync to eqpvp.com."); return; }
		if ( syncAcctID > 0 ) { c->Message(13,"Error! Your account is already synched to another account, you can only do this once."); return; }

		int32 acctID = database.GetSyncAccountByName(sep->arg[1],sep->arg[2]);
		if ( acctID < 1 ) { c->Message(13,"Error! I could not find an account %s with the expected sync key %s.  Please make sure the account name and key are correct.",sep->arg[1],sep->arg[2]); }
		else if ( acctID == c->AccountID() )
		{
			c->Message(13,"Error! You cannot sync an account ID to itself...!");
		}
		else
		{
			int8 charCount = database.GetSyncCharacterCount(c->AccountID(),acctID);
			if ( charCount > 8 ) { c->Message(13,"Error!  Too many characters between both accounts.  There are %i characters and you may only have a max of 8 over both accounts.",charCount); }
			else
			{
				c->Message(4,"Synching accounts...");
				database.UpdateAccountSetSyncID(c->AccountID(),acctID);
				database.UpdateAccountSetSyncID(acctID,c->AccountID());
				c->Message(4,"Accounts synched.  You must log out completely to the login server for the changes to take effect.");
			}
		}
	}
}

bool Client::OpenShroudWindow(Mob* npc)
{
	char shroudselect_str[] = {
		"PROGRESSION" "\007" // 'PROGRESSION'
		"1" "\007" // unknown integer
		"Aberrations" "\007" // Progression name
		"2" "\007" // unknown integer
		"BRANCH" "\007" // 'BRANCH'
		"3" "\007" // unknown integer
		"Imp Wizard" "\007" // Branch name
		"4" "\007" // unknown integer
		"100" "\007" // Percentage complete
		"TEMPLATE" "\007" // 'TEMPLATE'
		"Imp Wizard 5" "\007" // Template name
		"5" "\007" // Template level
		"5" "\007" // UniqueID for this template
		"1" "\007" // 1=there's more, 0=end

		"TEMPLATE" "\007" // another template
		"Imp Wizard 10" "\007"
		"10" "\007"
		"6" "\007"
		"1" "\007"

		"TEMPLATE" "\007" // another template
		"Imp Wizard 15" "\007"
		"15" "\007"
		"6" "\007"
		"1" "\007"
	};

	int32 lengthStr = strlen(shroudselect_str);
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_ShroudSelectionWindow, sizeof(ShroudSelectionWindow) + lengthStr);
	ShroudSelectionWindow* ct = (ShroudSelectionWindow*) outapp->pBuffer;

	/*if ( this->IsShroudNPC() || npc == NULL)
	{
		if ( npc != NULL )
			ct->triggerNPCID = npc->GetID();
		else
			ct->triggerNPCID = 0;
		// causes error bank has items
		ct->numShroudBankItems = 1; // set to > 0 if not available to player
		strncpy(ct->shroudSelectionTreeString,shroudselect_str,lengthStr);
		DumpPacket(outapp);
		FastQueuePacket(&outapp);
		printf("NPC was null \n");

		return false;
	}*/

	ct->triggerNPCID = npc->GetID();
	ct->numShroudBankItems = 0; // set to > 0 if not available to player
	strncpy(ct->shroudSelectionTreeString,shroudselect_str,lengthStr);

	DumpPacket(outapp);

	FastQueuePacket(&outapp);
	return true;
}

/*

char progression_str[] =	{
"PROGRESSION" "\007"	 // must be the word 'PROGRESSION'
"1" "\007"				 // unknown integer
"Aberrations" "\007"	 // Progression name
"Imp Wizard 5" "\007"	 // Class name
"3" "\007"				 // How many unlocked?
"14" "\007"				 // How many total?
"Evil Eye Psion" "\007"  // Next branch name
"Imp Wizard 10" "\007"	 // Next class name
"6" "\007"				 // Percentage complete

"PROGRESSION" "\007"	 // Here follows a second
"2" "\007"				 //  progression described in
"Animals" "\007"		 //  the same packet.
"Bear Beast 5" "\007"	
"7" "\007"				
"12" "\007"				
"Wolf Beast" "\007"		
"Bear Beast 10" "\007"	
"25" "\007"		}		
;
int32 lengthStr = strlen(progression_str);
EQApplicationPacket* outapp = new EQApplicationPacket(OP_ShroudProgressionUpdate, sizeof(ShroudSelectionWindow) + lengthStr);
ShroudSelectionWindow* ct = (ShroudSelectionWindow*) outapp->pBuffer;
ct->numItems = 2;
strncpy(ct->shroudSelectionTreeString,progression_str,lengthStr);
DumpPacket(outapp);
FastQueuePacket(&outapp);

char ability_str[] =	{
"1" "\007"				 // unknown integer
"7506" "\007"				 // unknown integer
"2" "\007"				 // unknown integer
"7507" "\007"				 // unknown integer
"1" "\007"				 // unknown integer
"7506" "\007"				 // unknown integer
"2" "\007"				 // unknown integer
"7507" "\007"				 // unknown integer
"1" "\007"				 // unknown integer
"7506" "\007"				 // unknown integer
"2" "\007"				 // unknown integer
"7507" "\007"				 // unknown integer
"0" "\007"				 // unknown integer
}		
;
lengthStr = strlen(ability_str);
EQApplicationPacket* outapp2 = new EQApplicationPacket(OP_ShroudRequestStats, sizeof(ShroudUpdateSelectWindow) + lengthStr);
ShroudUpdateSelectWindow* ct2 = (ShroudUpdateSelectWindow*) outapp2->pBuffer;
ct2->numItems = 2;
strncpy(ct2->shroudSelectionTreeString,ability_str,lengthStr);
DumpPacket(outapp2);
FastQueuePacket(&outapp2);*/

void Client::Handle_OP_ShroudUnknown1(const EQApplicationPacket *app)
{
return;
if ( app->size > 0 && app->pBuffer[0] == 0x06 )
{
	printf("Handle submit monster\n");
EQApplicationPacket* outapp = new EQApplicationPacket(OP_ShroudUnknown1, 8);
FastQueuePacket(&outapp);
	return;
}
	//printf("IncData:\n");
	//DumpPacket(app);

	//printf("OutData:\n");

	char ability_str[] =	{
		"1" "\007"				 // unknown integer
	};
	int32 lengthStr = strlen(ability_str);
	EQApplicationPacket* outapp2 = new EQApplicationPacket(OP_ShroudRequestStats, sizeof(ShroudUpdateSelectWindow) + lengthStr);
	ShroudUpdateSelectWindow* ct2 = (ShroudUpdateSelectWindow*) outapp2->pBuffer;
	ct2->numItems = 3;
	strncpy(ct2->shroudSelectionTreeString,ability_str,lengthStr);
	DumpPacket(outapp2);
	FastQueuePacket(&outapp2);
}


void Client::Handle_OP_Shroud(const EQApplicationPacket *app)
{

}

bool Mob::IsAttackLevelPermitted(Mob* c1, Mob* c2, bool ignoreInfamy)
{
	char tmp[20];
	database.GetVariable("ServerType", tmp, 9);
	int pvpFlag = atoi(tmp);

	int lvlRange = RuleI(PVPCombat, PVPLevelRange);

	if ( lvlRange > 0 ) // overrides the lvl range hard coded by server type
	{
		int lvl = abs(c1->GetLevel()-c2->GetLevel());
		if ( lvl < lvlRange )
			return true;
	}
	else
	{
		switch(pvpFlag)
		{
		case SERVERTYPE_RED69:
			{
				int lvl = abs(c1->GetLevel()-c2->GetLevel());
				if ( lvl < 11 )
					return true;

				break;
			}
		case SERVERTYPE_KAB:
		case SERVERTYPE_ZEK:
			{
				int lvl = abs(c1->GetLevel()-c2->GetLevel());

				if ( ignoreInfamy )
				{
					if ( ((c1->IsClient() && c1->CastToClient()->GetAttackTarget() != NULL) || (c2->IsClient() && c2->CastToClient()->GetAttackTarget() != NULL))  && lvl > RuleI(PVPCombat, PVPHealLevelRange) )
						return false;

					if ( lvl > RuleI(PVPCombat, HealLevelRange) )
						return false;
					else
						return true;
				}
				else if ( ( RuleI(PVPCombat, PVPZekLevelRange) > 0 && lvl < RuleI(PVPCombat, PVPZekLevelRange) ) || ( lvl < 5 ) )
					return true;

				break;
			}
		}
	}

	//infamy logic
	if ( !ignoreInfamy && c1->IsClient() && c2->IsClient() )
	{
		if ( c1->CastToClient()->IsInfamyReached() )
		{
			if ( c2->GetLevel() >= c1->GetLevel() )
				return true;
		}
		if ( c2->CastToClient()->IsInfamyReached() )
		{
			if ( c1->GetLevel() >= c2->GetLevel() )
				return true;
		}
	}

	return false;
}

bool KingsBandits::IsCitySacked(uint32 kingdomID, int32& charIDOwner, int32& guildIDOwner, int32& teamIDOwner, int32& charLevel)
{
	TeamSettings_Struct* team = database.GetTeamSettings(kingdomID);

	// these kingdoms cannot be conquered at this time
	switch(team->teamID)
	{
	case BANDITTEAMID:
		{
			return false;
			break;
		}
	}

	charIDOwner = 0;
	guildIDOwner = 0;
	teamIDOwner = 0;

	if ( team == NULL || team->citymode == CITYDISABLED || team->citymode == CITYENABLED )
		return false;

	charIDOwner = team->charid;
	guildIDOwner = team->guildid;
	teamIDOwner = team->ownerteamid;

	if ( team->totalGuardsCount < 1)
		return false;

	std::map<int32,int32> characterkills;
	std::map<int32,int32> guildidlist;
	std::map<int32,int32> teamidlist;
	std::map<int32,int32> characterlevel;

	for(int i=0;i<team->totalGuardsCount;i++)
	{
		if ( team->TeamGuards[i].lastkilledtimestamp == 0 && team->TeamGuards[i].didMobSpawn )
			return false;
		else if ( team->TeamGuards[i].lastkilledcharid > 0 )
		{
			// If a guildid of 0 is passed it means that there was a previous encounter in which the city was taken over
			// all guards must respawn first before they can take over
			
			int32 guildID = team->TeamGuards[i].lastkilledguildid;

			// no guild, or same guild, we continue and skip it
			//if ( !RuleB(PVPCombat,TeamGuildWars) && ( guildID == 0 || guildID == GUILD_NONE || team->guildid == guildID ) )
			//	continue;

			int32 charID = team->TeamGuards[i].lastkilledcharid;

			if ( charID == 0 || team->TeamGuards[i].lastkilledteamid == BANDITTEAMID )
				continue;

			characterkills[charID] += 1;
			if ( team->TeamGuards[i].lastkilledlevel > 0 )
				characterlevel[charID] = team->TeamGuards[i].lastkilledlevel;
			
			guildidlist[charID] = guildID;
			teamidlist[charID] = team->TeamGuards[i].lastkilledteamid;
		}
	}

	int32 highestCharID = 0;
	int32 highestkillCount = 0;
	std::map<int32,int32>::iterator iter;
	for (iter = characterkills.begin(); iter != characterkills.end(); iter++) {
		if ( iter->second > highestkillCount )
		{
			highestCharID = iter->first;
			highestkillCount = iter->second;
		}
	}
	
	int32 newGuildID = guildidlist[highestCharID];
	int32 newTeamID = teamidlist[highestCharID];
	if ( newTeamID == 0 )
	{
		highestCharID = 0;
	}

	int32 cityCount = 0;
	for(int i=0;i<MAXKINGDOMS;i++)
	{
		// everything above the erudin team id is a camp
		if ( i > ERUDINTEAMID )
			break;

		TeamSettings_Struct* team = database.GetTeamSettings(i);
		if ( team )
		{
			if ( RuleB(PVPCombat,TeamGuildWars) )
			{
				// no new team member id with 0 should take place due to return false above
				if ( team->ownerteamid == newTeamID )
					cityCount++;
			}
			else if ( team->guildid == newGuildID )
				cityCount++;
		}
	}

	if ( kingdomID < GREATDIVIDEENCAMPMENT && cityCount > RuleI(Kingdoms,MaxTeamCities) )
	{
		Client* client = entity_list.GetClientByCharID(highestCharID);
		if ( client != NULL )
			client->Message(13,"Your team already owns two cities and you may not claim another.");

		return false;
	}

	if ( highestCharID > 0 )
	{
		charIDOwner = highestCharID;
		guildIDOwner = newGuildID;
		teamIDOwner = newTeamID;
		charLevel = characterlevel[highestCharID];
	}

	if ( highestCharID == 0 && kingdomID < GREATDIVIDEENCAMPMENT )
		return false;

	return true;
}

void KingsBandits::ResetKingdomGuards(uint32 kingdomID)
{
	TeamSettings_Struct* team = database.GetTeamSettings(kingdomID);

	if ( team == NULL )
		return;

	for(int i=0;i<team->totalGuardsCount;i++)
	{
		// set guild id to 0 this will mean it was killed but the guild id is not present so we used it already
		team->TeamGuards[i].lastkilledguildid = 0;
		team->TeamGuards[i].didMobSpawn = false;
	}
}


void KingsBandits::ReloadCityGuards(uint32 kingdomID)
{
	TeamSettings_Struct* team = database.GetTeamSettings(kingdomID);

	if ( team == NULL )
		return;

	if ( team->citymode == CAMPCONSTRUCTED )
		return;
	
		LinkedListIterator<Spawn2*> iterator(zone->spawn2_list);
		iterator.Reset();
		char errbuf[MYSQL_ERRMSG_SIZE];
		char *query = 0;
		while (iterator.MoreElements()) 
		{
			GuardSettings_Struct* gss = database.GetGuardSettings(kingdomID, iterator.GetData()->CurrentNPCID(), iterator.GetData()->SpawnGroupID());
			if ( !gss )
			{
				iterator.Advance();
				continue;
			}
			
				database.RunQuery(query, MakeAnyLenString(&query, "DELETE FROM respawn_times WHERE id=%lu" 
					" AND instance_id=%lu",(unsigned long)iterator.GetData()->GetID(), (unsigned long)zone->GetInstanceID()), errbuf);
				safe_delete_array(query);
				if ( !iterator.GetData()->isTimerEnabled() )
					iterator.GetData()->Disable(true);

				if ( team->citymode == CAMPDESTROYED )
				{
					gss->didMobSpawn = false;
					gss->guardNPCTypeID = 0;

 					database.RunQuery(query, MakeAnyLenString(&query, "DELETE FROM spawn2 WHERE id='%i'",iterator.GetData()->GetID()), errbuf);
 					safe_delete_array(query);
 					database.RunQuery(query, MakeAnyLenString(&query, "DELETE FROM spawngroup WHERE id='%i'",iterator.GetData()->SpawnGroupID()), errbuf);
 					safe_delete_array(query);
 					database.RunQuery(query, MakeAnyLenString(&query, "DELETE FROM spawnentry WHERE spawngroupID='%i'",iterator.GetData()->SpawnGroupID()), errbuf);
 					safe_delete_array(query);
					database.DeleteGuardSettings(team->teamID, iterator.GetData()->CurrentNPCID(), iterator.GetData()->SpawnGroupID());
					iterator.RemoveCurrent(true);
					
					continue;
				}
				else
					iterator.GetData()->Repop(RuleI(Kingdoms, GuardRepopTimerMS));

				iterator.Advance();
		}
}
#endif
