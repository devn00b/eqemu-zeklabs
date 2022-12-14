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
#include <stdlib.h>
#include <math.h>
#include "masterentity.h"
#include "faction.h"
#include "map.h"
#include "spdat.h"
#include "../common/skills.h"
#include "../common/MiscFunctions.h"
#include "../common/rulesys.h"
#include "StringIDs.h"
#include <iostream>

// Kings & Bandits include aggro.cpp
#include "../KingsBandits/KingsBandits.h"

extern Zone* zone;
//#define LOSDEBUG 6

//look around a client for things which might aggro the client.
void EntityList::CheckClientAggro(Client *around) {
	_ZP(EntityList_CheckClientAggro);

	LinkedListIterator<Mob*> iterator(mob_list);
	for(iterator.Reset(); iterator.MoreElements(); iterator.Advance()) {
		_ZP(EntityList_CheckClientAggro_Loop);
		Mob* mob = iterator.GetData();
		if(mob->IsClient())	//also ensures that mob != around
			continue;

		if(mob->CheckWillAggro(around)) {
			if(mob->IsEngaged())
			{
				mob->AddToHateList(around);
			}
			else
			{
				mob->AddToHateList(around, mob->GetLevel());
			}
		}
	}
}

void EntityList::DescribeAggro(Client *towho, NPC *from_who, float d, bool verbose) {
	float d2 = d*d;

	towho->Message(0, "Describing aggro for %s", from_who->GetName());

	bool engaged = from_who->IsEngaged();
	if(engaged) {
		Mob *top = from_who->GetHateTop();
		towho->Message(0, ".. I am currently fighting with %s", top == NULL?"(NULL)":top->GetName());
	}
	bool check_npcs = from_who->WillAggroNPCs();

	if(verbose) {
		char namebuf[256];

		int my_primary = from_who->GetPrimaryFaction();
		Mob *own = from_who->GetOwner();
		if(own != NULL)
			my_primary = own->GetPrimaryFaction();

		if(my_primary == 0) {
			strcpy(namebuf, "(No faction)");
		} else if(my_primary < 0) {
			strcpy(namebuf, "(Special faction)");
		} else {
			if(!database.GetFactionName(my_primary, namebuf, sizeof(namebuf)))
				strcpy(namebuf, "(Unknown)");
		}
		towho->Message(0, ".. I am on faction %s (%d)\n", namebuf, my_primary);
	}

	LinkedListIterator<Mob*> iterator(mob_list);
	for(iterator.Reset(); iterator.MoreElements(); iterator.Advance()) {
		Mob* mob = iterator.GetData();
		if(mob->IsClient())	//also ensures that mob != around
			continue;

		if(mob->DistNoRoot(*from_who) > d2)
			continue;

		if(engaged) {
			int32 amm = from_who->GetHateAmount(mob);
			if(amm == 0) {
				towho->Message(0, "... %s is not on my hate list.", mob->GetName());
			} else {
				towho->Message(0, "... %s is on my hate list with value %lu", mob->GetName(), (unsigned long)amm);
			}
		} else if(!check_npcs && mob->IsNPC()) {
			towho->Message(0, "... %s is an NPC and my npc_aggro is disabled.", mob->GetName());
		} else {
			from_who->DescribeAggro(towho, mob, verbose);
		}
	}
}

void NPC::DescribeAggro(Client *towho, Mob *mob, bool verbose) {
	//this logic is duplicated from below, try to keep it up to date.
	float iAggroRange = GetAggroRange();

	float t1, t2, t3;
	t1 = mob->GetX() - GetX();
	t2 = mob->GetY() - GetY();
	t3 = mob->GetZ() - GetZ();
	//Cheap ABS()
	if(t1 < 0)
		t1 = 0 - t1;
	if(t2 < 0)
		t2 = 0 - t2;
	if(t3 < 0)
		t3 = 0 - t3;
	if(   ( t1 > iAggroRange)
		|| ( t2 > iAggroRange)
		|| ( t3 > iAggroRange) ) {
			towho->Message(0, "...%s is out of range (fast). distances (%.3f,%.3f,%.3f), range %.3f", mob->GetName(),
				t1, t2, t3, iAggroRange);
			return;
	}

	if(mob->IsInvisible(this)) {
		towho->Message(0, "...%s is invisible to me. ", mob->GetName());
		return;
	}
	if((mob->IsClient() &&
		(!mob->CastToClient()->Connected()
		|| mob->CastToClient()->IsLD()
		|| mob->CastToClient()->IsBecomeNPC()
		|| mob->CastToClient()->GetGM()
		)   
		))
	{
		towho->Message(0, "...%s is my owner. ", mob->GetName());
		return;
	}


	if(mob == GetOwner()) {
		towho->Message(0, "...%s a GM or is not connected. ", mob->GetName());
		return;
	}

	float dist2  = mob->DistNoRoot(*this);
	float iAggroRange2 = iAggroRange*iAggroRange;
	if( dist2 > iAggroRange2 ) {
		towho->Message(0, "...%s is out of range. %.3f > %.3f ", mob->GetName(),
			dist2, iAggroRange2);
		return;
	}

	if(GetINT() > 75 && mob->GetLevelCon(GetLevel()) == CON_GREEN ) {
		towho->Message(0, "...%s is red to me (basically)", mob->GetName(),
			dist2, iAggroRange2);
		return;
	}

	if(verbose) {
		int my_primary = GetPrimaryFaction();
		int mob_primary = mob->GetPrimaryFaction();
		Mob *own = GetOwner();
		if(own != NULL)
			my_primary = own->GetPrimaryFaction();
		own = mob->GetOwner();
		if(mob_primary > 0 && own != NULL)
			mob_primary = own->GetPrimaryFaction();

		if(mob_primary == 0) {
			towho->Message(0, "...%s has no primary faction", mob->GetName());
		} else if(mob_primary < 0) {
			towho->Message(0, "...%s is on special faction %d", mob->GetName(), mob_primary);
		} else {
			char namebuf[256];
			if(!database.GetFactionName(mob_primary, namebuf, sizeof(namebuf)))
				strcpy(namebuf, "(Unknown)");
			list<struct NPCFaction*>::iterator cur,end;
			cur = faction_list.begin();
			end = faction_list.end();
			bool res = false;
			for(; cur != end; cur++) {
				struct NPCFaction* fac = *cur;
				if ((sint32)fac->factionID == mob_primary) {
					if (fac->npc_value > 0) {
						towho->Message(0, "...%s is on ALLY faction %s (%d) with %d", mob->GetName(), namebuf, mob_primary, fac->npc_value);
						res = true;
						break;
					} else if (fac->npc_value < 0) {
						towho->Message(0, "...%s is on ENEMY faction %s (%d) with %d", mob->GetName(), namebuf, mob_primary, fac->npc_value);
						res = true;
						break;
					} else {
						towho->Message(0, "...%s is on NEUTRAL faction %s (%d) with 0", mob->GetName(), namebuf, mob_primary);
						res = true;
						break;
					}
				}
			}
			if(!res) {
				towho->Message(0, "...%s is on faction %s (%d), which I have no entry for.", mob->GetName(), namebuf, mob_primary);
			}
		}
	}

	FACTION_VALUE fv = mob->GetReverseFactionCon(this);

	if(!(
		fv == FACTION_SCOWLS
		||
		(mob->GetPrimaryFaction() != GetPrimaryFaction() && mob->GetPrimaryFaction() == -4 && GetOwner() == NULL)
		||
		fv == FACTION_THREATENLY
		)) {
			towho->Message(0, "...%s faction not low enough. value='%s'", mob->GetName(), FactionValueToString(fv));
			return;
	}
	if(fv == FACTION_THREATENLY) {
		towho->Message(0, "...%s threatening to me, so they only have a %d chance per check of attacking.", mob->GetName());
	}

	if(!CheckLosFN(mob)) {
		towho->Message(0, "...%s is out of sight.", mob->GetName());		
	}

	towho->Message(0, "...%s meets all conditions, I should be attacking them.", mob->GetName());	
}

/*
If you change this function, you should update the above function
to keep the #aggro command accurate.
*/
bool Mob::CheckWillAggro(Mob *mob) {
	if(!mob)
		return false;
	_ZP(Mob_CheckWillAggro);

	//sometimes if a client has some lag while zoning into a dangerous place while either invis or a GM
	//they will aggro mobs even though it's supposed to be impossible, to lets make sure we've finished connecting
	if(mob->IsClient() && !mob->CastToClient()->ClientFinishedLoading())
		return false;

	Mob *ownr = mob->GetOwner();
	if(ownr && ownr->IsClient() && !ownr->CastToClient()->ClientFinishedLoading())
		return false;

	//im not sure I understand this..
	//if I have an owner and it is not this mob, then I cannot
	//aggro this mob...???
	//changed to be 'if I have an owner and this is it'
	if(mob == GetOwner()) {
		return(false);
	}

	if ( IsNPC() && mob->IsNPC() && 
		( GetClass() == MERCHANT || mob->GetClass() == MERCHANT ) && 
		GetOwner() == NULL && mob->GetOwner() == NULL )
		return false;


	float iAggroRange = GetAggroRange();

	// Check If it's invisible and if we can see invis
	// Check if it's a client, and that the client is connected and not linkdead,
	//   and that the client isn't Playing an NPC, with thier gm flag on
	// Check if it's not a Interactive NPC
	// Trumpcard: The 1st 3 checks are low cost calcs to filter out unnessecary distance checks. Leave them at the beginning, they are the most likely occurence.
	// Image: I moved this up by itself above faction and distance checks because if one of these return true, theres no reason to go through the other information

	float t1, t2, t3;
	t1 = mob->GetX() - GetX();
	t2 = mob->GetY() - GetY();
	t3 = mob->GetZ() - GetZ();
	//Cheap ABS()
	if(t1 < 0)
		t1 = 0 - t1;
	if(t2 < 0)
		t2 = 0 - t2;
	if(t3 < 0)
		t3 = 0 - t3;
	if(   ( t1 > iAggroRange)
		|| ( t2 > iAggroRange)
		|| ( t3 > iAggroRange)
		||(mob->IsInvisible(this))
		|| (mob->IsClient() &&
		(!mob->CastToClient()->Connected()
		|| mob->CastToClient()->IsLD()
		|| mob->CastToClient()->IsBecomeNPC()
		|| mob->CastToClient()->GetGM()
		)   
		))
	{
		return(false);
	}

	float dist2  = mob->DistNoRoot(*this);
	float iAggroRange2 = iAggroRange*iAggroRange;
	
	if ( mob->IsClient() && mob->sneaking )
		iAggroRange2 = iAggroRange2 * RuleR(Combat,SneakAggroReduction);

	if( dist2 > iAggroRange2 ) {
		// Skip it, out of range
		return(false);
	}

	//Image: Get their current target and faction value now that its required
	//this function call should seem backwards
	FACTION_VALUE fv = mob->GetReverseFactionCon(this);

	// Make sure they're still in the zone
	// Are they in range?
	// Are they kos?
	// Are we stupid or are they green
	// and they don't have their gm flag on
	int heroicCHA_mod = mob->itembonuses.HeroicCHA/25; // 800 Heroic CHA cap
	if(heroicCHA_mod > THREATENLY_ARRGO_CHANCE)
		heroicCHA_mod = THREATENLY_ARRGO_CHANCE;

	if ( CastToNPC()->teamID > 0 && mob->IsClient() )
	{
		bool passStatus = false;

			TeamSettings_Struct* settings = database.GetTeamSettings(CastToNPC()->teamID);

			if ( settings )
			{
				uint8 cityMode = settings->citymode;
				if ( cityMode == CITYENABLED || cityMode == CAMPCONSTRUCTED )
					cityMode = HOSTILE;

				switch(settings->guildid)
				{
				case 0:
					{
						if ( !mob->IsClient() )
							break;

						bool overrideRed = RuleB(PVPCombat,PureRed);
						bool teamGuildWars = RuleB(PVPCombat,TeamGuildWars);

						if ( teamGuildWars && 
							( mob->CastToClient()->GetAttackTarget() != NULL && 
								( (settings->ownerteamid != mob->CastToClient()->GetPVPValue()) ||
							// Image (Jul 29, 2019): rewritten if statement to include bandit team id to check on guildid
								  (settings->ownerteamid == BANDITTEAMID && settings->guildid != mob->CastToClient()->GuildID())
								)
							))
							passStatus = true;
						
						if ( !teamGuildWars && !overrideRed && mob->CastToClient()->GetAttackTarget() != NULL )
							passStatus = true;
						if ( !teamGuildWars && !overrideRed && mob->CastToClient()->GetAttackTarget() != NULL && settings->teamID != mob->CastToClient()->GetPVPValue() )
							passStatus = true;
						if ( overrideRed && mob->CastToClient()->GetAttackTarget() == NULL && 
							mob->GetLevel() < RuleI(Kingdoms,GuardAttacksLevel) && ( ( mob->CastToClient()->GuildID() == 0 || mob->CastToClient()->GuildID() == GUILD_NONE ) ) )
						{
							// do nothing
						}
						else if ( overrideRed && mob->CastToClient()->GetAttackTarget() != NULL )
						{
							if ( mob->CastToClient()->GetAttackTarget() != NULL && mob->CastToClient()->IsInitiatorOfCombat() )
								passStatus = true;
							else if ( overrideRed && cityMode == RESTRICTIVE 
								&& mob->CastToClient()->GuildID() > 0 && mob->CastToClient()->GuildID() != GUILD_NONE )
							{
								GuildHate_Struct* ghs = NULL;
								ghs = database.GetGuildHateList(CastToNPC()->teamID,mob->CastToClient()->GuildID());

								if ( ghs != NULL )
									passStatus = true;
							}
						}
						// KaB 2018-2019 - Allow guildwars flag enabled, while pure red disabled and check team id to make sure town is a match when no guild owner (default cities)
						else if ( teamGuildWars && !overrideRed && settings->ownerteamid > 0 && 
							( (settings->ownerteamid != mob->CastToClient()->GetPVPValue()) || 
							// Image (Jul 29, 2019): rewritten if statement to include bandit team id to check on guildid
							  (settings->ownerteamid == BANDITTEAMID && settings->guildid != mob->CastToClient()->GuildID())
							) )
							passStatus = true;
						break;
					}
				default:
					{
						if ( !mob->IsClient() )
							break;

						bool teamGuildWars = RuleB(PVPCombat,TeamGuildWars);
						
						Mob* target = NULL;
						if ( mob->CastToClient()->GetAttackTarget() != NULL )
						{
							if ( mob->CastToClient()->GetAttackTarget()->HasOwner() )
								target = mob->CastToClient()->GetAttackTarget()->GetOwner();
							else
								target = mob->CastToClient()->GetAttackTarget();
						}

						if ( teamGuildWars )
						{
							if ( target != NULL && target->IsClient() && 
								target->CastToClient()->GetPVPValue() == settings->ownerteamid && 
								mob->CastToClient()->GetPVPValue() != settings->ownerteamid )
								passStatus = true;
							else if ( target != NULL && target->IsClient() && (( target->CastToClient()->GetPVPValue() != settings->ownerteamid && 
								mob->CastToClient()->GetPVPValue() == settings->ownerteamid) || 
							// Image (Jul 29, 2019): rewritten if statement to include bandit team id to check on guildid
								( settings->ownerteamid == BANDITTEAMID && settings->guildid != target->CastToClient()->GuildID()))
								)
							{
								if ( CheckLosFN(target) )
									this->AddToHateList(target);
								// do nothing else we added an alternative target
							}
							else if ( mob->CastToClient()->GetPVPValue() > 0 && ( (mob->CastToClient()->GetPVPValue() != settings->ownerteamid) || // only people on different teams apply
							// Image (Jul 29, 2019): rewritten if statement to include bandit team id to check on guildid
								( settings->ownerteamid == BANDITTEAMID && settings->guildid != mob->CastToClient()->GuildID() ))
								)
							{
								if ( cityMode == HOSTILE )
									passStatus = true;
								else if ( cityMode == RESTRICTIVE )
								{
									GuildHate_Struct* ghs = NULL;
									ghs = database.GetGuildHateList(CastToNPC()->teamID,mob->CastToClient()->GuildID());

									if ( ghs != NULL )
										passStatus = true;
								}
							}
						}
						else if ( mob->CastToClient()->GetAttackTarget() != NULL && mob->CastToClient()->GetAttackTarget()->IsClient() && 
							mob->CastToClient()->GetAttackTarget()->CastToClient()->GuildID() == settings->guildid && 
							mob->CastToClient()->GuildID() != settings->guildid )
							passStatus = true;
						else if ( target != NULL && target->IsClient() && target->CastToClient()->GuildID() != settings->guildid && 
								mob->CastToClient()->GuildID() == settings->guildid )
						{
								if ( CheckLosFN(target) && DistNoRoot(*target) < RuleR(Kingdoms,GuardAssistDistance) )
									this->AddToHateList(target);
								// do nothing else we added an alternative target
						}
						else if ( mob->CastToClient()->GetAttackTarget() == NULL && 
							mob->GetLevel() < RuleI(Kingdoms,GuardAttacksLevel) && ( mob->CastToClient()->GuildID() == 0 || mob->CastToClient()->GuildID() == GUILD_NONE ) )
						{
							// do nothing
						}
						else if ( mob->CastToClient()->GuildID() != settings->guildid ) // only people in guilds apply to aggro
						{
							if ( cityMode == HOSTILE )
								passStatus = true;
							else if ( mob->CastToClient()->GuildID() > 0 && mob->CastToClient()->GuildID() != GUILD_NONE && 
								cityMode == RESTRICTIVE )
							{
								GuildHate_Struct* ghs = NULL;
								ghs = database.GetGuildHateList(CastToNPC()->teamID,mob->CastToClient()->GuildID());

								if ( ghs != NULL )
									passStatus = true;
							}
						}
						break;
					}
				} 
			}
		if(passStatus && CheckLosFN(mob)) {

			// Aggro
#if EQDEBUG>=6
			LogFile->write(EQEMuLog::Debug, "Check aggro for %s target %s.", GetName(), mob->GetName());
#endif
			return(true);
		}
	}
	else if
		(
		//old InZone check taken care of above by !mob->CastToClient()->Connected()
		(
		( GetINT() <= 75 )
		||( mob->IsClient() && mob->CastToClient()->IsSitting() )
		||( mob->GetLevelCon(GetLevel()) != CON_GREEN )

		)
		&&
		(
		(
		fv == FACTION_SCOWLS
		||
		(mob->GetPrimaryFaction() != GetPrimaryFaction() && mob->GetPrimaryFaction() == -4 && GetOwner() == NULL)
		||
		(
		fv == FACTION_THREATENLY
		&& MakeRandomInt(0,99) < THREATENLY_ARRGO_CHANCE - heroicCHA_mod
		)
		)
		)
		)
	{
		//FatherNiwtit: make sure we can see them. last since it is very expensive
		if(CheckLosFN(mob)) {

			// Aggro
#if EQDEBUG>=6
			LogFile->write(EQEMuLog::Debug, "Check aggro for %s target %s.", GetName(), mob->GetName());
#endif
			return(true);
		}
	}
#if EQDEBUG >= 6
	printf("Is In zone?:%d\n", mob->InZone());
	printf("Dist^2: %f\n", dist2);
	printf("Range^2: %f\n", iAggroRange2);
	printf("Faction: %d\n", fv);
	printf("Int: %d\n", GetINT());
	printf("Con: %d\n", GetLevelCon(mob->GetLevel()));
#endif		
	return(false);
}

Mob* EntityList::AICheckCloseAggro(Mob* sender, float iAggroRange, float iAssistRange) {
	if (!sender || !sender->IsNPC())
		return(NULL);
	_ZP(EntityList_AICheckCloseAggro);

#ifdef REVERSE_AGGRO
	//with reverse aggro, npc->client is checked elsewhere, no need to check again
	LinkedListIterator<NPC*> iterator(npc_list);
#else
	LinkedListIterator<Mob*> iterator(mob_list);
#endif
	iterator.Reset();
	//float distZ;
	while(iterator.MoreElements()) {
		Mob* mob = iterator.GetData();

		if(sender->CheckWillAggro(mob)) {
			return(mob);
		}

		iterator.Advance();
	}
	//LogFile->write(EQEMuLog::Debug, "Check aggro for %s no target.", sender->GetName());
	return(NULL);
}

int EntityList::GetHatedCount(Mob *attacker, Mob *exclude) {

	// Return a list of how many non-feared, non-mezzed, non-green mobs, within aggro range, hate *attacker

	if(!attacker) return 0;

	int Count = 0;

	LinkedListIterator<NPC*> iterator(npc_list);

	for(iterator.Reset(); iterator.MoreElements(); iterator.Advance()) {

		NPC* mob = iterator.GetData();

		if(!mob || (mob == exclude)) continue;

		if(!mob->IsEngaged()) continue;

		if(mob->IsFeared() || mob->IsMezzed()) continue;

		if(attacker->GetLevelCon(mob->GetLevel()) == CON_GREEN) continue;

		if(!mob->CheckAggro(attacker)) continue;

		float AggroRange = mob->GetAggroRange();

		// Square it because we will be using DistNoRoot

		AggroRange = AggroRange * AggroRange;

		if(mob->DistNoRoot(*attacker) > AggroRange) continue;

		Count++;

	}

	return Count;

}

void EntityList::AIYellForHelp(Mob* sender, Mob* attacker) {
	_ZP(EntityList_AIYellForHelp);
	if(!sender || !attacker)
		return;
	if (sender->GetPrimaryFaction() == 0 )
		return; // well, if we dont have a faction set, we're gonna be indiff to everybody

	// Kings & Bandits - fix for like factions helping each other when the mob in question has been charmed, owned by a client.
	else if ( sender->IsPet() && sender->IsCharmed() && sender->GetOwner() != NULL 
		&& sender->GetOwner()->IsClient() )
		return; // well we got charmed, we sure should stay out of this!

	LinkedListIterator<NPC*> iterator(npc_list);

	for(iterator.Reset(); iterator.MoreElements(); iterator.Advance()) {
		NPC* mob = iterator.GetData();
		if(!mob){
			continue;
		}
		float r = mob->GetAssistRange();
		r = r * r;

		if ( attacker != NULL && attacker->sneaking )
			r = r * RuleR(Combat,SneakAggroReduction);

		if(sender == NULL)
			return;

		if (
			mob != sender
			&& mob != attacker
			//			&& !mob->IsCorpse()
			//			&& mob->IsAIControlled()
			&& mob->GetPrimaryFaction() != 0
			&& mob->DistNoRoot(*sender) <= r
			&& !mob->IsEngaged()
			)
		{
			if ( ( sender->GetClass() == MERCHANT || attacker->GetClass() == MERCHANT ) && 
				sender->GetOwner() == NULL && sender->GetOwner() == NULL && 
				((sender->IsNPC() && sender->CastToNPC()->teamID > 0) || (attacker->IsNPC() && attacker->CastToNPC()->teamID > 0)) )
				continue;

			//if they are in range, make sure we are not green...
			//then jump in if they are our friend
			if(attacker->GetLevelCon(mob->GetLevel()) != CON_GREEN)
			{
				bool useprimfaction = false;
				if(mob->GetPrimaryFaction() == sender->CastToNPC()->GetPrimaryFaction())
				{
					const NPCFactionList *cf = database.GetNPCFactionEntry(mob->GetNPCFactionID());
					if(cf){
						if(cf->assistprimaryfaction != 0)
							useprimfaction = true;
					}
				}

				if(useprimfaction || sender->GetReverseFactionCon(mob) <= FACTION_AMIABLE )
				{
					//attacking someone on same faction, or a friend
					//Father Nitwit:  make sure we can see them.
					if(mob->CheckLosFN(sender)) {
#if (EQDEBUG>=5) 
						LogFile->write(EQEMuLog::Debug, "AIYellForHelp(\"%s\",\"%s\") %s attacking %s Dist %f Z %f", 
							sender->GetName(), attacker->GetName(), mob->GetName(), attacker->GetName(), mob->DistNoRoot(*sender), fabs(sender->GetZ()+mob->GetZ()));
#endif
						mob->AddToHateList(attacker, 1, 0, false);
					}
				}
			}
		}
	}
}

/*
solar: returns false if attack should not be allowed
I try to list every type of conflict that's possible here, so it's easy
to see how the decision is made.  Yea, it could be condensed and made
faster, but I'm doing it this way to make it readable and easy to modify
*/

bool Mob::IsAttackAllowed(Mob *target, bool isSpellAttack)
{
	Mob *mob1, *mob2, *tempmob;
	Client *c1, *c2, *becomenpc;
	//	NPC *npc1, *npc2;
	int reverse;

	if(!zone->CanDoCombat())
		return false;

	// some special cases
	if(!target)
		return false;

	if(this == target)	// you can attack yourself
		return true;

	// can't damage own pet (applies to everthing)
	Mob *target_owner = target->GetOwner();
	Mob *our_owner = GetOwner();
	if(target_owner && target_owner == this)
		return false;
	else if(our_owner && our_owner == target)
		return false;


	if ( ( IsClient() && !CastToClient()->ClientFinishedLoading() ) || 
		( target->IsClient() && !target->CastToClient()->ClientFinishedLoading() ) )
		return false;


	//cannot hurt untargetable mobs
	bodyType bt = target->GetBodyType();

	if(bt == BT_NoTarget || bt == BT_NoTarget2)
		return(false);

	if(!isSpellAttack)
	{
		if(GetClass() == LDON_TREASURE)
		{
			return false;
		}
	}

	// solar: the format here is a matrix of mob type vs mob type.
	// redundant ones are omitted and the reverse is tried if it falls through.

	// first figure out if we're pets.  we always look at the master's flags.
	// no need to compare pets to anything
	mob1 = our_owner ? our_owner : this;
	mob2 = target_owner ? target_owner : target;

	uint8 pvpValue = zone->CanPVPValue();
	reverse = 0;
	do
	{
		if(_CLIENT(mob1))
		{
			if(_CLIENT(mob2))					// client vs client
			{
				c1 = mob1->CastToClient();
				c2 = mob2->CastToClient();
				if	// if they're dueling they can go at it
					(
					c1->IsDueling() &&
					c2->IsDueling() &&
					c1->GetDuelTarget() == c2->GetID() &&
					c2->GetDuelTarget() == c1->GetID()
					)
					return true;
				// KaB (Red69) - BecomeNPC fix - 8/5/2011
				// allow people to attack each other if one is a NPC
				else if ( c1->IsBecomeNPC() || c2->IsBecomeNPC() )
					return true;
				else if ( pvpValue == PVPALLVALUE )
				{
					if ( c1->IsGrouped() && c1->GetGroup() != NULL && c1->GetGroup() == c2->GetGroup() )
						return false;
					else if ( c1->IsRaidGrouped() && c1->GetRaid() != NULL && c1->GetRaid() == c2->GetRaid() )
						return false;

					return true;
				}
				else if ( pvpValue > 0 && RuleB(PVPCombat,CanAttackInfamyNewbies) && (c1->IsInfamyReached() && c1->GetPVP() == 0 && c2->GetPVPValue() != 0 ) )
				{
					return IsAttackLevelPermitted(c1, c2);
				}
				else if ( pvpValue > 0 && RuleB(PVPCombat,CanAttackInfamyNewbies) && (c2->IsInfamyReached() && c2->GetPVP() == 0 && c1->GetPVPValue() != 0 ) )
				{
					return IsAttackLevelPermitted(c2, c1);
				}
				else if
					// if both are pvp they can fight
					// Kings & Bandits Modification, uint8 pvp value
					( c1->GetPVPValue() > 0 && c2->GetPVPValue() > 0 )
				{
					if ( pvpValue == NEUTRALPVPVALUE )
					{
						if ( c1->GetLevel() > 39 && c2->GetLevel() > 39 )
							pvpValue = PVPTEAMVALUE;
						else
							return false;
					}

					if ( pvpValue == NOPVPVALUE || pvpValue == DISABLEACTIONVALUE )
						return false;
					else if ( pvpValue == PVPFFANOLEVELVALUE )
						return true;
					else if ( pvpValue == PVPFFAVALUE )
					{
						return IsAttackLevelPermitted(c1, c2);
					}
					else if ( pvpValue == PVPTEAMVALUE || pvpValue == PVPTEAMNOLEVELVALUE )
					{
						if ( RuleB(PVPCombat,PureRed) || ( c1->GetPVPValue() == BANDITTEAMID && c2->GetPVPValue() == BANDITTEAMID ) ) // check if either person is bandit, can attack anyone and anyone can attack them
						{
							if ( c1->IsGrouped() && c1->GetGroup() != NULL && c1->GetGroup() == c2->GetGroup() )
								return false;
							else if ( c1->IsRaidGrouped() && c1->GetRaid() != NULL && c1->GetRaid() == c2->GetRaid() )
								return false;
							else
							{
								if ( pvpValue == PVPTEAMNOLEVELVALUE )
									return true;
								else
									return IsAttackLevelPermitted(c1, c2);
							}
						}
						else if ( c1->GetPVPValue() != c2->GetPVPValue() ) // team allow
						{
							if ( pvpValue == PVPTEAMNOLEVELVALUE )
								return true;
							else
								return IsAttackLevelPermitted(c1, c2);
						}
						else if ( RuleB(PVPCombat, SameTeamCanAttackInfamyPlayers) && ( c1->IsInfamyReached() || c2->IsInfamyReached() ) && !c1->IsBountySet() && !c2->IsBountySet() )
						{
							if ( c1->IsGrouped() && c1->GetGroup() != NULL && c1->GetGroup() == c2->GetGroup() )
								return false;
							else if ( c1->IsRaidGrouped() && c1->GetRaid() != NULL && c1->GetRaid() == c2->GetRaid() )
								return false;
							else
								return IsAttackLevelPermitted(c1, c2);
						}
						else
							return false;
					}
					else
						return false; // otherwise if none of our code hits up
				}
				else
					return false;
			}
			else if(_NPC(mob2))				// client vs npc
			{
				// invis race					// no target npc
				if ( mob2->GetRace() == 240 || mob2->GetBodyType() == BT_NoTarget
					|| mob2->GetBodyType() == BT_NoTarget2  || mob2->GetBodyType() == BT_Special )
					return false;

				if ( !KingsBandits::IsGuardAttackAllowed(mob1->CastToClient(), mob2) )
					return false;

				return true;
			}
			else if(_BECOMENPC(mob2))	// client vs becomenpc
			{
				c1 = mob1->CastToClient();
				becomenpc = mob2->CastToClient();

				if(c1->GetLevel() > becomenpc->GetBecomeNPCLevel())
					return false;
				else
					return true;
			}	
			else if(_CLIENTCORPSE(mob2))	// client vs client corpse
			{
				return false;
			}
			else if(_NPCCORPSE(mob2))	// client vs npc corpse
			{
				return false;
			}
		}
		else if(_NPC(mob1))
		{
			if(_NPC(mob2))						// npc vs npc
			{
				if ( ( mob1->GetClass() == MERCHANT || mob2->GetClass() == MERCHANT ) && 
					mob1->GetOwner() == NULL && mob2->GetOwner() == NULL )
					return false;

				return true;
			}
			else if(_BECOMENPC(mob2))	// npc vs becomenpc
			{
				return true;
			}
			else if(_CLIENTCORPSE(mob2))	// npc vs client corpse
			{
				return false;
			}
			else if(_NPCCORPSE(mob2))	// npc vs npc corpse
			{
				return false;
			}
			else if(_CLIENT(mob2))
			{
				// invis race					// no target npc
				if ( mob1->GetRace() == 240 || mob1->GetBodyType() == BT_NoTarget
					|| mob1->GetBodyType() == BT_NoTarget2  || mob1->GetBodyType() == BT_Special )
					return false;

				if ( !KingsBandits::IsGuardAttackAllowed(mob2->CastToClient(), mob1) )
					return false;

			}
		}
		else if(_BECOMENPC(mob1))
		{
			if(_BECOMENPC(mob2))			// becomenpc vs becomenpc
			{
				return true;
			}
			else if(_CLIENTCORPSE(mob2))	// becomenpc vs client corpse
			{
				return false;
			}
			else if(_NPCCORPSE(mob2))	// becomenpc vs npc corpse
			{
				return false;
			}
		}
		else if(_CLIENTCORPSE(mob1))
		{
			if(_CLIENTCORPSE(mob2))		// client corpse vs client corpse
			{
				return false;
			}
			else if(_NPCCORPSE(mob2))	// client corpse vs npc corpse
			{
				return false;
			}
		}
		else if(_NPCCORPSE(mob1))
		{
			if(_NPCCORPSE(mob2))			// npc corpse vs npc corpse
			{
				return false;
			}
		}

#ifdef BOTS
		bool HasRuleDefined = false;
		bool IsBotAttackAllowed = false;
		IsBotAttackAllowed = Bot::IsBotAttackAllowed(mob1, mob2, HasRuleDefined);
		if(HasRuleDefined)
			return IsBotAttackAllowed;
#endif //BOTS

		// we fell through, now we swap the 2 mobs and run through again once more
		tempmob = mob1;
		mob1 = mob2;
		mob2 = tempmob;
	}
	while( reverse++ == 0 );

	LogFile->write(EQEMuLog::Debug, "Mob::IsAttackAllowed: don't have a rule for this - %s vs %s\n", this->GetName(), target->GetName());
	return false;
}


// solar: this is to check if non detrimental things are allowed to be done
// to the target.  clients cannot affect npcs and vice versa, and clients
// cannot affect other clients that are not of the same pvp flag as them.
// also goes for their pets
bool Mob::IsBeneficialAllowed(Mob *target)
{
	Mob *mob1, *mob2, *tempmob;
	Client *c1, *c2;
	int reverse;

	if(!target)
		return false;

	// solar: see IsAttackAllowed for notes

	// first figure out if we're pets.  we always look at the master's flags.
	// no need to compare pets to anything
	mob1 = this->GetOwnerID() ? this->GetOwner() : this;
	mob2 = target->GetOwnerID() ? target->GetOwner() : target;

	// if it's self target or our own pet it's ok
	if(mob1 == mob2)
		return true;

	reverse = 0;
	do
	{
		if(_CLIENT(mob1))
		{
			if(_CLIENT(mob2))					// client to client
			{
				c1 = mob1->CastToClient();
				c2 = mob2->CastToClient();

				// Kings & Bandits Modification, uint8 pvp value

				// This mod is for ZEK 2014 specific, hard coded without pulling variable to save a step since
				// this is frequently called

				if ( c1->GetGM() || c2->GetGM() )
					return true;
				else if ( c1->GetPVPValue() > 0 && c2->GetPVPValue() == 0 )
					return false;
				else if ( c2->GetPVPValue() > 0 && c1->GetPVPValue() == 0 )
					return false;
				else if ( !RuleB(PVPCombat,BeneficialOtherTeams) && ( c1->GetPVPValue() != c2->GetPVPValue() && (c1->GetPVPValue() > 0 && c2->GetPVPValue() > 0) ) )
					return false;
				else if ( !RuleB(PVPCombat,BeneficialTeamsDiscord) && ( c1->GetPVPValue() != c2->GetPVPValue() && (c1->GetPVPValue() != BANDITTEAMID || c2->GetPVPValue() != BANDITTEAMID) ) )
					return false;

				return true; // not sure we care?  Just let people heal each other!!

				/*if(c1->GetPVPValue() == c2->GetPVPValue())
				return true;
				else if ( c1->GetPVPValue () < 1 && c2->GetPVPValue () > 0 )
				return true;
				else if ( c2->GetPVPValue () < 1 && c1->GetPVPValue () > 0 )
				return true;

				else if	// if they're dueling they can heal each other too
				(
				c1->IsDueling() &&
				c2->IsDueling() &&
				c1->GetDuelTarget() == c2->GetID() &&
				c2->GetDuelTarget() == c1->GetID()
				)
				return true;
				else
				return false;*/
			}
			else if(_NPC(mob2))				// client to npc
			{
				/* fall through and swap positions */
			}
			else if(_BECOMENPC(mob2))	// client to becomenpc
			{
				return false;
			}
			else if(_CLIENTCORPSE(mob2))	// client to client corpse
			{
				return true;
			}
			else if(_NPCCORPSE(mob2))	// client to npc corpse
			{
				return false;
			}
#ifdef BOTS
			else if(mob2->IsBot())
				return true;
#endif
		}
		else if(_NPC(mob1))
		{
			if(_CLIENT(mob2))
			{
				return false;
			}
			if(_NPC(mob2))						// npc to npc
			{
				return true;
			}
			else if(_BECOMENPC(mob2))	// npc to becomenpc
			{
				return true;
			}
			else if(_CLIENTCORPSE(mob2))	// npc to client corpse
			{
				return false;
			}
			else if(_NPCCORPSE(mob2))	// npc to npc corpse
			{
				return false;
			}
		}
		else if(_BECOMENPC(mob1))
		{
			if(_BECOMENPC(mob2))			// becomenpc to becomenpc
			{
				return true;
			}
			else if(_CLIENTCORPSE(mob2))	// becomenpc to client corpse
			{
				return false;
			}
			else if(_NPCCORPSE(mob2))	// becomenpc to npc corpse
			{
				return false;
			}
		}
		else if(_CLIENTCORPSE(mob1))
		{
			if(_CLIENTCORPSE(mob2))		// client corpse to client corpse
			{
				return false;
			}
			else if(_NPCCORPSE(mob2))	// client corpse to npc corpse
			{
				return false;
			}
		}
		else if(_NPCCORPSE(mob1))
		{
			if(_NPCCORPSE(mob2))			// npc corpse to npc corpse
			{
				return false;
			}
		}

		// we fell through, now we swap the 2 mobs and run through again once more
		tempmob = mob1;
		mob1 = mob2;
		mob2 = tempmob;
	}
	while( reverse++ == 0 );

	LogFile->write(EQEMuLog::Debug, "Mob::IsBeneficialAllowed: don't have a rule for this - %s to %s\n", this->GetName(), target->GetName());
	return false;
}

bool Mob::CombatRange(Mob* other)
{
	if(!other)
		return(false);

	float size_mod = GetSize();
	float other_size_mod = other->GetSize();

	// self size checks
	// dragon (49), wurm (158), dragon (196)
	if(GetRace() == 49 || GetRace() == 158 || GetRace() == 196) //For races with a fixed size
		size_mod = 60.0f;
	// if size is less than 6 then its defaulted to 8
	else if (size_mod < 6.0)
		size_mod = 8.0f;
	
	// other (target) size checks
	// dragon (49), wurm (158), dragon (196)
	if(other->GetRace() == 49 || other->GetRace() == 158 || other->GetRace() == 196) //For races with a fixed size
		other_size_mod = 60.0f;
	// if size is less than 6 then its defaulted to 8
	else if (other_size_mod < 6.0)
		other_size_mod = 8.0f;

	// take the targets size if it is larger as the radius
	if (other_size_mod > size_mod)
	{
		size_mod = other_size_mod;
	}

	// increase sizes for large NPC's
	if (size_mod > 29)
		size_mod *= size_mod;
	else if (size_mod > 19)
		size_mod *= size_mod * 2;
	else
		size_mod *= size_mod * 3.5f; // eqemu base uses 4x, we reduced to 3.5

	// prevention of ridiculously sized hit boxes
	if (size_mod > 10000)
		size_mod = size_mod / 7;
	
	// pet mod per 2018 S5 request for reduction of size(radius)
	if ( IsPet() && IsNPC() && !IsCharmed() && RuleR(Combat,PetRangeReduction) > 0 && RuleR(Combat,PetRangeReduction) != 100.0 )
		size_mod = size_mod * (float)(RuleR(Combat,PetRangeReduction)/100.0);

	// if our distance squared is less than or equal to the size mod then its in range
	if (DistNoRoot(*other) <= size_mod)
	{
		return true;
	}
	return false;
}

//Old LOS function, prolly not used anymore
//Not removed because I havent looked it over to see if anything
//useful is in here before we delete it.
bool Mob::CheckLos(Mob* other) {
	if (zone->zonemap == 0)
	{
		return true;
	}
	float tmp_x = GetX();
	float tmp_y = GetY();
	float tmp_z = GetZ();
	float trg_x = other->GetX();
	float trg_y = other->GetY();
	float trg_z = other->GetZ();
	float perwalk_x = 0.5;
	float perwalk_y = 0.5;
	float perwalk_z = 0.5;
	float dist_x = tmp_x - trg_x;
	if (dist_x < 0)
		dist_x *= -1;
	float dist_y = tmp_y - trg_y;
	if (dist_y < 0)
		dist_y *= -1;
	float dist_z = tmp_z - trg_z;
	if (dist_z < 0)
		dist_z *= -1;
	if (dist_x  < dist_y && dist_z < dist_y)
	{
		perwalk_x /= (dist_y/dist_x);
		perwalk_z /= (dist_y/dist_z);
	}
	else if (dist_y  < dist_x && dist_z < dist_x)
	{
		perwalk_y /= (dist_x/dist_y);
		perwalk_z /= (dist_x/dist_z);
	}
	else if (dist_x  < dist_z && dist_y < dist_z)
	{
		perwalk_x /= (dist_z/dist_x);
		perwalk_y /= (dist_z/dist_y);
	}
	float steps = (dist_x/perwalk_x + dist_y/perwalk_y + dist_z/perwalk_z)*10; //Just a safety check to prevent endless loops.
	while (steps > 0) {
		steps--;
		if (tmp_x < trg_x)
		{
			if (tmp_x + perwalk_x < trg_x)
				tmp_x += perwalk_x;
			else
				tmp_x = trg_x;
		}
		if (tmp_y < trg_y)
		{
			if (tmp_y + perwalk_y < trg_y)
				tmp_y += perwalk_y;
			else
				tmp_y = trg_y;
		}
		if (tmp_z < trg_z)
		{
			if (tmp_z + perwalk_z < trg_z)
				tmp_z += perwalk_z;
			else
				tmp_z = trg_z;
		}
		if (tmp_x > trg_x)
		{
			if (tmp_x - perwalk_x > trg_x)
				tmp_x -= perwalk_x;
			else
				tmp_x = trg_x;
		}
		if (tmp_y > trg_y)
		{
			if (tmp_y - perwalk_y > trg_y)
				tmp_y -= perwalk_y;
			else
				tmp_y = trg_y;
		}
		if (tmp_z > trg_z)
		{
			if (tmp_z - perwalk_z > trg_z)
				tmp_z -= perwalk_z;
			else
				tmp_z = trg_z;
		}
		if (tmp_y == trg_y && tmp_x == trg_x && tmp_z == trg_z)
		{
			return true;
		}

		//I believe this is contributing to breaking mob spawns when a map is loaded
		//		NodeRef pnode = zone->zonemap->SeekNode( zone->zonemap->GetRoot(), tmp_x, tmp_y );
		NodeRef pnode = NODE_NONE;
		if (pnode != NODE_NONE)
		{
			const int *iface = zone->zonemap->SeekFace( pnode, tmp_x, tmp_y );
			if (*iface == -1) {
				return false;
			}
			float temp_z = 0;
			float best_z = 999999;
			while(*iface != -1)
			{
				temp_z = zone->zonemap->GetFaceHeight( *iface, x_pos, y_pos );
				//UMM.. OMG... sqrtf(pow(x, 2)) == x.... retards
				float best_dist = sqrtf((float)(pow(best_z-tmp_z, 2)));
				float tmp_dist = sqrtf((float)(pow(tmp_z-tmp_z, 2)));
				if (tmp_dist < best_dist)
				{
					best_z = temp_z;
				}
				iface++;
			}
			/*	solar: our aggro code isn't using this right now, just spells, so i'm
			taking out the +-10 check for now to make it work right on hills
			if (best_z - 10 > trg_z || best_z + 10 < trg_z)
			{
			return false;
			}
			*/
		}
	}
	return true;
}


// Image (10/14/2012) KaB/Red69/Zek: Added LOSEntry as a cache to slow down the attempts against los on the same mob in a short time frame
bool Mob::FindLosCacheEntry(Mob* other, bool& inLOS, int32& hits)
{
	if ( other == NULL || !RuleB(Performance,LOSCacheEnabled) )
		return false;

	std::map<int16, LOSEntry>::iterator iter = m_LOSCache.find(other->GetID());
	if ( iter != m_LOSCache.end() )
	{
		if ( iter->second.entry == other ) // this means the entry is valid and we have the right mob
		{
			if ( Timer::GetCurrentTime() < iter->second.lastChecked )
			{
				// Update the entry with the new hits, here we could essentially 'increase' the cache too
				iter->second.hits += 1;
				// if this is true then we already have an entry that is at the same location, save it for another period of time
				if ( iter->second.lastX == other->GetX() &&
					iter->second.lastY == other->GetY() &&
					iter->second.ourLastX == GetX() &&
					iter->second.ourLastY == GetY() )
					iter->second.lastChecked = Timer::GetCurrentTime() + RuleI(Performance, LOSCacheEntryMS);

				inLOS = iter->second.isLOS;
				hits = iter->second.hits;
				return true;
			}
		}
	}

	return false;
}

//Father Nitwit's LOS code
bool Mob::CheckLosFN(Mob* other) {
	bool Result = false;

	if ( other == this )
		return true;

	int32 hits = 0;

	// Image (10/14/2012) KaB/Red69/Zek: Added LOSEntry as a cache to slow down the attempts against los on the same mob in a short time frame
	if ( FindLosCacheEntry( other, Result, hits) )
		return Result;

	if(other)
	{
		Result = CheckLosFN(other->GetX(), other->GetY(), other->GetZ(), other->GetSize());

		if ( RuleB(Performance,LOSCacheEnabled) )
		{
			// Setup the cache entry with the new information
			int16 entID = other->GetID();
			m_LOSCache[entID].entry = other;
			m_LOSCache[entID].isLOS = Result;
			m_LOSCache[entID].lastChecked = Timer::GetCurrentTime() + RuleI(Performance, LOSCacheEntryMS);
			m_LOSCache[entID].hits = 1;
			m_LOSCache[entID].lastX = other->GetX();
			m_LOSCache[entID].lastY = other->GetY();
			m_LOSCache[entID].lastZ = other->GetZ();
			m_LOSCache[entID].ourLastX = GetX();
			m_LOSCache[entID].ourLastY = GetY();
			m_LOSCache[entID].ourLastZ = GetZ();
		}
	}

	return Result;
}

bool Mob::CheckLosFN(float posX, float posY, float posZ, float mobSize) {
	if(zone->zonemap == NULL) {
		//not sure what the best return is on error
		//should make this a database variable, but im lazy today
#ifdef LOS_DEFAULT_CAN_SEE
		return(true);
#else
		return(false);
#endif
	}
	_ZP(Mob_CheckLosFN);

	VERTEX myloc;
	VERTEX oloc;

#define LOS_DEFAULT_HEIGHT 8.0f

	myloc.x = GetX();
	myloc.y = GetY();
	myloc.z = GetZ() + (GetSize()==0.0?LOS_DEFAULT_HEIGHT:GetSize())/2 * HEAD_POSITION;

	oloc.x = posX;
	oloc.y = posY;
	oloc.z = posZ + (mobSize==0.0?LOS_DEFAULT_HEIGHT:mobSize)/2 * SEE_POSITION;

#if LOSDEBUG>=5
	LogFile->write(EQEMuLog::Debug, "LOS from (%.2f, %.2f, %.2f) to (%.2f, %.2f, %.2f) sizes: (%.2f, %.2f)", myloc.x, myloc.y, myloc.z, oloc.x, oloc.y, oloc.z, GetSize(), mobSize);
#endif

	FACE *onhit;
	NodeRef mynode;
	NodeRef onode;

	VERTEX hit;
	//see if anything in our node is in the way
	mynode = zone->zonemap->SeekNode( zone->zonemap->GetRoot(), myloc.x, myloc.y);
	if(mynode != NODE_NONE) {
		if(zone->zonemap->LineIntersectsNode(mynode, myloc, oloc, &hit, &onhit)) {
#if LOSDEBUG>=5
			LogFile->write(EQEMuLog::Debug, "Check LOS for %s target position, cannot see.", GetName());
			LogFile->write(EQEMuLog::Debug, "\tPoly: (%.2f, %.2f, %.2f) (%.2f, %.2f, %.2f) (%.2f, %.2f, %.2f)\n",
				onhit->a.x, onhit->a.y, onhit->a.z,
				onhit->b.x, onhit->b.y, onhit->b.z, 
				onhit->c.x, onhit->c.y, onhit->c.z);
#endif
			return(false);
		}
	}
#if LOSDEBUG>=5
	else {
		LogFile->write(EQEMuLog::Debug, "WTF, I have no node, what am I standing on??? (%.2f, %.2f).", myloc.x, myloc.y);
	}
#endif

	//see if they are in a different node.
	//if so, see if anything in their node is blocking me.
	if(! zone->zonemap->LocWithinNode(mynode, oloc.x, oloc.y)) {
		onode = zone->zonemap->SeekNode( zone->zonemap->GetRoot(), oloc.x, oloc.y);
		if(onode != NODE_NONE && onode != mynode) {
			if(zone->zonemap->LineIntersectsNode(onode, myloc, oloc, &hit, &onhit)) {
#if LOSDEBUG>=5
				LogFile->write(EQEMuLog::Debug, "Check LOS for %s target position, cannot see (2).", GetName());
				LogFile->write(EQEMuLog::Debug, "\tPoly: (%.2f, %.2f, %.2f) (%.2f, %.2f, %.2f) (%.2f, %.2f, %.2f)\n",
					onhit->a.x, onhit->a.y, onhit->a.z,
					onhit->b.x, onhit->b.y, onhit->b.z, 
					onhit->c.x, onhit->c.y, onhit->c.z);
#endif
				return(false);
			}
		}
#if LOSDEBUG>=5
		else if(onode == NODE_NONE) {
			LogFile->write(EQEMuLog::Debug, "WTF, They have no node, what are they standing on??? (%.2f, %.2f).", myloc.x, myloc.y);
		}
#endif
	}

	/*
	if(zone->zonemap->LineIntersectsZone(myloc, oloc, CHECK_LOS_STEP, &onhit)) {
	#if LOSDEBUG>=5
	LogFile->write(EQEMuLog::Debug, "Check LOS for %s target %s, cannot see.", GetName(), other->GetName() );
	LogFile->write(EQEMuLog::Debug, "\tPoly: (%.2f, %.2f, %.2f) (%.2f, %.2f, %.2f) (%.2f, %.2f, %.2f)\n",
	onhit->a.x, onhit->a.y, onhit->a.z,
	onhit->b.x, onhit->b.y, onhit->b.z, 
	onhit->c.x, onhit->c.y, onhit->c.z);
	#endif
	return(false);
	}*/

#if LOSDEBUG>=5
	LogFile->write(EQEMuLog::Debug, "Check LOS for %s target position, CAN SEE.", GetName());
#endif

	return(true);
}

//offensive spell aggro
sint32 Mob::CheckAggroAmount(int16 spellid) {
	int16 spell_id = spellid;
	sint32 AggroAmount = 0;
	sint32 nonModifiedAggro = 0;
	int16 slevel = GetLevel();

	for (int o = 0; o < EFFECT_COUNT; o++) {
		switch(spells[spell_id].effectid[o]) {
		case SE_CurrentHPOnce:
		case SE_CurrentHP:{
			int val = CalcSpellEffectValue_formula(spells[spell_id].formula[o], spells[spell_id].base[o], spells[spell_id].max[o], this->GetLevel(), spell_id);
			if(val < 0)
				AggroAmount -= val;
			break;
						  }
		case SE_MovementSpeed: {
			int val = CalcSpellEffectValue_formula(spells[spell_id].formula[o], spells[spell_id].base[o], spells[spell_id].max[o], this->GetLevel(), spell_id);
			if (val < 0)
			{
				AggroAmount += (2 + ((slevel * slevel) / 8));
				break;
			}
			break;
							   }
		case SE_AttackSpeed:
		case SE_AttackSpeed2:
		case SE_AttackSpeed3:{
			int val = CalcSpellEffectValue_formula(spells[spell_id].formula[o], spells[spell_id].base[o], spells[spell_id].max[o], this->GetLevel(), spell_id);
			if (val < 100)
			{
				AggroAmount += (5 + ((slevel * slevel) / 5));
			}
			break;
							 }
		case SE_Stun: {
			AggroAmount += (5 + ((slevel * slevel) / 6));
			break;
					  }
		case SE_Blind: {
			AggroAmount += (5 + ((slevel * slevel) / 6));
			break;
					   }
		case SE_Mez: {
			AggroAmount += (5 + ((slevel * slevel) / 5));
			break;
					 }
		case SE_Charm: {
			AggroAmount += (5 + ((slevel * slevel) / 5));
			break;
					   }
		case SE_Root: {
			AggroAmount += (2 + ((slevel * slevel) / 8));
			break;
					  }
		case SE_Fear: {
			AggroAmount += (5 + ((slevel * slevel) / 6));
			break;
					  }
		case SE_ATK:
		case SE_ArmorClass:	{
			int val = CalcSpellEffectValue_formula(spells[spell_id].formula[o], spells[spell_id].base[o], spells[spell_id].max[o], this->GetLevel(), spell_id);
			if (val < 0)
			{
				AggroAmount -= val*2;			
			}				
			break;
							}
		case SE_ResistMagic:
		case SE_ResistFire:
		case SE_ResistCold:
		case SE_ResistPoison:
		case SE_ResistDisease:{
			int val = CalcSpellEffectValue_formula(spells[spell_id].formula[o], spells[spell_id].base[o], spells[spell_id].max[o], this->GetLevel(), spell_id);
			if (val < 0)
			{
				AggroAmount -= val*3;
			}
			break;
							  }
		case SE_ResistAll:{
			int val = CalcSpellEffectValue_formula(spells[spell_id].formula[o], spells[spell_id].base[o], spells[spell_id].max[o], this->GetLevel(), spell_id);
			if (val < 0)
			{
				AggroAmount -= val*6;
			}
			break;
						  }
		case SE_STR:
		case SE_STA:
		case SE_DEX:
		case SE_AGI:
		case SE_INT:
		case SE_WIS:
		case SE_CHA:{
			int val = CalcSpellEffectValue_formula(spells[spell_id].formula[o], spells[spell_id].base[o], spells[spell_id].max[o], this->GetLevel(), spell_id);
			if (val < 0)
			{
				AggroAmount -= val*2;
			}
			break;
					}
		case SE_AllStats:{
			int val = CalcSpellEffectValue_formula(spells[spell_id].formula[o], spells[spell_id].base[o], spells[spell_id].max[o], this->GetLevel(), spell_id);
			if (val < 0)
			{
				AggroAmount -= val*6;
			}
			break;
						 }
		case SE_BardAEDot:{
			AggroAmount += slevel*2;
			break;
						  }
		case SE_SpinTarget:{
			AggroAmount += (5 + ((slevel * slevel) / 5));
			break;
						   }
		case SE_Amnesia:
		case SE_Silence:{
			AggroAmount += slevel*2;			
			break;
						}
		case SE_Destroy:{
			AggroAmount += slevel*2;
			break;
						}
		case SE_Harmony:
		case SE_CastingLevel:
		case SE_MeleeMitigation:
		case SE_CriticalHitChance:
		case SE_AvoidMeleeChance:
		case SE_RiposteChance:
		case SE_DodgeChance:
		case SE_ParryChance:
		case SE_DualWieldChance:
		case SE_DoubleAttackChance:
		case SE_MeleeSkillCheck:
		case SE_HitChance:
		case SE_DamageModifier:
		case SE_MinDamageModifier:
		case SE_IncreaseBlockChance:
		case SE_Accuracy:
		case SE_DamageShield:
		case SE_SpellDamageShield:
		case SE_ReverseDS:{
			AggroAmount += slevel*2;
			break;
						  }
		case SE_CurrentMana:	
		case SE_ManaPool:
		case SE_CurrentEndurance:{
			int val = CalcSpellEffectValue_formula(spells[spell_id].formula[o], spells[spell_id].base[o], spells[spell_id].max[o], this->GetLevel(), spell_id);
			if (val < 0)
			{
				AggroAmount -= val*2;
			}
			break;
								 }
		case SE_CancelMagic:
		case SE_DispelDetrimental:{
			AggroAmount += slevel;			
			break;
								  }
		case SE_Calm:{
			int val = CalcSpellEffectValue_formula(spells[spell_id].formula[o], spells[spell_id].base[o], spells[spell_id].max[o], this->GetLevel(), spell_id);
			nonModifiedAggro = val;
			break;
					 }
		}
	}

	if(IsAEDurationSpell(spell_id))
	{
		AggroAmount /= 2;
	}

	if(spells[spell_id].HateAdded > 0)
	{
		AggroAmount = spells[spell_id].HateAdded;
	}

	if (IsBardSong(spell_id))
		AggroAmount = AggroAmount * RuleI(Aggro, SongAggroMod) / 100;
	if (GetOwner())
		AggroAmount = AggroAmount * RuleI(Aggro, PetSpellAggroMod) / 100;

	if(AggroAmount > 0)
	{

		int HateMod = RuleI(Aggro, SpellAggroMod);

		if(IsClient())
		{
			HateMod += CastToClient()->GetFocusEffect(focusSpellHateMod, spell_id);
		}

		int aaSubtlety = ( GetAA(aaSpellCastingSubtlety) > GetAA(aaSpellCastingSubtlety2) ) ? GetAA(aaSpellCastingSubtlety) : GetAA(aaSpellCastingSubtlety2);

		switch (aaSubtlety)
		{
		case 0:
			break;
		case 1:
			HateMod -= 5;
		case 2:
			HateMod -= 10;
		case 3:
			HateMod -= 20;
			break;
		}

		AggroAmount = (AggroAmount * HateMod) / 100;

		//made up number probably scales a bit differently on live but it seems like it will be close enough
		//every time you cast on live you get a certain amount of "this is a spell" aggro
		//confirmed by EQ devs to be 100 exactly at level 85. From their wording it doesn't seem like it's affected
		//by hate modifiers either.
		//AggroAmount += (slevel*slevel/72); 
		// Saved so I can reimplement it; 
		// this should only be on the spell to aggro the npc not every spell

	}



	AggroAmount += spells[spell_id].bonushate + nonModifiedAggro;
	return AggroAmount;
}

//healing and buffing aggro
sint32 Mob::CheckHealAggroAmount(int16 spellid, int32 heal_possible) {
	int16 spell_id = spellid;
	sint32 AggroAmount = 0;

	for (int o = 0; o < EFFECT_COUNT; o++) {
		switch(spells[spell_id].effectid[o]) {
		case SE_CurrentHP: 
		case SE_Rune: {
			AggroAmount += spells[spell_id].mana;
			break;
					  }
		case SE_HealOverTime:{
			AggroAmount += CalcSpellEffectValue_formula(spells[spell_id].formula[o], spells[spell_id].base[o], spells[spell_id].max[o], this->GetLevel(), spell_id);
			break;
							 }
		default:{
			break;
				}
		}
	}
	if (IsBardSong(spell_id))
		AggroAmount = AggroAmount * RuleI(Aggro, SongAggroMod) / 100;
	if (GetOwner())
		AggroAmount = AggroAmount * RuleI(Aggro, PetSpellAggroMod) / 100;

	if(AggroAmount > 0)
	{
		int HateMod = RuleI(Aggro, SpellAggroMod);

		if(IsClient())
		{
			HateMod += CastToClient()->GetFocusEffect(focusSpellHateMod, spell_id);
		}

		int aaSubtlety = ( GetAA(aaSpellCastingSubtlety) > GetAA(aaSpellCastingSubtlety2) ) ? GetAA(aaSpellCastingSubtlety) : GetAA(aaSpellCastingSubtlety2);

		switch (aaSubtlety)
		{
		case 0:
			break;
		case 1:
			HateMod -= 5;
		case 2:
			HateMod -= 10;
		case 3:
			HateMod -= 20;
			break;
		}

		AggroAmount = (AggroAmount * HateMod) / 100;

		//made up number probably scales a bit differently on live but it seems like it will be close enough
		//every time you cast on live you get a certain amount of "this is a spell" aggro
		//confirmed by EQ devs to be 100 exactly at level 85. From their wording it doesn't seem like it's affected
		//by hate modifiers either.
		//AggroAmount += (slevel*slevel/72); // Moved Below


	}


	if(AggroAmount < 0)
		return 0;
	else
		return AggroAmount;
}

void Mob::AddFeignMemory(Client* attacker) {
	if(feign_memory_list.empty() && AIfeignremember_timer != NULL)
		AIfeignremember_timer->Start(AIfeignremember_delay);
	feign_memory_list.insert(attacker->CharacterID());
}

void Mob::RemoveFromFeignMemory(Client* attacker) {
	feign_memory_list.erase(attacker->CharacterID());
	if(feign_memory_list.empty() && AIfeignremember_timer != NULL)
		AIfeignremember_timer->Disable();
	if(feign_memory_list.empty())
	{
		minLastFightingDelayMoving = RuleI(NPC, LastFightingDelayMovingMin);
		maxLastFightingDelayMoving = RuleI(NPC, LastFightingDelayMovingMax);
		if(AIfeignremember_timer != NULL)
			AIfeignremember_timer->Disable();
	}
}

void Mob::ClearFeignMemory() {
	feign_memory_list.clear();
	minLastFightingDelayMoving = RuleI(NPC, LastFightingDelayMovingMin);
	maxLastFightingDelayMoving = RuleI(NPC, LastFightingDelayMovingMax);
	if(AIfeignremember_timer != NULL)
		AIfeignremember_timer->Disable();
}

bool Mob::PassCharismaCheck(Mob* caster, Mob* spellTarget, int16 spell_id, int32 tics_remaining) {
	bool Result = false;

	if(!caster) return false;

	if(spells[spell_id].ResistDiff <= -600)
		return true;

	double minValue = -15;

	bool isNecroSpell = false;
	if ( ( spell_id > 195 && spell_id < 199 ) || spell_id == 1624 || spell_id == 1629 )
	{
		isNecroSpell = true;
		minValue = 15;
	}

	double modTics = (double)(tics_remaining % 11);
	int lvlDiff = (spellTarget->GetLevel() - caster->GetLevel()) + 1;

	if ( isNecroSpell )
		lvlDiff *= -1;

	float r1 = ((((float)spellTarget->GetMR() + spellTarget->GetLevel()) / 3) / spellTarget->GetMaxMR()) + ((float)MakeRandomFloat(minValue, modTics + lvlDiff) / 100.0f);
	float r2 = 0.0f;

	if(IsCharmSpell(spell_id)) {
		// Assume this is a charm spell
		int32 TotalDominationRank = 0.00f;
		float TotalDominationBonus = 0.00f;

		if(caster->IsClient())
			TotalDominationRank = caster->CastToClient()->GetAA(aaTotalDomination);

		// WildcardX: If someone ever finds for certain what value the TotalDomination ranks provide, please change the values
		// I implemented below.

		switch(TotalDominationRank) {
		case 1 :
			TotalDominationBonus = 0.05f;
			break;
		case 2 :
			TotalDominationBonus = 0.10f;
			break;
		case 3 :
			TotalDominationBonus = 0.15f;
			break;
		default :
			TotalDominationBonus = 0.00f;
		}

		if ( tics_remaining > 0 )
		{
			double ticsToUse = (double)tics_remaining;
			if ( ticsToUse > 10 )
			{
				ticsToUse = (double)(tics_remaining % 9);
				ticsToUse += (modTics/1.1);
			}

			double hpRatio = (double)caster->GetHP() / ((double)caster->GetMaxHP()+1.0);

			if ( hpRatio > .75 )
				hpRatio = 1;
			else
				hpRatio = hpRatio + .2;

			//printf("hpratio: %f\n",hpRatio);
			if ( isNecroSpell )
				r2 = ( ((((float)caster->GetCHA()  + caster->GetLevel()) / 5) / caster->GetMaxCHA()) - ((float)MakeRandomFloat(-5, ticsToUse) / 100.0f) + TotalDominationBonus ) * hpRatio;
			else
				r2 = ( ((((float)caster->GetCHA()  + caster->GetLevel()) / 3.5) / caster->GetMaxCHA()) + ((float)MakeRandomFloat(2, ticsToUse) / 100.0f) + TotalDominationBonus ) * hpRatio;
		}
		else
			r2 = ((((float)caster->GetCHA()  + caster->GetLevel()) / 4) / caster->GetMaxCHA()) + ((float)MakeRandomFloat(0, 10) / 100.0f) + TotalDominationBonus;
	}
	else
		// Assume this is a harmony/pacify spell
		r2 = ((((float)caster->GetCHA()  + caster->GetLevel()) / 3) / caster->GetMaxCHA()) + ((float)MakeRandomFloat(-2, (modTics+1)) / 100.0f);

	//printf("(r1: %f < r2: %f)\n",r1,r2);
	if ( isNecroSpell ) // dominate(196)/beguile(197)/cajole(198) undead
	{
		//printf("(r1: %f > r2: %f)\n",r1,r2);
		if(r1 > r2)
			Result = true;
	}
	else if(r1 < r2)
		Result = true;

	return Result;
}



