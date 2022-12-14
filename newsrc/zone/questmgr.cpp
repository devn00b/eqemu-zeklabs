/*  EQEMu:  Everquest Server Emulator
Copyright (C) 2001-2005  EQEMu Development Team (http://eqemulator.net)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY except by those people which sell it, which
are required to give you total support for your newly bought product;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*

Assuming you want to add a new perl quest function named joe
that takes 1 integer argument....

1. Add the prototype to the quest manager:
questmgr.h: add (~line 50)
void joe(int arg);

2. Define the actual function in questmgr.cpp:
void QuestManager::joe(int arg) {
//... do something
}

3. Copy one of the XS routines in perlparser.cpp, preferably
one with the same number of arguments as your routine. Rename
as needed.
Finally, add your routine to the list at the bottom of perlparser.cpp


4.
If you want it to work in old mode perl and .qst, edit parser.cpp
Parser::ExCommands (~line 777)
else if (!strcmp(command,"joe")) {
quest_manager.joe(atoi(arglist[0]));
}

And then at then end of embparser.cpp, add:
"sub joe{push(@cmd_queue,{func=>'joe',args=>join(',',@_)});}"



*/

#include "../common/debug.h"
#include "entity.h"
#include "masterentity.h"

#include <sstream>
#include <iostream>
#include <list>
using namespace std;

#include "worldserver.h"
#include "net.h"
#include "../common/skills.h"
#include "../common/classes.h"
#include "../common/races.h"
#include "zonedb.h"
#include "../common/files.h"
#include "spdat.h"
#include "../common/packet_functions.h"
#include "../common/MiscFunctions.h"
#include "spawn2.h"
#include "zone.h"
#include "parser.h"
#include "event_codes.h"
#include "guild_mgr.h"
#include "../common/rulesys.h"
#include "QGlobals.h"
#include "spdat.h"
#include "../KingsBandits/KingsBandits.h"

#ifdef BOTS
#include "bot.h"
#endif


extern Zone* zone;
extern WorldServer worldserver;
extern EntityList entity_list;

#ifdef EMBPERL
#include "embparser.h"
#endif

#include "questmgr.h"

//declare our global instance
QuestManager quest_manager;

QuestManager::QuestManager() {
	depop_npc = false;
	HaveProximitySays = false;
}

QuestManager::~QuestManager() {
}

void QuestManager::Process() {
	list<QuestTimer>::iterator cur = QTimerList.begin(), end, tmp;

	end = QTimerList.end();
	while (cur != end) {
		if (cur->Timer_.Enabled() && cur->Timer_.Check()) {
			//make sure the mob is still in zone.
			if(entity_list.IsMobInZone(cur->mob)){
#ifdef EMBPERL
				if(cur->mob->IsNPC()) {
					parse->Event(EVENT_TIMER, cur->mob->GetNPCTypeID(), cur->name.c_str(), cur->mob->CastToNPC(), NULL);
				}
				else{
					((PerlembParser*)parse)->Event(EVENT_TIMER, 0, cur->name.c_str(), (NPC*)NULL, cur->mob);
				}
#else
				parse->Event(EVENT_TIMER, cur->mob->GetNPCTypeID(), cur->name.c_str(), cur->mob, NULL);
#endif

				//we MUST reset our iterator since the quest could have removed/added any
				//number of timers... worst case we have to check a bunch of timers twice
				cur = QTimerList.begin();
				end = QTimerList.end();	//dunno if this is needed, cant hurt...
			} else {
				tmp = cur;
				tmp++;
				QTimerList.erase(cur);
				cur = tmp;
			}
		} else
			cur++;
	}

	list<SignalTimer>::iterator curS, endS, tmpS;

	curS = STimerList.begin();
	endS = STimerList.end();
	while (curS != endS) {
		if(!curS->Timer_.Enabled()) {
			//remove the timer
			tmpS = curS;
			tmpS++;
			STimerList.erase(curS);
			curS = tmpS;
		} else if(curS->Timer_.Check()) {
			//disable the timer so it gets deleted.
			curS->Timer_.Disable();

			//signal the event...
			entity_list.SignalMobsByNPCID(curS->npc_id, curS->signal_id);

			//restart for the same reasons as above.
			curS = STimerList.begin();
			endS = STimerList.end();
		} else
			curS++;
	}
}

void QuestManager::StartQuest(Mob *_owner, Client *_initiator, ItemInst* _questitem) {
	quest_mutex.lock();
	owner = _owner;
	initiator = _initiator;
	questitem = _questitem;
	depop_npc = false;
}

void QuestManager::EndQuest() {
	quest_mutex.unlock();

	if(depop_npc && owner->IsNPC()) {
		//clear out any timers for them...
		list<QuestTimer>::iterator cur = QTimerList.begin(), end, tmp;

		end = QTimerList.end();
		while (cur != end) {
			if(cur->mob == owner) {
				tmp = cur;
				tmp++;
				QTimerList.erase(cur);
				cur = tmp;
			} else {
				cur++;
			}
		}

		owner->Depop();
		owner = NULL;	//just to be safe
	}
}

void QuestManager::ClearAllTimers() {

	list<QuestTimer>::iterator cur = QTimerList.begin(), end, tmp;

	end = QTimerList.end();
	while (cur != end)
	{
		tmp = cur;
		tmp++;
		QTimerList.erase(cur);
		cur = tmp;
	}

	QTimerList.clear();
}

//quest perl functions
void QuestManager::echo(int colour, const char *str) {
	entity_list.MessageClose(initiator, false, 200, colour, str);
}

void QuestManager::say(const char *str) {

	if(RuleB(NPC, EnableNPCQuestJournal) && initiator)
		owner->QuestJournalledSay(initiator, str);
	else
		owner->Say(str);
}

void QuestManager::say(const char *str, int8 language) {
	entity_list.ChannelMessage(owner, 8, language, str);
}

void QuestManager::me(const char *str) {
	if (!initiator)
		return;
	entity_list.MessageClose(initiator, false, 200, 10, str);
}

void QuestManager::summonitem(int32 itemid, uint8 charges) {
	if(!initiator)
		return;

	if ( initiator->IsClient() && owner != NULL )
	{
		const Item_Struct* item = database.GetItem(itemid);

		if ( item )
		{
		string turnIn("Summon Item");
		char itemData[128];
		_snprintf(itemData,128,"%s (%i) charges: %i", item->Name,itemid, charges);
		database.logevents(initiator->CastToClient()->AccountName(),initiator->CastToClient()->AccountID(),initiator->CastToClient()->Admin(),initiator->CastToClient()->GetName(),owner->GetName(),(char*)turnIn.c_str(),itemData,22,initiator->GetX(),
		initiator->GetY(),initiator->GetZ(), (char*)database.GetZoneName(initiator->GetZoneID(), initiator->GetPP().zoneInstance, true),owner->GetX(),owner->GetY(),owner->GetZ());
		}
	}

	initiator->SummonItem(itemid, charges);
}

void QuestManager::write(const char *file, const char *str) {
	FILE * pFile;
	pFile = fopen (file, "a");
	if(!pFile)
		return;
	fprintf(pFile, "%s\n", str);
	fclose (pFile);
}

int16 QuestManager::spawn2(int npc_type, int grid, int unused, float x, float y, float z, float heading) {
	const NPCType* tmp = 0;
	if ((tmp = database.GetNPCType(npc_type)))
	{
		NPC* npc = new NPC(tmp, 0, x, y, z, heading, FlyMode3);
		npc->AddLootTable();
		entity_list.AddNPC(npc,true,true);
		// Quag: Sleep in main thread? ICK!
		// Sleep(200);
		// Quag: check is irrelevent, it's impossible for npc to be 0 here
		// (we're in main thread, nothing else can possibly modify it)
		if(grid > 0)
		{
			npc->AssignWaypoints(grid);
		}
		npc->SendPosUpdate();
		return(npc->GetID());
	}
	return(0);
}

int16 QuestManager::unique_spawn(int npc_type, int grid, int unused, float x, float y, float z, float heading) {
	Mob *other = entity_list.GetMobByNpcTypeID(npc_type);
	if(other != NULL) {
		return(other->GetID());
	}

	const NPCType* tmp = 0;
	if ((tmp = database.GetNPCType(npc_type)))
	{
		NPC* npc = new NPC(tmp, 0, x, y, z, heading, FlyMode3);
		npc->AddLootTable();
		entity_list.AddNPC(npc,true,true);
		// Quag: Sleep in main thread? ICK!
		// Sleep(200);
		// Quag: check is irrelevent, it's impossible for npc to be 0 here
		// (we're in main thread, nothing else can possibly modify it)
		if(grid > 0)
		{
			npc->AssignWaypoints(grid);
		}
		npc->SendPosUpdate();
		return(npc->GetID());
	}
	return(0);
}

int16 QuestManager::spawn_from_spawn2(int32 spawn2_id)
{
	LinkedListIterator<Spawn2*> iterator(zone->spawn2_list);
	iterator.Reset();
	Spawn2 *found_spawn = NULL;

	while(iterator.MoreElements())
	{
		Spawn2* cur = iterator.GetData();
		iterator.Advance();
		if(cur->GetID() == spawn2_id)
		{
			found_spawn = cur;
			break;
		}
	}

	if(found_spawn)
	{
		SpawnGroup* sg = zone->spawn_group_list.GetSpawnGroup(found_spawn->SpawnGroupID());
		if(!sg)
		{
			database.LoadSpawnGroupsByID(found_spawn->SpawnGroupID(),&zone->spawn_group_list);
			sg = zone->spawn_group_list.GetSpawnGroup(found_spawn->SpawnGroupID());
			if(!sg)
			{
				return 0;
			}
		}
		int32 npcid = sg->GetNPCType();
		if(npcid == 0)
		{
			return 0;
		}

		const NPCType* tmp = database.GetNPCType(npcid);
		if(!tmp)
		{
			return 0;
		}

		if(tmp->unique_spawn_by_name)
		{
			if(!entity_list.LimitCheckName(tmp->name))
			{
				return 0;
			}
		}

		if(tmp->spawn_limit > 0)
		{
			if(!entity_list.LimitCheckType(npcid, tmp->spawn_limit))
			{
				return 0;
			}
		}

		database.UpdateSpawn2Timeleft(spawn2_id, zone->GetInstanceID(), 0);
		found_spawn->SetCurrentNPCID(npcid);

		NPC* npc = new NPC(tmp, found_spawn, found_spawn->GetX(), found_spawn->GetY(), found_spawn->GetZ(),
			found_spawn->GetHeading(), FlyMode3);

		found_spawn->SetNPCPointer(npc);
		npc->AddLootTable();
		npc->SetSp2(found_spawn->SpawnGroupID());
		entity_list.AddNPC(npc);
		entity_list.LimitAddNPC(npc);

		if(sg->roamdist && sg->roambox[0] && sg->roambox[1] && sg->roambox[2] && sg->roambox[3] && sg->delay)
			npc->AI_SetRoambox(sg->roamdist,sg->roambox[0],sg->roambox[1],sg->roambox[2],sg->roambox[3],sg->delay);
		if(zone->InstantGrids())
		{
			found_spawn->LoadGrid();
		}

		return npc->GetID();
	}
	return 0;
}

void QuestManager::enable_spawn2(int32 spawn2_id)
{
	database.UpdateSpawn2Status(spawn2_id, 1);
	ServerPacket* pack = new ServerPacket(ServerOP_SpawnStatusChange, sizeof(ServerSpawnStatusChange_Struct));
	ServerSpawnStatusChange_Struct* ssc = (ServerSpawnStatusChange_Struct*) pack->pBuffer;
	ssc->id = spawn2_id;
	ssc->new_status = 1;
	worldserver.SendPacket(pack);
	safe_delete(pack);
}

void QuestManager::disable_spawn2(int32 spawn2_id)
{
	database.UpdateSpawn2Status(spawn2_id, 0);
	ServerPacket* pack = new ServerPacket(ServerOP_SpawnStatusChange, sizeof(ServerSpawnStatusChange_Struct));
	ServerSpawnStatusChange_Struct* ssc = (ServerSpawnStatusChange_Struct*) pack->pBuffer;
	ssc->id = spawn2_id;
	ssc->new_status = 0;
	worldserver.SendPacket(pack);
	safe_delete(pack);
}

void QuestManager::setstat(int stat, int value) {
	if (initiator)
		initiator->SetStats(stat, value);
}

void QuestManager::incstat(int stat, int value) { //old setstat command aza
	if (initiator)
		initiator->IncStats(stat, value);
}

void QuestManager::castspell(int spell_id, int target_id) {
	if (owner) {
		Mob *tgt = entity_list.GetMob(target_id);
		if(tgt != NULL)
			owner->SpellFinished(spell_id, tgt, 10, 0, -1, spells[spell_id].ResistDiff);
	}
}

void QuestManager::selfcast(int spell_id) {
	if (initiator)
		initiator->SpellFinished(spell_id, initiator, 10, 0, -1, spells[spell_id].ResistDiff);
}

void QuestManager::addloot(int item_id, int charges, bool equipitem) {
	if(item_id != 0){
		if(owner->IsNPC())
			owner->CastToNPC()->AddItem(item_id, charges, equipitem);
	}
}

void QuestManager::Zone(const char *zone_name) {
	if (initiator && initiator->IsClient())
	{
		ServerPacket* pack = new ServerPacket(ServerOP_ZoneToZoneRequest, sizeof(ZoneToZone_Struct));
		ZoneToZone_Struct* ztz = (ZoneToZone_Struct*) pack->pBuffer;
		ztz->response = 0;
		ztz->current_zone_id = zone->GetZoneID();
		ztz->current_instance_id = zone->GetInstanceID();
		ztz->requested_zone_id = database.GetZoneID(zone_name);
		ztz->admin = initiator->Admin();
		strcpy(ztz->name, initiator->GetName());
		ztz->guild_id = initiator->GuildID();
		ztz->ignorerestrictions = 3;
		worldserver.SendPacket(pack);
		safe_delete(pack);
	}
}

void QuestManager::settimer(const char *timer_name, int seconds) {

	list<QuestTimer>::iterator cur = QTimerList.begin(), end;

	end = QTimerList.end();
	while (cur != end) {
		if (cur->mob == owner && cur->name == timer_name) {
			cur->mob = owner;
			cur->Timer_.Enable();
			cur->Timer_.Start(seconds * 1000, false);
			//printf("Reseting: %s for %d seconds\n", cur->name.c_str(), seconds);
			return;
		}
		cur++;
	}

	/*	timers * tmp = new timers;
	tmp->mob = npc;
	tmp->Timer_ = new Timer(seconds * 1000,0);
	tmp->Timer_->Start(seconds * 1000,false);
	tmp->name = timer_name;
	printf("Adding: %s for %d seconds\n", tmp->name.c_str(), seconds);
	QTimerList.push_back(tmp);*/
	QTimerList.push_back(QuestTimer(seconds * 1000, owner, timer_name));
}

void QuestManager::stoptimer(const char *timer_name) {

	list<QuestTimer>::iterator cur = QTimerList.begin(), end;

	end = QTimerList.end();
	while (cur != end)
	{
		if(cur->mob == owner && cur->name == timer_name)
		{
			QTimerList.erase(cur);
			return;
		}
		cur++;
	}
}

void QuestManager::stopalltimers() {

	list<QuestTimer>::iterator cur = QTimerList.begin(), end, tmp;

	end = QTimerList.end();
	while (cur != end)
	{
		if(cur->mob == owner)
		{
			tmp = cur;
			tmp++;
			QTimerList.erase(cur);
			cur = tmp;
		}
		else
		{
			cur++;
		}
	}
}

void QuestManager::emote(const char *str) {
	owner->Emote(str);
}

void QuestManager::shout(const char *str) {
	owner->Shout(str);
}

void QuestManager::shout2(const char *str) {
	worldserver.SendEmoteMessage(0,0,0,13, "%s shouts, '%s'", owner->GetCleanName(), str);
}

void QuestManager::gmsay(const char *str, int32 color, bool send_to_world) {
	if(send_to_world) {
		worldserver.SendEmoteMessage(0, 0, 80, color, "%s", str);
	}
	else {
		entity_list.MessageStatus(0, 80, color, "%s", str);
	}
}

void QuestManager::depop(int npc_type) {
	if(!owner->IsNPC())
		return;
	if (npc_type != 0){
		Mob * tmp = entity_list.GetMobByNpcTypeID(npc_type);
		if (tmp) {
			if(tmp != owner){
				tmp->CastToNPC()->Depop();
			}
			else
				depop_npc = true;
		}
	}
	else {	//depop self
		depop_npc = true;
	}
}

void QuestManager::depopall(int npc_type) {
	if(owner->IsNPC() && npc_type > 0)
		entity_list.DepopAll(npc_type);
}

void QuestManager::depopzone(bool StartSpawnTimer) {
	if(zone)
		zone->Depop(StartSpawnTimer);
}

void QuestManager::repopzone() {
	if(zone)
		zone->Repop();
}

void QuestManager::settarget(const char *type, int target_id) {
	if(!owner->IsNPC())
		return;
	Mob* tmp = NULL;
	if (!strcasecmp(type,"npctype")) {
		tmp = entity_list.GetMobByNpcTypeID(target_id);
	}
	else if (!strcasecmp(type, "entity")) {
		tmp = entity_list.GetMob(target_id);
	}
	if(tmp != NULL) {
		owner->SetTarget(tmp);
	}
}

void QuestManager::follow(int entity_id, int distance) {
	if(!owner->IsNPC())
		return;
	owner->SetFollowID(entity_id);
	owner->SetFollowDistance(distance * distance);
}

void QuestManager::sfollow() {
	if(!owner->IsNPC())
		return;
	owner->SetFollowID(0);
}

void QuestManager::changedeity(int diety_id) {
	//Cofruben:-Changes the deity.
	if (initiator && initiator->IsClient())
	{
		initiator->SetDeity(diety_id);
		initiator->Message(15,"Your Deity has been changed/set to: %i", diety_id);
		initiator->Save(1);
		initiator->Kick();
	}
	else
		initiator->Message(15,"Error changing Deity");

}

void QuestManager::exp(int amt) {
	if (initiator && initiator->IsClient())
		initiator->AddEXP(amt);
}

void QuestManager::level(int newlevel) {
	if (initiator && initiator->IsClient())
		initiator->SetLevel(newlevel, true);
}

void QuestManager::traindisc(int discipline_tome_item_id) {
	if (initiator && initiator->IsClient())
		initiator->TrainDiscipline(discipline_tome_item_id);
}

bool QuestManager::isdisctome(int item_id) {
	//get the item info
	const Item_Struct *item = database.GetItem(item_id);
	if(item == NULL) {
		return(false);
	}

	if(item->ItemClass != ItemClassCommon || item->ItemType != ItemTypeSpell) {
		return(false);
	}

	//Need a way to determine the difference between a spell and a tome
	//so they cant turn in a spell and get it as a discipline
	//this is kinda a hack:
	if(!(
		item->Name[0] == 'T' &&
		item->Name[1] == 'o' &&
		item->Name[2] == 'm' &&
		item->Name[3] == 'e' &&
		item->Name[4] == ' '
		)) {
			return(false);
	}

	//we know for sure none of the int casters get disciplines
	uint32 cbit = 0;
	cbit |= 1 << (WIZARD-1);
	cbit |= 1 << (ENCHANTER-1);
	cbit |= 1 << (MAGICIAN-1);
	cbit |= 1 << (NECROMANCER-1);
	if(item->Classes & cbit) {
		return(false);
	}

	int32 spell_id = item->Scroll.Effect;
	if(!IsValidSpell(spell_id)) {
		return(false);
	}

	//we know for sure none of the int casters get disciplines
	const SPDat_Spell_Struct &spell = spells[spell_id];
	if(
		spell.classes[WIZARD - 1] != 255 &&
		spell.classes[ENCHANTER - 1] != 255 &&
		spell.classes[MAGICIAN - 1] != 255 &&
		spell.classes[NECROMANCER - 1] != 255
		) {
			return(false);
	}

	return(true);
}

void QuestManager::safemove() {
	if (initiator && initiator->IsClient())
		initiator->GoToSafeCoords(zone->GetZoneID(), 0);
}

void QuestManager::rain(int weather) {
	zone->zone_weather = weather;
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_Weather, 8);
	*((int32*) &outapp->pBuffer[4]) = (int32) weather; // Why not just use 0x01/2/3?
	entity_list.QueueClients(owner, outapp);
	safe_delete(outapp);
}

void QuestManager::snow(int weather) {
	zone->zone_weather = weather + 1;
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_Weather, 8);
	outapp->pBuffer[0] = 0x01;
	*((int32*) &outapp->pBuffer[4]) = (int32)weather;
	entity_list.QueueClients(initiator, outapp);
	safe_delete(outapp);
}

void QuestManager::surname(const char *name) {
	//Cofruben:-Changes the last name.
	if (initiator && initiator->IsClient())
	{
		initiator->ChangeLastName(name);
		initiator->Message(15,"Your surname has been changed/set to: %s", name);
	}
	else
		initiator->Message(15,"Error changing/setting surname");
}

void QuestManager::permaclass(int class_id) {
	//Cofruben:-Makes the client the class specified
	initiator->SetBaseClass(class_id);
	initiator->Save(2);
	initiator->Kick();
}

void QuestManager::permarace(int race_id) {
	//Cofruben:-Makes the client the race specified
	initiator->SetBaseRace(race_id);
	initiator->Save(2);
	initiator->Kick();
}

void QuestManager::permagender(int gender_id) {
	//Cofruben:-Makes the client the gender specified
	initiator->SetBaseGender(gender_id);
	initiator->Save(2);
	initiator->Kick();
}

uint16 QuestManager::scribespells(uint8 max_level, uint8 min_level) {
	uint16 book_slot, count;
	int16 curspell;

	int16 Char_ID = initiator->CharacterID();
	bool SpellGlobalRule = RuleB(Spells, EnableSpellGlobals);
	bool SpellGlobalCheckResult = 0;


	for(curspell = 0, book_slot = initiator->GetNextAvailableSpellBookSlot(), count = 0; curspell < SPDAT_RECORDS && book_slot < MAX_PP_SPELLBOOK; curspell++, book_slot = initiator->GetNextAvailableSpellBookSlot(book_slot))
	{
		if
			(
			spells[curspell].classes[WARRIOR] != 0 &&       //check if spell exists
			spells[curspell].classes[initiator->GetPP().class_-1] <= max_level &&   //maximum level
			spells[curspell].classes[initiator->GetPP().class_-1] >= min_level &&   //minimum level
			spells[curspell].skill != 52
			)
		{
			if (book_slot == -1)    //no more book slots
				break;
			if(!IsDiscipline(curspell) && !initiator->HasSpellScribed(curspell)) {  //isn't a discipline & we don't already have it scribed
				if (SpellGlobalRule) {
					// Bool to see if the character has the required QGlobal to scribe it if one exists in the Spell_Globals table
					SpellGlobalCheckResult = initiator->SpellGlobalCheck(curspell, Char_ID);
					if (SpellGlobalCheckResult) {
						initiator->ScribeSpell(curspell, book_slot);
						count++;
					}
				}
				else {
					initiator->ScribeSpell(curspell, book_slot);
					count++;
				}
			}
		}
	}
	return count;   //how many spells were scribed successfully
}

uint16 QuestManager::traindiscs(uint8 max_level, uint8 min_level) {
	uint16 count;
	int16 curspell;

	int16 Char_ID = initiator->CharacterID();
	bool SpellGlobalRule = RuleB(Spells, EnableSpellGlobals);
	bool SpellGlobalCheckResult = 0;

	for(curspell = 0, count = 0; curspell < SPDAT_RECORDS; curspell++)
	{
		if
			(
			spells[curspell].classes[WARRIOR] != 0 &&	//check if spell exists
			spells[curspell].classes[initiator->GetPP().class_-1] <= max_level &&	//maximum level
			spells[curspell].classes[initiator->GetPP().class_-1] >= min_level &&	//minimum level
			spells[curspell].skill != 52
			)
		{
			if(IsDiscipline(curspell)){
				//we may want to come up with a function like Client::GetNextAvailableSpellBookSlot() to help speed this up a little
				for(int r = 0; r < MAX_PP_DISCIPLINES; r++) {
					if(initiator->GetPP().disciplines.values[r] == curspell) {
						//initiator->Message(13, "You already know this discipline.");
						break;	//continue the 1st loop
					}
					else if(initiator->GetPP().disciplines.values[r] == 0) {
						if (SpellGlobalRule) {
							// Bool to see if the character has the required QGlobal to train it if one exists in the Spell_Globals table
							SpellGlobalCheckResult = initiator->SpellGlobalCheck(curspell, Char_ID);
							if (SpellGlobalCheckResult) {
								initiator->GetPP().disciplines.values[r] = curspell;
								initiator->SendDisciplineUpdate();
								initiator->Message(0, "You have learned a new discipline!");
								count++;	//success counter
							}
							break;	//continue the 1st loop
						}
						else {
							initiator->GetPP().disciplines.values[r] = curspell;
							initiator->SendDisciplineUpdate();
							initiator->Message(0, "You have learned a new discipline!");
							count++;	//success counter
							break;	//continue the 1st loop
						}
					}	//if we get to this point, there's already a discipline in this slot, so we skip it
				}
			}
		}
	}
							
	if ( count > 0 )
		initiator->Message(0, "Total of %i disciplines were added.", count);
	else
		initiator->Message(0, "No disciplines to add.");

	return count;	//how many disciplines were learned successfully
}

void QuestManager::unscribespells() {
	initiator->UnscribeSpellAll();
}

void QuestManager::untraindiscs() {
	initiator->UntrainDiscAll();
}

void QuestManager::givecash(int copper, int silver, int gold, int platinum, bool convertCopper) {
	if (initiator && initiator->IsClient() && ((copper + silver + gold + platinum) > 0))
	{
		if ( convertCopper )
			initiator->AddMoneyToPP(copper,true);
		else
			initiator->AddMoneyToPP(copper, silver, gold, platinum, true);

		string tmp;
		if (platinum > 0)
		{
			tmp = "You receive ";
			tmp += itoa(platinum);
			tmp += " platinum";
		}
		if (gold > 0)
		{
			if (tmp.length() == 0)
				tmp = "You receive ";
			else
				tmp += ",";

			tmp += itoa(gold);
			tmp += " gold";
		}
		if(silver > 0)
		{
			if (tmp.length() == 0)
				tmp = "You receive ";
			else
				tmp += ",";

			tmp += itoa(silver);
			tmp += " silver";
		}
		if(copper > 0)
		{
			if (tmp.length() == 0)
				tmp = "You receive ";
			else
				tmp += ",";

			tmp += itoa(copper);
			tmp += " copper";
		}
		tmp += " pieces.";

		if ( convertCopper )
			tmp += "  This merchant has converted the copper into silver, gold or platinum if necessary.";

		if (initiator)
			initiator->Message(MT_OOC, tmp.c_str());
	}
}

void QuestManager::pvp(const char *mode) {
	if (!strcasecmp(mode,"on"))
	{
		if (initiator)
			initiator->SetPVP(true);
	}
	else
		if (initiator)
			initiator->SetPVP(false);
}

void QuestManager::pvpvalue(const char *mode) {
	if (initiator)
		initiator->SetPVP((uint8)atoul(mode));
}

void QuestManager::summonlootgroup(int groupid) {
	if(initiator == NULL)
		Perl_croak(aTHX_ "THIS is NULL, avoiding crash.");

	if ( groupid < 1 )
		Perl_croak(aTHX_ "Bad group id for SummonLootGroup!");

	RandomLootDropResult_Struct rlds;
	memset(&rlds,0,sizeof(RandomLootDropResult_Struct));
	if ( database.GenerateRandomLootGroupResult(&rlds, groupid) )
	{
		for(int i=0;i<rlds.itemDropCount;i++)
		{
			uint32 inItemID = rlds.items[i].itemID;
			if ( inItemID > 0 )
				initiator->SummonItem(inItemID);
		}

		initiator->AddMoneyToPP(rlds.coinDropValue,true);
	}
	else
		Perl_croak(aTHX_ "Failed to generate RandomLootGroupResult Struct in QuestManager::summonlootgroup.");
}

void QuestManager::movepc(int zone_id, float x, float y, float z, float heading) {
	if (initiator && initiator->IsClient())
		initiator->MovePC(zone_id, x, y, z, heading);
}

void QuestManager::gmmove(float x, float y, float z) {
	if (initiator && initiator->IsClient())
		initiator->GMMove(x, y, z);
}

void QuestManager::movegrp(int zoneid, float x, float y, float z) {
#ifdef IPC
	if (initiator && initiator->IsClient()|| (initiator->IsNPC() && initiator->CastToNPC()->IsInteractive()) )
#else
	if (initiator && initiator->IsClient())
#endif
	{
		Group *g = entity_list.GetGroupByClient(initiator);
		if (g != NULL){
			g->TeleportGroup(owner, zoneid, 0, x, y, z, 0.0f);
		} else {
			Raid *r = entity_list.GetRaidByClient(initiator);
			if (r != NULL){
				int32 gid = r->GetGroup(initiator);
				if (gid >= 0 && gid < 12) {
					r->TeleportGroup(owner, zoneid, 0, x, y, z, 0.0f, gid);
				} else {
					initiator->MovePC(zoneid, x, y, z, 0.0f);
				}
			} else {
				initiator->MovePC(zoneid, x, y, z, 0.0f);
			}
		}
	}
}

void QuestManager::doanim(int anim_id) {
	owner->DoAnim(anim_id);
}

void QuestManager::addskill(int skill_id, int value) {
	if(skill_id < 0 || skill_id > HIGHEST_SKILL)	//must check before casting.
		return;
	if (initiator && initiator->IsClient())
		initiator->AddSkill((SkillType) skill_id, value);
}

void QuestManager::setlanguage(int skill_id, int value) {
	if (initiator && initiator->IsClient())
		initiator->SetLanguageSkill(skill_id, value);
}

void QuestManager::setskill(int skill_id, int value) {
	if(skill_id < 0 || skill_id > HIGHEST_SKILL)	//must check before casting.
		return;
	if (initiator && initiator->IsClient())
		initiator->SetSkill((SkillType) skill_id, value);
}

void QuestManager::setallskill(int value) {
	if (!initiator)
		return;
	if (initiator && initiator->IsClient()) {
		SkillType sk;
		for (sk = _1H_BLUNT; sk <= HIGHEST_SKILL; sk = (SkillType)(sk+1)) {
			int16 max = initiator->CastToClient()->GetMaxSkillAfterSpecializationRules(sk, initiator->CastToClient()->MaxSkill(sk));
			int16 actualValue = (int16)value;
			if ( actualValue > max )
				actualValue = max;
			initiator->SetSkill(sk, actualValue);
		}
	}
}

void QuestManager::attack(const char *client_name) {
	if(!owner->IsNPC())
		return;
	Client* getclient = entity_list.GetClientByName(client_name);
	if(getclient && owner->IsAttackAllowed(getclient)) {
		owner->AddToHateList(getclient,1,0,true,false,false,true);
	} else {
		owner->Say("I am unable to attack %s.", client_name);
	}
}

void QuestManager::attacknpc(int npc_entity_id) {
	if(!owner->IsNPC())
		return;
	Mob *it = entity_list.GetMob(npc_entity_id);
	if(it && owner->IsAttackAllowed(it)) {
		owner->AddToHateList(it,1,0,true,false,false,true);
	} else {
		if(it)
			owner->Say("I am unable to attack %s.", it->GetName());
		else
			owner->Say("I am unable to locate NPC entity %i", npc_entity_id);
	}
}

void QuestManager::attacknpctype(int npc_type_id) {
	if(!owner->IsNPC())
		return;
	Mob *it = entity_list.GetMobByNpcTypeID(npc_type_id);
	if(it && owner->IsAttackAllowed(it)) {
		owner->AddToHateList(it,1,0,true,false,false,true);
	} else {
		if(it)
			owner->Say("I am unable to attack %s.", it->GetName());
		else
			owner->Say("I am unable to locate NPC type %i", npc_type_id);
	}
}

// Kings & Bandits facenpctype addition
void QuestManager::facenpctype(int npc_type_id) {
	if(!owner->IsNPC())
		return;
	Mob *it = entity_list.GetMobByNpcTypeID(npc_type_id);
	if(it) {
		owner->FaceTarget(it);
	}
}

// Kings & Bandits kingdom additions
void QuestManager::setcitycitizentaxrate(uint32 kingdomid, uint8 taxratepct) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	// set value for citizen tax rate and return
	if ( settings )
	{
		settings->citizenTaxRate = taxratepct;

		char query[256];
		snprintf(query,256,"UPDATE team_settings set citizentaxrate=%i where teamid=%i",settings->citizenTaxRate,settings->teamID);
		database.UpdateDatabaseQuery(query);
	}

	// set tax rate percent /100 in memshare for kingdomid
}

void QuestManager::setcitynewbietaxrate(uint32 kingdomid, uint8 taxratepct) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	// set value for newbie tax rate and return
	if ( settings )
	{
		settings->newbieTaxRate = taxratepct;

		char query[256];
		snprintf(query,256,"UPDATE team_settings set newbietaxrate=%i where teamid=%i",settings->newbieTaxRate,settings->teamID);
		database.UpdateDatabaseQuery(query);
	}

	// set tax rate percent /100 in memshare for kingdomid
}

uint8 QuestManager::getcitycitizentaxrate(uint32 kingdomid) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	// get value for citizen tax rate and return
	if ( settings )
		return settings->citizenTaxRate;

	// get tax rate percent in memshare for kingdom id
	return 0;
}

uint8 QuestManager::getcitynewbietaxrate(uint32 kingdomid) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	// get value for newbie tax rate and return
	if ( settings )
		return settings->newbieTaxRate;

	return 0;
}

void QuestManager::setcitybankreserve(uint32 kingdomid, uint32 value) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	if ( settings )
	{
		settings->bankcopper = value;

		char query[256];
		snprintf(query,256,"UPDATE team_settings set bankcopper=%i,banksilver=%i,bankgold=%i,bankplatinum=%i where teamid=%i",settings->bankcopper,settings->banksilver,settings->bankgold,settings->bankplatinum,settings->teamID);
		database.UpdateDatabaseQuery(query);
	}

	// set value into bank reserve and save
}

uint32 QuestManager::getcitybankreserve(uint32 kingdomid) {

	if(kingdomid < 1)
		return 0;


	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	// get value into bank reserve and return
	if ( settings )
		return settings->bankcopper;

	return 0;
}

uint32 QuestManager::getcityupkeepcost(uint32 kingdomid) {

	if(kingdomid < 1)
		return 0;

	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	// get reserve upkeep cost and return
	if ( settings )
		return settings->upkeepRate;

	return 0;
}


uint8 QuestManager::getteamexpbonus(uint32 kingdomid) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	// get value for citizen tax rate and return
	if ( settings )
		return settings->teamXPBonus;

	// get tax rate percent in memshare for kingdom id
	return 0;
}

uint8 QuestManager::setteamexpbonus(uint32 kingdomid, uint8 value) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	// get value for citizen tax rate and return
	if ( settings )
	{
		settings->teamXPBonus = value;
		return 1;
	}

	return 0;
}

uint32 QuestManager::findcampteamid(uint32 guildid) {
	for (uint32 i=1;i<MAXKINGDOMS;i++)
	{
		TeamSettings_Struct* team = (TeamSettings_Struct*)database.GetTeamSettings(i);
		if ( team == NULL )
			break;
		
		//printf("TeamID: %i, GuildID: %i, CityMode: %i\n",i,team->guildid,team->citymode);
		if ( team->guildid == guildid && (team->citymode == CAMPCONSTRUCTED || team->citymode == CAMPDESTROYED ) )
			return team->teamID;
	}
	return 0;
}

uint32 QuestManager::findguardteamid(uint32 npctypeid, uint32 spawngroupid) {
	int32 teamid = database.GuardTeamFlag(npctypeid, spawngroupid);
	return teamid;
}

uint8 QuestManager::addguardtype(uint32 kingdomid, uint32 npcid, uint16 classid, uint32 spawngroupid) {
	GuardSettings_Struct* settings = database.GetGuardSettings(kingdomid,npcid, spawngroupid);
	// get value for newbie tax rate and return
	if ( !settings )
	{
		database.AddGuardSettings(kingdomid, npcid, classid, 0, 0, 0, 0, 0, 0, spawngroupid);
		return 1;
	}
	else
	{
		settings->spawngroupid = spawngroupid;
		settings->classID = classid;
		database.UpdateGuardSettings(kingdomid, npcid, classid, 0, 0, 0, 0, 0, 0, spawngroupid);
		return 1;
	}

	return 0;
}

uint8 QuestManager::loadguardtype(uint32 kingdomid, uint32 npcid) {
	owner->CastToNPC()->teamID = kingdomid;
	owner->CastToNPC()->npcID = npcid;

	TeamSettings_Struct* team = database.GetTeamSettings(kingdomid);
	if ( team == NULL || team->citymode == CITYDISABLED )
	{
		owner->CastToNPC()->teamID = 0;
		owner->CastToNPC()->npcID = 0;
		return 1;
	}
	else if ( team != NULL && team->guildid > 0 && strlen(owner->GetLastName()) < 1 )
	{
		const char* guildName = guild_mgr.GetGuildName(team->guildid);
			if ( guildName != NULL && strlen(guildName) > 0 )
				owner->SetLastName(guildName);
	}
	else
		owner->SetLastName(KingsBandits::GetKingdomName(kingdomid,false,false,true));

	GuardSettings_Struct* settings = database.GetGuardSettings(kingdomid,npcid,owner->CastToNPC()->GetSp2());
	// get value for newbie tax rate and return
	if ( !settings )
	{
		database.AddGuardSettings(kingdomid, npcid, owner->GetClass(), 0, 0, 0, 0, 0, 0, owner->CastToNPC()->GetSp2());
	}
	else
	{
		settings->lastkilledcharid = 0;
		settings->lastkilledguildid = 0;
		settings->lastkilledtimestamp = 0;
		settings->lastkilledteamid = 0;
		settings->lastkilledlevel = 0;
		settings->didMobSpawn = true;

		// HP and Mana regen rates are not set in the Mob class
		owner->SetMobMaxHP(owner->GetMaxHP() + settings->hpBonus);

		sint32 newValue = owner->GetMobBaseMana() + (sint32)settings->manaBonus;
		if ( newValue < 0 )
			newValue = 0;

		owner->SetMobBaseMana((uint32)newValue);

		newValue = owner->GetMaxMana() + (sint32)settings->manaBonus;
		if ( newValue < 0 )
			newValue = 0;
		
		owner->SetMobMaxMana((uint32)newValue);
		owner->SetBaseAC(owner->GetBaseAC() + settings->AC);
		owner->SetLevel(owner->GetLevel() + settings->level,true);
		// There is no class override...
							
		// update npc with all regen stats
		owner->CastToNPC()->CalcNPCRegen((sint16)settings->hpRegenBonus,(sint16)settings->manaRegenBonus,true);

		owner->CastToNPC()->isGuildNPC = true;
		
		char errbuf[MYSQL_ERRMSG_SIZE];
		char *query = 0;
		MYSQL_RES *result;
		MYSQL_ROW row;

		if(database.RunQuery(query,MakeAnyLenString(&query,"SELECT itemid,charges FROM team_guard_items where npcid=%i and teamid=%i and zoneid=%i and (spawngroupid=0 or spawngroupid=%i)",npcid,kingdomid,zone->GetZoneID(),owner->CastToNPC()->GetSp2()),errbuf,&result))
		{
			while((row = mysql_fetch_row(result)))
			{
				int32 itemID = atoi(row[0]);
				sint16 charges = atoi(row[1]);
				owner->CastToNPC()->AddItem(itemID,(sint16)charges);
			}
			mysql_free_result(result);
		}
		safe_delete_array(query);

		owner->CastToNPC()->ReloadSpellList();
	}

	return 1;
}

uint32 QuestManager::addteamid(uint32 guildid, uint32 ownerteamid, uint32 charid, float x, float y, float z, float radius, uint32 zoneid) {
	int32 teamid = findcampteamid(guildid);
	if ( teamid != 0 )
	{
		return teamid;
	}
	else
	{
		teamid = database.AddTeam(guildid,charid,CAMPDESTROYED,ownerteamid,x,y,z,radius,zoneid);
		char* query = NULL;
		database.RunQuery(query,MakeAnyLenString(&query,"INSERT INTO team_settings set teamid=%i,guildid=%i,ownerteamid=%i,x=%f,y=%f,z=%f,radius=%f,zoneid=%i,charid=%i,citymode=%i", teamid, guildid, ownerteamid, x, y, z, radius, zoneid, charid,CAMPDESTROYED));
		safe_delete_array(query);
	}
	return teamid;
}

uint32 QuestManager::updateteamid(uint32 guildid, uint32 ownerteamid, uint32 charid, float x, float y, float z, float radius, uint32 zoneid) {
	int32 teamid = findcampteamid(guildid);
	if ( teamid != 0 )
	{
		TeamSettings_Struct* team = database.GetTeamSettings(teamid);
		if ( team )
		{
			team->ownerteamid = ownerteamid;
			team->charid = charid;
			team->x = x;
			team->y = y;
			team->z = z;
			team->radius = radius;
			team->zoneid = zoneid;
			char* query = NULL;
			database.RunQuery(query,MakeAnyLenString(&query,"UPDATE team_settings set ownerteamid=%i,x=%f,y=%f,z=%f,radius=%f,zoneid=%i,charid=%i,citymode=%i where teamid=%i", ownerteamid, x, y, z, radius, zoneid, charid,team->citymode,teamid));
			safe_delete_array(query);
		}
		return teamid;
	}

	return 0;
}

uint32 QuestManager::getguardfield(uint32 kingdomid, uint32 npcid, const char* fieldname) {
	
	TeamSettings_Struct* team = database.GetTeamSettings(kingdomid);
	if ( team != NULL )
	{
		if ( !strcasecmp(fieldname,"guildid") ) // guild id owner of city
		{
			return team->guildid;
		}
		else if ( !strcasecmp(fieldname,"charid") ) // owner of city
		{
			return team->charid;
		}
		else if ( !strcasecmp(fieldname,"ownerteamid") )
		{
			return team->ownerteamid;
		}
	}

	GuardSettings_Struct* settings = database.GetGuardSettings(kingdomid,npcid,owner->CastToNPC()->GetSp2());
	// get value for newbie tax rate and return
	if ( !settings )
		return 0;
	else
	{
		// this is ugly but hopefully this is only used when we need it in quests
		if ( !strcasecmp(fieldname,"hp") )
			return settings->hpBonus;
		else if ( !strcasecmp(fieldname,"mana") )
			return settings->manaBonus;
		else if ( !strcasecmp(fieldname,"ac") )
			return settings->AC;
		else if ( !strcasecmp(fieldname,"hpregen") )
			return settings->hpRegenBonus;
		else if ( !strcasecmp(fieldname,"manaregen") )
			return settings->manaRegenBonus;
		else if ( !strcasecmp(fieldname,"level") )
			return settings->level;
		else if ( !strcasecmp(fieldname,"teamid") )
			return settings->teamID;
	}

	return 0;
}


uint32 QuestManager::resetguardfields(uint32 kingdomid, uint32 npcid, int16 entityid) {
	GuardSettings_Struct* settings = database.GetGuardSettings(kingdomid,npcid,owner->CastToNPC()->GetSp2());
	TeamSettings_Struct* team = database.GetTeamSettings(kingdomid);
	// nothing here don't do anything
	if ( !settings || !team )
		return 0;
	else
	{
		// it exists so lets update it
		settings->AC = 0;
		settings->classID = 1;
		settings->hpBonus = 0;
		settings->manaBonus = 0;
		settings->hpRegenBonus = 0;
		settings->manaRegenBonus = 0;
		settings->level = 0;
		settings->lastkilledtimestamp = Timer::GetCurrentTime();
		settings->didMobSpawn = false;
		Mob* mob = entity_list.GetMob(entityid);
		if ( mob != NULL )
		{
			if ( mob->IsClient() && mob->CastToClient()->GuildID() > 0 )
			{
				settings->lastkilledcharid = mob->CastToClient()->CharacterID();
				settings->lastkilledguildid = mob->CastToClient()->GuildID();
				settings->lastkilledteamid = mob->CastToClient()->GetPVPValue();
				settings->lastkilledlevel = mob->CastToClient()->GetLevel();
			}
			else if ( mob->IsNPC() && mob->GetOwner() != NULL && mob->GetOwner()->IsClient() )
			{
				settings->lastkilledcharid = mob->GetOwner()->CastToClient()->CharacterID();
				settings->lastkilledguildid = mob->GetOwner()->CastToClient()->GuildID();
				settings->lastkilledteamid = mob->GetOwner()->CastToClient()->GetPVPValue();
				settings->lastkilledlevel = mob->GetOwner()->CastToClient()->GetLevel();
			}
			else
			{
				settings->lastkilledcharid = 0;
				settings->lastkilledguildid = 0;
				settings->lastkilledteamid = 0;
				settings->lastkilledlevel = 0;
			}
			if ( mob->IsClient() )
			{
				Client* client = mob->CastToClient();

				NPC* origMob = entity_list.GetNPCByNPCTypeID(npcid);
				if ( origMob != NULL )
				{
					if ( team->guildid > 0 )
					worldserver.SendEmoteMessage("",team->guildid,0,13,"%s (%i) has been killed by %s (City: %s).",origMob->GetCleanName(),npcid,client->GetName(),
					KingsBandits::GetKingdomName(kingdomid,false,true,true));

					database.logdatabase(client->AccountName(),client->AccountID(),0,client->GetName(),"Death","GuardStatus",LOG_GUARDSTATUS,(char*)zone->GetShortName(),"%s (%i) has been killed by %s (City: %s).",origMob->GetCleanName(),npcid,client->GetName(),
					KingsBandits::GetKingdomName(kingdomid,false,true,true));
				}
			}
			else if ( mob->IsNPC() )
			{
				NPC* other = mob->CastToNPC();

				NPC* origMob = entity_list.GetNPCByNPCTypeID(npcid);
				if ( origMob != NULL )
				{
					if ( team->guildid > 0 )
					worldserver.SendEmoteMessage("",team->guildid,0,13,"%s (%i) has been killed by %s (City: %s).",origMob->GetCleanName(),npcid,other->GetCleanName(),
					KingsBandits::GetKingdomName(kingdomid,false,true,true));

					database.logdatabase("",0,0,"","Death","GuardStatus",LOG_GUARDSTATUS,(char*)zone->GetShortName(),"%s (%i) has been killed by %s (City: %s).",origMob->GetCleanName(),npcid,other->GetCleanName(),
					KingsBandits::GetKingdomName(kingdomid,false,true,true));
				}
			}
		}

		if ( RuleB(PVPCombat,TakeLocationPermitted) )
		{
			// check if we should be allowing a takeover of the city
			int32 charID = 0;
			int32 guildID = 0;
			int32 teamID = 0;
			int32 charLevel = 0;
			bool sacked = KingsBandits::IsCitySacked(kingdomid, charID, guildID, teamID, charLevel);

			if ( RuleB(PVPCombat, TakeCitiesDisabled) && KingsBandits::isCityLocation(kingdomid) )
				sacked = false;

			if ( !sacked && team->citymode != CITYDISABLED && team->citymode != CITYENABLED )
			{
				uint32 guardsAlive = 0;
				uint32 guardsInZone = 0;
				for(int i=0;i<team->totalGuardsCount;i++)
				{
					if ( team->TeamGuards[i].didMobSpawn )
					{
						NPC* npc = entity_list.GetNPCByNPCTypeID(team->TeamGuards[i].guardNPCTypeID);
						if ( npc )
							guardsInZone++;

						guardsAlive++;
					}
				}

				bool notTeamGuildwars = !RuleB(PVPCombat,TeamGuildWars);
				if ( kingdomid >= QEYNOSTEAMID && KingsBandits::isCityLocation(kingdomid) )
					entity_list.Message(0,13,"%s has %i guards standing, %i remaining in %s.",KingsBandits::GetKingdomName(kingdomid,false,false,notTeamGuildwars),guardsAlive,guardsInZone,zone->GetLongName());
				else
					entity_list.Message(0,13,"%s has %i guards standing, %i remaining in the encampment.",KingsBandits::GetKingdomName(kingdomid,false,false,notTeamGuildwars),guardsAlive,guardsInZone);

			}
			else if ( sacked && charID > 0 && guildID > 0 )
			{
				database.UpdateGuardSettings(settings->teamID,npcid,settings->classID,settings->hpBonus,
					settings->manaBonus,settings->hpRegenBonus,settings->manaRegenBonus,settings->AC,settings->level, owner->CastToNPC()->GetSp2());

				database.DeleteGuardItems(settings->teamID,npcid,zone->GetZoneID(),owner->CastToNPC()->GetSp2());

				database.DeleteGuardSpellList(settings->teamID,npcid, owner->CastToNPC()->GetSp2());
				
				database.DeleteGuardSpellOmitList(settings->teamID,npcid, owner->CastToNPC()->GetSp2());
				if ( setkingdomowner(kingdomid, guildID, charID, teamID, charLevel) )
				{
					return 1;
				}
				return 0;
			}
		}

		database.UpdateGuardSettings(settings->teamID,npcid,settings->classID,settings->hpBonus,
			settings->manaBonus,settings->hpRegenBonus,settings->manaRegenBonus,settings->AC,settings->level, owner->CastToNPC()->GetSp2());

		database.DeleteGuardItems(settings->teamID,npcid,zone->GetZoneID(),owner->CastToNPC()->GetSp2());

		database.DeleteGuardSpellList(settings->teamID,npcid, owner->CastToNPC()->GetSp2());
		
		database.DeleteGuardSpellOmitList(settings->teamID,npcid, owner->CastToNPC()->GetSp2());
	}

	return 0;
}

uint32 QuestManager::spawninnpc(uint32 kingdomid, char* spawnName, uint32 npc_type_id, float x, float y, float z, float heading, int respawntime) {
	int32 tmp = 0;
	int32 spawngroupid = 0;
	char errbuf[MYSQL_ERRMSG_SIZE];
	char tmpstr[64];
	char *query = 0;

	snprintf(tmpstr, 64, "%s-%s", zone->GetShortName(), spawnName);
	if (!database.RunQuery(query, MakeAnyLenString(&query, "INSERT INTO spawngroup (id, name) values(%i, '%s')", tmp, tmpstr), errbuf, 0, 0, &spawngroupid)) {
		safe_delete(query);
		return 0;
	}
	safe_delete_array(query);
	if (!database.RunQuery(query, MakeAnyLenString(&query, "INSERT INTO spawn2 (zone, version, x, y, z, respawntime, heading, spawngroupID) values('%s', %u, %f, %f, %f, %i, %f, %i)", zone->GetShortName(), zone->GetInstanceVersion(), x, y, z, respawntime, heading, spawngroupid), errbuf, 0, 0, &tmp)) {
		safe_delete(query);
		return 0;
	}
	safe_delete_array(query);
	if (!database.RunQuery(query, MakeAnyLenString(&query, "INSERT INTO spawnentry (spawngroupID, npcID, chance) values(%i, %i, %i)", spawngroupid, npc_type_id, 100), errbuf, 0)) {
		safe_delete(query);
		return 0;
	}

	database.AddGuardSettings(kingdomid, npc_type_id, 1, 0, 0, 0, 0, 0, 0, spawngroupid);

	database.LoadSpawn2(zone->spawn2_list, tmp, 0);
	
	safe_delete_array(query);
	return spawngroupid;
}

uint8 QuestManager::getisupkeeppaid(uint32 kingdomid) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	// get value for citizen tax rate and return
	if ( settings )
		return (uint8)settings->upkeepPaid;

	// get tax rate percent in memshare for kingdom id
	return 0;
}


uint8 QuestManager::zonepointnearby(float x, float y, float z, float maxdist) {
	ZonePoint* zone_point = zone->GetClosestZonePointWithoutZone(x,y,z, initiator, maxdist);
	if(zone_point) {
		return 1;
	}
	return 0;
}

uint8 QuestManager::findnpcnear(float x, float y, float z, float maxdist) {
	NPC* npc = entity_list.FindNearbyNPC(x,y,z,maxdist,owner->IsNPC() ? owner->CastToNPC() : NULL);
	if(npc) {
		return 1;
	}
	return 0;
}

bool QuestManager::setkingdomowner(uint32 kingdomid, int32 guildid, int32 charid, int32 teamID, int32 charLevel) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	bool wasAlreadyDestroyed = false;
	bool canTakeoverLocation = true;
	
	if ( charLevel > 0 && KingsBandits::isCityLocation(kingdomid) && charLevel <= RuleI(PVPCombat,MinLevelTakeCity))
		canTakeoverLocation = false;
	else if ( guildid == 0 || guildid == GUILD_NONE )
		canTakeoverLocation = false;
	
	if ( settings )
	{
		if ( guildid == settings->guildid )
			canTakeoverLocation = false;
		
	if ( guildid == 0 && charid == 0 && teamID == 0 )
		canTakeoverLocation = true; // reset
	
		// used to track rejeuvinating a camp
		if ( settings->citymode == CAMPDESTROYED )
		{
			wasAlreadyDestroyed = true;
			canTakeoverLocation = true;
		}

		int32 prevCharID = settings->charid;
		if ( canTakeoverLocation )
		{
			if ( prevCharID != 0 )
			{
				Client* client = entity_list.GetClientByCharID ( prevCharID );
				if ( client != NULL && client->GetInv().cityBankCoinLoaded )
				{
					client->GetPP().copper_bank = settings->placeholdercopper;
					client->GetPP().silver_bank = settings->placeholdersilver;
					client->GetPP().gold_bank = settings->placeholdergold;
					client->GetPP().platinum_bank = settings->placeholderplatinum;

					// reload player to their original coin bank and not the cities
					KingsBandits::ReloadPlayerBank(client);

					client->Save();
				
					settings->placeholdercopper = 0;
					settings->placeholdersilver = 0;
					settings->placeholdergold = 0;
					settings->placeholderplatinum = 0;
				}
				
				if ( client != NULL )
				{
					if ( client->IsBountySet() )
					{
						client->SetBounty(false);
						client->UpdateInfamyLevel(-999999);
						client->Message(13,"[BOUNTY] Bounty was cancelled.");
					}
				}
				else
				{
					char leadername[64];
					database.GetCharName(prevCharID, leadername);
					if ( strnlen(leadername,sizeof(leadername)) > 0 )
					{
						worldserver.SendCommandRequest(UPDATE_CLIENT_SETBOUNTY,0,leadername);
						char query[256];
						_snprintf(query,256,"UPDATE characterdb_stats set bountyset=0,infamy=0 where charid=%i",prevCharID);
						database.UpdateDatabaseQuery(query);
					}
				}

				if ( client != NULL && client->GetInv().cityBankLoaded )
				{
					// evac player to reset
					client->CastSpell(1628, client->GetID(),0 , 0, 0);
				}
				else if ( settings->charid > 0 )
					worldserver.SendCommandRequest(UPDATE_CLIENT_LOSTLEADERSHIP, settings->charid, "");
			}
		}

		const char* prevGuildName = NULL;
		if ( settings->guildid != 0 )
			prevGuildName = guild_mgr.GetGuildName(settings->guildid);

		if ( canTakeoverLocation && settings->citymode != CAMPCONSTRUCTED )
		{
			char query[256];

			settings->guildid = guildid;
			settings->charid = charid;
			settings->ownerteamid = teamID;
			Client* curClient = entity_list.GetClientByCharID ( charid );
			if ( curClient != NULL )
			{
				settings->x = curClient->GetX();
				settings->y = curClient->GetY();
				settings->z = curClient->GetZ();
				settings->zoneid = zone->GetZoneID();
				snprintf(query,256,"UPDATE team_settings set x=%f,y=%f,z=%f,zoneid=%i where teamid=%i",settings->x,settings->y,settings->z,settings->zoneid,settings->teamID);
				database.UpdateDatabaseQuery(query);
			}
			snprintf(query,256,"UPDATE team_settings set guildid=%i, charid=%i, ownerteamid=%i where teamid=%i",settings->guildid,settings->charid,settings->ownerteamid,settings->teamID);
			database.UpdateDatabaseQuery(query);
		}
		else if ( settings->citymode == CAMPCONSTRUCTED )
		{
			setcitymode(settings->teamID,CAMPDESTROYED,true);
		}
		
		// start of big if block
		if ( ( charid == 0 || guildid == 0 ) && settings->citymode != CAMPDESTROYED )
		{
			if ( prevGuildName != NULL )
				database.logdatabase("",0,0,"","Reset","KingdomStatus",LOG_KINGDOMSTATUS,(char*)zone->GetShortName(),"City leadership has been reset for %s by %s.",KingsBandits::GetKingdomName(kingdomid,false,true,true),prevGuildName);
			else
				database.logdatabase("",0,0,"","Reset","KingdomStatus",LOG_KINGDOMSTATUS,(char*)zone->GetShortName(),"City leadership has been reset for %s.",KingsBandits::GetKingdomName(kingdomid,false,true,true));

			worldserver.SendEmoteMessage(0,0,0,18,"City leadership has been reset for %s.",KingsBandits::GetKingdomName(kingdomid,false,true,true));
			setcitymode(kingdomid,RuleI(Kingdoms,CityResetCityMode));
		}
		else // begin else block
		{
			if ( canTakeoverLocation )
			{
				KingsBandits::ResetKingdomGuards(kingdomid);
				
				// reset guild hate list for team id
				database.UpdateGuildHateList((uint32)kingdomid, 0, 0, true);
			}


			if ( settings->citymode == CAMPDESTROYED )
			{
				if ( wasAlreadyDestroyed )
					setcitymode(settings->teamID,CAMPCONSTRUCTED,true);
				else
				{
					char name[64];
					database.GetCharName(charid, name);
					const char* guildName = guild_mgr.GetGuildName(guildid);
					worldserver.SendEmoteMessage(0,0,0,18,"[%s - Guild Camp] %s has been destroyed by %s of %s",(char*)zone->GetLongName(), KingsBandits::GetKingdomName(kingdomid,false,false,true),name,strlen(guildName) > 0 ? guildName : "<Unguilded>");
					
					database.logdatabase("",0,0,name,"Taken","KingdomStatus",LOG_KINGDOMSTATUS,(char*)zone->GetShortName(),"%s Guild Camp has been destroyed by %s of %s",KingsBandits::GetKingdomName(kingdomid,false,false, true),name,guildName);

				}
			}
			else if ( charid != 0 && charid != prevCharID && canTakeoverLocation)
			{
				char name[64];
				database.GetCharName(charid, name);
				const char* guildName = guild_mgr.GetGuildName(guildid);
				setcitymode(kingdomid,RuleI(Kingdoms,CityTakenCityMode));
				worldserver.SendEmoteMessage(0,0,0,18,"%s has been taken over by %s of %s",KingsBandits::GetKingdomName(kingdomid,false,true,true),name,guildName);
		
				if ( prevGuildName != NULL )
					database.logdatabase("",0,0,name,"Taken","KingdomStatus",LOG_KINGDOMSTATUS,(char*)zone->GetShortName(),"%s has been taken from %s by %s of %s",KingsBandits::GetKingdomName(kingdomid,false,true, true),prevGuildName,name,guildName);
				else
					database.logdatabase("",0,0,name,"Taken","KingdomStatus",LOG_KINGDOMSTATUS,(char*)zone->GetShortName(),"%s has been taken over by %s of %s",KingsBandits::GetKingdomName(kingdomid,false,true, true),name,guildName);
			}

			if ( charid != 0 && charid != prevCharID && canTakeoverLocation)
			{
				Client* newOwner = entity_list.GetClientByCharID( settings->charid );
				if ( newOwner != NULL && settings->citymode != CAMPDESTROYED )
				{
					if ( !KingsBandits::isCityLocation(kingdomid) )
					{
						newOwner->Message(13,"Congratulations!  Leadership has been provided to you of the camp!");
					}
					else
					{
						newOwner->Message(13,"Congratulations!  Leadership has been provided to you of the city!");
						newOwner->Message(13,"If you are in a city bank zone you must zone out of the current zone and back in to view the city bank.");
					}
					//newOwner->Message(13,"City NPC's will respawn on their normal 60 minute timer which will also include the banker.");
				}
				else if ( settings->citymode != CAMPDESTROYED )
				{
					char charidstr[64];
					_snprintf(charidstr,64,"%d",settings->charid);
					// 1 stands for the PVP pts earned
					worldserver.SendCommandRequest(UPDATE_CLIENT_GIVELEADERSHIP, TAKELEADERSHIPPVPPOINTS, charidstr);
				}
			}
		} // end else block
		
		if ( KingsBandits::isCityLocation(kingdomid) && (canTakeoverLocation || RuleB(Kingdoms,ResetOnFailedTakeover)) )
			worldserver.SendCommandRequest(UPDATE_CITY_MOBS, kingdomid, "");
		else if ( canTakeoverLocation || RuleB(Kingdoms,ResetOnFailedTakeover) )
			KingsBandits::ReloadCityGuards(kingdomid);

		return true;
	}

	return false;
}

int32 QuestManager::getkingdomownercharid(uint32 kingdomid) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	// get value for citizen tax rate and return
	if ( settings )
		return settings->charid;

	return 0;
}

int32 QuestManager::getkingdomownerguildid(uint32 kingdomid) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	// get value for citizen tax rate and return
	if ( settings )
		return settings->guildid;

	return 0;
}

bool QuestManager::setkingdomownerlastvisit(uint32 kingdomid, int32 timestamp) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	if ( settings )
	{
		settings->ownerlastvisit = timestamp;

		char query[256];
		snprintf(query,256,"UPDATE team_settings set ownerlastvisit=%i where teamid=%i",settings->ownerlastvisit);
		database.UpdateDatabaseQuery(query);

		return true;
	}

	return false;
}

int32 QuestManager::getkingdomownerlastvisit(uint32 kingdomid) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	// get value for citizen tax rate and return
	if ( settings )
		return settings->ownerlastvisit;

	return 0;
}


void QuestManager::setcitymode(uint32 kingdomid, uint8 citymode, bool updateDB) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	if ( settings )
	{
		settings->citymode = citymode;

		char query[256];
		snprintf(query,256,"UPDATE team_settings set citymode=%i where teamid=%i",settings->citymode,settings->teamID);
		database.UpdateDatabaseQuery(query);
		
		// updateDB refers to creating an entry for the log table since a user initiated the change
		if ( updateDB )
		{
			switch(citymode)
			{
			case RESTRICTIVE:
				{
					database.logdatabase("",0,0,"","Mode","KingdomStatus",LOG_KINGDOMSTATUS,(char*)zone->GetShortName(),"%s: City mode has been set to RESTRICTIVE.",KingsBandits::GetKingdomName(kingdomid,false,true,true));
					break;
				}
			case HOSTILE:
				{
					database.logdatabase("",0,0,"","Mode","KingdomStatus",LOG_KINGDOMSTATUS,(char*)zone->GetShortName(),"%s: City mode has been set to HOSTILE.",KingsBandits::GetKingdomName(kingdomid,false,true,true));
					break;
				}
			case FRIENDLY:
				{
					database.logdatabase("",0,0,"","Mode","KingdomStatus",LOG_KINGDOMSTATUS,(char*)zone->GetShortName(),"%s: City mode has been set to FRIENDLY.",KingsBandits::GetKingdomName(kingdomid,false,true,true));
					break;
				}
			case CITYDISABLED:
				{
					database.logdatabase("",0,0,"","Mode","KingdomStatus",LOG_KINGDOMSTATUS,(char*)zone->GetShortName(),"%s: City mode has been set to DISABLED.",KingsBandits::GetKingdomName(kingdomid,false,true,true));
					break;
				}
			case CITYENABLED:
				{
					database.logdatabase("",0,0,"","Mode","KingdomStatus",LOG_KINGDOMSTATUS,(char*)zone->GetShortName(),"%s: City mode has been set to ENABLED (city takeover disabled).",KingsBandits::GetKingdomName(kingdomid,false,true,true));
					break;
				}
			}
		}
	}

}

void QuestManager::setcitylocation(uint32 kingdomid, float x, float y, float z, float radius, int32 zoneid) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	if ( settings )
	{
		settings->x = x;
		settings->y = y;
		settings->z = z;
		settings->radius = radius;
		settings->zoneid = zoneid;
		
		char query[256];
		snprintf(query,256,"UPDATE team_settings set x=%f,y=%f,z=%f,radius=%f,zoneid=%i where teamid=%i",settings->x,settings->y,settings->z,settings->radius,settings->zoneid,settings->teamID);
		database.UpdateDatabaseQuery(query);
	}
}

uint8 QuestManager::getcitymode(uint32 kingdomid) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	if ( settings )
		return settings->citymode;

	return 0;
}

TeamSettings_Struct* QuestManager::getteamsettings(uint32 kingdomid) {
	TeamSettings_Struct* settings = database.GetTeamSettings((uint32)kingdomid);
	return settings;
}

void QuestManager::save() {
	if (initiator && initiator->IsClient())
		initiator->Save();
}

void QuestManager::faction(int faction_id, int faction_value) {
	if (initiator && initiator->IsClient()) {
		if(faction_id != 0 && faction_value != 0) {
			// SCORPIOUS2K - fixed faction command
			//Client *p;
			initiator->SetFactionLevel2(
				initiator->CharacterID(),
				faction_id,
				initiator->GetBaseClass(),
				initiator->GetBaseRace(),
				initiator->GetDeity(),
				faction_value);

		}
	}
}

void QuestManager::setsky(uint8 new_sky, uint8 new_red, uint8 new_green, uint8 new_blue, bool override_) {
	if (zone)
		zone->newzone_data.sky = new_sky;
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_NewZone, sizeof(NewZone_Struct));
	memcpy(outapp->pBuffer, &zone->newzone_data, outapp->size);
	if ( override_ )
	{
		for (int z=0; z<4; z++) {
			zone->newzone_data.fog_red[z] = new_red;
			zone->newzone_data.fog_green[z] = new_green;
			zone->newzone_data.fog_blue[z] = new_blue;
		}
	}
	entity_list.QueueClients(initiator, outapp);
	safe_delete(outapp);
}

void QuestManager::setguild(int32 new_guild_id, int8 new_rank) {
	if (initiator && initiator->IsClient()) {
		guild_mgr.SetGuild(initiator->CharacterID(), new_guild_id, new_rank);
	}
}

void QuestManager::settime(int8 new_hour, int8 new_min) {
	if (zone)
		zone->SetTime(new_hour + 1, new_min);
}

void QuestManager::itemlink(int item_id) {
	const ItemInst* inst = database.CreateItem(item_id);
	char* link = 0;
	if (initiator->MakeItemLink(link, inst))
		initiator->Message(0, "%s tells you, %c%s%s%c", owner->GetCleanName(), 0x12, link, inst->GetItem()->Name, 0x12);
	safe_delete_array(link);
	safe_delete(inst);
}

void QuestManager::signalwith(int npc_id, int signal_id, int wait_ms) {
	// SCORPIOUS2K - signal command
	// signal(npcid) - generates EVENT_SIGNAL on specified npc
	if(wait_ms > 0) {
		STimerList.push_back(SignalTimer(wait_ms, npc_id, signal_id));
		return;
	}

	if (npc_id<1)
	{
		printf("signal() bad npcid=%i\n",npc_id);
	}
	else
	{
		//initiator* signalnpc=0;
		entity_list.SignalMobsByNPCID(npc_id, signal_id);
	}
}

void QuestManager::signal(int npc_id, int wait_ms) {
	signalwith(npc_id, 0, wait_ms);
}

void QuestManager::setglobal(const char *varname, const char *newvalue, int options, const char *duration) {
	// SCORPIOUS2K - qglobal variable commands
	// setglobal(varname,value,options,duration)
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	//MYSQL_ROW row;
	int qgZoneid=zone->GetZoneID();
	int qgCharid=0;
	int qgNpcid = owner->GetNPCTypeID();

	/*	options value determines the availability of global variables to NPCs when a quest begins
	------------------------------------------------------------------
	value		   npcid	  player		zone
	------------------------------------------------------------------
	0			this		this		this
	1			all			this		this
	2			this		all			this
	3			all			all			this
	4			this		this		all
	5			all			this		all
	6			this		all			all
	7			all			all			all
	*/
	if (initiator && initiator->IsClient())  // some events like waypoint and spawn don't have a player involved
	{
		qgCharid=initiator->CharacterID();
	}

	else
	{
		qgCharid=-qgNpcid;		// make char id negative npc id as a fudge
	}
	if (options < 0 || options > 7)
	{
		cerr << "Invalid options for global var " << varname << " using defaults" << endl;
	}	// default = 0 (only this npcid,player and zone)
	else
	{
		if (options & 1)
			qgNpcid=0;
		if (options & 2)
			qgCharid=0;
		if (options & 4)
			qgZoneid=0;
	}

	InsertQuestGlobal(qgCharid, qgNpcid, qgZoneid, varname, newvalue, QGVarDuration(duration));
}

/* Inserts global variable into quest_globals table */
int QuestManager::InsertQuestGlobal(
	int charid, int npcid, int zoneid,
	const char *varname, const char *varvalue,
	int duration)
{
	char *query = 0;
	char errbuf[MYSQL_ERRMSG_SIZE];

	// Make duration string either "unix_timestamp(now()) + xxx" or "NULL"
	stringstream duration_ss;
	if (duration == INT_MAX)
	{
		duration_ss << "NULL";
	}
	else
	{
		duration_ss << "unix_timestamp(now()) + " << duration;
	}

	//NOTE: this should be escaping the contents of arglist
	//npcwise a malicious script can arbitrarily alter the DB
	uint32 last_id = 0;
	if (!database.RunQuery(query, MakeAnyLenString(&query,
		"REPLACE INTO quest_globals (charid, npcid, zoneid, name, value, expdate)"
		"VALUES (%i, %i, %i, '%s', '%s', %s)",
		charid, npcid, zoneid, varname, varvalue, duration_ss.str().c_str()
		), errbuf, NULL, NULL, &last_id))
	{
		cerr << "setglobal error inserting " << varname << " : " << errbuf << endl;
	}
	safe_delete_array(query);

	if(zone)
	{
		//first delete our global
		ServerPacket* pack = new ServerPacket(ServerOP_QGlobalDelete, sizeof(ServerQGlobalDelete_Struct));
		ServerQGlobalDelete_Struct *qgd = (ServerQGlobalDelete_Struct*)pack->pBuffer;
		qgd->npc_id = npcid;
		qgd->char_id = charid;
		qgd->zone_id = zoneid;
		qgd->from_zone_id = zone->GetZoneID();
		qgd->from_instance_id = zone->GetInstanceID();
		strcpy(qgd->name, varname);

		entity_list.DeleteQGlobal(std::string((char*)qgd->name), qgd->npc_id, qgd->char_id, qgd->zone_id);
		zone->DeleteQGlobal(std::string((char*)qgd->name), qgd->npc_id, qgd->char_id, qgd->zone_id);

		worldserver.SendPacket(pack);
		safe_delete(pack);

		//then create a new one with the new id
		pack = new ServerPacket(ServerOP_QGlobalUpdate, sizeof(ServerQGlobalUpdate_Struct));
		ServerQGlobalUpdate_Struct *qgu = (ServerQGlobalUpdate_Struct*)pack->pBuffer;
		qgu->npc_id = npcid;
		qgu->char_id = charid;
		qgu->zone_id = zoneid;
		if(duration == INT_MAX)
		{
			qgu->expdate = 0xFFFFFFFF;
		}
		else
		{
			qgu->expdate = Timer::GetTimeSeconds() + duration;
		}
		strcpy((char*)qgu->name, varname);
		strcpy((char*)qgu->value, varvalue);
		qgu->id = last_id;
		qgu->from_zone_id = zone->GetZoneID();
		qgu->from_instance_id = zone->GetInstanceID();

		QGlobal temp;
		temp.npc_id = npcid;
		temp.char_id = charid;
		temp.zone_id = zoneid;
		temp.expdate = qgu->expdate;
		temp.name.assign(qgu->name);
		temp.value.assign(qgu->value);
		entity_list.UpdateQGlobal(qgu->id, temp);
		zone->UpdateQGlobal(qgu->id, temp);

		worldserver.SendPacket(pack);
		safe_delete(pack);
	}

	return 0;
}

void QuestManager::targlobal(const char *varname, const char *value, const char *duration, int qgNpcid, int qgCharid, int qgZoneid)
{
	InsertQuestGlobal(qgCharid, qgNpcid, qgZoneid, varname, value, QGVarDuration(duration));
}

void QuestManager::delglobal(const char *varname) {
	// delglobal(varname)
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	int qgZoneid=zone->GetZoneID();
	int qgCharid=0;
	int qgNpcid=owner->GetNPCTypeID();
	if (initiator && initiator->IsClient())  // some events like waypoint and spawn don't have a player involved
	{
		qgCharid=initiator->CharacterID();
	}

	else
	{
		qgCharid=-qgNpcid;		// make char id negative npc id as a fudge
	}
	if (!database.RunQuery(query,
		MakeAnyLenString(&query,
		"DELETE FROM quest_globals WHERE name='%s'"
		" && (npcid=0 || npcid=%i) && (charid=0 || charid=%i) && (zoneid=%i || zoneid=0)",
		varname,qgNpcid,qgCharid,qgZoneid),errbuf))
	{
		cerr << "delglobal error deleting " << varname << " : " << errbuf << endl;
	}
	safe_delete_array(query);

	if(zone)
	{
		ServerPacket* pack = new ServerPacket(ServerOP_QGlobalDelete, sizeof(ServerQGlobalDelete_Struct));
		ServerQGlobalDelete_Struct *qgu = (ServerQGlobalDelete_Struct*)pack->pBuffer;

		qgu->npc_id = qgNpcid;
		qgu->char_id = qgCharid;
		qgu->zone_id = qgZoneid;
		strcpy(qgu->name, varname);

		entity_list.DeleteQGlobal(std::string((char*)qgu->name), qgu->npc_id, qgu->char_id, qgu->zone_id);
		zone->DeleteQGlobal(std::string((char*)qgu->name), qgu->npc_id, qgu->char_id, qgu->zone_id);

		worldserver.SendPacket(pack);
		safe_delete(pack);
	}
}

// Converts duration string to duration value (in seconds)
// Return of INT_MAX indicates infinite duration
int QuestManager::QGVarDuration(const char *fmt)
{
	int duration = 0;

	// format:	Y#### or D## or H## or M## or S## or T###### or C#######

	int len = strlen(fmt);

	// Default to no duration
	if (len < 1)
		return 0;

	// Set val to value after type character
	// e.g., for "M3924", set to 3924
	int val = atoi(&fmt[0] + 1);

	switch (fmt[0])
	{
		// Forever
	case 'F':
	case 'f':
		duration = INT_MAX;
		break;
		// Years
	case 'Y':
	case 'y':
		duration = val * 31556926;
		break;
	case 'D':
	case 'd':
		duration = val * 86400;
		break;
		// Hours
	case 'H':
	case 'h':
		duration = val * 3600;
		break;
		// Minutes
	case 'M':
	case 'm':
		duration = val * 60;
		break;
		// Seconds
	case 'S':
	case 's':
		duration = val;
		break;
		// Invalid
	default:
		duration = 0;
		break;
	}

	return duration;
}

void QuestManager::ding() {
	//-Cofruben:makes a sound.
	if (initiator && initiator->IsClient())
		initiator->SendSound();

}

void QuestManager::rebind(int zoneid, float x, float y, float z) {
	if(initiator && initiator->IsClient()) {
		initiator->SetBindPoint(zoneid, x, y, z);
	}
}

void QuestManager::start(sint32 wp) {
	if(!owner->IsNPC())
		return;
	owner->CastToNPC()->AssignWaypoints(wp);
}

void QuestManager::stop() {
	if(!owner->IsNPC())
		return;
	owner->CastToNPC()->StopWandering();
}

void QuestManager::pause(int duration) {
	if(!owner->IsNPC())
		return;
	owner->CastToNPC()->PauseWandering(duration);
}

void QuestManager::moveto(float x, float y, float z, float h, bool saveguardspot) {
	if(!owner->IsNPC())
		return;
	owner->CastToNPC()->MoveTo(x, y, z, h, saveguardspot);
}

void QuestManager::resume() {
	if(!owner->IsNPC())
		return;
	owner->CastToNPC()->ResumeWandering();
}

void QuestManager::addldonpoints(sint32 points, int32 theme) {
	if(initiator)
		initiator->UpdateLDoNPoints(points, theme);
}

void QuestManager::setnexthpevent(int at) {
	owner->SetNextHPEvent( at );
}

void QuestManager::setnextinchpevent(int at) {
	owner->SetNextIncHPEvent( at );
}

void QuestManager::respawn(int npc_type, int grid) {
	if(!owner->IsNPC())
		return;
	//char tempa[100];
	float x,y,z,h;
	if ( !owner )
		return;

	x = owner->GetX();
	y = owner->GetY();
	z = owner->GetZ();
	h = owner->GetHeading();
	depop_npc = true;

	const NPCType* tmp = 0;
	if ((tmp = database.GetNPCType(npc_type)))
	{
		owner = new NPC(tmp, 0, x, y, z, h, FlyMode3);
		owner->CastToNPC()->AddLootTable();
		entity_list.AddNPC(owner->CastToNPC(),true,true);
		if(grid > 0)
			owner->CastToNPC()->AssignWaypoints(grid);

		owner->SendPosUpdate();
	}
}

void QuestManager::set_proximity(float minx, float maxx, float miny, float maxy, float minz, float maxz) {
	if(!owner->IsNPC())
		return;

	entity_list.AddProximity(owner->CastToNPC());

	owner->CastToNPC()->proximity->min_x = minx;
	owner->CastToNPC()->proximity->max_x = maxx;
	owner->CastToNPC()->proximity->min_y = miny;
	owner->CastToNPC()->proximity->max_y = maxy;
	owner->CastToNPC()->proximity->min_z = minz;
	owner->CastToNPC()->proximity->max_z = maxz;

	owner->CastToNPC()->proximity->say = ((PerlembParser *)parse)->HasQuestSub(owner->CastToNPC()->GetNPCTypeID(),"EVENT_PROXIMITY_SAY");

	if(owner->CastToNPC()->proximity->say)
		HaveProximitySays = true;
}

void QuestManager::clear_proximity() {
	if(!owner->IsNPC())
		return;
	entity_list.RemoveProximity(owner->GetID());
	safe_delete(owner->CastToNPC()->proximity);
}

void QuestManager::setanim(int npc_type, int animnum) {
	//Cisyouc: adds appearance changes
	Mob* thenpc = entity_list.GetMobByNpcTypeID(npc_type);
	if(animnum < 0 || animnum >= _eaMaxAppearance)
		return;
	thenpc->SetAppearance(EmuAppearance(animnum));
}


//displays an in game path based on a waypoint grid
void QuestManager::showgrid(int grid) {
	if(initiator == NULL)
		return;

	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;

	FindPerson_Point pt;
	vector<FindPerson_Point> pts;

	pt.x = initiator->GetX();
	pt.y = initiator->GetY();
	pt.z = initiator->GetZ();
	pts.push_back(pt);

	// Retrieve all waypoints for this grid
	if(database.RunQuery(query,MakeAnyLenString(&query,"SELECT `x`,`y`,`z` FROM grid_entries WHERE `gridid`=%i AND `zoneid`=%i ORDER BY `number`",grid,zone->GetZoneID()),errbuf,&result))
	{
		while((row = mysql_fetch_row(result)))
		{
			pt.x = atof(row[0]);
			pt.y = atof(row[1]);
			pt.z = atof(row[2]);
			pts.push_back(pt);
		}
		mysql_free_result(result);

		initiator->SendPathPacket(pts);
	}
	else	// DB query error!
	{
		LogFile->write(EQEMuLog::Quest, "Error loading grid %d for showgrid(): %s", grid, errbuf);
		return;
	}
	safe_delete_array(query);
}

//displays an in game path based on path finding.
void QuestManager::showpath(float x, float y, float z) {
	say("showpath not implemented yet.");
}

//causes the npc to use path finding to walk to x,y,z
void QuestManager::pathto(float x, float y, float z) {
	say("pathto not implemented yet.");
}

//change the value of a spawn condition
void QuestManager::spawn_condition(const char *zone_short, uint32 instance_id, uint16 condition_id, short new_value) {
	zone->spawn_conditions.SetCondition(zone_short, instance_id, condition_id, new_value);
}

//get the value of a spawn condition
short QuestManager::get_spawn_condition(const char *zone_short, uint32 instance_id, uint16 condition_id) {
	return(zone->spawn_conditions.GetCondition(zone_short, instance_id, condition_id));
}

//toggle a spawn event
void QuestManager::toggle_spawn_event(int event_id, bool enable, bool reset_base) {
	zone->spawn_conditions.ToggleEvent(event_id, enable, reset_base);
}

bool QuestManager::has_zone_flag(int zone_id) {
	return(initiator->HasZoneFlag(zone_id));
}

void QuestManager::set_zone_flag(int zone_id) {
	initiator->SetZoneFlag(zone_id);
}

void QuestManager::clear_zone_flag(int zone_id) {
	initiator->ClearZoneFlag(zone_id);
}

void QuestManager::sethp(int hpperc) {
	int newhp;

	newhp = (owner->GetMaxHP()*(100-hpperc))/100;
	owner->Damage(owner, newhp, SPELL_UNKNOWN, HAND_TO_HAND, 0, false, 0, false);
}

bool QuestManager::summonburriedplayercorpse(int32 char_id, float dest_x, float dest_y, float dest_z, float dest_heading) {
	bool Result = false;

	if(char_id > 0) {
		Corpse* PlayerCorpse = database.SummonBurriedPlayerCorpse(char_id, zone->GetZoneID(), zone->GetInstanceID(), dest_x, dest_y, dest_z, dest_heading);

		if(PlayerCorpse) {
			PlayerCorpse->Spawn();
			Result = true;
		}
	}

	return Result;
}

int32 QuestManager::getnextcorpsedecaytime(int32 char_id) {
	int32 Result = 0;

	if(char_id > 0) {
		Result = database.GetNextPlayerBuryDecayTime(char_id);
	}

	return Result;
}

int32 QuestManager::getplayerburriedcorpsecount(int32 char_id) {
	int32 Result = 0;

	if(char_id > 0) {
		Result = database.GetPlayerBurriedCorpseCount(char_id);
	}

	return Result;
}

bool QuestManager::buryplayercorpse(int32 char_id) {
	bool Result = false;

	if(char_id > 0) {
		int32 PlayerCorpse = database.GetFirstCorpseID(char_id);

		if(PlayerCorpse > 0){
			database.BuryPlayerCorpse(PlayerCorpse);
			Corpse* CorpseEntity = entity_list.GetCorpseByDBID(PlayerCorpse);

			if(CorpseEntity > 0){
				CorpseEntity->Save();
				CorpseEntity->DepopCorpse();
				Result = true;
			}
		}
	}
	return Result;
}

void QuestManager::forcedooropen(int32 doorid) {
	Doors* d = entity_list.FindDoor(doorid);
	if(d){
		if(GetInitiator())
			d->ForceOpen(GetInitiator());
		else if(GetOwner())
			d->ForceOpen(GetOwner());
	}
}

void QuestManager::forcedoorclose(int32 doorid) {
	Doors* d = entity_list.FindDoor(doorid);
	if(d){
		if(GetInitiator())
			d->ForceClose(GetInitiator());
		else if(GetOwner())
			d->ForceClose(GetOwner());
	}
}

bool QuestManager::isdooropen(int32 doorid) {
	Doors* d = entity_list.FindDoor(doorid);
	if(d){
		return d->IsDoorOpen();
	}
	return false;
}
void QuestManager::npcrace(int race_id)
{
	owner->SendIllusionPacket(race_id);
}

void QuestManager::npcgender(int gender_id)
{
	owner->SendIllusionPacket(owner->GetRace(), gender_id);
}
void QuestManager::npcsize(int newsize)
{
	owner->ChangeSize(newsize, 0, true);
}
void QuestManager::npctexture(int newtexture)
{
	owner->SendIllusionPacket(owner->GetRace(), 0xFF, newtexture);
}

void QuestManager::playerrace(int race_id)
{
	initiator->SendIllusionPacket(race_id);
}

void QuestManager::playergender(int gender_id)
{
	initiator->SendIllusionPacket(initiator->GetRace(), gender_id);
}

void QuestManager::playersize(int newsize)
{
	initiator->ChangeSize(newsize, 0, true);
}

void QuestManager::playertexture(int newtexture)
{
	initiator->SendIllusionPacket(initiator->GetRace(), 0xFF, newtexture);
}

void QuestManager::playerfeature(char *feature, int setting)
{
	int16 Race = initiator->GetRace();
	int8 Gender = initiator->GetGender();
	int8 Texture = 0xFF;
	int8 HelmTexture = 0xFF;
	int8 HairColor = initiator->GetHairColor();
	int8 BeardColor = initiator->GetBeardColor();
	int8 EyeColor1 = initiator->GetEyeColor1();
	int8 EyeColor2 = initiator->GetEyeColor2();
	int8 HairStyle = initiator->GetHairStyle();
	int8 LuclinFace = initiator->GetLuclinFace();
	int8 Beard = initiator->GetBeard();
	int32 DrakkinHeritage = initiator->GetDrakkinHeritage();
	int32 DrakkinTattoo = initiator->GetDrakkinTattoo();
	int32 DrakkinDetails = initiator->GetDrakkinDetails();
	float Size = initiator->GetSize();

	if (!strcasecmp(feature,"race"))
		Race = setting;
	else if (!strcasecmp(feature,"gender"))
		Gender = setting;
	else if (!strcasecmp(feature,"texture"))
		Texture = setting;
	else if (!strcasecmp(feature,"helm"))
		HelmTexture = setting;
	else if (!strcasecmp(feature,"haircolor"))
		HairColor = setting;
	else if (!strcasecmp(feature,"beardcolor"))
		BeardColor = setting;
	else if (!strcasecmp(feature,"eyecolor1"))
		EyeColor1 = setting;
	else if (!strcasecmp(feature,"eyecolor2"))
		EyeColor2 = setting;
	else if (!strcasecmp(feature,"hair"))
		HairStyle = setting;
	else if (!strcasecmp(feature,"face"))
		LuclinFace = setting;
	else if (!strcasecmp(feature,"beard"))
		Beard = setting;
	else if (!strcasecmp(feature,"heritage"))
		DrakkinHeritage = setting;
	else if (!strcasecmp(feature,"tattoo"))
		DrakkinTattoo = setting;
	else if (!strcasecmp(feature,"details"))
		DrakkinDetails = setting;
	else if (!strcasecmp(feature,"size"))
		Size = (float)setting / 10;	//dividing by 10 to allow 1 decimal place for adjusting size
	else
		return;

	initiator->SendIllusionPacket(Race, Gender, Texture, HelmTexture, HairColor, BeardColor,
		EyeColor1, EyeColor2, HairStyle, LuclinFace, Beard, 0xFF,
		DrakkinHeritage, DrakkinTattoo, DrakkinDetails, Size);
}

void QuestManager::npcfeature(char *feature, int setting)
{
	int16 Race = owner->GetRace();
	int8 Gender = owner->GetGender();
	int8 Texture = 0xFF;
	int8 HelmTexture = 0xFF;
	int8 HairColor = owner->GetHairColor();
	int8 BeardColor = owner->GetBeardColor();
	int8 EyeColor1 = owner->GetEyeColor1();
	int8 EyeColor2 = owner->GetEyeColor2();
	int8 HairStyle = owner->GetHairStyle();
	int8 LuclinFace = owner->GetLuclinFace();
	int8 Beard = owner->GetBeard();
	int32 DrakkinHeritage = owner->GetDrakkinHeritage();
	int32 DrakkinTattoo = owner->GetDrakkinTattoo();
	int32 DrakkinDetails = owner->GetDrakkinDetails();
	float Size = owner->GetSize();

	if (!strcasecmp(feature,"race"))
		Race = setting;
	else if (!strcasecmp(feature,"gender"))
		Gender = setting;
	else if (!strcasecmp(feature,"texture"))
		Texture = setting;
	else if (!strcasecmp(feature,"helm"))
		HelmTexture = setting;
	else if (!strcasecmp(feature,"haircolor"))
		HairColor = setting;
	else if (!strcasecmp(feature,"beardcolor"))
		BeardColor = setting;
	else if (!strcasecmp(feature,"eyecolor1"))
		EyeColor1 = setting;
	else if (!strcasecmp(feature,"eyecolor2"))
		EyeColor2 = setting;
	else if (!strcasecmp(feature,"hair"))
		HairStyle = setting;
	else if (!strcasecmp(feature,"face"))
		LuclinFace = setting;
	else if (!strcasecmp(feature,"beard"))
		Beard = setting;
	else if (!strcasecmp(feature,"heritage"))
		DrakkinHeritage = setting;
	else if (!strcasecmp(feature,"tattoo"))
		DrakkinTattoo = setting;
	else if (!strcasecmp(feature,"details"))
		DrakkinDetails = setting;
	else if (!strcasecmp(feature,"size"))
		Size = (float)setting / 10;	//dividing by 10 to allow 1 decimal place for adjusting size
	else
		return;

	owner->SendIllusionPacket(Race, Gender, Texture, HelmTexture, HairColor, BeardColor,
		EyeColor1, EyeColor2, HairStyle, LuclinFace, Beard, 0xFF,
		DrakkinHeritage, DrakkinTattoo, DrakkinDetails, Size);
}

void QuestManager::popup(char *title, char *text, int32 popupid, int32 buttons, int32 Duration)
{
	if(initiator) initiator->SendPopupToClient(title, text, popupid, buttons, Duration);
}

#ifdef BOTS

int QuestManager::createbotcount() {
	return RuleI(Bots, CreateBotCount);
}

int QuestManager::spawnbotcount() {
	return RuleI(Bots, SpawnBotCount);
}

bool QuestManager::botquest()
{
	return RuleB(Bots, BotQuest);
}

bool QuestManager::createBot(const char *name, const char *lastname, uint8 level, uint16 race, uint8 botclass, uint8 gender)
{
	std::string TempErrorMessage;
	int32 MaxBotCreate = RuleI(Bots, CreateBotCount);

	if (initiator && initiator->IsClient())
	{
		if(Bot::SpawnedBotCount(initiator->CharacterID(), &TempErrorMessage) >= MaxBotCreate)
		{
			initiator->Message(15,"You have the maximum number of bots allowed.");
			return false;
		}

		if(!TempErrorMessage.empty())
		{
			initiator->Message(13, "Database Error: %s", TempErrorMessage.c_str());
			return false;
		}

		NPCType DefaultNPCTypeStruct = Bot::CreateDefaultNPCTypeStructForBot(name, lastname, level, race, botclass, gender);
		Bot* NewBot = new Bot(DefaultNPCTypeStruct, initiator);

		if(NewBot)
		{
			if(!NewBot->IsValidRaceClassCombo()) {
				initiator->Message(0, "That Race/Class combination cannot be created.");
				return false;
			}

			if(!NewBot->IsValidName()) {
				initiator->Message(0, "%s has invalid characters. You can use only the A-Z, a-z and _ characters in a bot name.", NewBot->GetCleanName());
				return false;
			}

			if(!NewBot->IsBotNameAvailable(&TempErrorMessage)) {
				initiator->Message(0, "The name %s is already being used. Please choose a different name.", NewBot->GetCleanName());
				return false;
			}

			if(!TempErrorMessage.empty()) {
				initiator->Message(13, "Database Error: %s", TempErrorMessage.c_str());
				return false;
			}

			// Now that all validation is complete, we can save our newly created bot
			if(!NewBot->Save())
			{
				initiator->Message(0, "Unable to save %s as a bot.", NewBot->GetCleanName());
			}
			else
			{
				initiator->Message(0, "%s saved as bot %u.", NewBot->GetCleanName(), NewBot->GetBotID());
				return true;
			}
		}
	}
	return false;
}

#endif //BOTS

void QuestManager::taskselector(int taskcount, int *tasks) {
	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && owner && taskmanager)
		taskmanager->SendTaskSelector(initiator, owner, taskcount, tasks);
}
void QuestManager::enabletask(int taskcount, int *tasks) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && taskmanager)
		initiator->EnableTask(taskcount, tasks);
}

void QuestManager::disabletask(int taskcount, int *tasks) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && taskmanager)
		initiator->DisableTask(taskcount, tasks);
}

bool QuestManager::istaskenabled(int taskid) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && taskmanager)
		return initiator->IsTaskEnabled(taskid);

	return false;
}

void QuestManager::tasksetselector(int tasksetid) {
	_log(TASKS__UPDATE, "TaskSetSelector called for task set %i", tasksetid);
	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && owner && taskmanager)
		initiator->TaskSetSelector(owner, tasksetid);
}

bool QuestManager::istaskactive(int task) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		return initiator->IsTaskActive(task);

	return false;
}
bool QuestManager::istaskactivityactive(int task, int activity) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		return initiator->IsTaskActivityActive(task, activity);

	return false;
}

void QuestManager::updatetaskactivity(int task, int activity, int count) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		initiator->UpdateTaskActivity(task, activity, count);
}

void QuestManager::resettaskactivity(int task, int activity) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		initiator->ResetTaskActivity(task, activity);
}

void QuestManager::taskexploredarea(int exploreid) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		initiator->UpdateTasksOnExplore(exploreid);
}

void QuestManager::assigntask(int taskid) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && owner)
		initiator->AssignTask(taskid, owner->GetID());
}

void QuestManager::failtask(int taskid) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		initiator->FailTask(taskid);
}

int QuestManager::tasktimeleft(int taskid) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		return initiator->TaskTimeLeft(taskid);

	return -1;
}

int QuestManager::enabledtaskcount(int taskset) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		return initiator->EnabledTaskCount(taskset);

	return -1;
}
int QuestManager::firsttaskinset(int taskset) {

	if(RuleB(TaskSystem, EnableTaskSystem) && taskmanager)
		return taskmanager->FirstTaskInSet(taskset);

	return -1;
}
int QuestManager::lasttaskinset(int taskset) {

	if(RuleB(TaskSystem, EnableTaskSystem) && taskmanager)
		return taskmanager->LastTaskInSet(taskset);

	return -1;
}
int QuestManager::nexttaskinset(int taskset, int taskid) {

	if(RuleB(TaskSystem, EnableTaskSystem) && taskmanager)
		return taskmanager->NextTaskInSet(taskset, taskid);

	return -1;
}
int QuestManager::activespeaktask() {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && owner)
		return initiator->ActiveSpeakTask(owner->GetNPCTypeID());

	return 0;
}
int QuestManager::activespeakactivity(int taskid) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && owner)
		return initiator->ActiveSpeakActivity(owner->GetNPCTypeID(), taskid);

	return 0;
}
int QuestManager::istaskcompleted(int taskid) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		return initiator->IsTaskCompleted(taskid);

	return -1;
}
int QuestManager::activetasksinset(int taskset) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		return initiator->ActiveTasksInSet(taskset);

	return -1;
}
int QuestManager::completedtasksinset(int taskset) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator)
		return initiator->CompletedTasksInSet(taskset);

	return -1;
}

bool QuestManager::istaskappropriate(int task) {

	if(RuleB(TaskSystem, EnableTaskSystem) && initiator && taskmanager)
		return taskmanager->AppropriateLevel(task, initiator->GetLevel());

	return false;
}
void QuestManager::clearspawntimers() {
	if(zone)  {
		//TODO: Dec 19, 2008, replace with code updated for current spawn timers.
		LinkedListIterator<Spawn2*> iterator(zone->spawn2_list);
		iterator.Reset();
		while (iterator.MoreElements())
		{
			char errbuf[MYSQL_ERRMSG_SIZE];
			char *query = 0;
			database.RunQuery(query, MakeAnyLenString(&query, "DELETE FROM respawn_times WHERE id=%lu AND "
				"instance_id=%lu",(unsigned long)iterator.GetData()->GetID(), (unsigned long)zone->GetInstanceID()), errbuf);
			safe_delete_array(query);
			iterator.Advance();
		}
	}
}
void QuestManager::ze(int type, const char *str) {
	entity_list.Message(0, type, str);
}

void QuestManager::we(int type, const char *str) {
	worldserver.SendEmoteMessage(0, 0, type, str);
}

int QuestManager::getlevel(uint8 type)
{
	if (type == 0)
	{
		return (initiator->GetLevel());
	}
	else if(type == 1)
	{
		Group *g = entity_list.GetGroupByClient(initiator);
		if (g != NULL)
			return (g->GetAvgLevel());
		else
			return 0;
	}
	else if(type == 2)
	{
		Raid *r = entity_list.GetRaidByClient(initiator);
		if (r != NULL)
			return (r->GetAvgLevel());
		else
			return 0;
	}
	else if(type == 3)
	{
		Raid *r = entity_list.GetRaidByClient(initiator);
		if(r != NULL)
		{
			return (r->GetAvgLevel());
		}
		Group *g = entity_list.GetGroupByClient(initiator);
		if(g != NULL)
		{
			return (g->GetAvgLevel());
		}
		else
			return (initiator->GetLevel());
	}
	else if(type == 4 && initiator->IsClient())
	{
		return (initiator->CastToClient()->GetLevel2());
	}
	else
		return 0;
}

int16 QuestManager::CreateGroundObject(int32 itemid, float x, float y, float z, float heading, int32 decay_time, uint32 requiredlevel, char* qglobalvalue)
{
	int16 entid = 0; //safety check
	entid = entity_list.CreateGroundObject(itemid, x, y, z, heading, decay_time, requiredlevel, qglobalvalue);
	return entid;
}

int16 QuestManager::CreateGroundObjectFromModel(const char *model, float x, float y, float z, float heading, int8 type, int32 decay_time)
{
	int16 entid = 0; //safety check
	entid = entity_list.CreateGroundObjectFromModel(model, x, y, z, heading, type, decay_time);
	return entid;
}

void QuestManager::ModifyNPCStat(const char *identifier, const char *newValue)
{
	if(owner){
		if(owner->IsNPC())
		{
			owner->CastToNPC()->ModifyNPCStat(identifier, newValue);
		}
	}
}

int QuestManager::collectitems_processSlot(sint16 slot_id, uint32 item_id,
	bool remove)
{
	ItemInst *item;
	int quantity = 0;

	item = initiator->GetInv().GetItem(slot_id);

	// If we have found matching item, add quantity
	if (item && item->GetID() == item_id)
	{
		// If item is stackable, add its charges (quantity)
		if (item->IsStackable())
		{
			quantity = item->GetCharges();
		}
		else
		{
			quantity = 1;
		}

		// Remove item from inventory
		if (remove)
		{
			initiator->DeleteItemInInventory(slot_id, 0, true);
		}
	}

	return quantity;
}

// Returns number of item_id that exist in inventory
// If remove is true, items are removed as they are counted.
int QuestManager::collectitems(uint32 item_id, bool remove)
{
	int quantity = 0;
	int slot_id;

	for (slot_id = 22; slot_id <= 29; ++slot_id)
	{
		quantity += collectitems_processSlot(slot_id, item_id, remove);
	}

	for (slot_id = 251; slot_id <= 330; ++slot_id)
	{
		quantity += collectitems_processSlot(slot_id, item_id, remove);
	}

	return quantity;
}

void QuestManager::UpdateSpawnTimer(int32 id, int32 newTime)
{
	bool found = false;

	database.UpdateSpawn2Timeleft(id, 0, (newTime/1000));
	LinkedListIterator<Spawn2*> iterator(zone->spawn2_list);
	iterator.Reset();
	while (iterator.MoreElements())
	{
		if(iterator.GetData()->GetID() == id)
		{
			if(!iterator.GetData()->NPCPointerValid())
			{
				iterator.GetData()->SetTimer(newTime);
			}
			found = true;
			break;
		}
		iterator.Advance();
	}

	if(!found)
	{
		//Spawn wasn't in this zone...
		//Tell the other zones to update their spawn time for this spawn point
		ServerPacket *pack = new ServerPacket(ServerOP_UpdateSpawn, sizeof(UpdateSpawnTimer_Struct));
		UpdateSpawnTimer_Struct *ust = (UpdateSpawnTimer_Struct*)pack->pBuffer;
		ust->id = id;
		ust->duration = newTime;
		worldserver.SendPacket(pack);
		safe_delete(pack);
	}
}

// used to set the number of an item in the selected merchant's temp item list.  Defaults to zero if no quantity is specified.
void QuestManager::MerchantSetItem(int32 NPCid, int32 itemid, int32 quantity) {
	Mob* merchant = entity_list.GetMobByNpcTypeID(NPCid);

	if (merchant == 0 || !merchant->IsNPC() || (merchant->GetClass() != MERCHANT))
		return;	// don't do anything if NPCid isn't a merchant

	const Item_Struct* item = NULL;
	item = database.GetItem(itemid);
	if (!item) return;		// if the item id doesn't correspond to a real item, do nothing

	zone->SaveTempItem(merchant->CastToNPC()->MerchantType, NPCid, itemid, quantity);
}

int32 QuestManager::MerchantCountItem(int32 NPCid, int32 itemid) {
	Mob* merchant = entity_list.GetMobByNpcTypeID(NPCid);

	if (merchant == 0 || !merchant->IsNPC() || (merchant->GetClass() != MERCHANT))
		return 0;	// if it isn't a merchant, it doesn't have any items

	const Item_Struct* item = NULL;
	item = database.GetItem(itemid);
	if (!item) return 0;		// likewise, if it isn't a valid item, the merchant doesn't have any

	// look for the item in the merchant's temporary list
	std::list<TempMerchantList> MerchList = zone->tmpmerchanttable[NPCid];
	std::list<TempMerchantList>::const_iterator itr;
	TempMerchantList ml;
	int32 Quant = 0;

	for(itr = MerchList.begin(); itr != MerchList.end(); itr++){
		ml = *itr;
		if (ml.item == itemid) {	// if this is the item we're looking for
			Quant = ml.charges;
			break;
		}
	}

	return Quant;	// return the quantity of itemid (0 if it was never found)
}

// Item Link for use in Variables - "my $example_link = quest::varlink(item_id);"
const char* QuestManager::varlink(char* perltext, int item_id) {
	const ItemInst* inst = database.CreateItem(item_id);
	if (!inst) return 0;	// return an empty string if the item is invalid
	char* link = 0;
	char* tempstr = 0;
	if (initiator->MakeItemLink(link, inst)) {	// make a link to the item
		MakeAnyLenString(&tempstr, "%c%s%s%c", 0x12, link, inst->GetItem()->Name, 0x12);
		strn0cpy(perltext, tempstr,250);	// the perl string is only 250 chars, so make sure the link isn't too large
		safe_delete_array(tempstr);	// MakeAnyLenString() uses new, so clean up after it
	}
	safe_delete_array(link);	// MakeItemLink() uses new also
	safe_delete(inst);
	return perltext;
}

int16 QuestManager::CreateInstance(const char *zone, int16 version, int32 duration)
{
	if(initiator)
	{
		int32 zone_id = database.GetZoneID(zone);
		if(zone_id == 0)
			return 0;

		int16 id = 0;
		if(!database.GetUnusedInstanceID(id))
		{
			initiator->Message(13, "Server was unable to find a free instance id.");
			return 0;
		}

		if(!database.CreateInstance(id, zone_id, version, duration))
		{
			initiator->Message(13, "Server was unable to create a new instance.");
			return 0;
		}
		return id;
	}
	return 0;
}

void QuestManager::DestroyInstance(int16 instance_id)
{
	database.DeleteInstance(instance_id);
}

int16 QuestManager::GetInstanceID(const char *zone, int16 version)
{
	if(initiator)
	{
		return database.GetInstanceID(zone, initiator->CharacterID(), version);
	}
	return 0;
}

void QuestManager::AssignToInstance(int16 instance_id)
{
	if(initiator)
	{
		database.AddClientToInstance(instance_id, initiator->CharacterID());
	}
}

void QuestManager::AssignGroupToInstance(int16 instance_id)
{
	if(initiator)
	{
		Group *g = initiator->GetGroup();
		if(g)
		{
			int32 gid = g->GetID();
			database.AssignGroupToInstance(gid, instance_id);
		}
	}
}

void QuestManager::AssignRaidToInstance(int16 instance_id)
{
	if(initiator)
	{
		Raid *r = initiator->GetRaid();
		if(r)
		{
			int32 rid = r->GetID();
			database.AssignRaidToInstance(rid, instance_id);
		}
	}
}

void QuestManager::MovePCInstance(int zone_id, int instance_id, float x, float y, float z, float heading)
{
	if(initiator)
	{
		initiator->MovePC(zone_id, instance_id, x, y, z, heading);
	}
}

void QuestManager::FlagInstanceByGroupLeader(int32 zone, int16 version)
{
	if(initiator)
	{
		Group *g = initiator->GetGroup();
		if(g){
			database.FlagInstanceByGroupLeader(zone, version, initiator->CharacterID(), g->GetID());
		}
	}
}

void QuestManager::FlagInstanceByRaidLeader(int32 zone, int16 version)
{
	if(initiator)
	{
		Raid *r = initiator->GetRaid();
		if(r)
		{
			database.FlagInstanceByRaidLeader(zone, version, initiator->CharacterID(), r->GetID());
		}
	}
}

const char* QuestManager::saylink(char* Phrase, bool silent, char* LinkName) {

	const char *ERR_MYSQLERROR = "Error in saylink phrase queries";
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	int sayid = 0;

	// Query for an existing phrase and id in the saylink table
	if(database.RunQuery(query,MakeAnyLenString(&query,"SELECT `id` FROM `saylink` WHERE `phrase` = '%s'", Phrase),errbuf,&result))
	{
		if (mysql_num_rows(result) >= 1)
		{
			while((row = mysql_fetch_row(result)))
			{
				sayid = atoi(row[0]);
			}
			mysql_free_result(result);
		}
		else   // Add a new saylink entry to the database and query it again for the new sayid number
		{
			safe_delete_array(query);

			database.RunQuery(query,MakeAnyLenString(&query,"INSERT INTO `saylink` (`phrase`) VALUES ('%s')", Phrase),errbuf);
			safe_delete_array(query);

			if(database.RunQuery(query,MakeAnyLenString(&query,"SELECT `id` FROM saylink WHERE `phrase` = '%s'", Phrase),errbuf,&result))
			{
				if (mysql_num_rows(result) >= 1)
				{
					while((row = mysql_fetch_row(result)))
					{
						sayid = atoi(row[0]);
					}
					mysql_free_result(result);
				}
			}
			else
			{
				LogFile->write(EQEMuLog::Error, ERR_MYSQLERROR, errbuf);
			}
			safe_delete_array(query);
		}
	}
	safe_delete_array(query);

	if(silent)
		sayid = sayid + 750000;
	else
		sayid = sayid + 500000;

	//Create the say link as an item link hash
	char linktext[250];

	if(initiator)
	{
		if (initiator->GetClientVersion() >= EQClientSoF)
		{
			sprintf(linktext,"%c%06X%s%s%c",0x12,sayid,"00000000000000000000000000000000000000000000",LinkName,0x12);
		}
		else
		{
			sprintf(linktext,"%c%06X%s%s%c",0x12,sayid,"000000000000000000000000000000000000000",LinkName,0x12);
		}
	}
	else {	// If no initiator, create an SoF saylink, since Titanium handles SoF ones better than SoF handles Titanium ones.
		sprintf(linktext,"%c%06X%s%s%c",0x12,sayid,"00000000000000000000000000000000000000000000",LinkName,0x12);
	}
	strcpy(Phrase,linktext);
	return Phrase;

}

void QuestManager::SetRunning(bool val)
{
	if(!owner)
		return;
	owner->SetRunning(val);
}

bool QuestManager::IsRunning()
{
	if(!owner)
		return false;
	return owner->IsRunning();
}

void QuestManager::FlyMode(int8 flymode)
{
	if(initiator)
	{
		if (flymode >= 0 && flymode < 3) {
			initiator->SendAppearancePacket(AT_Levitate, flymode);
			return;
		}
	}
	if(owner)
	{
		if (flymode >= 0 && flymode < 3) {
			owner->SendAppearancePacket(AT_Levitate, flymode);
			return;
		}
	}
}

uint8 QuestManager::FactionValue()
{
	FACTION_VALUE oldfac;
	uint8 newfac = 0;
	if(initiator && owner->IsNPC()) {
		oldfac = initiator->GetFactionLevel(initiator->GetID(), owner->GetID(), initiator->GetRace(), initiator->GetClass(), initiator->GetDeity(), owner->GetPrimaryFaction(), owner);

		// now, reorder the faction to have it make sense (higher values are better)
		switch (oldfac) {
		case FACTION_SCOWLS:
			newfac = 1;
			break;
		case FACTION_THREATENLY:
			newfac = 2;
			break;
		case FACTION_DUBIOUS:
			newfac = 3;
			break;
		case FACTION_APPREHENSIVE:
			newfac = 4;
			break;
		case FACTION_INDIFFERENT:
			newfac = 5;
			break;
		case FACTION_AMIABLE:
			newfac = 6;
			break;
		case FACTION_KINDLY:
			newfac = 7;
			break;
		case FACTION_WARMLY:
			newfac = 8;
			break;
		case FACTION_ALLY:
			newfac = 9;
			break;
		}
	}

	return newfac;
}

void QuestManager::enabletitle(int titleset) {
	initiator->EnableTitle(titleset);
}



bool QuestManager::checktitle(int titleset) {
	return initiator->CheckTitle(titleset);
}

void QuestManager::removetitle(int titleset) {
	initiator->RemoveTitle(titleset);
}

void QuestManager::wearchange(int8 slot, int16 texture)
{
	if(owner){
		owner->SendTextureWC(slot, texture);
		if(owner->IsNPC()) {
			owner->CastToNPC()->NPCSlotTexture(slot, texture);
		}
	}
}

void QuestManager::voicetell(char *str, int macronum, int racenum, int gendernum)
{
	if(owner && str)
	{
		Client *c = entity_list.GetClientByName(str);

		if(c)
		{
			EQApplicationPacket* outapp = new EQApplicationPacket(OP_VoiceMacroOut, sizeof(VoiceMacroOut_Struct));

			VoiceMacroOut_Struct* vmo = (VoiceMacroOut_Struct*)outapp->pBuffer;

			strn0cpy(vmo->From, owner->GetCleanName(), sizeof(vmo->From));

			vmo->Type = 1;

			vmo->Voice = (racenum * 2) + gendernum;

			vmo->MacroNumber = macronum;

			c->QueuePacket(outapp);

			safe_delete(outapp);
		}
		else
			LogFile->write(EQEMuLog::Quest, "QuestManager::voicetell from %s. Client %s not found.", owner->GetName(), str);
	}
}

void QuestManager::LearnRecipe(uint32 recipe_id) {
	if(!initiator)
		return;
	initiator->LearnRecipe(recipe_id);
}

bool QuestManager::TurnInItem(int32 itm, int charges)
{
	if ( owner && owner->IsNPC() )
	{
		if ( owner->CastToNPC()->DoesQuestItemExist(itm, charges, true) )
			return true;
	}

	return false;
}

void QuestManager::CompleteHandIn()
{
	if ( owner && owner->IsNPC() )
	{
		owner->CastToNPC()->RemoveQuestDeleteItems();
	}
}

void QuestManager::ResetHandIn()
{
	if ( owner && owner->IsNPC() )
	{
		owner->CastToNPC()->ResetQuestDeleteList();
	}
}

float QuestManager::GetBestZ(float x, float y, float z)
{
	if (zone->zonemap == NULL) {
		return -999999.0f;
	}
	
	NodeRef pnode;
	pnode = zone->zonemap->SeekNode( zone->zonemap->GetRoot(), x, y );
	if (pnode == NODE_NONE) {
		return -999999.0f;
	}
	
	VERTEX me;
	me.x = x;
	me.y = y;
	if ( initiator )
		me.z = z + (initiator->GetSize()==0.0?6:initiator->GetSize()) * HEAD_POSITION;
	else if ( owner )
		me.z = z + (owner->GetSize()==0.0?6:owner->GetSize()) * HEAD_POSITION;
	
	VERTEX hit;
	VERTEX bme(me);
	bme.z -= 500;
	
	float best_z = zone->zonemap->FindBestZ(pnode, me, &hit, NULL);
	
	if (best_z == -999999)
	{
		return -999999.0f;
	}

	return best_z;
}

void QuestManager::ReloadRules()
{
	worldserver.SendCommandRequest(RELOAD_ZONE_RULES, 0, "QUEST_MANAGER");
}