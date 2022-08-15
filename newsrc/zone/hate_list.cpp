/*  EQEMu:  Everquest Server Emulator
    Copyright (C) 2001-2002  EQEMu Development Team (http://eqemu.org)

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

#include "../common/debug.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "masterentity.h"
#include "../common/linked_list.h"
#include "../common/rulesys.h"
#include "../common/MiscFunctions.h"
#include "hate_list.h"
#include "../KingsBandits/KingsBandits.h"

HateList::HateList()
{
	owner = NULL;
}

HateList::~HateList()
{
}

// neotokyo: added for frenzy support
// checks if target still is in frenzy mode
void HateList::CheckFrenzyHate()
{
    LinkedListIterator<tHateEntry*> iterator(list);
    iterator.Reset();
	while(iterator.MoreElements())
    {
        if (iterator.GetData()->ent->GetHPRatio() >= 20)
            iterator.GetData()->bFrenzy = false;
        iterator.Advance();
    }
}

void HateList::Wipe()
{
	LinkedListIterator<tHateEntry*> iterator(list);
	iterator.Reset();

	while(iterator.MoreElements()) 
	{
		Mob* m = iterator.GetData()->ent;
		parse->Event(EVENT_HATE_LIST, owner->GetNPCTypeID(), "0", owner->CastToNPC(), m);
       	iterator.RemoveCurrent();

		if(m->IsClient())
			m->CastToClient()->DecrementAggroCount();
	}
}

bool HateList::IsOnHateList(Mob *mob)    
{    
	if (Find(mob))    
		return true;    
	return false;    
} 

tHateEntry *HateList::Find(Mob *ent)
{
	_ZP(HateList_Find);
    LinkedListIterator<tHateEntry*> iterator(list);
    iterator.Reset();
	while(iterator.MoreElements())
    {
        if(iterator.GetData()->ent == ent)
            return iterator.GetData();
        iterator.Advance();
    }
	return NULL;
}

void HateList::Set(Mob* other, int32 in_hate, int32 in_dam)
{
    tHateEntry *p = Find(other);
    if(p)
    {
		if(in_dam > 0)
        	p->damage = in_dam;
		if(in_hate > 0)
    		p->hate = in_hate;
    }
}

// Kings & Bandits - fix pets attack target.  Find the highest damage hate.
int32 HateList::GetHateTopValue(Mob* hater)
{
	_ZP(HateList_GetDamageTop);
	int32 hate_amt = 0;
	Group* grp = NULL;
	Raid* r = NULL;

    LinkedListIterator<tHateEntry*> iterator(list);
    iterator.Reset();
	while(iterator.MoreElements())
    {
		grp = NULL;
		r = NULL;

		if(iterator.GetData()->ent && iterator.GetData()->ent->IsClient()){
			r = entity_list.GetRaidByClient(iterator.GetData()->ent->CastToClient());
		}

        grp = entity_list.GetGroupByMob(iterator.GetData()->ent);

		if(iterator.GetData()->ent && r){
			if(r->GetTotalRaidDamage(hater) > hate_amt)
			{
				hate_amt = iterator.GetData()->hate;
			}
		}
		else if (iterator.GetData()->ent != NULL && grp != NULL)
        {
			if (grp->GetTotalGroupDamage(hater) > hate_amt)
            {
				hate_amt = iterator.GetData()->hate;
            }
        }
        else if (iterator.GetData()->ent != NULL && (int32)iterator.GetData()->hate > hate_amt)
        {
				hate_amt = iterator.GetData()->hate;
        }
        iterator.Advance();
    }
	return hate_amt;
}


Mob* HateList::GetDamageTop(Mob* hater, bool ignoreGroupLogic)
{
	_ZP(HateList_GetDamageTop);
	Mob* current = NULL;
	Group* grp = NULL;
	Raid* r = NULL;
	// this should be a sint32 to make sure as negatives may be passed into damage (invul is -5)
	sint32 dmg_amt = 0;

	Mob* topNPCHate = NULL;
	int32 dmg_npc_top = 0;
	int32 totaldmg = 0;
    LinkedListIterator<tHateEntry*> iterator(list);
    iterator.Reset();
	while(iterator.MoreElements())
    {
		grp = NULL;
		r = NULL;

		if(iterator.GetData()->ent && iterator.GetData()->ent->IsClient()){
			r = entity_list.GetRaidByClient(iterator.GetData()->ent->CastToClient());
		}

        grp = entity_list.GetGroupByMob(iterator.GetData()->ent);

		if(!ignoreGroupLogic && iterator.GetData()->ent && r){
			if(r->GetTotalRaidDamage(hater) >= dmg_amt)
			{
				current = iterator.GetData()->ent;
				dmg_amt = r->GetTotalRaidDamage(hater);
			}
		}
		else if (!ignoreGroupLogic && iterator.GetData()->ent != NULL && grp != NULL)
        {
			if (grp->GetTotalGroupDamage(hater) >= dmg_amt)
            {
				current = iterator.GetData()->ent;
				dmg_amt = grp->GetTotalGroupDamage(hater);
            }
        }
        else if (iterator.GetData()->ent != NULL && (int32)iterator.GetData()->damage >= dmg_amt)
        {
			current = iterator.GetData()->ent;
			dmg_amt = iterator.GetData()->damage;
        }
		else if ( iterator.GetData()->ent && iterator.GetData()->ent->IsNPC() && iterator.GetData()->damage > dmg_npc_top )
		{
			Mob* ult = iterator.GetData()->ent->GetUltimateOwner();
			if ( ult && ult->IsNPC() )
			{
				topNPCHate = iterator.GetData()->ent;
				dmg_npc_top = iterator.GetData()->damage;
			}
		}

		totaldmg += iterator.GetData()->damage;

        iterator.Advance();
    }

	if ( dmg_npc_top > 0 )
	{
		int32 fract = (int32)(((float)dmg_npc_top / (float)totaldmg)*100);
		if ( fract > 10 )
			return topNPCHate;
	}
	return current;
}

Mob* HateList::GetClosest(Mob *hater) {
	_ZP(HateList_GetClosest);
	Mob* close = NULL;
	float closedist = 99999.9f;
	float thisdist;
	
    LinkedListIterator<tHateEntry*> iterator(list);
    iterator.Reset();
	while(iterator.MoreElements()) {
		thisdist = iterator.GetData()->ent->DistNoRootNoZ(*hater);
		if(iterator.GetData()->ent != NULL && thisdist <= closedist) {
			closedist = thisdist;
			close = iterator.GetData()->ent;
		}
        iterator.Advance();
	}
										// if we have a target but it has divine aura, replace it
	if (!close && hater->IsNPC() || (close && close->DivineAura()) )
		close = hater->CastToNPC()->GetHateTop();
	
	return close;
}


// neotokyo: a few comments added, rearranged code for readability
// devn00b added +25 hate to all inhate to increase hate generation a bit. should work on all players/npcs
void HateList::Add(Mob *ent, sint32 in_hate, sint32 in_dam, bool bFrenzy, bool iAddIfNotExist)
{
	if(!ent)
        	return;

	if(ent->IsCorpse())
		return;

	if(ent->IsClient() && ent->CastToClient()->IsDead())
		return;

	tHateEntry *p = Find(ent);
	if (p)
	{
		p->damage+=in_dam;
		p->hate+=in_hate;
		p->bFrenzy = bFrenzy;
	}
	else if (iAddIfNotExist) {
		bool isHateListEmpty = IsEmpty();
        p = new tHateEntry;
        p->ent = ent;
        p->damage = in_dam;
        p->hate = in_hate;
        p->bFrenzy = bFrenzy;
        list.Append(p);
		parse->Event(EVENT_HATE_LIST, owner->GetNPCTypeID(), "1", owner->CastToNPC(), ent);

		Client* actualClient = NULL;
		if ( !ent->IsClient() && ent->GetOwner() != NULL && ent->GetOwner()->IsClient() )
			actualClient = ent->GetOwner()->CastToClient();
		else if ( ent->IsClient() )
			actualClient = ent->CastToClient();
		if ( actualClient != NULL )
		{
			if ( ( !owner->IsClient() && owner->GetOwner() == NULL )  || ( owner->GetOwner() != NULL && !owner->GetOwner()->IsClient() ) )
			{
					int32 con = actualClient->GetLevelCon(owner->GetLevel());
					if(actualClient->GetAggroCount() > RuleI(PVPCombat, InfamyMaxAggroCount) || con == CON_RED)
					{
						if ( ( owner->GetLevel() - actualClient->GetLevel() ) > RuleI(PVPCombat, TrainLevelRange) )
						{
								if ( !owner->IsNPC() || ( owner->IsNPC() && !owner->SpecAttacks[NO_INFAMY] ) )
									actualClient->UpdateInfamyLevel(RuleI(PVPCombat, TrainInfamyIncrease));
						}
						else if ( ( actualClient->GetLevel() - owner->GetLevel() ) > RuleI(PVPCombat, TrainLowerLevelRange) )
						{
								if ( !owner->IsNPC() || ( owner->IsNPC() && !owner->SpecAttacks[NO_INFAMY] ) )
									actualClient->UpdateInfamyLevel(RuleI(PVPCombat, TrainInfamyLowerLevelIncrease));
						}
					}
			}
			if ( isHateListEmpty && actualClient != NULL && owner->IsNPC() && owner->CastToNPC()->teamID > 0 && actualClient->GetInfamyLevel() < (RuleI(PVPCombat,InfamyAttackLevel)*2))
			{
				TeamSettings_Struct* settings = database.GetTeamSettings(owner->CastToNPC()->teamID);
				
				if ( settings != NULL )
				{
					bool teamGuildWars = RuleB(PVPCombat,TeamGuildWars);

					if ( teamGuildWars )
					{
						if ( actualClient->GetPVPValue() > 0 && actualClient->GetPVPValue() != settings->ownerteamid )
						{
							actualClient->UpdateInfamyLevel(RuleI(PVPCombat,InfamyAttackLevel)*2);
						}
					}// this is broken for KaB (shouldnt be used there) - was added later
					else if ( actualClient->GuildID() > 0 && actualClient->GuildID() != GUILD_NONE
						&& actualClient->GuildID() != settings->guildid )
					{
							actualClient->UpdateInfamyLevel(RuleI(PVPCombat,InfamyAttackLevel)*2);
					}
				}
			}
		}

		if(isHateListEmpty && actualClient != NULL && actualClient->IsClient() && !actualClient->IsSitting() )
		{
			actualClient->IncrementAggroCount();

			if ( owner->IsNPC() && owner->CastToNPC()->teamID > 0 )
			{
				TeamSettings_Struct* settings = database.GetTeamSettings(owner->CastToNPC()->teamID);
				
				if ( settings != NULL )
				{
					uint8 cityMode = settings->citymode;
					if ( cityMode == CITYENABLED || cityMode == CAMPCONSTRUCTED )
						cityMode = HOSTILE;
					bool teamGuildWars = RuleB(PVPCombat,TeamGuildWars);

					if ( teamGuildWars )
					{
							// Image (Jul 29, 2019): rewritten if statement to include bandit team id to check on guildid
						if ( actualClient->GetPVPValue() > 0 && ( (settings->ownerteamid != BANDITTEAMID && actualClient->GetPVPValue() != settings->ownerteamid) || 
							(settings->ownerteamid == BANDITTEAMID && actualClient->GuildID() != settings->guildid)) )
						{
							if (  actualClient->GetInfamyLevel() < (RuleI(PVPCombat,InfamyAttackLevel)*2) )
								actualClient->UpdateInfamyLevel(RuleI(PVPCombat,InfamyAttackLevel)*2);
							if (cityMode == RESTRICTIVE )
							{
								timeval tv;
								gettimeofday(&tv, NULL);
								database.UpdateGuildHateList(owner->CastToNPC()->teamID, actualClient->GuildID(), tv.tv_sec);
							}
						}
					}// this is broken for KaB (shouldnt be used there) - was added later
					else if ( actualClient->GuildID() != settings->guildid )
					{	

						if (  actualClient->GetInfamyLevel() < (RuleI(PVPCombat,InfamyAttackLevel)*2) )
							actualClient->UpdateInfamyLevel(RuleI(PVPCombat,InfamyAttackLevel)*2);
						
						if (cityMode == RESTRICTIVE )
						{
							timeval tv;
							gettimeofday(&tv, NULL);
							database.UpdateGuildHateList(owner->CastToNPC()->teamID, actualClient->GuildID(), tv.tv_sec);
						}
					}
				}

			}
		}
    }
}

bool HateList::RemoveEnt(Mob *ent)
{
	bool found = false;
	LinkedListIterator<tHateEntry*> iterator(list);
	iterator.Reset();

	while(iterator.MoreElements())
	{
		if(iterator.GetData()->ent == ent)
		{
			parse->Event(EVENT_HATE_LIST, owner->GetNPCTypeID(), "0", owner->CastToNPC(), ent);
			iterator.RemoveCurrent();
			found = true;

			if(ent->IsClient())
				ent->CastToClient()->DecrementAggroCount();

        	}
		else
			iterator.Advance();
	}
	return found;
}

void HateList::DoFactionHits(sint32 nfl_id) {
	_ZP(HateList_DoFactionHits);
	if (nfl_id <= 0)
		return;
    LinkedListIterator<tHateEntry*> iterator(list);
    iterator.Reset();
	while(iterator.MoreElements())
    {
        Client *p;

        if (iterator.GetData()->ent && iterator.GetData()->ent->IsClient())
            p = iterator.GetData()->ent->CastToClient();
        else
            p = NULL;

        if (p)
			p->SetFactionLevel(p->CharacterID(), nfl_id, p->GetBaseClass(), p->GetBaseRace(), p->GetDeity());
        iterator.Advance();
    }
}

Mob *HateList::GetTop(Mob *center)
{
	_ZP(HateList_GetTop);
	Mob* top = NULL;
	sint32 hate = -1;
	
	if (RuleB(Aggro,SmartAggroList)){
		Mob* topClientInRange = NULL;
		sint32 hateClientInRange = -1;
		LinkedListIterator<tHateEntry*> iterator(list);
		iterator.Reset();
		while(iterator.MoreElements())
		{
    		tHateEntry *cur = iterator.GetData();
			sint32 aggroMod = 0;

			if(!cur){
				iterator.Advance();
				continue;
			}			

			if(!cur->ent){
				iterator.Advance();
				continue;
			}

			if(cur->ent->DivineAura() || cur->ent->IsMezzed() || cur->ent->IsFeared()){
				if(hate == -1)
				{
					top = cur->ent;
					hate = 0;
				}
				iterator.Advance();
				continue;
			}

			sint32 currentHate = cur->hate;

			if(cur->ent->IsClient()){
				
				if(!cur->ent->CastToClient()->GetFeigned() && 
					cur->ent->CastToClient()->IsSitting() && ( cur->damage > 0 || cur->hate > 0 ) ){
					int32 lvlAvg = (cur->ent->GetLevel() + center->GetLevel())/2;
					aggroMod += (RuleI(Aggro, SittingAggroMod) * lvlAvg)/50;
					
					int aggroModHalf = RuleI(Aggro, SittingAggroMod)/2;
					if ( aggroMod < aggroModHalf )
						aggroMod = aggroModHalf;

				}
				if(center){
					if(center->GetTarget() == cur->ent)
						aggroMod += RuleI(Aggro, CurrentTargetAggroMod);
					if(RuleI(Aggro, MeleeRangeAggroMod) != 0)
					{
						if(center->IsRooted() && center->CombatRange(cur->ent)){
							aggroMod += RuleI(Aggro, MeleeRangeAggroMod);

							if(currentHate > hateClientInRange || cur->bFrenzy){
								hateClientInRange = currentHate;
								topClientInRange = cur->ent;
							}
						}
					}
				}

			}
			else{
				if(center){
					if(center->GetTarget() == cur->ent)
						aggroMod += RuleI(Aggro, CurrentTargetAggroMod);
					if(RuleI(Aggro, MeleeRangeAggroMod) != 0)
					{
						if(center->IsRooted() && center->CombatRange(cur->ent)){
							aggroMod += RuleI(Aggro, MeleeRangeAggroMod);
						}
					}
				}
			}

			if(cur->ent->GetMaxHP() != 0 && ((cur->ent->GetHP()*100/cur->ent->GetMaxHP()) < 20)){
				aggroMod += RuleI(Aggro, CriticallyWoundedAggroMod);
			}

			if(aggroMod){
				currentHate += (sint32) aggroMod;
			}

			// KaB - Red69 - Zek / Update hate to the new value from the aggroMod.
			//cur->hate = currentHate;

			if(currentHate > hate || cur->bFrenzy){
				hate = currentHate;
				top = cur->ent;
			}

			iterator.Advance();
		}

		if(topClientInRange != NULL && top != NULL && !top->IsClient())
			return topClientInRange;
		else
			return top;
	}
	else{
		LinkedListIterator<tHateEntry*> iterator(list);
		iterator.Reset();
		while(iterator.MoreElements())
		{
    		tHateEntry *cur = iterator.GetData();
			if(cur->ent != NULL && ((cur->hate > hate) || cur->bFrenzy ))
			{
				top = cur->ent;
				hate = cur->hate;
			}
			iterator.Advance();
		}
		return top;
	}
}

Mob *HateList::GetMostHate(){
	_ZP(HateList_GetMostHate);

	Mob* top = NULL;
	sint32 hate = -1;

	LinkedListIterator<tHateEntry*> iterator(list);
	iterator.Reset();
	while(iterator.MoreElements())
	{
		tHateEntry *cur = iterator.GetData();
		if(cur->ent != NULL && (cur->hate > hate))
		{
			top = cur->ent;
			hate = cur->hate;
		}
		iterator.Advance();
	}
	return top;
}


Mob *HateList::GetRandom()
{
	int count = list.Count();

	if(count == 0)
		return NULL;
	if ( count == 1 )
		return list.GetFirst()->GetData()->ent;

    LinkedListIterator<tHateEntry*> iterator(list);

    int random = MakeRandomInt(0, count-1);
    iterator.Reset();
    for (int i = 0; i < random-1; i++)
        iterator.Advance();

	if ( iterator.GetData() != NULL )
	    return iterator.GetData()->ent;
	else
		return NULL;
}

sint32 HateList::GetEntHate(Mob *ent, bool damage)
{
	tHateEntry *p;

    p = Find(ent);
	
	if ( p && damage)
        return p->damage;
	else if (p)
		return p->hate;
	else
		return 0;
}

//looking for any mob with hate > -1
bool HateList::IsEmpty() {
	_ZP(HateList_IsEmpty);
	
	return(list.Count() == 0);
}

// Prints hate list to a client
void HateList::PrintToClient(Client *c)
{
	LinkedListIterator<tHateEntry*> iterator(list);
	iterator.Reset();
	while (iterator.MoreElements())
	{
		tHateEntry *e = iterator.GetData();
		c->Message(0, "- name: %s, damage: %d, hate: %d",
			(e->ent && e->ent->GetName()) ? e->ent->GetName() : "(null)",
			e->damage, e->hate);

		iterator.Advance();
	}
}

int HateList::AreaRampage(Mob *caster, Mob *target)
{
	if(!target || !caster)
		return 0;

	int ret = 0;
	std::list<uint32> id_list;
	LinkedListIterator<tHateEntry*> iterator(list);
	iterator.Reset();
	while (iterator.MoreElements())
	{
		tHateEntry *h = iterator.GetData();
		iterator.Advance();		
		if(h && h->ent && h->ent != caster)
		{
			if(caster->CombatRange(h->ent))
			{
				id_list.push_back(h->ent->GetID());
				++ret;
			}
		}
	}

	std::list<uint32>::iterator iter = id_list.begin();
	while(iter != id_list.end())
	{
		Mob *cur = entity_list.GetMobID((*iter));
		if(cur)
		{
			caster->Attack(cur);
		}
		iter++;
	}

	return ret;
}

void HateList::SpellCast(Mob *caster, uint32 spell_id, float range)
{
	if(!caster)
	{
		return;
	}

	//this is slower than just iterating through the list but avoids 
	//crashes when people kick the bucket in the middle of this call
	//that invalidates our iterator but there's no way to know sadly
	//So keep a list of entity ids and look up after
	std::list<uint32> id_list;
	range = range * range;
	LinkedListIterator<tHateEntry*> iterator(list);
	iterator.Reset();
	while (iterator.MoreElements())
	{
		tHateEntry *h = iterator.GetData();
		if(range > 0)
		{
			if(caster->DistNoRoot(*h->ent) <= range)
			{
				id_list.push_back(h->ent->GetID());
			}
		}
		else
		{
			id_list.push_back(h->ent->GetID());
		}
		iterator.Advance();		
	}

	std::list<uint32>::iterator iter = id_list.begin();
	while(iter != id_list.end())
	{
		Mob *cur = entity_list.GetMobID((*iter));
		if(cur)
		{
			caster->SpellOnTarget(spell_id, cur);
		}
		iter++;
	}
}


void HateList::GetHateList(std::list<tHateEntry*> &h_list)
{
	h_list.clear();
	LinkedListIterator<tHateEntry*> iterator(list); 
	iterator.Reset();
	while(iterator.MoreElements()) 
	{
		tHateEntry *ent = iterator.GetData();
		h_list.push_back(ent);
		iterator.Advance();
	}
}
