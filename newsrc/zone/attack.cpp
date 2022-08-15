/*  EQEMu:  Everquest Server Emulator
Copyright (C) 2001-2002  EQEMu Development Team (http://eqemulator.net)

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

#if EQDEBUG >= 5
//#define ATTACK_DEBUG 20
#endif

#include "../common/debug.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
using namespace std;
#include <assert.h>

#include "masterentity.h"
#include "NpcAI.h"
#include "../common/packet_dump.h"
#include "../common/eq_packet_structs.h"
#include "../common/eq_constants.h"
#include "../common/skills.h"
#include "spdat.h"
#include "zone.h"
#include "StringIDs.h"
#include "../common/MiscFunctions.h"
#include "../common/rulesys.h"
#include "worldserver.h"
#include "../common/misc.h"

#include "../KingsBandits/KingsBandits.h"

#ifdef WIN32
#define snprintf	_snprintf
#define strncasecmp	_strnicmp
#define strcasecmp  _stricmp
#endif

extern WorldServer worldserver;
extern EntityList entity_list;
#if !defined(NEW_LoadSPDat) && !defined(DB_LoadSPDat)
extern SPDat_Spell_Struct spells[SPDAT_RECORDS];
#endif

extern Zone* zone;

bool Mob::AttackAnimation(SkillType &skillinuse, int Hand, const ItemInst* weapon)
{
	// Determine animation
	int type = 0;
	if (weapon && weapon->IsType(ItemClassCommon)) {
		const Item_Struct* item = weapon->GetItem();
#if EQDEBUG >= 11
		LogFile->write(EQEMuLog::Debug, "Weapon skill:%i", item->ItemType);
#endif		
		switch (item->ItemType)
		{
		case ItemType1HS: // 1H Slashing
			{
				skillinuse = _1H_SLASHING;
				type = anim1HWeapon;
				break;
			}
		case ItemType2HS: // 2H Slashing
			{
				skillinuse = _2H_SLASHING;
				type = anim2HSlashing;
				break;
			}
		case ItemTypePierce: // Piercing
			{
				skillinuse = PIERCING;
				type = animPiercing;
				break;
			}
		case ItemType1HB: // 1H Blunt
			{
				skillinuse = _1H_BLUNT;
				type = anim1HWeapon;
				break;
			}
		case ItemType2HB: // 2H Blunt
			{
				skillinuse = _2H_BLUNT;
				type = anim2HWeapon;
				break;
			}
		case ItemType2HPierce: // 2H Piercing
			{
				skillinuse = PIERCING;
				type = anim2HWeapon;
				break;
			}
		case ItemTypeHand2Hand:
			{
				skillinuse = HAND_TO_HAND;
				type = animHand2Hand;
				break;
			}
		default:
			{
				skillinuse = HAND_TO_HAND;
				type = animHand2Hand;
				break;
			}
		}// switch
	}
	else if(IsNPC()) {

		// Kings & Bandits - melee texture fix
		if ( Hand == 13 && CastToNPC()->GetMeleeTexture1() > 0 )
		{
			switch(CastToNPC()->GetMeleeTexture1())
			{
			case 2: // 2h slash
			case 11: // 2h sword (nice old markings)
			case 12: // 2h staff (shammy epic)
			case 13: // crystal staff with spikes (2h/1h?)
			case 22: // nice sword with line down middle green handle (2h/1h?)
			case 33: // large hammer (1h/2h?)
			case 49: // large dark hammer (1h/2h?)
			case 53: // large black sword (1h/2h?)
			case 62: // flaming sword (1h/2h?)
			case 80: // large axe 2 sided
			case 88: // large 2h weapon
				{
					type = anim2HSlashing;
					CastToNPC()->SetPrimSkill(_2H_SLASHING);
					break;
				}
			case 4: // bow
				{
					type = animShootBow;
					CastToNPC()->SetPrimSkill(ARCHERY);
					break;
				}
			case 5: // dagger
			case 16: // 1h spear
			case 17: // 1h spear
			case 20: // 1h lance
			case 23: // 2h spear (long)
			case 30: // twin spike spear
			case 50: // tiny dagger
			case 79: // 3 spiked spear
				{
					type = animPiercing;
					CastToNPC()->SetPrimSkill(PIERCING);
					break;
				}
			default:
				{
					type = anim1HWeapon;
					CastToNPC()->SetPrimSkill(_1H_SLASHING);
					break;
				}
			} // end of switch

			if ( CastToNPC()->GetMeleeTexture2() > 0 && type == anim2HWeapon )
				type = anim1HWeapon;

		} // end of primary melee
		else if ( CastToNPC()->GetMeleeTexture2() > 0 )
		{
			switch(CastToNPC()->GetMeleeTexture1())
			{
			case 4: // bow
				{
					type = animShootBow;
					CastToNPC()->SetPrimSkill(ARCHERY);
					break;
				}
			case 5: // dagger
			case 16: // 1h spear
			case 17: // 1h spear
			case 20: // 1h lance
			case 23: // 2h spear (long)
			case 30: // twin spike spear
			case 50: // tiny dagger
			case 79: // 3 spiked spear
				{
					type = animPiercing;
					CastToNPC()->SetPrimSkill(PIERCING);
					break;
				}
			default:
				{
					type = anim1HWeapon;
					CastToNPC()->SetPrimSkill(_1H_SLASHING);
					break;
				}
			} // end of switch

		} // end of secondary melee check

		if ( type < 1 )
		{
			switch (skillinuse)
			{
			case _1H_SLASHING: // 1H Slashing
				{
					type = anim1HWeapon;
					break;
				}
			case _2H_SLASHING: // 2H Slashing
				{
					type = anim2HSlashing;
					break;
				}
			case PIERCING: // Piercing
				{
					type = animPiercing;
					break;
				}
			case _1H_BLUNT: // 1H Blunt
				{
					type = anim1HWeapon;
					break;
				}
			case _2H_BLUNT: // 2H Blunt
				{
					type = anim2HWeapon;
					break;
				}
			case 99: // 2H Piercing
				{
					type = anim2HWeapon;
					break;
				}
			case HAND_TO_HAND:
				{
					type = animHand2Hand;
					break;
				}
			default:
				{
					type = animHand2Hand;
					break;
				}
			}// switch
		} // end of non melee texdture check

	}
	else {
		skillinuse = HAND_TO_HAND;
		type = animHand2Hand;
	}

	// If we're attacking with the secondary hand, play the dual wield anim
	if (Hand == 14)	// DW anim
		type = animDualWield;

	DoAnim(type);
	return true;
}

// called when a mob is attacked, does the checks to see if it's a hit
// and does other mitigation checks.  'this' is the mob being attacked.
bool Mob::CheckHitChance(Mob* other, SkillType skillinuse, int Hand)
{
	/*
	Reworked a lot of this code to achieve better balance at higher levels.
	The old code basically meant that any in high level (50+) combat,
	both parties always had 95% chance to hit the other one.
	*/
	Mob *attacker=other;
	Mob *defender=this;
	float chancetohit = RuleR(Combat, BaseHitChance);
	
	if(attacker->IsNPC() && !attacker->IsPet())
		chancetohit += RuleR(Combat, NPCBonusHitChance);
	
	if(skillinuse == BACKSTAB)
		{
			if ( other && other->IsClient() && IsClient() )
				chancetohit += RuleR(PVPCombat, BackstabHitChanceBonus);
			else
				chancetohit += RuleR(Combat, BackstabHitChanceBonusPVE);
		}
#if ATTACK_DEBUG>=11
	LogFile->write(EQEMuLog::Debug, "CheckHitChance(%s) attacked by %s", defender->GetName(), attacker->GetName());
#endif

	bool pvpmode = false;
	if(IsClient() && other->IsClient())
		pvpmode = true;

	float bonus;

	////////////////////////////////////////////////////////
	// To hit calcs go here
	////////////////////////////////////////////////////////

	int8 attacker_level = attacker->GetLevel() ? attacker->GetLevel() : 1;
	int8 defender_level = defender->GetLevel() ? defender->GetLevel() : 1;

	//Calculate the level difference

	mlog(COMBAT__TOHIT, "Chance to hit before level diff calc %.2f", chancetohit);
	double level_difference = attacker_level - defender_level;
	double range = defender->GetLevel();
	range = ((range / 3) + 3);

	if(level_difference < 0)
	{
		if(level_difference >= -range)
		{
			if ( attacker->IsNPC() )
				range *= 2;

			chancetohit += (level_difference / range) * RuleR(Combat,HitFalloffMinor); //5
		}
		else if (level_difference >= -(range+3.0))
		{
			chancetohit -= RuleR(Combat,HitFalloffMinor);
			chancetohit += ((level_difference+range) / (3.0)) * RuleR(Combat,HitFalloffModerate); //7
		}
		else
		{
			chancetohit -= (RuleR(Combat,HitFalloffMinor) + RuleR(Combat,HitFalloffModerate));
			chancetohit += ((level_difference+range+3.0)/12.0) * RuleR(Combat,HitFalloffMajor); //50
		}
	}
	else
	{
		chancetohit += (RuleR(Combat,HitBonusPerLevel) * level_difference); 
	}

	mlog(COMBAT__TOHIT, "Chance to hit after level diff calc %.2f", chancetohit);

	chancetohit -= ((float)defender->GetAGI() * RuleR(Combat, AgiHitFactor));

	mlog(COMBAT__TOHIT, "Chance to hit after agil calc %.2f", chancetohit);

	if(attacker->IsClient())
	{
		// Kings & Bandits - WAR ROGUE MONK change for chance to hit (hitchance)
		switch(attacker->GetClass())
		{
		case WARRIOR:
			chancetohit -= ((RuleR(Combat,WeaponSkillFalloff)*.10) * (attacker->CastToClient()->MaxSkill(skillinuse) - attacker->GetSkill(skillinuse)));
			break;
		case ROGUE:
		case RANGER:
		case MONK:
		case BARD:
		case BEASTLORD:
			{
				if ( skillinuse == ARCHERY && attacker->GetClass() == RANGER )
					chancetohit += ((RuleR(Combat,WeaponSkillFalloff)*.10) * (attacker->CastToClient()->MaxSkill(skillinuse) - attacker->GetSkill(skillinuse)));
				else
					chancetohit -= ((RuleR(Combat,WeaponSkillFalloff)*.10) * (attacker->CastToClient()->MaxSkill(skillinuse) - attacker->GetSkill(skillinuse)));
				break;
			}
		case SHADOWKNIGHT:
		case PALADIN:
		case BERSERKER:
			{
				chancetohit -= ((RuleR(Combat,WeaponSkillFalloff)*.10) * (attacker->CastToClient()->MaxSkill(skillinuse) - attacker->GetSkill(skillinuse)));
				break;
			}
		default:
			{
				chancetohit -= ((RuleR(Combat,WeaponSkillFalloff)*.125) * (attacker->CastToClient()->MaxSkill(skillinuse) - attacker->GetSkill(skillinuse)));
				break;
			}
		}

		mlog(COMBAT__TOHIT, "Chance to hit after weapon falloff calc (attack) %.2f", chancetohit);
	}

	if(defender->IsClient())
	{
		chancetohit += (RuleR(Combat,WeaponSkillFalloff) * (defender->CastToClient()->MaxSkill(DEFENSE) - defender->GetSkill(DEFENSE)));
		mlog(COMBAT__TOHIT, "Chance to hit after weapon falloff calc (defense) %.2f", chancetohit);
	}

	//I dont think this is 100% correct, but at least it does something...
	if(attacker->spellbonuses.MeleeSkillCheckSkill == skillinuse || attacker->spellbonuses.MeleeSkillCheckSkill == 255) {
		chancetohit += attacker->spellbonuses.MeleeSkillCheck;
		mlog(COMBAT__TOHIT, "Applied spell melee skill bonus %d, yeilding %.2f", attacker->spellbonuses.MeleeSkillCheck, chancetohit);
	}
	if(attacker->itembonuses.MeleeSkillCheckSkill == skillinuse || attacker->itembonuses.MeleeSkillCheckSkill == 255) {
		chancetohit += attacker->itembonuses.MeleeSkillCheck;
		mlog(COMBAT__TOHIT, "Applied item melee skill bonus %d, yeilding %.2f", attacker->spellbonuses.MeleeSkillCheck, chancetohit);
	}

	//subtract off avoidance by the defender
	bonus = defender->spellbonuses.AvoidMeleeChance + defender->itembonuses.AvoidMeleeChance;
	if(bonus > 0) {
		chancetohit -= ((bonus * chancetohit) / 1000);
		mlog(COMBAT__TOHIT, "Applied avoidance chance %.2f/10, yeilding %.2f", bonus, chancetohit);
	}

	if(attacker->IsNPC())
		chancetohit += (chancetohit * attacker->CastToNPC()->GetAccuracyRating() / 1000);

	mlog(COMBAT__TOHIT, "Chance to hit after accuracy rating calc %.2f", chancetohit);

	uint16 AA_mod = 0;
	switch(GetAA(aaCombatAgility))
	{
	case 1:
		AA_mod = 2;
		break;
	case 2:
		AA_mod = 5;
		break;
	case 3:
		AA_mod = 10;
		break;
	}
	AA_mod += 3*GetAA(aaPhysicalEnhancement);
	AA_mod += 2*GetAA(aaLightningReflexes);
	AA_mod += GetAA(aaReflexiveMastery);
	chancetohit -= chancetohit * AA_mod / 100;

	mlog(COMBAT__TOHIT, "Chance to hit after AA calc %.2f", chancetohit);

	//add in our hit chance bonuses if we are using the right skill
	//does the hit chance cap apply to spell bonuses from disciplines?
	float hitBonus = 0;
	if(attacker->spellbonuses.HitChanceSkill == 255 || attacker->spellbonuses.HitChanceSkill == skillinuse)
	{
		hitBonus = (attacker->spellbonuses.HitChance / 15.0f > attacker->spellbonuses.Accuracy) ? attacker->spellbonuses.HitChance / 15.0f : attacker->spellbonuses.Accuracy;
		chancetohit += chancetohit * hitBonus / 100;
		mlog(COMBAT__TOHIT, "Applied spell melee hit chance %.2f, yeilding %.2f", hitBonus, chancetohit);
	}
	else if(attacker->spellbonuses.Accuracy) {
		hitBonus = attacker->spellbonuses.Accuracy;
		chancetohit += chancetohit * hitBonus / 100;
		mlog(COMBAT__TOHIT, "Applied spell melee accuracy chance %.2f, yeilding %.2f", hitBonus, chancetohit);
	}

	hitBonus = 0;
	if(attacker->itembonuses.HitChanceSkill == 255 || attacker->itembonuses.HitChanceSkill == skillinuse)
	{
		hitBonus = (attacker->itembonuses.HitChance / 15.0f > attacker->itembonuses.Accuracy) ? attacker->itembonuses.HitChance / 15.0f : attacker->itembonuses.Accuracy;
		chancetohit += chancetohit * hitBonus / 100;
		mlog(COMBAT__TOHIT, "Applied item melee hit chance %.2f, yeilding %.2f", hitBonus, chancetohit);
	} 
	else if(attacker->itembonuses.Accuracy)
	{
		hitBonus = attacker->itembonuses.Accuracy;
		chancetohit += chancetohit * hitBonus / 100;
		mlog(COMBAT__TOHIT, "Applied item melee accuracy chance %.2f, yeilding %.2f", hitBonus, chancetohit);
	}

	if (attacker->IsClient()) {
		int modAA = 100;
		switch (attacker->CastToClient()->GetAA(aaPrecisionofthePathfinder)) {
		case 1:
			modAA += 2;
			break;
		case 2:
			modAA += 4;
			break;
		case 3:
			modAA += 6;
			break;
		}
		chancetohit = ((chancetohit * modAA) / 100);
	}
	//Wolftousen - Add Berserker Dead Aim AA accuracy bonus for throwing
	if(skillinuse == THROWING)
	{
		switch(GetAA(aaDeadAim))
		{
		case 1:
			chancetohit = chancetohit * 105/100;
			break;
		case 2:
			chancetohit = chancetohit * 110/100;
			break;
		case 3:
			chancetohit = chancetohit * 115/100;
			break;
		}
	}

	if(skillinuse == ARCHERY)
		chancetohit -= (chancetohit * .10f * RuleR(Combat, ArcheryHitPenalty)) / 100.0f;

	if ( other && other->IsClient() && other->CastToClient()->IsSitting() )
		chancetohit = 1001;
	
	// Chance to hit;   Max 95%, Min 30%
	if(chancetohit > 1000) {
		//if chance to hit is crazy high, that means a discipline is in use, and let it stay there
	} 
	else if(chancetohit > 95) {
		chancetohit = 95;
	} 
	else if(chancetohit < 5) {
		chancetohit = 5;
	}

	//I dont know the best way to handle a garunteed hit discipline being used
	//agains a garunteed riposte (for example) discipline... for now, garunteed hit wins


#if EQDEBUG>=11
	LogFile->write(EQEMuLog::Debug, "3 FINAL calculated chance to hit is: %5.2f", chancetohit);
#endif

	//
	// Did we hit?
	//

	float tohit_roll = MakeRandomFloat(0, 100);

	mlog(COMBAT__TOHIT, "Final hit chance: %.2f%%. Hit roll %.2f", chancetohit, tohit_roll);

	return(tohit_roll <= chancetohit);
}

bool Mob::AvoidDamage(Mob* other, sint32 &damage)
{
	/* solar: called when a mob is attacked, does the checks to see if it's a hit
	*  and does other mitigation checks.  'this' is the mob being attacked.
	* 
	* special return values:
	*    -1 - block
	*    -2 - parry
	*    -3 - riposte
	*    -4 - dodge
	* 
	*/
	float skill;
	float bonus;
	float RollTable[4] = {-1.0f,-1.0f,-1.0f,-1.0f};
	Mob *attacker=other;
	Mob *defender=this;

	//garunteed hit
	bool ghit = false;
	if((attacker->spellbonuses.MeleeSkillCheck + attacker->itembonuses.MeleeSkillCheck) > 500)
		ghit = true;

	//////////////////////////////////////////////////////////
	// make enrage same as riposte
	/////////////////////////////////////////////////////////
	if (IsEnraged() && !other->BehindMob(this, other->GetX(), other->GetY())) {
		damage = -3;
		mlog(COMBAT__DAMAGE, "I am enraged, riposting frontal attack.");
	}

	/////////////////////////////////////////////////////////
	// riposte
	/////////////////////////////////////////////////////////
	float riposte_chance = 0.0f;
	if (damage > 0 && CanThisClassRiposte() && !other->BehindMob(this, other->GetX(), other->GetY()))
	{
		riposte_chance = (100.0f + (float)defender->spellbonuses.RiposteChance + (float)defender->itembonuses.RiposteChance) / 100.0f;
		riposte_chance /= 50.0f;
		riposte_chance += GetDEX()/200;

		skill = GetSkill(RIPOSTE);
		riposte_chance += skill/60.0;

		if (IsClient()) {
			CastToClient()->CheckIncreaseSkill(RIPOSTE, other, -10);
//devn00b if they use riposte disc we need to make sure it works every hit
		if (spellbonuses.RiposteChance == 10000 || aabonuses.RiposteChance == 10000 || itembonuses.RiposteChance == 10000) {
                damage = -3;
                mlog(COMBAT__DAMAGE, "I am using a DISC, riposting frontal attack.");
                }
		}

		if (!ghit) {	//if they are not using a garunteed hit discipline
			RollTable[0] = riposte_chance;
		}
	}

	///////////////////////////////////////////////////////	
	// block
	///////////////////////////////////////////////////////

	bool bBlockFromRear = false;

	if (this->IsClient()) {
		float aaChance = 0;

		// a successful roll on this does not mean a successful block is forthcoming. only that a chance to block
		// from a direction other than the rear is granted.
		switch (GetAA(aaHightenedAwareness)) {
		case 1:
			aaChance = 8;
			break;
		case 2:
			aaChance = 16;
			break;
		case 3:
			aaChance = 24;
			break;
		case 4:
			aaChance = 32;
			break;
		case 5:
			aaChance = 40;
			break;
		}

		if (aaChance > MakeRandomInt(1, 100))
			bBlockFromRear = true;
	}

	float block_chance = 0.0f;
	if (damage > 0 && CanThisClassBlock() && (!other->BehindMob(this, other->GetX(), other->GetY()) || bBlockFromRear)) {
		block_chance = (100.0f + (float)spellbonuses.IncreaseBlockChance + (float)itembonuses.IncreaseBlockChance) / 100.0f;
		block_chance /= 25;
		block_chance += (GetDEX()/200);
		skill = CastToClient()->GetSkill(BLOCKSKILL);
		block_chance += (skill/35.0);
		if (IsClient()) {
			CastToClient()->CheckIncreaseSkill(BLOCKSKILL, other, -10);
		}

		if (!ghit) {	//if they are not using a garunteed hit discipline
			RollTable[1] = block_chance;
		}
	}

	if(damage > 0 && GetAA(aaShieldBlock) && (!other->BehindMob(this, other->GetX(), other->GetY()))) {
		bool equiped = CastToClient()->m_inv.GetItem(14);
		if(equiped) {
			uint8 shield = CastToClient()->m_inv.GetItem(14)->GetItem()->ItemType;

			if(shield == ItemTypeShield) {
				switch(GetAA(aaShieldBlock)) {
				case 1:
					RollTable[1] = RollTable[1] + 2.50;
					break;
				case 2:
					RollTable[1] = RollTable[1] + 5.00;
					break;
				case 3:
					RollTable[1] = RollTable[1] + 10.00;
					break;
				}
			}
		}
	}

	//////////////////////////////////////////////////////		
	// parry
	//////////////////////////////////////////////////////
	float parry_chance = 0.0f;
	if (damage > 0 && CanThisClassParry() && !other->BehindMob(this, other->GetX(), other->GetY()))
	{
		parry_chance = (100.0f + (float)defender->spellbonuses.ParryChance + (float)defender->itembonuses.ParryChance) / 100.0f;
		parry_chance /= 45;
		parry_chance += GetDEX()/200;
		skill = CastToClient()->GetSkill(PARRY);
		parry_chance += skill/60.0;
		if (IsClient()) {
			CastToClient()->CheckIncreaseSkill(PARRY, other, -10); 
		}

		if (!ghit) {	//if they are not using a garunteed hit discipline
			RollTable[2] = parry_chance;
		}
	}

	////////////////////////////////////////////////////////
	// dodge
	////////////////////////////////////////////////////////
	float dodge_chance = 0.0f;
	if (damage > 0 && CanThisClassDodge() && !other->BehindMob(this, other->GetX(), other->GetY()))
	{
		dodge_chance = (100.0f + (float)defender->spellbonuses.DodgeChance + (float)defender->itembonuses.DodgeChance) / 100.0f;
		dodge_chance /= 45;
		dodge_chance += GetAGI()/200;

		skill = CastToClient()->GetSkill(DODGE);
		dodge_chance += skill/60.0;

		if (IsClient()) {
			CastToClient()->CheckIncreaseSkill(DODGE, other, -10);
		}

		if (!ghit) {	//if they are not using a garunteed hit discipline
			RollTable[3] = dodge_chance;
		}
	}

	if(damage > 0){
		int roll = MakeRandomInt(0,100);
		if(roll <= (int)RollTable[0]){
			damage = -3;
		}

		if ( damage > 0 )
		{
		roll = MakeRandomInt(0,100);
		if(roll <= (int)RollTable[1]){
			damage = -1;
		}
		}

		if ( damage > 0 )
		{
		roll = MakeRandomInt(0,100);
		if(roll <= (int)RollTable[2]){
			damage = -2;
		}
		}

		if ( damage > 0 )
		{
		roll = MakeRandomInt(0,100);
		if(roll <= (int)RollTable[3]){
			damage = -4;
		}
		}
	}

	mlog(COMBAT__DAMAGE, "Final damage after all avoidances: %d", damage);

	if (damage < 0)
		return true;
	return false;
}

void Mob::MeleeMitigation(Mob *attacker, sint32 &damage, sint32 minhit)
{
	if(damage <= 0)
		return;

	Mob* defender = this;
	float aa_mit = 0;

	switch(GetAA(aaCombatStability)){
	case 1:
		aa_mit += 0.02;
		break;
	case 2:
		aa_mit += 0.05;
		break;
	case 3:
		aa_mit += 0.10;
		break;
	}

	aa_mit += GetAA(aaPhysicalEnhancement) * 0.02;
	aa_mit += GetAA(aaInnateDefense) * 0.03;
	aa_mit += GetAA(aaDefensiveInstincts)*0.02;

	if(RuleB(Combat, UseIntervalAC))
	{
		float softcap = 0.0;
		float mitigation_rating = 0.0;
		float attack_rating = 0.0;
		int shield_ac = 0;
		int armor;
		float weight = 0.0;
		if(IsClient())
		{
			armor = CastToClient()->GetRawACNoShield(shield_ac);
			weight = (CastToClient()->CalcCurrentWeight() / 10.0);
		}
		else if(IsNPC())
		{
			armor = spellbonuses.AC + itembonuses.AC + (CastToNPC()->GetRawAC() / RuleR(Combat, NPCACFactor)) + 1;
		}

		if ( attacker && attacker->IsClient() )
		{
			switch(GetClass())
			{
			case WARRIOR:
				{
					armor += (int)((float)armor * RuleR(PVPCombat,WarriorACBonus));
					break;
				}
			case CLERIC:
				{
					armor += (int)((float)armor * RuleR(PVPCombat,ClericACBonus));
					break;
				}
			case PALADIN:
				{
					armor += (int)((float)armor * RuleR(PVPCombat,PaladinACBonus));
					break;
				}
			case RANGER:
				{
					armor += (int)((float)armor * RuleR(PVPCombat,RangerACBonus));
					break;
				}
			case SHADOWKNIGHT:
				{
					armor += (int)((float)armor * RuleR(PVPCombat,ShadowKnightACBonus));
					break;
				}
			case DRUID:
				{
					armor += (int)((float)armor * RuleR(PVPCombat,DruidACBonus));
					break;
				}
			case MONK:
				{
					armor += (int)((float)armor * RuleR(PVPCombat,MonkACBonus));
					break;
				}
			case BARD:
				{
					armor += (int)((float)armor * RuleR(PVPCombat,BardACBonus));
					break;
				}
			case ROGUE:
				{
					armor += (int)((float)armor * RuleR(PVPCombat,RogueACBonus));
					break;
				}
			case SHAMAN:
				{
					armor += (int)((float)armor * RuleR(PVPCombat,ShamanACBonus));
					break;
				}
			case NECROMANCER:
				{
					armor += (int)((float)armor * RuleR(PVPCombat,NecromancerACBonus));
					break;
				}
			case WIZARD:
				{
					armor += (int)((float)armor * RuleR(PVPCombat,WizardACBonus));
					break;
				}
			case MAGICIAN:
				{
					armor += (int)((float)armor * RuleR(PVPCombat,MagicianACBonus));
					break;
				}
			case ENCHANTER:
				{
					armor += (int)((float)armor * RuleR(PVPCombat,EnchanterACBonus));
					break;
				}
			}
		}
		else
		{
			switch(GetClass())
			{
			case WARRIOR:
				{
					armor += (int)((float)armor * RuleR(Combat,WarriorArmorBonus));
					break;
				}
			case CLERIC:
				{
					armor += (int)((float)armor * RuleR(Combat,ClericArmorBonus));
					break;
				}
			case PALADIN:
				{
					armor += (int)((float)armor * RuleR(Combat,PaladinArmorBonus));
					break;
				}
			case RANGER:
				{
					armor += (int)((float)armor * RuleR(Combat,RangerArmorBonus));
					break;
				}
			case SHADOWKNIGHT:
				{
					armor += (int)((float)armor * RuleR(Combat,ShadowKnightArmorBonus));
					break;
				}
			case DRUID:
				{
					armor += (int)((float)armor * RuleR(Combat,DruidArmorBonus));
					break;
				}
			case MONK:
				{
					armor += (int)((float)armor * RuleR(Combat,MonkArmorBonus));
					break;
				}
			case BARD:
				{
					armor += (int)((float)armor * RuleR(Combat,BardArmorBonus));
					break;
				}
			case ROGUE:
				{
					armor += (int)((float)armor * RuleR(Combat,RogueArmorBonus));
					break;
				}
			case SHAMAN:
				{
					armor += (int)((float)armor * RuleR(Combat,ShamanArmorBonus));
					break;
				}
			case NECROMANCER:
				{
					armor += (int)((float)armor * RuleR(Combat,NecromancerArmorBonus));
					break;
				}
			case WIZARD:
				{
					armor += (int)((float)armor * RuleR(Combat,WizardArmorBonus));
					break;
				}
			case MAGICIAN:
				{
					armor += (int)((float)armor * RuleR(Combat,MagicianArmorBonus));
					break;
				}
			case ENCHANTER:
				{
					armor += (int)((float)armor * RuleR(Combat,EnchanterArmorBonus));
					break;
				}
			}
		}

		if(GetClass() == WIZARD || GetClass() == MAGICIAN || GetClass() == NECROMANCER || GetClass() == ENCHANTER)
		{
			softcap = RuleI(Combat, ClothACSoftcap);
		}
		else if(GetClass() == MONK && weight <= 15.0)
		{
			softcap = RuleI(Combat, MonkACSoftcap);
		}
		else if(GetClass() == DRUID || GetClass() == BEASTLORD || GetClass() == MONK)
		{
			softcap = RuleI(Combat, LeatherACSoftcap);
		}
		else if(GetClass() == SHAMAN || GetClass() == ROGUE || GetClass() == BERSERKER || GetClass() == RANGER)
		{
			softcap = RuleI(Combat, ChainACSoftcap);
		}
		else
		{
			softcap = RuleI(Combat, PlateACSoftcap);
		}

		softcap += shield_ac;
		armor += shield_ac;
		softcap += (softcap * (aa_mit * RuleR(Combat, AAMitigationACFactor)));
		if(armor > softcap)
		{
			int softcap_armor = armor - softcap;
			if(GetClass() == WARRIOR)
			{
				softcap_armor = softcap_armor * RuleR(Combat, WarriorACSoftcapReturn);
			}
			else if(GetClass() == SHADOWKNIGHT || GetClass() == PALADIN || (GetClass() == MONK && weight <= 15.0))
			{
				softcap_armor = softcap_armor * RuleR(Combat, KnightACSoftcapReturn);
			}
			else if(GetClass() == CLERIC || GetClass() == BARD || GetClass() == BERSERKER || GetClass() == ROGUE || GetClass() == SHAMAN || GetClass() == MONK)
			{
				softcap_armor = softcap_armor * RuleR(Combat, LowPlateChainACSoftcapReturn);
			}
			else if(GetClass() == RANGER || GetClass() == BEASTLORD)
			{
				softcap_armor = softcap_armor * RuleR(Combat, LowChainLeatherACSoftcapReturn);
			}
			else if(GetClass() == WIZARD || GetClass() == MAGICIAN || GetClass() == NECROMANCER || GetClass() == ENCHANTER || GetClass() == DRUID)
			{
				softcap_armor = softcap_armor * RuleR(Combat, CasterACSoftcapReturn);
			}
			else
			{
				softcap_armor = softcap_armor * RuleR(Combat, MiscACSoftcapReturn);
			}
			armor = softcap + softcap_armor;
		}

		mitigation_rating = 0.0;
		if(GetClass() == WIZARD || GetClass() == MAGICIAN || GetClass() == NECROMANCER || GetClass() == ENCHANTER)
		{
			mitigation_rating = ((GetSkill(DEFENSE) + this->GetAGI()/10) / 4.0) + armor + 1;
		}
		else
		{
			mitigation_rating = ((GetSkill(DEFENSE) + this->GetAGI()/10) / 3.0) + (armor * 1.333333) + 1;
		}

		// Kings & Bandits - Mitigation rating this probably is a good variable to change to make AC weaker/stronger overall
		if ( attacker->IsClient() && IsClient())
			mitigation_rating *= .975f;
		else if ( attacker->IsCharmed() && attacker->IsNPC() )
			mitigation_rating *= .9f;
		else if ( attacker->IsPet() && attacker->IsNPC() )
			mitigation_rating *= 1.0f;
		else if ( attacker->IsNPC() && IsClient())
			mitigation_rating *= .90f;
		else
			mitigation_rating *= 0.847;


		if(attacker->IsClient())
		{
			// Kings & Bandits - client fix for STR being low, we subtract and can reduce attack rating
			attack_rating = (attacker->CastToClient()->CalcATK() + ((attacker->GetSTR()-60) * 1.3) + (attacker->GetSkill(OFFENSE)*1.345));
		}
		else
		{
			// Kings & Bandits - reduced NPC's attack rating
			if ( attacker->IsCharmed() )
			{
				int str = attacker->GetSTR();
				int mod = 70;
				if ( str > 255 )
					str = 255;

				attack_rating = (attacker->GetATK() + (attacker->GetSkill(OFFENSE)*1.2) + ((str-mod) * 0.65));
			}
			else
				attack_rating = (attacker->GetATK() + (attacker->GetSkill(OFFENSE)*1.2) + ((attacker->GetSTR()-70) * 0.8));
		}

		float d = 10.0;
		float mit_roll = MakeRandomFloat(0, mitigation_rating);
		float atk_roll = MakeRandomFloat(0, attack_rating);

		if(atk_roll > mit_roll)
		{
			float a_diff = (atk_roll - mit_roll);
			float thac0 = attack_rating * RuleR(Combat, ACthac0Factor);
			d -= 10.0 * (a_diff / thac0);
			float thac0cap = ((attacker->GetLevel() * 9) + 20);
			if(thac0 > thac0cap)
			{
				thac0 = thac0cap;
			}
		}
		else if(mit_roll > atk_roll)
		{
			float m_diff = (mit_roll - atk_roll);
			float thac20 = mitigation_rating * RuleR(Combat, ACthac20Factor);
			d += 10 * (m_diff / thac20);
			float thac20cap = ((defender->GetLevel() * 9) + 20);
			if(thac20 > thac20cap)
			{
				thac20 = thac20cap;
			}
		}

		if(d < 0.0)
		{
			d = 0.0;
		}

		if(d > 20)
		{
			d = 20.0;
		}

		float interval = (damage - minhit) / 20.0;
		damage = damage - ((int)d * interval);
	}
	else{
		////////////////////////////////////////////////////////
		// Scorpious2k: Include AC in the calculation
		// use serverop variables to set values
		int myac = GetAC();
		if (damage > 0 && myac > 0) {
			int acfail=1000;
			char tmp[10];

			if (database.GetVariable("ACfail", tmp, 9)) {
				acfail = (int) (atof(tmp) * 100);
				if (acfail>100) acfail=100;
			}

			if (acfail<=0 || MakeRandomInt(0, 100)>acfail) {
				float acreduction=1;
				int acrandom=300;
				if (database.GetVariable("ACreduction", tmp, 9))
				{
					acreduction=atof(tmp);
					if (acreduction>100) acreduction=100;
				}

				if (database.GetVariable("ACrandom", tmp, 9))
				{
					acrandom = (int) ((atof(tmp)+1) * 100);
					if (acrandom>10100) acrandom=10100;
				}

				if (acreduction>0) {
					damage -= (int) (GetAC() * acreduction/100.0f);
				}		
				if (acrandom>0) {
					damage -= (myac * MakeRandomInt(0, acrandom) / 10000);
				}
				if (damage<1) damage=1;
				mlog(COMBAT__DAMAGE, "AC Damage Reduction: fail chance %d%%. Failed. Reduction %.3f%%, random %d. Resulting damage %d.", acfail, acreduction, acrandom, damage);
			} else {
				mlog(COMBAT__DAMAGE, "AC Damage Reduction: fail chance %d%%. Did not fail.", acfail);
			}
		}

		damage -= (aa_mit * damage);

		if(damage != 0 && damage < minhit)
			damage = minhit;
	}


	//reduce the damage from shielding item and aa based on the min dmg
	//spells offer pure mitigation
	int itemMeleeMit = RuleI(Combat,ItemMeleeMitigation);
	if ( itemMeleeMit < 1 )
		itemMeleeMit = 1;

	int spellMeleeMit = RuleI(Combat,SpellMeleeMitigation);
	if ( spellMeleeMit < 1 )
		spellMeleeMit = 1;

	damage -= (damage * defender->itembonuses.MeleeMitigation / itemMeleeMit);
	damage -= (damage * defender->spellbonuses.MeleeMitigation / spellMeleeMit);

	if(damage < 0)
		damage = 0;
}

//Returns the weapon damage against the input mob
//if we cannot hit the mob with the current weapon we will get a value less than or equal to zero
//Else we know we can hit.
//GetWeaponDamage(mob*, const Item_Struct*) is intended to be used for mobs or any other situation where we do not have a client inventory item
//GetWeaponDamage(mob*, const ItemInst*) is intended to be used for situations where we have a client inventory item
int Mob::GetWeaponDamage(Mob *against, const Item_Struct *weapon_item) {
	_ZP(Mob_GetWeaponDamageA);
	int dmg = 0;
	int banedmg = 0;

	//can't hit invulnerable stuff with weapons.
	if(against->GetInvul() || against->SpecAttacks[IMMUNE_MELEE]){
		return 0;
	}

	//check to see if our weapons or fists are magical.
	if(!against->IsCharmed() && against->SpecAttacks[IMMUNE_MELEE_NONMAGICAL]){
		if(weapon_item){
			if(weapon_item->Magic){
				dmg = weapon_item->Damage;

				//this is more for non weapon items, ex: boots for kick
				//they don't have a dmg but we should be able to hit magical
				dmg = dmg <= 0 ? 1 : dmg;
			}
			else
				return 0;
		}
		else{
			if((GetClass() == MONK || GetClass() == BEASTLORD) && GetLevel() >= 30){
				dmg = GetMonkHandToHandDamage();
			}
			// 10/15/2012 (Image) KaB/Red69/Zek - Magical hands should do damage to mobs that require 'magical' weapons or fists.
			// no weapon is equipped so it should be ok!
			else if ( IsClient() )
			{
				ItemInst* handEquip = CastToClient()->GetInv().GetItem(SLOT_HANDS);
				if ( handEquip != NULL && handEquip->GetItem()->Magic )
				{
					if((GetClass() == MONK || GetClass() == BEASTLORD))
						dmg = GetMonkHandToHandDamage();
					else
						dmg = 1;
				}
			}
			else if(GetOwner() && GetLevel() >= RuleI(Combat, PetAttackMagicLevel)){
				//pets wouldn't actually use this but...
				//it gives us an idea if we can hit due to the dual nature of this function
				dmg = 1;						   
			}
			else if(SpecAttacks[SPECATK_MAGICAL])
			{
				dmg = 1;
			}
			else
				return 0;
		}
	}
	else{
		if(weapon_item){
			dmg = weapon_item->Damage;

			dmg = dmg <= 0 ? 1 : dmg;
		}
		else{
			if(GetClass() == MONK || GetClass() == BEASTLORD){
				dmg = GetMonkHandToHandDamage();
			}
			else{
				dmg = 1;
			}
		}
	}

	int eledmg = 0;
	if(!against->SpecAttacks[IMMUNE_MAGIC]){
		if(weapon_item && weapon_item->ElemDmgAmt){
			//we don't check resist for npcs here
			eledmg = weapon_item->ElemDmgAmt;
			dmg += eledmg;
		}
	}

	if(against->SpecAttacks[IMMUNE_MELEE_EXCEPT_BANE]){
		if(weapon_item){
			if(weapon_item->BaneDmgBody == against->GetBodyType()){
				banedmg += weapon_item->BaneDmgAmt;
			}

			if(weapon_item->BaneDmgRace == against->GetRace()){
				banedmg += weapon_item->BaneDmgRaceAmt;
			}
		}

		if(!eledmg && !banedmg){
			if(!SpecAttacks[SPECATK_BANE])
				return 0;
			else
				return 1;
		}
		else
			dmg += banedmg;
	}
	else{
		if(weapon_item){
			if(weapon_item->BaneDmgBody == against->GetBodyType()){
				banedmg += weapon_item->BaneDmgAmt;
			}

			if(weapon_item->BaneDmgRace == against->GetRace()){
				banedmg += weapon_item->BaneDmgRaceAmt;
			}
		}

		dmg += (banedmg + eledmg);
	}

	if(dmg <= 0){
		return 0;
	}
	else
		return dmg;
}

int Mob::GetWeaponDamage(Mob *against, const ItemInst *weapon_item, int32 *hate)
{
	_ZP(Mob_GetWeaponDamageB);
	int dmg = 0;
	int banedmg = 0;

	if(against->GetInvul() || against->SpecAttacks[IMMUNE_MELEE]){
		return 0;
	}

	//check for items being illegally attained
	if(weapon_item){
		const Item_Struct *mWeaponItem = weapon_item->GetItem();
		if(mWeaponItem){
			if(mWeaponItem->ReqLevel > GetLevel()){
				return 0;
			}

			if(!weapon_item->IsEquipable(GetBaseRace(), GetClass())){
				return 0;
			}
		}
		else{
			return 0;
		}
	}

	if(against->SpecAttacks[IMMUNE_MELEE_NONMAGICAL]){
		if(weapon_item){
			// check to see if the weapon is magic
			bool MagicWeapon = false;
			if(weapon_item->GetItem() && weapon_item->GetItem()->Magic) 
				MagicWeapon = true;
			else {					
				if(spellbonuses.MagicWeapon || itembonuses.MagicWeapon)
					MagicWeapon = true;
			}

			if(MagicWeapon) {

				if(IsClient() && GetLevel() < weapon_item->GetItem()->RecLevel){
					dmg = CastToClient()->CalcRecommendedLevelBonus(GetLevel(), weapon_item->GetItem()->RecLevel, weapon_item->GetItem()->Damage);
				}
				else{
					dmg = weapon_item->GetItem()->Damage;
				}

				for(int x = 0; x < 5; x++){
					if(weapon_item->GetAugment(x) && weapon_item->GetAugment(x)->GetItem()){
						dmg += weapon_item->GetAugment(x)->GetItem()->Damage;
						if (hate) *hate += weapon_item->GetAugment(x)->GetItem()->Damage + weapon_item->GetAugment(x)->GetItem()->ElemDmgAmt;
					}
				}
				dmg = dmg <= 0 ? 1 : dmg;
			}
		}
		else if((GetClass() == MONK || GetClass() == BEASTLORD) && GetLevel() >= 30){
			dmg = GetMonkHandToHandDamage();
			if (hate) *hate += dmg;
		}
		// 10/15/2012 (Image) KaB/Red69/Zek - Magical hands should do damage to mobs that require 'magical' weapons or fists.
		// no weapon is equipped so it should be ok!
		else if ( IsClient() )
		{
			ItemInst* handEquip = CastToClient()->GetInv().GetItem(SLOT_HANDS);
			if ( handEquip != NULL && handEquip->GetItem()->Magic )
			{			
				if((GetClass() == MONK || GetClass() == BEASTLORD))
					dmg = GetMonkHandToHandDamage();
				else
					dmg = 1;
			}
			else
				return 0;
		}
		else{
			if(GetOwner() && GetLevel() >= RuleI(Combat, PetAttackMagicLevel)){ //pets wouldn't actually use this but...
				dmg = 1;						   //it gives us an idea if we can hit
			}
			else if(SpecAttacks[SPECATK_MAGICAL]){
				dmg = 1;
			}
			else
				return 0;
		}
	}
	else{
		if(weapon_item){
			if(weapon_item->GetItem()){

				if(IsClient() && GetLevel() < weapon_item->GetItem()->RecLevel){
					dmg = CastToClient()->CalcRecommendedLevelBonus(GetLevel(), weapon_item->GetItem()->RecLevel, weapon_item->GetItem()->Damage);
				}
				else{
					dmg = weapon_item->GetItem()->Damage;
				}

				for(int x = 0; x < 5; x++){
					if(weapon_item->GetAugment(x) && weapon_item->GetAugment(x)->GetItem()){
						dmg += weapon_item->GetAugment(x)->GetItem()->Damage;
						if (hate) *hate += weapon_item->GetAugment(x)->GetItem()->Damage + weapon_item->GetAugment(x)->GetItem()->ElemDmgAmt;
					}
				}
				dmg = dmg <= 0 ? 1 : dmg;
			}
		}
		else{
			if(GetClass() == MONK || GetClass() == BEASTLORD){
				dmg = GetMonkHandToHandDamage();
				if (hate) *hate += dmg;
			}
			else{
				dmg = 1;
			}
		}
	}

	int eledmg = 0;
	if(!against->SpecAttacks[IMMUNE_MAGIC]){
		if(weapon_item && weapon_item->GetItem() && weapon_item->GetItem()->ElemDmgAmt){
			if(IsClient() && GetLevel() < weapon_item->GetItem()->RecLevel){
				eledmg = CastToClient()->CalcRecommendedLevelBonus(GetLevel(), weapon_item->GetItem()->RecLevel, weapon_item->GetItem()->ElemDmgAmt);
			}
			else{
				eledmg = weapon_item->GetItem()->ElemDmgAmt;
			}

			if(eledmg)
			{
				eledmg = (eledmg * against->ResistSpell(weapon_item->GetItem()->ElemDmgType, 0, this) / 100);
			}
		}

		if(weapon_item){
			for(int x = 0; x < 5; x++){
				if(weapon_item->GetAugment(x) && weapon_item->GetAugment(x)->GetItem()){
					if(weapon_item->GetAugment(x)->GetItem()->ElemDmgAmt)
						eledmg += (weapon_item->GetAugment(x)->GetItem()->ElemDmgAmt * against->ResistSpell(weapon_item->GetAugment(x)->GetItem()->ElemDmgType, 0, this) / 100);
				}
			}
		}
	}

	if(against->SpecAttacks[IMMUNE_MELEE_EXCEPT_BANE]){
		if(weapon_item && weapon_item->GetItem()){
			if(weapon_item->GetItem()->BaneDmgBody == against->GetBodyType()){
				if(IsClient() && GetLevel() < weapon_item->GetItem()->RecLevel){
					banedmg += CastToClient()->CalcRecommendedLevelBonus(GetLevel(), weapon_item->GetItem()->RecLevel, weapon_item->GetItem()->BaneDmgAmt);
				}
				else{
					banedmg += weapon_item->GetItem()->BaneDmgAmt;
				}
			}

			if(weapon_item->GetItem()->BaneDmgRace == against->GetRace()){
				if(IsClient() && GetLevel() < weapon_item->GetItem()->RecLevel){
					banedmg += CastToClient()->CalcRecommendedLevelBonus(GetLevel(), weapon_item->GetItem()->RecLevel, weapon_item->GetItem()->BaneDmgRaceAmt);
				}
				else{
					banedmg += weapon_item->GetItem()->BaneDmgRaceAmt;
				}
			}

			for(int x = 0; x < 5; x++){
				if(weapon_item->GetAugment(x) && weapon_item->GetAugment(x)->GetItem()){
					if(weapon_item->GetAugment(x)->GetItem()->BaneDmgBody == against->GetBodyType()){
						banedmg += weapon_item->GetAugment(x)->GetItem()->BaneDmgAmt;
					}

					if(weapon_item->GetAugment(x)->GetItem()->BaneDmgRace == against->GetRace()){
						banedmg += weapon_item->GetAugment(x)->GetItem()->BaneDmgRaceAmt;
					}
				}
			}
		}

		if(!eledmg && !banedmg)
		{
			if(!SpecAttacks[SPECATK_BANE])
				return 0;
			else
				return 1;
		}
		else {
			dmg += (banedmg + eledmg);
			if (hate) *hate += banedmg;
		}
	}
	else{
		if(weapon_item && weapon_item->GetItem()){
			if(weapon_item->GetItem()->BaneDmgBody == against->GetBodyType()){
				if(IsClient() && GetLevel() < weapon_item->GetItem()->RecLevel){
					banedmg += CastToClient()->CalcRecommendedLevelBonus(GetLevel(), weapon_item->GetItem()->RecLevel, weapon_item->GetItem()->BaneDmgAmt);
				}
				else{
					banedmg += weapon_item->GetItem()->BaneDmgAmt;
				}
			}

			if(weapon_item->GetItem()->BaneDmgRace == against->GetRace()){
				if(IsClient() && GetLevel() < weapon_item->GetItem()->RecLevel){
					banedmg += CastToClient()->CalcRecommendedLevelBonus(GetLevel(), weapon_item->GetItem()->RecLevel, weapon_item->GetItem()->BaneDmgRaceAmt);
				}
				else{
					banedmg += weapon_item->GetItem()->BaneDmgRaceAmt;
				}
			}

			for(int x = 0; x < 5; x++){
				if(weapon_item->GetAugment(x) && weapon_item->GetAugment(x)->GetItem()){
					if(weapon_item->GetAugment(x)->GetItem()->BaneDmgBody == against->GetBodyType()){
						banedmg += weapon_item->GetAugment(x)->GetItem()->BaneDmgAmt;
					}

					if(weapon_item->GetAugment(x)->GetItem()->BaneDmgRace == against->GetRace()){
						banedmg += weapon_item->GetAugment(x)->GetItem()->BaneDmgRaceAmt;
					}
				}
			}
		}
		dmg += (banedmg + eledmg);
		if (hate) *hate += banedmg;
	}

	if(dmg <= 0){
		return 0;
	}
	else
		return dmg;
}

//note: throughout this method, setting `damage` to a negative is a way to
//stop the attack calculations
bool Client::Attack(Mob* other, int Hand, bool bRiposte, bool IsStrikethrough)
{
	_ZP(Client_Attack);

	if (!other) {
		SetTarget(NULL);
		LogFile->write(EQEMuLog::Error, "A null Mob object was passed to Client::Attack() for evaluation!");
		return false;
	}

	if(!GetTarget())
		SetTarget(other);

	mlog(COMBAT__ATTACKS, "Attacking %s with hand %d %s", other?other->GetName():"(NULL)", Hand, bRiposte?"(this is a riposte)":"");

	//SetAttackTimer();
	if (
		(IsCasting() && GetClass() != BARD)
		|| other == NULL
		|| ((IsClient() && CastToClient()->dead) || (other->IsClient() && other->CastToClient()->dead))
		|| (GetHP() < 0)
		|| (!IsAttackAllowed(other))
		) {
			mlog(COMBAT__ATTACKS, "Attack canceled, invalid circumstances.");
			return false; // Only bards can attack while casting
	}
	if(DivineAura() && !GetGM()) {//cant attack while invulnerable unless your a gm
		mlog(COMBAT__ATTACKS, "Attack canceled, Divine Aura is in effect.");
		Message_StringID(10, DIVINE_AURA_NO_ATK);	//You can't attack while invulnerable!
		return false;
	}

	if (GetFeigned())
		return false; // Rogean: How can you attack while feigned? Moved up from Aggro Code.

	// track for 2h damage	
	uint8 itemType = 0;

	ItemInst* weapon;
	if (Hand == 14)	// Kaiyodo - Pick weapon from the attacking hand
		weapon = GetInv().GetItem(SLOT_SECONDARY);
	else
		weapon = GetInv().GetItem(SLOT_PRIMARY);

	if(weapon != NULL) {
		if ( weapon->IsWeapon() )
			itemType = weapon->GetItem()->ItemType;

		if (!weapon->IsWeapon()) {
			mlog(COMBAT__ATTACKS, "Attack canceled, Item %s (%d) is not a weapon.", weapon->GetItem()->Name, weapon->GetID());
			return(false);
		}
		mlog(COMBAT__ATTACKS, "Attacking with weapon: %s (%d)", weapon->GetItem()->Name, weapon->GetID());
	} else {
		mlog(COMBAT__ATTACKS, "Attacking without a weapon.");
	}

	// calculate attack_skill and skillinuse depending on hand and weapon
	// also send Packet to near clients
	SkillType skillinuse;
	AttackAnimation(skillinuse, Hand, weapon);
	mlog(COMBAT__ATTACKS, "Attacking with %s in slot %d using skill %d", weapon?weapon->GetItem()->Name:"Fist", Hand, skillinuse);

	/// Now figure out damage
	int damage = 0;
	bool isCritHit = false;
	int8 mylevel = GetLevel() ? GetLevel() : 1;
	int32 hate = 0;
	if (weapon) hate = weapon->GetItem()->Damage + weapon->GetItem()->ElemDmgAmt;
	int weapon_damage = GetWeaponDamage(other, weapon, &hate);
	if (hate == 0 && weapon_damage > 1) hate = weapon_damage;

	//if weapon damage > 0 then we know we can hit the target with this weapon
	//otherwise we cannot and we set the damage to -5 later on
	if(weapon_damage > 0){

		//Berserker Berserk damage bonus
		if(berserk && GetClass() == BERSERKER){
			int bonus = 3 + GetLevel()/10;		//unverified
			weapon_damage = weapon_damage * (100+bonus) / 100;
			mlog(COMBAT__DAMAGE, "Berserker damage bonus increases DMG to %d", weapon_damage);
		}

		//try a finishing blow.. if successful end the attack
		if(TryFinishingBlow(other, skillinuse, itemType))
			return (true);

		int min_hit = 1;
		int max_hit = 2; // switch statement handles max damage in default statement unless otherwise stated

		switch(itemType)
		{
		case ItemType2HS:
		case ItemType2HB:
		case ItemType2HPierce:
			{
				max_hit = ((int)((float)3.0f*weapon_damage)*GetDamageTable(skillinuse)) / 100;
				break;
			}
		default:
			{
				max_hit = (2*weapon_damage*GetDamageTable(skillinuse)) / 100;
				break;
			}
		}

		if(GetLevel() < 10 && max_hit > 20)
			max_hit = 20;
		else if(GetLevel() < 20 && max_hit > 40)
			max_hit = 40;

		// ***************************************************************
		// *** Calculate the damage bonus, if applicable, for this hit ***
		// ***************************************************************

		// If you include the preprocessor directive "#define EQEMU_NO_WEAPON_DAMAGE_BONUS", that indicates that you do not
		// want damage bonuses added to weapon damage at all. This feature was requested by ChaosSlayer on the EQEmu Forums.
		//
		// This is not recommended for normal usage, as the damage bonus represents a non-trivial component of the DPS output
		// of weapons wielded by higher-level melee characters (especially for two-handed weapons).

		if( Hand == 13 && GetLevel() >= 28 && IsWarriorClass() )
		{
			// Damage bonuses apply only to hits from the main hand (Hand == 13) by characters level 28 and above
			// who belong to a melee class. If we're here, then all of these conditions apply.

			int8 ucDamageBonus = GetWeaponDamageBonus( weapon ? weapon->GetItem() : (const Item_Struct*) NULL);

			min_hit += (int) ucDamageBonus;
			max_hit += (int) ucDamageBonus;
			hate += ucDamageBonus;
		}

		CheckIncreaseSkill(skillinuse, other, -5);
		CheckIncreaseSkill(OFFENSE, other, -5);

		min_hit = min_hit * (100 + itembonuses.MinDamageModifier + spellbonuses.MinDamageModifier) / 100;

		if (Hand==14) {
			if(GetAA(aaSinisterStrikes)) {
				int sinisterBonus = MakeRandomInt(5, 10);
				min_hit += (min_hit * sinisterBonus / 100);
				max_hit += (max_hit * sinisterBonus / 100);
				hate += (hate * sinisterBonus / 100);
			}
		}

		if(max_hit < min_hit)
			max_hit = min_hit;

		if(RuleB(Combat, UseIntervalAC))
			damage = max_hit;
		else
			damage = MakeRandomInt(min_hit, max_hit);

		mlog(COMBAT__DAMAGE, "Damage calculated to %d (min %d, max %d, str %d, skill %d, DMG %d, lv %d)",
			damage, min_hit, max_hit, GetSTR(), GetSkill(skillinuse), weapon_damage, mylevel);
		//THIS IS WHERE WE CHECK TO SEE IF WE HIT:
		if(other->IsClient() && other->CastToClient()->IsSitting()) {
			mlog(COMBAT__DAMAGE, "Client %s is sitting. Hitting for max damage (%d).", other->GetName(), max_hit);

			// Kings & Bandits - melee damage modifier bonus
			int modifier = 10;
			if ( IsClient() )
			{
				switch(GetClass()) {
				case WARRIOR:
					{
						modifier = 6;

						if(GetBaseRace() == OGRE)
						{
							modifier = 8;
						}
						break;
					}
				case SHADOWKNIGHT:
				case PALADIN:
				case BERSERKER:
					{
						modifier = 7;
						
						if(GetBaseRace() == OGRE)
						{
							modifier = 8;
						}
						break;
					}
				case BARD:
				case RANGER:
				case ROGUE:
					{
						modifier = 6;
						break;
					}
				case MONK:
				case BEASTLORD:
					{
						modifier = 5;
						break;
					}
				case MAGICIAN:
				case ENCHANTER:
				case NECROMANCER:
					{
						modifier = 10;
						break;
					}
				}
			}

			if ( GetLevel() < 11 )
				modifier = 15;
			else if ( GetLevel() > 49 )
				modifier = modifier + (int)((float)modifier * (.5f));
			else if ( GetLevel() > 46 )
				modifier = modifier + (int)((float)modifier * (.2f));

			damage = max_hit;
			damage += (GetSTR() / modifier) + (damage * other->GetSkillDmgTaken(skillinuse) / 100) + GetSkillDmgAmt(skillinuse);
		}
		//check to see if we hit..
		else if(!other->CheckHitChance(this, skillinuse, Hand)) {
			mlog(COMBAT__ATTACKS, "Attack missed. Damage set to 0.");
			damage = 0;
		} else {	//we hit, try to avoid it
			other->AvoidDamage(this, damage);
			other->MeleeMitigation(this, damage, min_hit);
			if(damage > 0) {
				ApplyMeleeDamageBonus(skillinuse, damage, other);


				// Kings & Bandits - melee damage modifier bonus
				int modifier = 10;
				if ( IsClient() )
				{
					switch(GetClass()) {
					case WARRIOR:
						{
							modifier = 6;

							if(GetBaseRace() == OGRE)
							{
								modifier = 8;
							}

							break;
						}
					case SHADOWKNIGHT:
					case PALADIN:
					case BERSERKER:
					{
						modifier = 7;

						if(GetBaseRace() == OGRE)
						{
							modifier = 8;
						}
						break;
					}
					case BARD:
					case RANGER:
					case ROGUE:
						{
							modifier = 6;
							break;
						}
					case MONK:
					case BEASTLORD:
						{
							modifier = 5;
							break;
						}
					case MAGICIAN:
					case ENCHANTER:
					case NECROMANCER:
						{
							modifier = 10;
							break;
						}
					}
				}

				if ( GetLevel() < 11 )
					modifier = 15;
				else if ( GetLevel() > 49 )
					modifier = modifier + (int)((float)modifier * (.5f));
				else if ( GetLevel() > 46 )
					modifier = modifier + (int)((float)modifier * (.2f));

				damage += (GetSTR() / modifier) + (damage * other->GetSkillDmgTaken(skillinuse) / 100) + GetSkillDmgAmt(skillinuse);

				isCritHit = TryCriticalHit(other, skillinuse, itemType, damage);
			}
			mlog(COMBAT__DAMAGE, "Final damage after all reductions: %d (critical hit not determined if attacking client)", damage);
		}

		//riposte
		bool slippery_attack = false; // Part of hack to allow riposte to become a miss, but still allow a Strikethrough chance (like on Live)
		if (damage == -3)  {
			if (bRiposte) return false;
			else {
				if (Hand == 14 && GetAA(aaSlipperyAttacks) > 0) {// Do we even have it & was attack with mainhand? If not, don't bother with other calculations
					if (MakeRandomInt(0, 100) < (GetAA(aaSlipperyAttacks) * 20)) {
						damage = 0; // Counts as a miss
						slippery_attack = true;
					} else 
						DoRiposte(other);
				}
				else 
					DoRiposte(other);
			}
		}

		//strikethrough..
		int aaStrikethroughBonus = 0;
		switch (GetAA(aaStrikethrough))
		{
		case 1:
			aaStrikethroughBonus = 2;
			break;
		case 2:
			aaStrikethroughBonus = 4;
			break;
		case 3:
			aaStrikethroughBonus = 6;
			break;
		}
		switch (GetAA(aaTacticalMastery))
		{
		case 1:
			aaStrikethroughBonus = 2;
			break;
		case 2:
			aaStrikethroughBonus = 4;
			break;
		case 3:
			aaStrikethroughBonus = 6;
			break;
		}
		if (((damage < 0) || slippery_attack) && !bRiposte && !IsStrikethrough) { // Hack to still allow Strikethrough chance w/ Slippery Attacks AA
			if(MakeRandomInt(0, 100) < (itembonuses.StrikeThrough + spellbonuses.StrikeThrough + aaStrikethroughBonus)) {
				Message_StringID(MT_StrikeThrough, STRIKETHROUGH_STRING); // You strike through your opponents defenses!
				Attack(other, Hand, false, true); // Strikethrough only gives another attempted hit
				return false;
			}
		}
	}
	else{
		damage = -5;
	}


	// Hate Generation is on a per swing basis, regardless of a hit, miss, or block, its always the same.
	// If we are this far, this means we are atleast making a swing.
	if (!bRiposte) // Ripostes never generate any aggro.
		other->AddToHateList(this, hate);

	///////////////////////////////////////////////////////////
	//////    Send Attack Damage
	///////////////////////////////////////////////////////////
	other->Damage(this, damage, SPELL_UNKNOWN, skillinuse, itemType, true, -1, false, false, isCritHit); // pass isCritHit so we know if they are attacking a client then we dont do pvp mit again

	if(damage > 0 && (spellbonuses.MeleeLifetap || itembonuses.MeleeLifetap))
	{
		int lifetap_amt = spellbonuses.MeleeLifetap + itembonuses.MeleeLifetap;
		if(lifetap_amt > 100)
			lifetap_amt = 100;

		lifetap_amt = damage * lifetap_amt / 100;

		mlog(COMBAT__DAMAGE, "Melee lifetap healing for %d damage.", damage);
		//heal self for damage done..
		HealDamage(lifetap_amt);
	}

	//break invis & hide when you attack
	BreakMobInvis();
	BreakMobHide();

	if (damage > 0)
	{
		// Give the opportunity to throw back a defensive proc, if we are successful in affecting damage on our target
		other->TriggerDefensiveProcs(this);

		return true;
	}
	else
		return false;
}

//used by complete heal and #heal
void Mob::Heal()
{
	SetMaxHP();
	SendHPUpdate();
}

void Client::Damage(Mob* other, sint32 damage, int16 spell_id, SkillType attack_skill, uint8 itemType, 
bool avoidable, sint8 buffslot, bool iBuffTic, bool isDmgShield, bool isCritHit, int32 player_id, bool isDoT)
{
	if(dead || IsCorpse())
		return;

	if(spell_id==0)
		spell_id = SPELL_UNKNOWN;

	Mob* target = other;
	if ( target && target->HasOwner() )
		target = target->GetOwner();
	if ( target && target->IsClient() && target != this && !isDmgShield )
	{
		target->CastToClient()->SetAttackTarget(this);
	}

	ResetUnderAttack();
	// cut all PVP spell damage to 2/3 -solar
	// EverHood - Blasting ourselfs is considered PvP 
	//Don't do PvP mitigation if the caster is damaging himself
	if((other && other->IsClient() && (other != this)) || (player_id != CharacterID() && player_id > 0 && player_id != MAX_PLAYERID) && damage > 0) {
		if ( !isCritHit ) // crit hit is handled for pvp in Mob::TryCriticalHit
		{
			int8 level = 0;
			int8 player_class = 0;
			if ( other )
			{
				level = other->GetLevel();
				player_class = other->GetClass();
			}
			else if ( buffslot > -1 )
			{
				level = buffs[buffslot].casterlevel;
				player_class = buffs[buffslot].player_class;
			}
			ApplyPVPMitigation(level, player_class, attack_skill, itemType, damage, spell_id, isDmgShield );
		}
	}

	// Kings & Bandits - Pet check, see if our pets doing anything or if it should help
	if ( HasPet() && other != NULL )
	{
		if ( GetPet()->IsNPC() || ( GetPet()->IsClient() && ( other->IsClient() || (other->GetOwner() != NULL && other->GetOwner()->IsClient()))))
			GetPet()->AddToHateList(other,1);
	}

	if(!ClientFinishedLoading())
		damage = -5;

	//do a majority of the work...
	CommonDamage(other, damage, spell_id, attack_skill, avoidable, buffslot, iBuffTic, player_id, isDoT);

	if (damage > 0) {
		if ( IsClient() && CastToClient()->IsCampTimerEnabled() )
		{
			CastToClient()->DisableCampTimer();
		}
		if (spell_id == SPELL_UNKNOWN)
			CheckIncreaseSkill(DEFENSE, other, -5);
	}
}


void Client::ApplyPVPMitigation(int8 level, int8 player_class, SkillType attack_skill, uint8 itemType, sint32 &damage, int16 spell_id, bool isDmgShield, bool isDoT)
{
	int PvPMitigation = 100;
	bool appliedByItemType = false;
	switch(itemType)
	{
	case ItemType2HPierce:
		{
			PvPMitigation = RuleI(PVPCombat, Mitigation2HPiercing);
			appliedByItemType = true;
			break;
		}
	case ItemType2HS:
		{
			PvPMitigation = RuleI(PVPCombat, Mitigation2HSlashing);
			appliedByItemType = true;
			break;
		}
	case ItemType2HB:
		{
			PvPMitigation = RuleI(PVPCombat, Mitigation2HBlunt);
			appliedByItemType = true;
			break;
		}
	}
	
	switch(player_class)
	{
		case BERSERKER:
		{
			PvPMitigation = RuleI(PVPCombat, MitigationBerserker);
			appliedByItemType = true;
			break;
		}
	}
					
	if ( !appliedByItemType )
	{
		switch(attack_skill)
		{
		case _1H_SLASHING:
		case _1H_BLUNT:
		case PIERCING:
			{
				PvPMitigation = RuleI(PVPCombat, MitigationBySkill);
				break;
			}
		case ARCHERY:
			{
				switch(player_class)
				{
				case RANGER:
					{
						PvPMitigation = RuleI(PVPCombat, RangerMitigationArchery);
						break;
					}
				default:
					{
						PvPMitigation = RuleI(PVPCombat, MitigationArchery);
						break;
					}
				}
				break;
			}
		default:
			{
				if ( spell_id > 0 && spell_id != SPELL_UNKNOWN )
				{
					if ( player_class == BARD )
						PvPMitigation = RuleI(PVPCombat, SpellReductionBard);
					else
						PvPMitigation = RuleI(PVPCombat, SpellReductionOther);			
				}
				else
				{
					switch(player_class)
					{

					case RANGER:
					case BARD:
						{
							PvPMitigation = RuleI(PVPCombat, MitigationClassRangerBard);
							break;
						}
					case PALADIN:
					case SHADOWKNIGHT:
						{
							PvPMitigation = RuleI(PVPCombat, MitigationClassPaladinShadowKnight);
							break;
						}
					case CLERIC:
						{
							PvPMitigation = RuleI(PVPCombat, MitigationClassCleric);
							break;
						}
					case WARRIOR:
						{
							PvPMitigation = RuleI(PVPCombat, MitigationClassWarrior);
							break;
						}
					default:
						{
							PvPMitigation = RuleI(PVPCombat, MitigationClassOther);
							break;
						}
					}
				}

				break;
			}
		}
	}
	
	int diffLevel = level - GetLevel();
	if ( diffLevel > RuleI(PVPCombat,PVPSpellLvlMitigationLow) && diffLevel < RuleI(PVPCombat,PVPSpellLvlMitigationHigh) )
	{
		sint32 maxDmg = (sint32)(float)GetMaxHP()*RuleR(PVPCombat,SpellMaxHPDamageRatio);
		sint32 reducedDmg = (damage * PvPMitigation) / 100;
		if ( spell_id == 982 || spell_id == 88 )
		{
			PvPMitigation = 100;
		}
		else if ( reducedDmg > maxDmg ) // harmtouch and death touch are OK
		{
			PvPMitigation = (maxDmg*100)/damage;
		}
	}

	if ( isDmgShield )
		PvPMitigation = 100;
	
	if ( isDoT && RuleI(PVPCombat,MitigationDoT) > 0 )
		PvPMitigation = RuleI(PVPCombat,MitigationDoT);

	damage = (damage * PvPMitigation) / 100;
}

void Client::Death(Mob* killerMob, sint32 damage, int16 spell, SkillType attack_skill, int32 player_id)
{
	if(!ClientFinishedLoading())
		return;

	if(dead)
		return;	//cant die more than once...

	int exploss;

	mlog(COMBAT__HITS, "Fatal blow dealt by %s with %d damage, spell %d, skill %d, player_id = %i", killerMob ? killerMob->GetName() : "Unknown", damage, spell, attack_skill, player_id);

	//
	// #1: Send death packet to everyone
	//

	if(!spell) spell = SPELL_UNKNOWN;

	SendLogoutPackets();

	//make our become corpse packet, and queue to ourself before OP_Death.
	EQApplicationPacket app2(OP_BecomeCorpse, sizeof(BecomeCorpse_Struct));
	BecomeCorpse_Struct* bc = (BecomeCorpse_Struct*)app2.pBuffer;
	bc->spawn_id = GetID();
	bc->x = GetX();
	bc->y = GetY();
	bc->z = GetZ();
	QueuePacket(&app2);

	// make death packet
	EQApplicationPacket app(OP_Death, sizeof(Death_Struct));
	Death_Struct* d = (Death_Struct*)app.pBuffer;
	d->spawn_id = GetID();
	d->killer_id = killerMob ? killerMob->GetID() : 0;
	d->corpseid=GetID();
	d->bindzoneid = m_pp.binds[0].zoneId;
	d->spell_id = spell == SPELL_UNKNOWN ? 0xffffffff : spell;
	d->attack_skill = spell != SPELL_UNKNOWN ? 0xe7 : attack_skill;
	d->damage = damage;
	app.priority = 6;
	entity_list.QueueClients(this, &app);

	//
	// #2: figure out things that affect the player dying and mark them dead
	//

	InterruptSpell();
	SetPet(0);
	SetHorseId(0);
	dead = true;


	if (killerMob != NULL)
	{
		if(killerMob->IsClient() && (IsDueling() || killerMob->CastToClient()->IsDueling())) {
			SetDueling(false);
			SetDuelTarget(0);
			if (killerMob->IsClient() && killerMob->CastToClient()->IsDueling() && killerMob->CastToClient()->GetDuelTarget() == GetID())
			{
				//if duel opponent killed us...
				killerMob->CastToClient()->SetDueling(false);
				killerMob->CastToClient()->SetDuelTarget(0);
				entity_list.DuelMessage(killerMob,this,false);
				if ( RuleB(PVPCombat,DyeArmorOnDuel) )
					this->MakeDyePacket(RuleI(PVPCombat,DuelArmorDyeRed),RuleI(PVPCombat,DuelArmorDyeGreen),RuleI(PVPCombat,DuelArmorDyeBlue));
			} else {
				//otherwise, we just died, end the duel.
				Mob* who = entity_list.GetMob(GetDuelTarget());
				if(who && who->IsClient()) {
					who->CastToClient()->SetDueling(false);
					who->CastToClient()->SetDuelTarget(0);
				}
			}
		}
	}

	entity_list.RemoveFromTargets(this);
	hate_list.RemoveEnt(this);


	//remove ourself from all proximities
	ClearAllProximities();

	//
	// #3: exp loss and corpse generation
	//

	// figure out if they should lose exp
	if(RuleB(Character, UseDeathExpLossMult)){
		float GetNum [] = {0.005f,0.015f,0.025f,0.035f,0.045f,0.055f,0.065f,0.075f,0.085f,0.095f,0.110f };
		int Num = RuleI(Character, DeathExpLossMultiplier);
		if((Num < 0) || (Num > 10))
			Num = 3;
		float loss = GetNum[Num];
		exploss=(int)((float)GetEXP() * (loss)); //loose % of total XP pending rule (choose 0-10)
	}

	if(!RuleB(Character, UseDeathExpLossMult)){
		exploss = (int)(GetLevel() * (GetLevel() / 18.0) * 12000);
	}

	bool crossZonePVPDeath = false;
	char killerCharName[64] = { "" };

	if ( player_id > 0 && player_id != MAX_PLAYERID )
	{
		if ( player_id == CharacterID() )
		{
			player_id = 0;
			crossZonePVPDeath = false;
		}
		else
		{
			database.GetCharName(player_id, killerCharName);
			if ( strlen(killerCharName) < 1 )
			{
				player_id = 0;
				crossZonePVPDeath = false;
			}
			else
			{
				if ( killerMob == NULL )
				{
					crossZonePVPDeath = true;
					exploss = 0;
				}
				// killerMob can exist and be a corpse (the caster is already dead and their corpse assumed their entity ID below, this Mob pointer (killerMob) is passed from DoBuffTic)
				else if ( !killerMob->IsClient() && killerMob->IsCorpse() && killerMob->CastToCorpse()->GetCharID() > 0 )
				{
					crossZonePVPDeath = true;
					exploss = 0;
				}
			}
		}
	}

	if( (GetLevel() < RuleI(Character, DeathExpLossLevel)) || (GetLevel() > RuleI(Character, DeathExpLossMaxLevel)) || IsBecomeNPC() || IsCharmed() )
	{
		exploss = 0;
	}
	else if( killerMob && killerMob != this )
	{
		if( killerMob->IsClient() )
		{
			exploss = 0;
		}
		else if( killerMob->GetOwner() && killerMob->GetOwner()->IsClient() )
		{
			exploss = 0;
		}
	}

	if(IsValidSpell(spell))
	{
		uint32 buff_count = GetMaxTotalSlots();
		for(uint16 buffIt = 0; buffIt < buff_count; buffIt++)
		{
			if(buffs[buffIt].spellid == spell && buffs[buffIt].client)
			{
				exploss = 0;	// no exp loss for pvp dot
				break;
			}
		}
	}

	char tmp[20];
	database.GetVariable("ServerType", tmp, 9);
	int pvpFlag = atoi(tmp);

	bool LeftCorpse = false;

	bool pvpCorpse = false;
	// now we apply the exp loss, unmem their spells, and make a corpse
	// unless they're a GM (or less than lvl 10
	if(!GetGM())
	{

		Mob* compareMob = killerMob;
		if ( killerMob != NULL && killerMob->GetOwner() != NULL )
			compareMob = killerMob->GetOwner();
		
		if (killerMob != NULL && killerMob->IsNPC()) {
			parse->Event(EVENT_SLAY, killerMob->GetNPCTypeID(), 0, killerMob->CastToNPC(), this);
			killerMob->TrySpellOnKill();
		}

		// Kings & Bandits - Added CompareMob check, xp loss on NPC only
		if(exploss > 0 && compareMob != NULL && !compareMob->IsClient()) {
			sint32 newexp = GetEXP();
			if(exploss > newexp) {
				//lost more than we have... wtf..
				newexp = 1;
			} else {
				newexp -= exploss;
			}
			SetEXP(newexp, GetAAXP());
			//m_epp.perAA = 0;	//reset to no AA exp on death.
		}
		else
			exploss = 0;

		//this generates a lot of 'updates' to the client that the client does not need
		BuffFadeAll();
		if((GetClientVersionBit() & BIT_SoFAndLater) && RuleB(Character, RespawnFromHover))
			UnmemSpellAll(true);
		else
			UnmemSpellAll(false);

		bool npcDeath = true; // setup so that we drop loot if LeaveCorpses is disabled.

		if ( !RuleB(Character,ClientDeathToNPCLeaveCorpse) )
			npcDeath = false;

		// Kings & Bandits - Reset mana in Client fights
		if (  !IsCharmed() && (( compareMob != NULL && compareMob->IsClient() ) || crossZonePVPDeath ) )
		{
			SetHP(GetMaxHP()/2);
			SetMana(0);
			npcDeath = false;
		}

		bool fatalDeath = false; // this means they dont return

		if ( crossZonePVPDeath && player_id > 0 && player_id != MAX_PLAYERID && player_id != CharacterID() && !IsCharmed())
		{
			char msg[512];

			int8 attackerPVPFlag = database.TeamPVPFlag(player_id);
			int32 attackerGuildID = database.GetCharacterGuildID(player_id);
			char* loserGuildName = (char*)guild_mgr.GetGuildName(this->GuildID());
			char* winnerGuildName = (char*)guild_mgr.GetGuildName(attackerGuildID);
			if ( strlen(loserGuildName) < 1 )
				loserGuildName = "<NoGuild>";
			if ( strlen(winnerGuildName) < 1 )
				winnerGuildName = "<NoGuild>";

			char* loserName = KingsBandits::GetKingdomName(this->GetPVPValue(),true);
			char* winnerName = KingsBandits::GetKingdomName(attackerPVPFlag,true);

			if ( strlen(killerCharName) < 1 )
				database.GetCharName(player_id, killerCharName);

			if ( strlen(killerCharName) > 0 ) // we need to check because for whatever reason sometimes we pass a player_id even though its not valid??
			{
				char zoneType[64];
				_snprintf(zoneType,64,"CrossZone");
				if ( ZonedInLinkdead() )
				_snprintf(zoneType,64,"CrossZoneLinkDead");
			
				if ( RuleB(PVPCombat,TeamGuildWars) )
				{
					snprintf(msg,256,"<%s-%s> %s [%s] of The %s has been slain by %s [%s] of The %s!",zone->GetLongName(),zoneType,this->GetName(),loserGuildName,loserName,killerCharName,winnerGuildName, winnerName);
				}
				else
				{
					snprintf(msg,256,"<%s-%s> %s [%s] has been slain by %s [%s]!",zone->GetLongName(),zoneType,this->GetName(),loserGuildName,killerCharName,winnerGuildName);
				}
				worldserver.SendEmoteMessage(0, 0, 15, msg);

				if ( ZonedInLinkdead() )
					database.InsertMessageQueue((char*)GetName(),"!PVPCombatMessage!",7,0,0,(char*)msg,0,true);
			}

			compareMob = NULL; // nullify cause we want to use a cross zone death
		}

		if ( pvpFlag == SERVERTYPE_ZEK && RuleB(PVPCombat, DiscordDeathPerm) && GetPVPValue() == BANDITTEAMID && compareMob && compareMob->IsClient() && compareMob->CastToClient()->GetPVPValue() == BANDITTEAMID )
			fatalDeath = true; // this means we had a discord death bye bye char
		if(!IsCharmed() && (crossZonePVPDeath || fatalDeath || ( RuleB(Character, LeaveCorpses) && GetLevel() >= RuleI(Character, DeathItemLossLevel) ) || RuleB(Character, LeaveNakedCorpses) || IsBecomeNPC()))
		{
			if ( fatalDeath && !npcDeath ) // died to pvp discord, drop items
				npcDeath = true;

			// creating the corpse takes the cash/items off the player too
			Corpse *new_corpse = new Corpse(this, exploss, npcDeath);

			// Kings & Bandits - added a check to see if duel target id is the killer mob, we shouldn't drop items.
			if(!IsBecomeNPC() && pvpFlag>0 && 
				( ( compareMob == NULL && crossZonePVPDeath ) || 
				( ( compareMob != NULL && compareMob->IsClient() && !compareMob->CastToClient()->IsBecomeNPC() && compareMob != this ) && (!IsDueling() || (IsDueling() && compareMob != NULL && GetDuelTarget() != compareMob->GetID())) ) ) ){
					pvpCorpse = true;
					char tmp2[10] = {0};
					database.GetVariable("PvPreward", tmp, 9);
					int reward = atoi(tmp);
					if(reward==3){
						database.GetVariable("PvPitem", tmp2, 9);
						int pvpitem = atoi(tmp2);
						new_corpse->SetPKItem(pvpitem);
						if ( compareMob )
						{
							Group *g = compareMob->CastToClient()->GetGroup();
							if ( g )
							{
								for(int m=0;m<MAX_GROUP_MEMBERS;m++)
								{
									if ( g->members[m] != NULL )
										new_corpse->AddLooter(g->members[m]);
								}
							}
							else
								new_corpse->AddLooter(compareMob);
						}
						else if ( crossZonePVPDeath )
							new_corpse->AddLooter(player_id);
					}
					else if(reward==2)
					{
						if ( fatalDeath )
						{
							new_corpse->SetPKItem(-1);
							new_corpse->AddLooter(this);
							if ( compareMob && compareMob->IsClient() )
							{
								Group *g = compareMob->CastToClient()->GetGroup();
								if ( g )
								{
									for(int m=0;m<MAX_GROUP_MEMBERS;m++)
									{
										if ( g->members[m] != NULL )
											new_corpse->AddLooter(g->members[m]);
									}
								}
								else
									new_corpse->AddLooter(compareMob);
							}
							else if ( crossZonePVPDeath )
								new_corpse->AddLooter(player_id);
						}else
							new_corpse->SetPKItem(0);
					}
					else if(reward==1)
					{
						// Kings & Bandits - Set temporarily to 0, down below we override with the ear number
						new_corpse->SetPKItem(0);
					}
					else
						new_corpse->SetPKItem(0);

					bool legitKill = false;
					uint8 compareMobLvl = 0;
					if ( compareMob && compareMob->IsClient() )
					{
						compareMobLvl = compareMob->GetLevel();
						UpdatePVPStats(compareMob->CastToClient(),&legitKill);

						char buffer [33];
						_snprintf(buffer,33,"%d",compareMob->CastToClient()->CharacterID());
						parse->Event(EVENT_DEATH, 0, (char*)buffer, (NPC*)NULL, this, legitKill);
					}
					else if ( crossZonePVPDeath )
					{
						compareMobLvl = database.GetCharLevel(player_id);

						if ( strlen(killerCharName) < 1 )
							database.GetCharName(player_id, killerCharName);
						if ( strlen(killerCharName) > 0 )
						{
							int32 groupID = 0;
							Group* group = GetGroup();
							if ( group )
								groupID = group->GetID();
							database.InsertMessageQueue(killerCharName,(char*)GetName(),IsBountySet(),GetLevel(),GetPVPValue(),(char*)zone->GetShortName(),0,true,5,GetIP(),GuildID(), GetPP().PVPLastDeath.Name, groupID, (char*)AccountName(), GetPP().PVPKills, GetPP().PVPDeaths);
							time_t now;
							time(&now);

							SetLastDeathInfo(now, killerCharName, compareMobLvl, 0, zone->GetZoneID(), 0, 0);
								
							char buffer [33];
							_snprintf(buffer,33,"%d",player_id);
							parse->Event(EVENT_DEATH, 0, (char*)buffer, (NPC*)NULL, this);
						}
						else
						{
							parse->Event(EVENT_DEATH, 0, "0", (NPC*)NULL, this);
						}
							
					}
					else
					{
						parse->Event(EVENT_DEATH, 0, "0", (NPC*)NULL, this);
					}

					int lvldiff = abs(compareMobLvl - GetLevel());

					if ( fatalDeath )
						WipeCharacter();
					else if (RuleB(PVPCombat,PVPRandomItemDrop))
						new_corpse->RandomItemToCorpse(this);

					bool wasBounty = false;
					if ( IsBountySet() )
					{
						wasBounty = true;
						SetBounty(false);
						UpdateInfamyLevel(-999999);
					}

					if ( RuleB(PVPCombat, DropEars ) )
					{

						int lvlRange = RuleI(PVPCombat, PVPLevelRange);
						if ( lvlRange < 1) // default lvl range check
						{
							if ( RuleI(PVPCombat, PVPZekLevelRange) > 0 )
								lvlRange = RuleI(PVPCombat, PVPZekLevelRange);
							else
								lvlRange = 11;
						}

						// Kings & Bandits - EAR Drop code in attack.cpp
						RandomLootDropResult_Struct rlds;
						memset(&rlds,0,sizeof(RandomLootDropResult_Struct));

						if ( GetLevel() > RuleI(PVPCombat,PVPEarDropLevel) && (crossZonePVPDeath||legitKill) && lvldiff < lvlRange && database.GenerateRandomLootGroupResult(&rlds, EARTEMPLATEID,"UNKNOWN",(char*)GetName(),CharacterID()) )
						{
							for(int i=0;i<rlds.itemDropCount;i++)
							{
								uint32 inItemID = rlds.items[i].itemID;
								if ( inItemID > 0 )
								{
									if ( new_corpse->GetPKItem() == -1 )
										new_corpse->AddItem(inItemID,1);
									else
										new_corpse->SetPKItem(inItemID);
									break;
								}
							}
						}
					}

					if ( RuleB(PVPCombat,PVPCoins) )
					{
						if ( GetLevel() > RuleI(PVPCombat,CoinDropLevel) && legitKill && compareMob != NULL && compareMob->IsClient() )
						{
							// give one to the underdog
							this->PutItemInOpenSlot(RuleI(PVPCombat,CoinItem), 1);
							Message(4,"You have been awarded a PVP Ticket for the battle!");

							Group *g = compareMob->CastToClient()->GetGroup();
							sint32 chg = 1;
							if ( wasBounty )
								chg = 2;
							if ( g != NULL && g != GetGroup() )
							{
								g->GivePVPCoin(RuleI(PVPCombat,CoinItem),chg);
							}
							else
							{
								compareMob->CastToClient()->Message(4,"You have been awarded a PVP Ticket for the battle!");
								compareMob->CastToClient()->PutItemInOpenSlot(RuleI(PVPCombat,CoinItem),chg);
							}
						}
					}

			} // end of pvp servertype statement

			entity_list.AddCorpse(new_corpse, GetID());
			SetID(0);

			//send the become corpse packet to everybody else in the zone.
			entity_list.QueueClients(this, &app2, true);

			LeftCorpse = true;
		}

		//		if(!IsLD())//Todo: make it so an LDed client leaves corpse if its enabled
		//			MakeCorpse(exploss);
	}



#if 0	// solar: commenting this out for now TODO reimplement becomenpc stuff
	if (IsBecomeNPC() == true)
	{
		if (killerMob != NULL && killerMob->IsClient()) {
			if (killerMob->CastToClient()->isgrouped && entity_list.GetGroupByMob(killerMob) != 0)
				entity_list.GetGroupByMob(killerMob->CastToClient())->SplitExp((uint32)(level*level*75*3.5f), this);

			else
				killerMob->CastToClient()->AddEXP((uint32)(level*level*75*3.5f)); // Pyro: Comment this if NPC death crashes zone
			//hate_list.DoFactionHits(GetNPCFactionID());
		}

		Corpse* corpse = new Corpse(this->CastToClient(), 0);
		entity_list.AddCorpse(corpse, this->GetID());
		this->SetID(0);
		if(killerMob->GetOwner() != 0 && killerMob->GetOwner()->IsClient())
			killerMob = killerMob->GetOwner();
		if(killerMob != 0 && killerMob->IsClient()) {
			corpse->AllowMobLoot(killerMob, 0);
			if(killerMob->CastToClient()->isgrouped) {
				Group* group = entity_list.GetGroupByClient(killerMob->CastToClient());
				if(group != 0) {
					for(int i=0; i < MAX_GROUP_MEMBERS; i++) { // Doesnt work right, needs work
						if(group->members[i] != NULL) {
							corpse->AllowMobLoot(group->members[i],i);
						}
					}
				}
			}
		}
	}
#endif

	char TargetName[64] = { "" };
	float x = 0.0f, y=0.0f, z=0.0f;

	if ( killerMob )
	{
		strncpy(TargetName,killerMob->GetName(),64);
		x = killerMob->GetX();
		y = killerMob->GetY();
		z = killerMob->GetZ();
	}

	// KaB / Red69 / Zek - log combat events
	// Handle DB Queries
	if ( killerMob && killerMob->IsClient() )
		database.logcombatevents(this->account_name,this->AccountID(), this->Admin(), this->name, TargetName, "PC Killed by Target PC", 
		0, GetX(),GetY(),GetZ(),(char*)database.GetZoneName(GetZoneID(), GetPP().zoneInstance, true),x,y,z);
	else
		database.logcombatevents(this->account_name,this->AccountID(), this->Admin(), this->name, TargetName, "PC Killed by Target NPC", 
		0, GetX(),GetY(),GetZ(),(char*)database.GetZoneName(GetZoneID(), GetPP().zoneInstance, true),x,y,z);

	//
	// Finally, send em home
	//

	// we change the mob variables, not pp directly, because Save() will copy
	// from these and overwrite what we set in pp anyway
	//

	if(LeftCorpse && !pvpCorpse && (GetClientVersionBit() & BIT_SoFAndLater) && RuleB(Character, RespawnFromHover))
	{
		ClearDraggedCorpses();

		RespawnFromHoverTimer.Start(RuleI(Character, RespawnFromHoverTimer) * 1000);

		SendRespawnBinds();
	}
	else
	{
		if(isgrouped)
		{
			Group *g = GetGroup();
			if(g)
				g->MemberZoned(this);
		}

		Raid* r = entity_list.GetRaidByClient(this);

		if(r)
			r->MemberZoned(this);

		dead_timer.Start(5000, true);

		uint16 lastInstId = m_pp.zoneInstance;
		m_pp.zone_id = m_pp.binds[0].zoneId;

		bool resetPoint = false;
		// KaB only pvpflag setting to nexus on death by a pvp player
		if ( RuleB(PVPCombat,ForceNexusHubOnDeath) && ((pvpCorpse && this != killerMob) || (killerMob != NULL && killerMob->IsNPC() && killerMob->CastToNPC()->teamID > 0)) )
		{
			resetPoint = true;
			m_pp.zone_id = 152; // Nexus default zone
		}

		m_pp.zoneInstance = 0;

		database.MoveCharacterToZone(this->CharacterID(), database.GetZoneName(m_pp.zone_id,m_pp.zoneInstance));

		Save();

		if ( resetPoint )
			MovePC(m_pp.zone_id, 0.0f, 0.0f, 0.0f, 0.0f, 2, GatePVPDeath);
		else
			GoToDeath();
	}
}

bool NPC::Attack(Mob* other, int Hand, bool bRiposte, bool IsStrikethrough)	 // Kaiyodo - base function has changed prototype, need to update overloaded version
{
	_ZP(NPC_Attack);
	int damage = 0;

	if (!other) {
		SetTarget(NULL);
		LogFile->write(EQEMuLog::Error, "A null Mob object was passed to NPC::Attack() for evaluation!");
		//#ifndef WIN32
		//		print_stacktrace();
		//#endif
		return false;
	}

	if(DivineAura())
		return(false);

	if(!GetTarget())
		SetTarget(other);

	//Check that we can attack before we calc heading and face our target	
	if (!IsAttackAllowed(other)) {
		if (this->GetOwnerID())
			entity_list.MessageClose(this, 1, 200, 10, "%s says, 'That is not a legal target master.'", this->GetCleanName());
		if(other)
			RemoveFromHateList(other);
		mlog(COMBAT__ATTACKS, "I am not allowed to attack %s", other->GetName());
		return false;
	}

	if ( ( IsPet() || GetOwner() != NULL ) && GetPetOrder() == SPO_Sit )
	{
		SetPetOrder(SPO_Follow);
		SendAppearancePacket(AT_Anim, ANIM_STAND);
	}
	FaceTarget(GetTarget());

	SkillType skillinuse = HAND_TO_HAND;

	//figure out what weapon they are using, if any
	const Item_Struct* weapon = NULL;
	if (Hand == 13 && equipment[7] > 0)
		weapon = database.GetItem(equipment[7]);
	else if (equipment[8])
		weapon = database.GetItem(equipment[8]);

	uint8 itemType = 0;
	//We dont factor much from the weapon into the attack.
	//Just the skill type so it doesn't look silly using punching animations and stuff while wielding weapons
	if(weapon) {
		mlog(COMBAT__ATTACKS, "Attacking with weapon: %s (%d) (too bad im not using it for much)", weapon->Name, weapon->ID);

		itemType = weapon->ItemType;

		if(Hand == 14 && weapon->ItemType == ItemTypeShield){
			mlog(COMBAT__ATTACKS, "Attack with shield canceled.");
			return false;
		}

		switch(weapon->ItemType){
		case ItemType1HS:
			skillinuse = _1H_SLASHING;
			break;
		case ItemType2HS:
			skillinuse = _2H_SLASHING;
			break;
		case ItemTypePierce:
		case ItemType2HPierce:
			skillinuse = PIERCING;
			break;
		case ItemType1HB:
			skillinuse = _1H_BLUNT;
			break;
		case ItemType2HB:
			skillinuse = _2H_BLUNT;
			break;
		case ItemTypeBow:
			skillinuse = ARCHERY;
			break;
		case ItemTypeThrowing:
		case ItemTypeThrowingv2:
			skillinuse = THROWING;
			break;
		default:
			skillinuse = HAND_TO_HAND;
			break;
		}
	}

	int weapon_damage = GetWeaponDamage(other, weapon);

	//do attack animation regardless of whether or not we can hit below
	sint16 charges = 0;
	ItemInst weapon_inst(weapon, charges);

	AttackAnimation(skillinuse, Hand, &weapon_inst);

	// Kings & Bandits - not the cleanest way but lets find out if it changed because of the texture
	/*if (Hand == 13) {
	skillinuse = static_cast<SkillType>(GetPrimSkill());
	}
	else if (Hand == 14) {
	skillinuse = static_cast<SkillType>(GetSecSkill());
	}*/

	//Work-around for there being no 2HP skill - We use 99 for the 2HB animation and 36 for pierce messages
	if(skillinuse == 99)
		skillinuse = static_cast<SkillType>(36);

	//basically "if not immune" then do the attack
	if((weapon_damage) > 0) {

		//ele and bane dmg too
		//NPCs add this differently than PCs
		//if NPCs can't inheriently hit the target we don't add bane/magic dmg which isn't exactly the same as PCs
		int16 eleBane = 0;
		if(weapon){
			if(weapon->BaneDmgBody == other->GetBodyType()){
				eleBane += weapon->BaneDmgAmt;
			}

			if(weapon->BaneDmgRace == other->GetRace()){
				eleBane += weapon->BaneDmgRaceAmt;
			}

			if(weapon->ElemDmgAmt){
				eleBane += (weapon->ElemDmgAmt * other->ResistSpell(weapon->ElemDmgType, 0, this) / 100);
			}
		}

		if(!RuleB(NPC, UseItemBonusesForNonPets)){
			if(!GetOwner()){
				eleBane = 0;
			}
		}

		int8 otherlevel = other->GetLevel();
		int8 mylevel = this->GetLevel();

		otherlevel = otherlevel ? otherlevel : 1;
		mylevel = mylevel ? mylevel : 1;

		//instead of calcing damage in floats lets just go straight to ints
		if(RuleB(Combat, UseIntervalAC))
			damage = (max_dmg+eleBane);
		else
			damage = MakeRandomInt((min_dmg+eleBane),(max_dmg+eleBane));


		//check if we're hitting above our max or below it.
		if((min_dmg+eleBane) != 0 && damage < (min_dmg+eleBane)) {
			mlog(COMBAT__DAMAGE, "Damage (%d) is below min (%d). Setting to min.", damage, (min_dmg+eleBane));
			damage = (min_dmg+eleBane);
		}
		if((max_dmg+eleBane) != 0 && damage > (max_dmg+eleBane)) {
			mlog(COMBAT__DAMAGE, "Damage (%d) is above max (%d). Setting to max.", damage, (max_dmg+eleBane));
			damage = (max_dmg+eleBane);
		}

		sint32 hate = damage;
		if(IsPet())
		{
			hate = hate * 100 / GetDamageTable(skillinuse);
		}
		//THIS IS WHERE WE CHECK TO SEE IF WE HIT:
		if(other->IsClient() && other->CastToClient()->IsSitting()) {
			mlog(COMBAT__DAMAGE, "Client %s is sitting. Hitting for max damage (%d).", other->GetName(), (max_dmg+eleBane));
			damage = (max_dmg+eleBane);

			damage += (GetSTR() / 10) + (damage * other->GetSkillDmgTaken(skillinuse) / 100) + GetSkillDmgAmt(skillinuse);

			mlog(COMBAT__HITS, "Generating hate %d towards %s", hate, GetName());
			// now add done damage to the hate list
			other->AddToHateList(this, hate);
		} else {
			if(!other->CheckHitChance(this, skillinuse, Hand)) {
				damage = 0;	//miss
			} else {	//hit, check for damage avoidance
				other->AvoidDamage(this, damage);
				other->MeleeMitigation(this, damage, min_dmg+eleBane);
				if(damage > 0) {
					ApplyMeleeDamageBonus(skillinuse, damage);
					damage += (itembonuses.STR / 10) + (damage * other->GetSkillDmgTaken(skillinuse) / 100) + GetSkillDmgAmt(skillinuse);
					TryCriticalHit(other, skillinuse, itemType, damage);
				}
				mlog(COMBAT__HITS, "Generating hate %d towards %s", hate, GetName());
				// now add done damage to the hate list
				if(damage > 0)
				{
					other->AddToHateList(this, hate);
				}
				else
					other->AddToHateList(this, 0);
			}
		}

		mlog(COMBAT__DAMAGE, "Final damage against %s: %d", other->GetName(), damage);

		if(other->IsClient() && IsPet() && GetOwner()->IsClient()) {
			//pets do half damage to clients in pvp
			if ( IsCharmed() )
			{
				damage=(int) ((float)damage * ((float)RuleI(PVPCombat,CharmedMobDmgReduction)/100.0f));
			}
			else
				damage=(int) ((float)damage * ((float)RuleI(PVPCombat,PetMobDmgReduction)/100.0f));
		}
	}
	else
		damage = -5;

	//cant riposte a riposte
	if (bRiposte && damage == -3) {
		mlog(COMBAT__DAMAGE, "Riposte of riposte canceled.");
		return false;
	}

	if(GetHP() > 0 && other->GetHP() >= -11) {
		other->Damage(this, damage, SPELL_UNKNOWN, skillinuse, itemType, false); // Not avoidable client already had thier chance to Avoid
	}


	//break invis & hide when you attack
	BreakMobInvis();
	BreakMobHide();


	hidden = false;
	improved_hidden = false;

	//I doubt this works...
	if (!GetTarget())
		return true; //We killed them

	if( !bRiposte && other->GetHP() > 0 ) {
		TryWeaponProc(weapon, other, Hand);	//no weapon
	}

	// now check ripostes
	if (damage == -3) { // riposting
		DoRiposte(other);
	}

	if (damage > 0)
	{
		other->TriggerDefensiveProcs(this);
		return true;
	}
	else
		return false;
}

void NPC::Damage(Mob* other, sint32 damage, int16 spell_id, SkillType attack_skill, uint8 itemType, 
bool avoidable, sint8 buffslot, bool iBuffTic, bool isDmgShield, bool isCritHit, int32 player_id, bool isDoT) {
	if(spell_id==0)							 
		spell_id = SPELL_UNKNOWN;
	//handle EVENT_ATTACK. Resets after we have not been attacked for 12 seconds
	if(attacked_timer.Check() && other != NULL) 
	{
		mlog(COMBAT__HITS, "Triggering EVENT_ATTACK due to attack by %s", other->GetName());
		parse->Event(EVENT_ATTACK, this->GetNPCTypeID(), 0, this, other);
	}
	attacked_timer.Start(CombatEventTimer_expire);

	if (!IsEngaged())
		zone->AddAggroMob();

	if(GetClass() == LDON_TREASURE) 
	{
		if(IsLDoNLocked() && GetLDoNLockedSkill() != LDoNTypeMechanical) 
		{
			damage = -5;
		}
		else
		{
			if(IsLDoNTrapped())
			{
				Message_StringID(13, LDON_ACCIDENT_SETOFF2);
				SpellFinished(GetLDoNTrapSpellID(), other, 10, 0, -1, spells[GetLDoNTrapSpellID()].ResistDiff);
				SetLDoNTrapSpellID(0);
				SetLDoNTrapped(false);
				SetLDoNTrapDetected(false);
			}
		}
	}

	//do a majority of the work...
	CommonDamage(other, damage, spell_id, attack_skill, avoidable, buffslot, iBuffTic, isDoT);

	if(damage > 0) {
		//see if we are gunna start fleeing
		Mob* master = other;

		if(!IsPet()) CheckFlee();
	}
}

void NPC::Death(Mob* killerMob, sint32 damage, int16 spell, SkillType attack_skill, int32 player_id) {
	this->AddedLootGroupInPerl = false;
	_ZP(NPC_Death);
	mlog(COMBAT__HITS, "Fatal blow dealt by %s with %d damage, spell %d, skill %d", killerMob->GetName(), damage, spell, attack_skill);

	if (this->IsEngaged())
	{
		zone->DelAggroMob();
#if EQDEBUG >= 11
		LogFile->write(EQEMuLog::Debug,"NPC::Death() Mobs currently Aggro %i", zone->MobsAggroCount());
#endif
	}
	SetHP(0);
	SetPet(0);
	Mob* killer = GetHateDamageTop(this);

	entity_list.RemoveFromTargets(this);

	if(p_depop == true)
		return;

	BuffFadeAll();

	EQApplicationPacket* app= new EQApplicationPacket(OP_Death,sizeof(Death_Struct));
	Death_Struct* d = (Death_Struct*)app->pBuffer;
	d->spawn_id = GetID();
	d->killer_id = killerMob ? killerMob->GetID() : 0;
	//	d->unknown12 = 1;
	d->bindzoneid = 0;
	d->spell_id = spell == SPELL_UNKNOWN ? 0xffffffff : spell;
	d->attack_skill = SkillDamageTypes[attack_skill];
	d->damage = damage;
	app->priority = 6;
	entity_list.QueueClients(killerMob, app, false);

	if(respawn2) {
		float mod = 0.0f;
		if ( RuleB(Zone, DeathRespawnMod ) && CastToNPC()->teamID == 0 )
		{
			int32 deathCount = respawn2->getDeathCount();
			if ( deathCount > 0 )
				mod = (( 1.0f / ((float)log((float)deathCount)+(float)RuleR(Zone,DeathRespawnLogBase)) ) + (float)RuleR(Zone,DeathRespawnGraphBase));
		}

		respawn2->DeathReset(mod);
	}

	if (killerMob) {
		if(GetClass() != LDON_TREASURE)
			hate_list.Add(killerMob, damage);
	}

	safe_delete(app);

	Mob *give_exp = NULL;
	
	if ( teamID > 0 )
		give_exp = hate_list.GetDamageTop(this, true);
	else
		give_exp = hate_list.GetDamageTop(this);

	if(give_exp == NULL)
		give_exp = killer;

	if(give_exp && give_exp->HasOwner())
		give_exp = give_exp->GetUltimateOwner();

	Client *give_exp_client = NULL;
	if(give_exp && give_exp->IsClient())
		give_exp_client = give_exp->CastToClient();


	char TargetName[64] = { "" };
	float x = 0.0f, y=0.0f, z=0.0f;

	if ( killerMob )
	{
		strncpy(TargetName,killerMob->GetName(),64);
		x = killerMob->GetX();
		y = killerMob->GetY();
		z = killerMob->GetZ();
	}

	// KaB / Red69 / Zek - log combat events
	// Handle DB Queries
	if ( give_exp_client )
		database.logcombatevents(give_exp_client->AccountName(),give_exp_client->AccountID(), give_exp_client->Admin(), GetName(), TargetName, "NPC Killed by Target PC", 
		0, give_exp_client->GetX(),give_exp_client->GetY(),give_exp_client->GetZ(),(char*)database.GetZoneName(GetZoneID(), give_exp_client->GetPP().zoneInstance, true),x,y,z);
	else
		database.logcombatevents("NA",0,0,GetName(), TargetName, "NPC Killed by Target NPC", 
		0, GetX(),GetY(),GetZ(),(char*)zone->GetShortName(),x,y,z);

	bool IsLdonTreasure = (this->GetClass() == LDON_TREASURE);
	if (give_exp_client && !IsCorpse() && MerchantType == 0)
	{
		Group *kg = entity_list.GetGroupByClient(give_exp_client);
		Raid *kr = entity_list.GetRaidByClient(give_exp_client);

		if(kr)
		{
			if(!IsLdonTreasure) {
				kr->SplitExp((EXP_FORMULA), this);
				if(killerMob && (kr->IsRaidMember(killerMob->GetName()) || kr->IsRaidMember(killerMob->GetUltimateOwner()->GetName())))
					killerMob->TrySpellOnKill();
			}
			/* Send the EVENT_KILLED_MERIT event for all raid members */
			for (int i = 0; i < MAX_RAID_MEMBERS; i++) {
				if (kr->members[i].member != NULL) { // If Group Member is Client
					parse->Event(EVENT_KILLED_MERIT, GetNPCTypeID(), "killed", this, kr->members[i].member);
					if(RuleB(TaskSystem, EnableTaskSystem))
						kr->members[i].member->UpdateTasksOnKill(GetNPCTypeID());
				}
			}
		}
		else if (give_exp_client->IsGrouped() && kg != NULL)
		{
			if(!IsLdonTreasure) {
				kg->SplitExp((EXP_FORMULA), this);
				if(killerMob && (kg->IsGroupMember(killerMob->GetName()) || kg->IsGroupMember(killerMob->GetUltimateOwner()->GetName())))
					killerMob->TrySpellOnKill();
			}
			/* Send the EVENT_KILLED_MERIT event and update kill tasks
			* for all group members */
			for (int i = 0; i < MAX_GROUP_MEMBERS; i++) {
				if (kg->members[i] != NULL && kg->members[i]->IsClient()) { // If Group Member is Client
					Client *c = kg->members[i]->CastToClient();
					parse->Event(EVENT_KILLED_MERIT, GetNPCTypeID(), "killed", this, c);
					if(RuleB(TaskSystem, EnableTaskSystem))
						c->UpdateTasksOnKill(GetNPCTypeID());
				}
			}
		}
		else
		{
			if(!IsLdonTreasure && ( GetOwner() == NULL || (this->GetOwner() != NULL && !this->GetOwner()->IsClient()) ) ) {
				int conlevel = give_exp->GetLevelCon(GetLevel());
				if (conlevel != CON_GREEN)
				{
					give_exp_client->AddEXP((EXP_FORMULA), conlevel); // Pyro: Comment this if NPC death crashes zone
					if(killerMob && (killerMob->GetID() == give_exp_client->GetID() || killerMob->GetUltimateOwner()->GetID() == give_exp_client->GetID()))
						killerMob->TrySpellOnKill();
				}
			}
			/* Send the EVENT_KILLED_MERIT event */
			parse->Event(EVENT_KILLED_MERIT, GetNPCTypeID(), "killed", this, give_exp_client);
			if(RuleB(TaskSystem, EnableTaskSystem))
				give_exp_client->UpdateTasksOnKill(GetNPCTypeID());
		}
	}


	bool teamMob = false;
	if ( CastToNPC()->teamID > 0 )
		teamMob = true;

	//do faction hits even if we are a merchant, so long as a player killed us
	if(give_exp_client && !teamMob)
		hate_list.DoFactionHits(GetNPCFactionID());

	// Parse quests even if we're killed by an NPC
	if(killerMob) {
		Mob *oos = give_exp_client;
		if ( !oos )
			oos = killerMob->GetOwnerOrSelf();
		
		char buffer [33];
		_snprintf(buffer,33,"0");
		if ( oos && oos->IsClient() )
		{
			_snprintf(buffer,33,"%d",oos->CastToClient()->CharacterID());
		}
		parse->Event(EVENT_DEATH, this->GetNPCTypeID(),(char*)buffer, this->CastToNPC(), oos);

		if(oos->IsNPC())
		{
			parse->Event(EVENT_NPC_SLAY, this->GetNPCTypeID(), 0, oos->CastToNPC(), this);
			killerMob->TrySpellOnKill();
		}
	}

	if ( give_exp_client != NULL )
		killer = give_exp_client;
	else
		killer = NULL;

	if (!HasOwner() && class_ != MERCHANT && class_ != ADVENTUREMERCHANT && !GetSwarmInfo()
		&& MerchantType == 0 && killer && (killer->IsClient() || (killer->HasOwner() && killer->GetUltimateOwner()->IsClient()) ||
		(killer->IsNPC() && killer->CastToNPC()->GetSwarmInfo() && killer->CastToNPC()->GetSwarmInfo()->GetOwner() && killer->CastToNPC()->GetSwarmInfo()->GetOwner()->IsClient()))) {
			Corpse* corpse = new Corpse(this, &itemlist, GetNPCTypeID(), &NPCTypedata,level>54?RuleI(NPC,MajorNPCCorpseDecayTimeMS):RuleI(NPC,MinorNPCCorpseDecayTimeMS),teamMob);


			char tmp[20];
			database.GetVariable("ServerType", tmp, 9);
			int pvpFlag = atoi(tmp);

			if ( GetLevel() > RuleI(PVPCombat,GuardChanceLevel) && CastToNPC()->teamID > 0 && RuleB(PVPCombat, GuardDrop) && RuleI(PVPCombat,GuardItem) > 0 && KingsBandits::isCityLocation(CastToNPC()->teamID) )
			{
				TeamSettings_Struct* settings = database.GetTeamSettings(CastToNPC()->teamID);

				if ( settings )
				{
					bool teamGuildWars = RuleB(PVPCombat,TeamGuildWars);
					if ( give_exp_client != NULL && 
						(  (!teamGuildWars && pvpFlag != SERVERTYPE_KAB && (settings->guildid == 0 || settings->guildid != give_exp_client->GuildID()))
						|| (!teamGuildWars && pvpFlag == SERVERTYPE_KAB && CastToNPC()->teamID > 0 && give_exp_client->GetPVPValue() > 0 && 
						CastToNPC()->teamID != give_exp_client->GetPVPValue())
						|| (teamGuildWars && settings->ownerteamid > 0 && give_exp_client->GetPVPValue() > 0 && 
						settings->ownerteamid != give_exp_client->GetPVPValue()) )
						)
					{
						int conlevel = give_exp_client->GetLevelCon(GetLevel());
						int chanceToDrop = (int) (RuleI(PVPCombat, GuardChanceDrop) * ((float)GetLevel()/(float)(RuleI(PVPCombat, GuardChanceLevelCap)+1)));

						if ( conlevel != CON_GREEN && MakeRandomInt(0,100) < chanceToDrop ){
							corpse->AddItem(RuleI(PVPCombat,GuardItem),1);
							corpse->SetDecayTimer(600000);
						}
					}
				}
			}

			if ( give_exp_client != NULL && GetLevel() > RuleI(PVPCombat,NPCChanceLevel) && CastToNPC()->teamID == 0 && zone->IsPVPHotzone() > 0 )
			{
				int conlevel = give_exp_client->GetLevelCon(GetLevel());
				int chanceToDrop = (int) ((float)zone->IsPVPHotzone() * ((float)GetLevel()/(float)(RuleI(PVPCombat, NPCChanceLevelCap)+1)));
				if ( RuleI(PVPCombat,NPCItem) > 0 && conlevel != CON_GREEN && MakeRandomInt(0,100) < chanceToDrop ){
					corpse->AddItem(RuleI(PVPCombat,NPCItem),1);
					corpse->SetDecayTimer(600000);
				}
			}

			entity_list.LimitRemoveNPC(this);
			entity_list.AddCorpse(corpse, this->GetID());

			entity_list.UnMarkNPC(GetID());
			entity_list.RemoveNPC(GetID());
			this->SetID(0);

			// Kings & Bandits - move mob to player for easy loot
			if ( killer->IsClient() )
			{
				corpse->Teleport(VERTEX(killer->GetX(),killer->GetY(),killer->GetZ()));
				corpse->SendPosition();
			}

			if(killer != 0 && killer->IsClient()) {
				corpse->AllowMobLoot(killer, 0);
				if(killer->IsGrouped()) {
					Group* group = entity_list.GetGroupByClient(killer->CastToClient());
					if(group != 0) {
						for(int i=0;i<6;i++) { // Doesnt work right, needs work
							if(group->members[i] != NULL) {
								corpse->AllowMobLoot(group->members[i],i);
							}
						}
					}
				}
				else if(killer->IsRaidGrouped()){
					Raid* r = entity_list.GetRaidByClient(killer->CastToClient());
					if(r){
						int i = 0;
						for(int x = 0; x < MAX_RAID_MEMBERS; x++)
						{
							switch(r->GetLootType())
							{
							case 0:
							case 1:
								if(r->members[x].member && r->members[x].IsRaidLeader){
									corpse->AllowMobLoot(r->members[x].member, i);
									i++;
								}
								break;
							case 2:
								if(r->members[x].member && r->members[x].IsRaidLeader){
									corpse->AllowMobLoot(r->members[x].member, i);
									i++;
								}
								else if(r->members[x].member && r->members[x].IsGroupLeader){
									corpse->AllowMobLoot(r->members[x].member, i);
									i++;
								}
								break;
							case 3:
								if(r->members[x].member && r->members[x].IsLooter){
									corpse->AllowMobLoot(r->members[x].member, i);
									i++;
								}
								break;
							case 4:
								if(r->members[x].member)
								{
									corpse->AllowMobLoot(r->members[x].member, i);
									i++;
								}
								break;
							}
						}
					}
				}
			}

			if(zone && zone->adv_data)
			{
				ServerZoneAdventureDataReply_Struct *sr = (ServerZoneAdventureDataReply_Struct*)zone->adv_data;
				if(sr->type == Adventure_Kill)
				{
					zone->DoAdventureCountIncrease();
				}
				else if(sr->type == Adventure_Assassinate)
				{
					if(sr->data_id == GetNPCTypeID())
					{
						zone->DoAdventureCountIncrease();
					}
					else
					{
						zone->DoAdventureAssassinationCountIncrease();
					}
				}
			}
	}
	this->WipeHateList();
	p_depop = true;
	if(killerMob && killerMob->GetTarget() == this) //we can kill things without having them targeted
		killerMob->SetTarget(NULL); //via AE effects and such..

	entity_list.UpdateFindableNPCState(this, true);
}

void Mob::AddToHateList(Mob* other, sint32 hate, sint32 damage, bool iYellForHelp, bool bFrenzy, bool iBuffTic, bool skipEventAggro) {
	if ( other == NULL )
		return;

	if (other == this)
		return;

	if ( other->IsNPC() && IsNPC() && other->CastToNPC()->teamID > 0 && other->CastToNPC()->teamID == CastToNPC()->teamID && !other->IsCharmed() && !IsCharmed() )
		return;

	if(damage < 0){
		damage = 0;
		hate = 1;
	}

	bool wasengaged = IsEngaged();
	Mob* owner = other->GetOwner();
	Mob* mypet = this->GetPet();
	Mob* myowner = this->GetOwner();

	if(other){
		AddRampage(other);
		int hatemod = 100 + other->spellbonuses.hatemod + other->itembonuses.hatemod + other->aabonuses.hatemod;
		if(hatemod < 1)
			hatemod = 1;
		hate = ((hate * (hatemod))/100);
	}

	if(IsPet() && GetOwner() && GetOwner()->GetAA(aaPetDiscipline) && IsHeld()){
		return; 
	}	

	if(IsClient() && !IsAIControlled())
		return;

	if(IsFamiliar() || SpecAttacks[IMMUNE_AGGRO])
		return;	

	if (other == myowner)
		return;

	if(other->SpecAttacks[IMMUNE_TARGET])
		return;

	// first add self

	// The damage on the hate list is used to award XP to the killer. This check is to prevent Killstealing.
	// e.g. Mob has 5000 hit points, Player A melees it down to 500 hp, Player B executes a headshot (10000 damage).
	// If we add 10000 damage, Player B would get the kill credit, so we only award damage credit to player B of the
	// amount of HP the mob had left.
	//
	if(damage > GetHP())
		damage = GetHP();

	hate_list.Add(other, hate, damage, bFrenzy, !iBuffTic);

#ifdef BOTS
	// if other is a bot, add the bots client to the hate list
	if(other->IsBot()) {
		if(other->CastToBot()->GetBotOwner() && other->CastToBot()->GetBotOwner()->CastToClient()->GetFeigned()) {
			AddFeignMemory(other->CastToBot()->GetBotOwner()->CastToClient());
		}
		else {
			if(!hate_list.IsOnHateList(other->CastToBot()->GetBotOwner()))
				hate_list.Add(other->CastToBot()->GetBotOwner(), 0, 0, false, true);
		}
	}
#endif //BOTS

	// then add pet owner if there's one
	if (owner) { // Other is a pet, add him and it
		// EverHood 6/12/06
		// Can't add a feigned owner to hate list
		if(owner->IsClient() && owner->CastToClient()->GetFeigned()) {
			//they avoid hate due to feign death...
		} else {
			// cb:2007-08-17
			// owner must get on list, but he's not actually gained any hate yet
			if(!owner->SpecAttacks[IMMUNE_AGGRO])
				hate_list.Add(owner, 0, 0, false, !iBuffTic);
		}
	}	

	if (mypet && (!(GetAA(aaPetDiscipline) && mypet->IsHeld()))) { // I have a pet, add other to it
		if(!mypet->IsFamiliar() && !mypet->SpecAttacks[IMMUNE_AGGRO])
			mypet->hate_list.Add(other, 0, 0, bFrenzy);
	} else if (myowner) { // I am a pet, add other to owner if it's NPC/LD
		if (myowner->IsAIControlled() && !myowner->SpecAttacks[IMMUNE_AGGRO])
			myowner->hate_list.Add(other, 0, 0, bFrenzy);
	}
	if (!wasengaged) { 
		// 2018 - Removed client checks for other target, this was not triggering EVENT_AGGRO on NPC's due to this check (eg. pets)
		if(!skipEventAggro && IsNPC())
		{
			if ( (!aggroparser_timer.Enabled()) || aggroparser_timer.Check(false) )
			{
				parse->Event(EVENT_AGGRO, this->GetNPCTypeID(), 0, CastToNPC(), other);
				aggroparser_timer.Start(1000);
			}
		}
		AI_Event_Engaged(other, iYellForHelp); 
		adverrorinfo = 8293;
	}
}

// solar: this is called from Damage() when 'this' is attacked by 'other.  
// 'this' is the one being attacked
// 'other' is the attacker
// a damage shield causes damage (or healing) to whoever attacks the wearer
// a reverse ds causes damage to the wearer whenever it attack someone
// given this, a reverse ds must be checked each time the wearer is attacking
// and not when they're attacked
//a damage shield on a spell is a negative value but on an item it's a positive value so add the spell value and subtract the item value to get the end ds value
void Mob::DamageShield(Mob* attacker, bool spell_ds) {

	if(!attacker || this == attacker) 
		return;

	int DS = 0;
	int rev_ds = 0;
	int16 spellid = 0;

	if(!spell_ds) 
	{
		DS = spellbonuses.DamageShield;
		rev_ds = attacker->spellbonuses.ReverseDamageShield;

		if(spellbonuses.DamageShieldSpellID != 0 && spellbonuses.DamageShieldSpellID != SPELL_UNKNOWN)
			spellid = spellbonuses.DamageShieldSpellID;
	}
	else {	
		DS = spellbonuses.SpellDamageShield;
		rev_ds = 0;
		// This ID returns "you are burned", seemed most appropriate for spell DS
		spellid = 2166;
	}

	if(DS == 0 && rev_ds == 0)
		return;

	mlog(COMBAT__HITS, "Applying Damage Shield of value %d to %s", DS, attacker->GetName());

	//invert DS... spells yield negative values for a true damage shield
	if(DS < 0) {
		if(!spell_ds)	{
			if(IsClient()) {
				int dsMod = 100;
				switch (CastToClient()->GetAA(aaCoatofThistles))
				{
				case 1:
					dsMod = 110;
					break;
				case 2:
					dsMod = 115;
					break;
				case 3:
					dsMod = 120;
					break;
				case 4:
					dsMod = 125;
					break;
				case 5:
					dsMod = 130;
					break;
				}
				DS = ((DS * dsMod) / 100);
			}
			DS -= itembonuses.DamageShield; //+Damage Shield should only work when you already have a DS spell
			DS += attacker->itembonuses.DSMitigation + attacker->spellbonuses.DSMitigation;
		}
		attacker->Damage(this, -DS, spellid, ABJURE/*hackish*/,0, false, -1, false, true);
		//we can assume there is a spell now
		EQApplicationPacket* outapp = new EQApplicationPacket(OP_Damage, sizeof(CombatDamage_Struct));
		CombatDamage_Struct* cds = (CombatDamage_Struct*)outapp->pBuffer;
		cds->target = attacker->GetID();
		cds->source = GetID();
		cds->type = spellbonuses.DamageShieldType;
		cds->spellid = 0x0;
		cds->damage = DS;
		entity_list.QueueCloseClients(this, outapp);
		safe_delete(outapp);
	} else if (DS > 0 && !spell_ds) {
		//we are healing the attacker...
		attacker->HealDamage(DS);
		//TODO: send a packet???
	}

	//Reverse DS
	//this is basically a DS, but the spell is on the attacker, not the attackee
	//if we've gotten to this point, we know we know "attacker" hit "this" (us) for damage & we aren't invulnerable
	uint16 rev_ds_spell_id = SPELL_UNKNOWN;

	if(spellbonuses.ReverseDamageShieldSpellID != 0 && spellbonuses.ReverseDamageShieldSpellID != SPELL_UNKNOWN)
		rev_ds_spell_id = spellbonuses.ReverseDamageShieldSpellID;

	if(rev_ds < 0) {
		mlog(COMBAT__HITS, "Applying Reverse Damage Shield of value %d to %s", rev_ds, attacker->GetName());
		attacker->Damage(this, -rev_ds, rev_ds_spell_id, ABJURE/*hackish*/, -1, false, 0,false, true); //"this" (us) will get the hate, etc. not sure how this works on Live, but it'll works for now, and tanks will love us for this
		//do we need to send a damage packet here also?
		/*
		EQApplicationPacket* outapp = new EQApplicationPacket(OP_Damage, sizeof(CombatDamage_Struct));
		CombatDamage_Struct* cds = (CombatDamage_Struct*)outapp->pBuffer;
		cds->target = attacker->GetID();
		cds->source = GetID();
		cds->type = attacker->spellbonuses.ReverseDamageShieldType;
		cds->spellid = 0x0;
		cds->damage = rev_ds;
		entity_list.QueueCloseClients(this, outapp);
		safe_delete(outapp);
		*/
	}
}

//https://github.com/EQEmu/Server/commit/9408403a00c5f984a4cd1528dfd2a6afda09553d
uint8 Mob::GetWeaponDamageBonus(const Item_Struct *weapon, bool offhand) // offhand only applies to sinister strikes or AA, item bonuses for secondary
{
	// dev quote with old and new formulas
	// https://forums.daybreakgames.com/eq/index.php?threads/test-update-09-17-15.226618/page-5#post-3326194
	//
	// We assume that the level check is done before calling this function and sinister strikes is checked before
	// calling for offhand DB
	uint8 level = GetLevel();
	if (!weapon)
		return 1 + ((level - 28) / 3); // how does weaponless scale?

	uint8 delay = weapon->Delay;
	if (weapon->ItemType == ItemType1HS || weapon->ItemType == ItemType1HB ||
		weapon->ItemType == ItemTypeHand2Hand || weapon->ItemType == ItemTypePierce) {
			// we assume sinister strikes is checked before calling here
			if (!offhand) {
				if (delay <= 39)
					return 1 + ((level - 28) / 3);
				else if (delay < 43)
					return 2 + ((level - 28) / 3) + ((delay - 40) / 3);
				else if (delay < 45)
					return 3 + ((level - 28) / 3) + ((delay - 40) / 3);
				else if (delay >= 45)
					return 4 + ((level - 28) / 3) + ((delay - 40) / 3);
			} else {
				return 1 + ((level - 28) / 3) * (delay / 30);
			}
	} else {
		// 2h damage bonus
		if (delay <= 27)
			return 1 + ((level - 28) / 3);
		else if (delay < 40)
			return 1 + ((level - 28) / 3) + ((level - 30) / 5);
		else if (delay < 43)
			return 2 + ((level - 28) / 3) + ((level - 30) / 5) + ((delay - 40) / 3);
		else if (delay < 45)
			return 3 + ((level - 28) / 3) + ((level - 30) / 5) + ((delay - 40) / 3);
		else if (delay >= 45)
			return 4 + ((level - 28) / 3) + ((level - 30) / 5) + ((delay - 40) / 3);
	}
}

int Mob::GetMonkHandToHandDamage(void)
{
	// Kaiyodo - Determine a monk's fist damage. Table data from www.monkly-business.com
	// saved as static array - this should speed this function up considerably
	static int damage[66] = {
		//   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19
		99, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 9, 9, 9, 9, 9,10,10,10,10,10,11,11,11,11,11,
		12,12,12,12,12,13,13,13,13,13,14,14,14,14,14,14,14,14,14,14,
		14,14,15,15,15,15 };

		// Have a look to see if we have epic fists on

		if (IsClient() && CastToClient()->GetItemIDAt(12) == 10652)
			return(9);
		else
		{
			int Level = GetLevel();
			if (Level > 65)
				return(19);
			else
				return damage[Level];
		}
}

int Mob::GetMonkHandToHandDelay(void)
{
	// Kaiyodo - Determine a monk's fist delay. Table data from www.monkly-business.com
	// saved as static array - this should speed this function up considerably
	static int delayshuman[66] = {
		//  0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19
		99,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,
		36,36,36,36,36,35,35,35,35,35,34,34,34,34,34,33,33,33,33,33,
		32,32,32,32,32,31,31,31,31,31,30,30,30,29,29,29,28,28,28,27,
		26,24,22,20,20,20  };
		static int delaysiksar[66] = {
			//  0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19
			99,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,
			36,36,36,36,36,36,36,36,36,36,35,35,35,35,35,34,34,34,34,34,
			33,33,33,33,33,32,32,32,32,32,31,31,31,30,30,30,29,29,29,28,
			27,24,22,20,20,20 };

			// Have a look to see if we have epic fists on
			if (IsClient() && CastToClient()->GetItemIDAt(12) == 10652)
				return(16);
			else
			{
				int Level = GetLevel();
				if (GetRace() == HUMAN)
				{
					if (Level > 65)
						return(24);
					else
						return delayshuman[Level];
				}
				else	//heko: iksar table
				{
					if (Level > 65)
						return(25);
					else
						return delaysiksar[Level];
				}
			}
}

sint32 Mob::ReduceDamage(sint32 damage)
{
	if(damage <= 0)
		return damage;

	int slot = GetBuffSlotFromType(SE_NegateAttacks);
	if(slot >= 0) {
		if(CheckHitsRemaining(slot, false, true))
			return -6;
	}

	slot = GetBuffSlotFromType(SE_MitigateMeleeDamage);
	if(slot >= 0)
	{
		int damage_to_reduce = damage * GetPartialMeleeRuneReduction(buffs[slot].spellid) / 100;
		if(damage_to_reduce > buffs[slot].melee_rune)
		{
			mlog(SPELLS__EFFECT_VALUES, "Mob::ReduceDamage SE_MitigateMeleeDamage %d damage negated, %d"
				" damage remaining, fading buff.", damage_to_reduce, buffs[slot].melee_rune);
			damage -= damage_to_reduce;
			if(!TryFadeEffect(slot))
				BuffFadeBySlot(slot);
			UpdateRuneFlags();
		}
		else
		{
			mlog(SPELLS__EFFECT_VALUES, "Mob::ReduceDamage SE_MitigateMeleeDamage %d damage negated, %d"
				" damage remaining.", damage_to_reduce, buffs[slot].melee_rune);
			buffs[slot].melee_rune = (buffs[slot].melee_rune - damage_to_reduce);
			damage -= damage_to_reduce;
		}
	}

	if(damage < 1)
	{
		return -6;
	}

	slot = GetBuffSlotFromType(SE_Rune);
	while(slot >= 0)
	{
		int16 melee_rune_left = buffs[slot].melee_rune;
		if(melee_rune_left >= damage)
		{
			melee_rune_left -= damage;
			damage = -6;
			buffs[slot].melee_rune = melee_rune_left;
			break;
		}
		else
		{
			if(melee_rune_left > 0)
				damage -= melee_rune_left;
			if(!TryFadeEffect(slot))
				BuffFadeBySlot(slot);
			slot = GetBuffSlotFromType(SE_Rune);
			UpdateRuneFlags();
		}
	}

	if(damage < 1)
	{
		return -6;
	}

	slot = GetBuffSlotFromType(SE_ManaAbsorbPercentDamage);
	if(slot >= 0) {
		for (int i = 0; i < EFFECT_COUNT; i++) {
			if (spells[buffs[slot].spellid].effectid[i] == SE_ManaAbsorbPercentDamage) {
				if(GetMana() > damage * spells[buffs[slot].spellid].base[i] / 100) {
					damage -= (damage * spells[buffs[slot].spellid].base[i] / 100);
					SetMana(GetMana() - damage);
				}
			}
		}
	}

	return(damage);
}

sint32 Mob::AffectMagicalDamage(sint32 damage, int16 spell_id, const bool iBuffTic, Mob* attacker, bool isDoT) 
{
	if(damage <= 0)
	{
		return damage;
	}

	// See if we block the spell outright first
	int slot = GetBuffSlotFromType(SE_NegateAttacks);
	if(slot >= 0) {
		if(CheckHitsRemaining(slot, false, true))
			return -6;
	}

	// If this is a DoT, use DoT Shielding...
	if(isDoT)
	{
		damage -= (damage * this->itembonuses.DoTShielding / 100);
	}
	// This must be a DD then so lets apply Spell Shielding and runes.
	else 
	{
		// Reduce damage by the Spell Shielding first so that the runes don't take the raw damage.
		damage -= (damage * this->itembonuses.SpellShield / 100);

		// Do runes now.
		slot = GetBuffSlotFromType(SE_MitigateSpellDamage);
		if(slot >= 0)
		{
			int damage_to_reduce = damage * GetPartialMagicRuneReduction(buffs[slot].spellid) / 100;
			if(damage_to_reduce > buffs[slot].magic_rune)
			{
				mlog(SPELLS__EFFECT_VALUES, "Mob::ReduceDamage SE_MitigateSpellDamage %d damage negated, %d"
					" damage remaining, fading buff.", damage_to_reduce, buffs[slot].magic_rune);
				damage -= damage_to_reduce;
				if(!TryFadeEffect(slot))
					BuffFadeBySlot(slot);
				UpdateRuneFlags();
			}
			else
			{
				mlog(SPELLS__EFFECT_VALUES, "Mob::ReduceDamage SE_MitigateMeleeDamage %d damage negated, %d"
					" damage remaining.", damage_to_reduce, buffs[slot].magic_rune);
				buffs[slot].magic_rune = (buffs[slot].magic_rune - damage_to_reduce);
				damage -= damage_to_reduce;
			}
		}
		// Kings & Bandits - Rune absorb damage for spells - see if this works
		if ( damage > 0 )
		{
			slot = GetBuffSlotFromType(SE_Rune);
			while(slot >= 0)
			{
				int16 melee_rune_left = buffs[slot].melee_rune;
				if(melee_rune_left >= damage)
				{
					melee_rune_left -= damage;
					damage = -6;
					buffs[slot].melee_rune = melee_rune_left;
					break;
				}
				else
				{
					if(melee_rune_left > 0)
						damage -= melee_rune_left;
					if(!TryFadeEffect(slot))
						BuffFadeBySlot(slot);
					slot = GetBuffSlotFromType(SE_Rune);
					UpdateRuneFlags();
				}
			}
		}

		if(damage < 1)
		{
			return -6;
		}


		slot = GetBuffSlotFromType(SE_AbsorbMagicAtt);
		while(slot >= 0)
		{
			int16 magic_rune_left = buffs[slot].magic_rune;
			if(magic_rune_left >= damage)
			{
				magic_rune_left -= damage;
				damage = 0;
				buffs[slot].magic_rune = magic_rune_left;
				break;
			}
			else
			{
				if(magic_rune_left > 0)
					damage -= magic_rune_left;
				if(!TryFadeEffect(slot))
					BuffFadeBySlot(slot);
				slot = GetBuffSlotFromType(SE_AbsorbMagicAtt);
				UpdateRuneFlags();
			}
		}

		slot = GetBuffSlotFromType(SE_ManaAbsorbPercentDamage);
		if(slot >= 0) {
			for (int k = 0; k < EFFECT_COUNT; k++) {
				if (spells[buffs[slot].spellid].effectid[k] == SE_ManaAbsorbPercentDamage) {
					if(GetMana() > damage * spells[buffs[slot].spellid].base[k] / 100) {
						damage -= (damage * spells[buffs[slot].spellid].base[k] / 100);
						SetMana(GetMana() - damage);
					}
				}
			}
		}
	}
	return damage;
}

bool Mob::HasProcs() const
{
	for (int i = 0; i < MAX_PROCS; i++)
		if (PermaProcs[i].spellID != SPELL_UNKNOWN || SpellProcs[i].spellID != SPELL_UNKNOWN)
			return true;
	return false;
}

bool Mob::HasDefensiveProcs() const
{
	for (int i = 0; i < MAX_PROCS; i++)
		if (DefensiveProcs[i].spellID != SPELL_UNKNOWN)
			return true;
	return false;
}

bool Mob::HasRangedProcs() const
{
	for (int i = 0; i < MAX_PROCS; i++)
		if (RangedProcs[i].spellID != SPELL_UNKNOWN)
			return true;
	return false;
}

bool Client::CheckDoubleAttack(bool tripleAttack) {

	// If you don't have the double attack skill, return
	if(!HasSkill(DOUBLE_ATTACK) && !(GetClass() == BARD || GetClass() == BEASTLORD))
		return false;

	// You start with no chance of double attacking
	int chance = 0;

	// Used for maxSkill and triple attack calcs
	int8 classtype = GetClass();

	// The current skill level
	uint16 skill = GetSkill(DOUBLE_ATTACK);

	// Discipline bonuses give you 100% chance to double attack
	sint16 buffs = spellbonuses.DoubleAttackChance + itembonuses.DoubleAttackChance;

	// The maximum value for the Class based on the server rule of MaxLevel
	int16 maxSkill = MaxSkill(DOUBLE_ATTACK, classtype, RuleI(Character, MaxLevel));

	// AA bonuses for the melee classes
	int32 aaBonus =
		GetAA(aaBestialFrenzy) +
		GetAA(aaHarmoniousAttack) +
		GetAA(aaKnightsAdvantage)*10 +
		GetAA(aaFerocity)*10;

	// Bard Dance of Blades Double Attack bonus is not cumulative
	if(GetAA(aaDanceofBlades)) {
		aaBonus += 500;
	}

	// Half of Double Attack Skill used to check chance for Triple Attack
	if(tripleAttack) {
		// Only some Double Attack classes get Triple Attack
		if((classtype == MONK) || (classtype == WARRIOR) || (classtype == RANGER) || (classtype == BERSERKER)) {
			// We only get half the skill, but should get all the bonuses
			chance = (skill/2) + buffs + aaBonus;
		}
		else {
			return false;
		}
	}
	else {
		// This is the actual Double Attack chance
		chance = skill + buffs + aaBonus;
	}

	// If your chance is greater than the RNG you are successful! Always have a 5% chance to fail at max skills+bonuses.
	if(chance > MakeRandomInt(0, (maxSkill + itembonuses.DoubleAttackChance + aaBonus)*1.05)) {
		return true;
	}

	return false;
}

void Mob::CommonDamage(Mob* attacker, sint32 &damage, const int16 spell_id, const SkillType skill_used,
 bool &avoidable, const sint8 buffslot, const bool iBuffTic, int32 player_id, bool isDoT) {
	// This method is called with skill_used=ABJURE for Damage Shield damage. 
	bool FromDamageShield = (skill_used == ABJURE);

	// 11/16/2014 Image - received dmg clear the spells that are negated on combat (eg scale of wolf)
	ClearCombatNegatedSpells();
	//7/24/19 - Image(via devn00b) any damage cancel the damn bard mez.
 if(damage > 0) BuffFadeBySpellID(724);
	BreakMobHide();
    if ( attacker )
        attacker->ClearCombatNegatedSpells();


	mlog(COMBAT__HITS, "Applying damage %d done by %s with skill %d and spell %d, avoidable? %s, is %sa buff tic in slot %d",
		damage, attacker?attacker->GetName():"NOBODY", skill_used, spell_id, avoidable?"yes":"no", iBuffTic?"":"not ", buffslot);

	if (GetInvul() || DivineAura()) {
		mlog(COMBAT__DAMAGE, "Avoiding %d damage due to invulnerability.", damage);
		damage = -5;
	}

	if( spell_id != SPELL_UNKNOWN || attacker == NULL )
		avoidable = false;

	// only apply DS if physical damage (no spell damage)
	// damage shield calls this function with spell_id set, so its unavoidable
	if (attacker && damage > 0 && spell_id == SPELL_UNKNOWN && skill_used != ARCHERY && skill_used != THROWING) {
		this->DamageShield(attacker);
		uint32 buff_count = GetMaxTotalSlots();
		for(uint32 bs = 0; bs < buff_count; bs++){
			if((buffs[bs].spellid != SPELL_UNKNOWN) && IsEffectInSpell(buffs[bs].spellid, SE_DamageShield) && spells[buffs[bs].spellid].numhits > 0)
				CheckHitsRemaining(bs);
		}		
	}

	if(attacker){
		if(attacker->IsClient()){
			if(!attacker->CastToClient()->GetFeigned())
				AddToHateList(attacker, 0, damage, true, false, iBuffTic);
		}
		else
		{
			AddToHateList(attacker, 0, damage, true, false, iBuffTic);
			// KaB / Red69 / Zek - Fix for pet owners getting aggro for having their pet attack an npc
			if ( attacker->GetOwner() != NULL && ( ( attacker->GetOwner()->IsClient() && !attacker->GetOwner()->CastToClient()->GetFeigned() )
				|| attacker->GetOwner()->IsNPC() ) && 
				( !hate_list.IsOnHateList(attacker->GetOwner()) ) )
			{
				AddToHateList(attacker->GetOwner(), 1, 0, true, false, iBuffTic);
			}
		}
	}

	if(damage > 0) {
		//if there is some damage being done and theres an attacker involved
		if(attacker) {
			if(spell_id == SPELL_HARM_TOUCH2 && attacker->IsClient() && attacker->CastToClient()->CheckAAEffect(aaEffectLeechTouch)){
				int healed = damage;
				healed = attacker->GetActSpellHealing(spell_id, healed);
				attacker->HealDamage(healed);
				entity_list.MessageClose(this, true, 300, MT_Emote, "%s beams a smile at %s", attacker->GetCleanName(), this->GetCleanName() );
				attacker->CastToClient()->DisableAAEffect(aaEffectLeechTouch);
			}

			// if spell is lifetap add hp to the caster
			if (spell_id != SPELL_UNKNOWN && IsLifetapSpell( spell_id )) {
				int healed = damage;

				healed = attacker->GetActSpellHealing(spell_id, healed);				
				mlog(COMBAT__DAMAGE, "Applying lifetap heal of %d to %s", healed, attacker->GetName());
				attacker->HealDamage(healed);

				//we used to do a message to the client, but its gone now.
				// emote goes with every one ... even npcs
				entity_list.MessageClose(this, true, 300, MT_Emote, "%s beams a smile at %s", attacker->GetCleanName(), this->GetCleanName() );
			}
		}	//end `if there is some damage being done and theres anattacker person involved`

		Mob *pet = GetPet();
		if (pet && !pet->IsFamiliar() && !pet->SpecAttacks[IMMUNE_AGGRO] && !pet->IsEngaged() && attacker && attacker != this) 
		{
			mlog(PETS__AGGRO, "Sending pet %s into battle due to attack.", pet->GetName());
			pet->AddToHateList(attacker, 1);
			pet->SetTarget(attacker);
			Message_StringID(10, PET_ATTACKING, pet->GetCleanName(), attacker->GetCleanName());
		}	

		//see if any runes want to reduce this damage
		if(spell_id == SPELL_UNKNOWN) {
			damage = ReduceDamage(damage);
			mlog(COMBAT__HITS, "Melee Damage reduced to %d", damage);
		} else {
			sint32 origdmg = damage;
			damage = AffectMagicalDamage(damage, spell_id, iBuffTic, attacker, isDoT);
			mlog(COMBAT__HITS, "Melee Damage reduced to %d", damage);
			if (origdmg != damage && attacker && attacker->IsClient()) {
				if(attacker->CastToClient()->GetFilter(FILTER_DAMAGESHIELD) != FilterHide)
					attacker->Message(15, "The Spellshield absorbed %d of %d points of damage", origdmg - damage, origdmg);
			}
		}

		// Kings & Bandits - EQPVP Damage Reduction
		if ( IsClient() && ( (attacker && attacker->IsClient()) || (player_id > 0 && player_id != MAX_PLAYERID) ) )
		{
			int8 class_to_use = 0;
			if ( attacker )
				class_to_use = attacker->GetClass();
			else if ( buffslot > -1 )
				class_to_use = buffs[buffslot].player_class;

			if ( spell_id != SPELL_UNKNOWN )
			{
				// we can get some massive damage spells, some players don't have more than a thousand ~ hp
				if ( damage > 450 )
				{
					if ( class_to_use == WIZARD )
						damage = (sint32) (float) damage * MakeRandomFloat(0.93f,0.96f);
					else if ( class_to_use == DRUID || class_to_use == SHAMAN )
						damage = (sint32) (float) damage * MakeRandomFloat(0.88f,0.9f);
					else if ( class_to_use == MAGICIAN )
						damage = (sint32) (float) damage * MakeRandomFloat(0.75f,.85f);
					else
						damage = (sint32) (float) damage * MakeRandomFloat(0.91f,0.93f);
				}
				else
				{
					if ( class_to_use == MAGICIAN )
						damage = (sint32) (float) damage * MakeRandomFloat(.85f,.9f);
					else if ( class_to_use == BARD )
					{
						// skip this case don't reduce dmg
					}
					else
						damage = (sint32) (float) damage * MakeRandomFloat(0.9f,.95f);
				}
			}
		}


		// apparently you can go into combat with sneaking per http://everquest.allakhazam.com/db/skills.html?skill=31
		/*if(IsClient() && CastToClient()->sneaking){
			CastToClient()->sneaking = false;
			SendAppearancePacket(AT_Sneak, 0);
		}*/
		/*if(attacker && attacker->IsClient() && attacker->CastToClient()->sneaking){
			attacker->CastToClient()->sneaking = false;
			attacker->SendAppearancePacket(AT_Sneak, 0);
		}*/
		//final damage has been determined.

		/*
		//check for death conditions
		if(IsClient()) {
		if((GetHP()) <= -10) {
		Death(attacker, damage, spell_id, skill_used);
		return;
		}
		} else {
		if (damage >= GetHP()) {
		//killed...
		SetHP(-100);
		Death(attacker, damage, spell_id, skill_used);
		return;
		}
		}
		*/

		SetHP(GetHP() - damage);

		if ( ( spell_id == 0 || spell_id == SPELL_UNKNOWN ) 
			&& attacker != NULL && IsClient() 
			&& MakeRandomInt(0,100) < RuleI(Combat,MeleeKnockbackChance) )
			DoKnockback(attacker, RuleI(Combat,MeleeKnockbackAmount), 0);

		if(HasDied()) {
			bool IsSaved = false;

			if(HasDeathSaveChance()) {
				if(TryDeathSave()) {
					IsSaved = true;
				}
			}

			if(!IsSaved && !TrySpellOnDeath()) {
				SetHP(-500);

				if(attacker && attacker->IsClient() && (spell_id != SPELL_UNKNOWN) && damage>0) {
					char val1[20]={0};
					attacker->Message_StringID(4,OTHER_HIT_NONMELEE,GetCleanName(),ConvertArray(damage,val1));
				}

				Death(attacker, damage, spell_id, skill_used, player_id);
				return;
			}
		}

		//fade mez if we are mezzed
		if (IsMezzed()) {
			mlog(COMBAT__HITS, "Breaking mez due to attack.");
			BuffFadeByEffect(SE_Mez);
		}

		//check stun chances if bashing
		if (damage > 0 && ((skill_used == BASH || skill_used == KICK) && attacker && attacker->IsNPC()))
		{
			if( ( !attacker->IsPet() && !attacker->IsCharmed() ) || ( attacker->GetOwner() != NULL && !attacker->GetOwner()->IsNPC() ) )
			{
				// NPCs can stun with their bash/kick as soon as they recieve it.
				// Clients can stun mobs under level 56 with their bash/kick when they get level 55 or greater.
				bool resisted = false;
				if (MakeRandomInt(0,99) < (RuleI(Combat, NPCBashKickStunChance)))
				{
					int stun_resist = itembonuses.StunResist+spellbonuses.StunResist;

					if(this->IsClient())
						stun_resist += aabonuses.StunResist;

					if(resisted)
						mlog(COMBAT__HITS, "Stun Resisted, there was no shield to bash with.");
					else if(this->GetBaseRace() == OGRE && this->IsClient() && !attacker->BehindMob(this, attacker->GetX(), attacker->GetY())) 
					{
						mlog(COMBAT__HITS, "Stun Resisted. Ogres are immune to frontal melee stuns.");
					}
					else 
					{
						if(stun_resist <= 0 || MakeRandomInt(0,99) >= stun_resist) 
						{
							mlog(COMBAT__HITS, "Stunned. We had %d percent resist chance.");
							Stun(1000);
						} 
						else 
						{
							if(this->IsClient())
								Message_StringID(MT_Stun, SHAKE_OFF_STUN);

							mlog(COMBAT__HITS, "Stun Resisted. We had %dpercent resist chance.");
						}
					}
				}
			}
		}

		if(spell_id != SPELL_UNKNOWN && !iBuffTic) {
			//see if root will break
			if (IsRooted()) { // neotoyko: only spells cancel root
				if(GetAA(aaEnhancedRoot))
				{
					if (MakeRandomInt(0, 99) < 10) {
						mlog(COMBAT__HITS, "Spell broke root! 10percent chance");
						BuffFadeByEffect(SE_Root, buffslot); // buff slot is passed through so a root w/ dam doesnt cancel itself
					} else {
						mlog(COMBAT__HITS, "Spell did not break root. 10 percent chance");
					}
				}
				else
				{
					if (MakeRandomInt(0, 99) < 20) {
						mlog(COMBAT__HITS, "Spell broke root! 20percent chance");
						BuffFadeByEffect(SE_Root, buffslot); // buff slot is passed through so a root w/ dam doesnt cancel itself
					} else {
						mlog(COMBAT__HITS, "Spell did not break root. 20 percent chance");
					}
				}			
			}
		}
		else if(spell_id == SPELL_UNKNOWN)
		{
			//increment chances of interrupting
			if(IsCasting() && attacker != NULL) { //shouldnt interrupt on regular spell damage
				int32 con = GetLevelCon(attacker->GetLevel());
				switch(con)
				{
				case CON_GREEN:
					{
						attacked_count += RuleR(Combat, IncrementInterruptGreen);
						break;
					}
				case CON_LIGHTBLUE:
					{
						attacked_count += RuleR(Combat, IncrementInterruptLightBlue);
						break;
					}
				case CON_BLUE:
					{
						attacked_count += RuleR(Combat, IncrementInterruptBlue);
						break;
					}
				case CON_WHITE:
					{
						attacked_count += RuleR(Combat, IncrementInterruptWhite);
						break;
					}
				case CON_YELLOW:
					{
						attacked_count += RuleR(Combat, IncrementInterruptYellow);
						break;
					}
				case CON_RED:
					{
						attacked_count += RuleR(Combat, IncrementInterruptRed);
						break;
					}
				default:
					{
						attacked_count += 1.0;
						break;
					}
				}


				mlog(COMBAT__HITS, "Melee attack while casting. Attack count %d", attacked_count);
			}
		}

		//send an HP update if we are hurt
		if(GetHP() < GetMaxHP())
			SendHPUpdate(!iBuffTic);
	}	//end `if damage was done`

	//send damage packet...
	if(!iBuffTic) { //buff ticks do not send damage, instead they just call SendHPUpdate(), which is done below
		EQApplicationPacket* outapp = new EQApplicationPacket(OP_Damage, sizeof(CombatDamage_Struct));
		CombatDamage_Struct* a = (CombatDamage_Struct*)outapp->pBuffer;
		a->target = GetID();
		if (attacker == NULL)
			a->source = 0;
		else if (attacker->IsClient() && attacker->CastToClient()->GMHideMe())
			a->source = 0;
		else
			a->source = attacker->GetID();
		
		// 2018 - force target if client doesn't already have one, should fix issues when invis target.
		if ( GetTarget() == NULL && a->source > 0 )
		{
			if ( attacker == NULL || attacker->IsCorpse() )
				a->source = 0;
			else
				SetTarget(attacker);
		}

		a->type = SkillDamageTypes[skill_used]; // was 0x1c
		a->damage = damage;
		//		if (attack_skill != 231)
		//			a->spellid = SPELL_UNKNOWN;
		//		else
		a->spellid = spell_id;

		//Note: if players can become pets, they will not receive damage messages of their own
		//this was done to simplify the code here (since we can only effectively skip one mob on queue)
		eqFilterType filter;
		Mob *skip = attacker;
		if(attacker && attacker->GetOwnerID()) {
			//attacker is a pet, let pet owners see their pet's damage
			Mob* owner = attacker->GetOwner();
			if (owner && owner->IsClient()) {
				if (((spell_id != SPELL_UNKNOWN) || (FromDamageShield)) && damage>0) {
					//special crap for spell damage, looks hackish to me
					char val1[20]={0};
					owner->Message_StringID(MT_NonMelee,OTHER_HIT_NONMELEE,GetCleanName(),ConvertArray(damage,val1));
				} else {
					if(damage > 0) {
						if(spell_id != SPELL_UNKNOWN)
							filter = isDoT ? FilterDOT : FilterSpellDamage;
						else
							filter = FILTER_MYPETHITS;
					} else if(damage == -5)
						filter = FilterNone;	//cant filter invulnerable
					else
						filter = FILTER_MYPETMISSES;

					if(!FromDamageShield)
						owner->CastToClient()->QueuePacket(outapp,true,CLIENT_CONNECTED,filter);
				}
			}
			skip = owner;
		} else {
			//attacker is not a pet, send to the attacker

			//if the attacker is a client, try them with the correct filter
			if(attacker && attacker->IsClient()) {
				if (((spell_id != SPELL_UNKNOWN)||(FromDamageShield)) && damage>0) {
					//special crap for spell damage, looks hackish to me
					char val1[20]={0};
					attacker->Message_StringID(MT_NonMelee,OTHER_HIT_NONMELEE,GetCleanName(),ConvertArray(damage,val1));
				} else {
					if(damage > 0) {
						if(spell_id != SPELL_UNKNOWN)
							filter = isDoT ? FilterDOT : FilterSpellDamage;
						else
							filter = FilterNone;	//cant filter our own hits
					} else if(damage == -5)
						filter = FilterNone;	//cant filter invulnerable
					else
						filter = FILTER_MYMISSES;

					attacker->CastToClient()->QueuePacket(outapp, true, CLIENT_CONNECTED, filter);
				}
			}
			skip = attacker;
		}

		//send damage to all clients around except the specified skip mob (attacker or the attacker's owner) and ourself
		if(damage > 0) {
			if(spell_id != SPELL_UNKNOWN)
				filter = isDoT ? FilterDOT : FilterSpellDamage;
			else
				filter = FILTER_OTHERHITS;
		} else if(damage == -5)
			filter = FilterNone;	//cant filter invulnerable
		else
			filter = FILTER_OTHERMISSES;
		//make attacker (the attacker) send the packet so we can skip them and the owner
		//this call will send the packet to `this` as well (using the wrong filter) (will not happen until PC charm works)
		//LogFile->write(EQEMuLog::Debug, "Queue damage to all except %s with filter %d (%d), type %d", skip->GetName(), filter, IsClient()?CastToClient()->GetFilter(filter):-1, a->type);
		//
		// If this is Damage Shield damage, the correct OP_Damage packets will be sent from Mob::DamageShield, so 
		// we don't send them here.
		if(!FromDamageShield) {
			entity_list.QueueCloseClients(this, outapp, true, 200, skip, true, filter);
			//send the damage to ourself if we are a client
			if(IsClient()) {
				//I dont think any filters apply to damage affecting us
				CastToClient()->QueuePacket(outapp);
			}
		}

		safe_delete(outapp);
	} else {
		//else, it is a buff tic...
		// Everhood - So we can see our dot dmg like live shows it.
		if(spell_id != SPELL_UNKNOWN && damage > 0 && attacker && attacker != this && attacker->IsClient()) {
			//might filter on (attack_skill>200 && attack_skill<250), but I dont think we need it
			if(attacker->CastToClient()->GetFilter(FilterDOT) != FilterHide) {
				attacker->Message_StringID(MT_DoTDamage, OTHER_HIT_DOT, GetCleanName(),itoa(damage),spells[spell_id].name);
			}
		}
	} //end packet sending
}


void Mob::HealDamage(uint32 amount, Mob* caster) {
	uint32 maxhp = GetMaxHP();
	uint32 curhp = GetHP();
	uint32 acthealed = 0;
	if(amount > (maxhp - curhp))
		acthealed = (maxhp - curhp);
	else
		acthealed = amount;

	char *TempString = NULL;

	MakeAnyLenString(&TempString, "%d", acthealed);

	if(acthealed > 100){
		if(caster){
			Message_StringID(MT_NonMelee, YOU_HEALED, caster->GetCleanName(), TempString);
			if(caster != this){
				caster->Message_StringID(MT_NonMelee, YOU_HEAL, GetCleanName(), TempString);
			}
		}
		else{
			Message(MT_NonMelee, "You have been healed for %d points of damage.", acthealed);
		}	
	}		

	if (curhp < maxhp) {
		if ((curhp+amount)>maxhp)
			curhp=maxhp;
		else
			curhp+=amount;
		SetHP(curhp);

		SendHPUpdate();
	}
	safe_delete_array(TempString);
}



//proc chance includes proc bonus
float Mob::GetProcChances(float &ProcBonus, float &ProcChance, int16 weapon_speed) {
	int mydex = GetDEX();
	float AABonus = 0;
	ProcBonus = 0;
	ProcChance = 0;
	if(IsClient()) {
		//increases based off 1 guys observed results.
		switch(CastToClient()->GetAA(aaWeaponAffinity)) {
		case 1:
			AABonus = 0.10;
			break;
		case 2:
			AABonus = 0.20;
			break;
		case 3:
			AABonus = 0.30;
			break;
		case 4:
			AABonus = 0.40;
			break;
		case 5:
			AABonus = 0.50;
			break;
		}
	}

	float PermaHaste;
	if(GetHaste() > 0)
		PermaHaste = 1 / (1 + (float)GetHaste()/100);
	else if(GetHaste() < 0)
		PermaHaste = 1 * (1 - (float)GetHaste()/100);
	else
		PermaHaste = 1.0f;

	weapon_speed = ((int)(weapon_speed*(100.0f+attack_speed)*PermaHaste) / 100);
	if(weapon_speed < 10) // fast as a client can swing, so should be the floor of the proc chance
		weapon_speed = 10;

	// Kings & Bandits - track the last weapon speed
	SetLastWeaponSpeed(weapon_speed);

	ProcBonus += (float(itembonuses.ProcChance + spellbonuses.ProcChance) / 1000.0f + AABonus);

	if(RuleB(Combat, AdjustProcPerMinute) == true)
	{
		ProcChance = ((float)weapon_speed * RuleR(Combat, AvgProcsPerMinute) / 600.0f);
		ProcBonus += float(mydex) * RuleR(Combat, ProcPerMinDexContrib) / 100.0f;
		ProcChance = ProcChance + (ProcChance * ProcBonus);
	}
	else
	{
		ProcChance = RuleR(Combat, BaseProcChance) + float(mydex) / RuleR(Combat, ProcDexDivideBy);
		ProcChance = ProcChance + (ProcChance * ProcBonus);
	}

	mlog(COMBAT__PROCS, "Proc chance %.2f (%.2f from bonuses)", ProcChance, ProcBonus);
	return ProcChance;
}

void Mob::TryDefensiveProc(Mob *on) {
	// this should have already been checked, but just in case...
	if (!this->HasDefensiveProcs())
		return;

	if (!on) {
		SetTarget(NULL);
		LogFile->write(EQEMuLog::Error, "A null Mob object was passed to Mob::TryDefensiveProc for evaluation!");
		return;
	}

	// iterate through our defensive procs and try each of them
	for (int i = 0; i < MAX_PROCS; i++) {
		if (DefensiveProcs[i].spellID != SPELL_UNKNOWN &&
			IsValidSpell(DefensiveProcs[i].spellID)) {
				if (MakeRandomInt(0, 100) < MakeRandomInt(0, 20)) {
					ExecWeaponProc(DefensiveProcs[i].spellID, on);
				}
		}
	}

	return;
}

void Mob::TryWeaponProc(const ItemInst* weapon_g, Mob *on, int16 hand) {
	_ZP(Mob_TryWeaponProcA);
	if(!on) {
		SetTarget(NULL);
		LogFile->write(EQEMuLog::Error, "A null Mob object was passed  to Mob::TryWeaponProc for evaluation!");
		return;
	}

	if(!weapon_g) {
		TryWeaponProc((const Item_Struct*) NULL, on, hand);
		return;
	}

	if(!weapon_g->IsType(ItemClassCommon)) {
		TryWeaponProc((const Item_Struct*) NULL, on, hand);
		return;
	}

	//do main procs
	TryWeaponProc(weapon_g->GetItem(), on, hand);


	//we have to calculate these again, oh well
	int ourlevel = GetLevel();
	float ProcChance, ProcBonus;
	GetProcChances(ProcBonus, ProcChance, weapon_g->GetItem()->Delay);
	if(hand != 13)
	{
		ProcChance /= 2;
	}

	//do augment procs
	int r;
	for(r = 0; r < MAX_AUGMENT_SLOTS; r++) {
		const ItemInst* aug_i = weapon_g->GetAugment(r);
		if(!aug_i)
			continue;
		const Item_Struct* aug = aug_i->GetItem();
		if(!aug)
			continue;

		if (IsValidSpell(aug->Proc.Effect) 
			&& (aug->Proc.Type == ET_CombatProc)) {
				ProcChance = ProcChance*(100+aug->ProcRate)/100;
				if (MakeRandomFloat(0, 1) < ProcChance) {
					if(aug->Proc.Level > ourlevel) {
						Mob * own = GetOwner();
						if(own != NULL) {
							own->Message_StringID(13,PROC_PETTOOLOW);
						} else {
							Message_StringID(13,PROC_TOOLOW);
						}
					} else {
						ExecWeaponProc(aug->Proc.Effect, on);
					}
				}
		}
	}
}

void Mob::TryWeaponProc(const Item_Struct* weapon, Mob *on, int16 hand) {
	_ZP(Mob_TryWeaponProcB);
	int ourlevel = GetLevel();
	float ProcChance, ProcBonus;
	if(weapon!=NULL)
		GetProcChances(ProcBonus, ProcChance, weapon->Delay);
	else
		GetProcChances(ProcBonus, ProcChance);

	if(hand != 13)
		ProcChance /= 2;	

	//give weapon a chance to proc first.
	if(weapon != NULL) {
		if (IsValidSpell(weapon->Proc.Effect) && (weapon->Proc.Type == ET_CombatProc)) {
			float WPC = ProcChance*(100.0f+(float)weapon->ProcRate)/100.0f;
			if (MakeRandomFloat(0, 1) <= WPC) {	// 255 dex = 0.084 chance of proc. No idea what this number should be really.
				if(weapon->Proc.Level > ourlevel) {
					mlog(COMBAT__PROCS, "Tried to proc (%s), but our level (%d) is lower than required (%d)", weapon->Name, ourlevel, weapon->Proc.Level);
					Mob * own = GetOwner();
					if(own != NULL) {
						own->Message_StringID(13,PROC_PETTOOLOW);
					} else {
						Message_StringID(13,PROC_TOOLOW);
					}
				} else {
					mlog(COMBAT__PROCS, "Attacking weapon (%s) successfully procing spell %d (%.2f percent chance)", weapon->Name, weapon->Proc.Effect, ProcChance*100);
					ExecWeaponProc(weapon->Proc.Effect, on);
				}
			} else {
				mlog(COMBAT__PROCS, "Attacking weapon (%s) did no proc (%.2f percent chance).", weapon->Name, ProcChance*100);
			}
		}
	}

	if(ProcBonus == -1) {
		LogFile->write(EQEMuLog::Error, "ProcBonus was -1 value!");
		return;
	}

	bool bRangedAttack = false;
	if (weapon != NULL) {
		if (weapon->ItemType == ItemTypeBow || weapon->ItemType == ItemTypeThrowing || weapon->ItemType == ItemTypeThrowingv2) {
			bRangedAttack = true;
		}
	}

	bool isRanged = false;
	if(weapon)
	{
		if(weapon->ItemType == ItemTypeArrow ||
			weapon->ItemType == ItemTypeThrowing ||
			weapon->ItemType == ItemTypeThrowingv2 ||
			weapon->ItemType == ItemTypeBow)
		{
			isRanged = true;
		}
	}

	uint32 i;
	for(i = 0; i < MAX_PROCS; i++) {
		if (PermaProcs[i].spellID != SPELL_UNKNOWN) {
			if(MakeRandomInt(0, 100) < PermaProcs[i].chance) {
				mlog(COMBAT__PROCS, "Permanent proc %d procing spell %d (%d percent chance)", i, PermaProcs[i].spellID, PermaProcs[i].chance);
				ExecWeaponProc(PermaProcs[i].spellID, on);
			} else {
				mlog(COMBAT__PROCS, "Permanent proc %d failed to proc %d (%d percent chance)", i, PermaProcs[i].spellID, PermaProcs[i].chance);
			}
		}
		if(!isRanged)
		{
			if (SpellProcs[i].spellID != SPELL_UNKNOWN) {
				int chance = SpellProcs[i].chance;

				// Zek / Red69 / KaB - Rogue poison fix and also other chance based spell procs 9/25/2012
				// if it isn't poison then we can apply the proc chance of the weapon
				if ( !SpellProcs[i].isPoison )
				{
					chance = ProcChance * (SpellProcs[i].chance);
				}

				if(SpellProcs[i].isPoison || MakeRandomInt(0, 100) < chance) {
					mlog(COMBAT__PROCS, "Spell proc %d procing spell %d (%d percent chance)", i, SpellProcs[i].spellID, chance);
					ExecWeaponProc(SpellProcs[i].spellID, on);

					// KaB / Red69 / Zek - Fixes poisons only lasting one proc sept 26 2012
					if ( SpellProcs[i].isPoison )
					{
						SpellProcs[i].chance = 0;
						SpellProcs[i].isPoison = false;
						SpellProcs[i].spellID = SPELL_UNKNOWN;
					}
				} else {
					mlog(COMBAT__PROCS, "Spell proc %d failed to proc %d (%d percent chance)", i, SpellProcs[i].spellID, chance);
				}
			}
		}
		if (bRangedAttack && RangedProcs[i].spellID != SPELL_UNKNOWN) {
			if(MakeRandomInt(0, 100) < MakeRandomInt(0, 25)) {
				mlog(COMBAT__PROCS, "Ranged proc %d procing spell %d", i, RangedProcs[i].spellID, RangedProcs[i].chance);
				ExecWeaponProc(RangedProcs[i].spellID, on);
			} else {
				mlog(COMBAT__PROCS, "Ranged proc %d failed to proc %d", i, RangedProcs[i].spellID, RangedProcs[i].chance);
			}
		}

	}
}

void Mob::TryPetCriticalHit(Mob *defender, int16 skill, sint32 &damage)
{
	if(damage < 1)
		return;

	Client *owner = NULL;
	int critChance = RuleI(Combat, MeleeBaseCritChance);
	uint16 critMod = 200;

	if (damage < 1) //We can't critical hit if we don't hit.
		return;

	if (!this->IsPet() || !this->GetOwner()->IsClient())
		return;

	owner = this->GetOwner()->CastToClient();

	sint16 aaClass = -1;
	switch (owner->GetClass()) {
	case NECROMANCER:
		aaClass = aaDeathsFury;
		break;
	case MAGICIAN:
		aaClass = aaElementalFury;
		break;
	case BEASTLORD:
		aaClass = aaWardersFury;
		break;
	}

	switch (owner->GetAA(aaClass)) {
	case 1:
		critChance += 4;
		break;
	case 2:
		critChance += 8;
		break;
	case 3:
		critChance += 12;
		break;
	case 4:
		critChance += 16;
		break;
	case 5:
		critChance += 20;
		break;
	}

	int CritBonus = GetCriticalChanceBonus(skill);
	if(CritBonus > 0) {
		if(critChance == 0) //If we have a bonus to crit in items or spells but no actual chance to crit
			critChance = (CritBonus / 100) + 1; //Give them a small one so skills and items appear to have some effect.
		else
			critChance += (critChance * CritBonus / 100); //crit chance is a % increase to your reg chance
	}
	if (critChance > 0) {
		if (MakeRandomInt(0, 99) < critChance) {
			critMod += GetCritDmgMob(skill) * 2; // To account for base crit mod being 200 not 100
			damage = (damage * critMod) / 100;
			entity_list.MessageClose(this, false, 200, MT_CritMelee, "%s scores a critical hit!(%d)", GetCleanName(), damage);
		}
	}
}

bool Mob::TryCriticalHit(Mob *defender, int16 skill, uint8 itemType, sint32 &damage)
{
	if(damage < 1)
		return false;

	bool slayUndeadCrit = false;

	// decided to branch this into it's own function since it's going to be duplicating a lot of the
	// code in here, but could lead to some confusion otherwise
	if (this->IsPet() && this->GetOwner()->IsClient()) {
		this->TryPetCriticalHit(defender,skill,damage);
		return false;
	}

	int critChance = RuleI(Combat, MeleeBaseCritChance);
	if(IsClient())
		critChance += RuleI(Combat, ClientBaseCritChance);

	// Bonus to crippling blow chance 
	bool crip_success = false;
	if(MakeRandomInt(0,99) < GetCrippBlowChance())
		crip_success = true;

	uint16 critMod = 200; 
	if(((GetClass() == WARRIOR || GetClass() == BERSERKER) && GetLevel() >= 12 && IsClient()) || crip_success) 
	{
		if(CastToClient()->berserk || crip_success)
		{
			critChance += RuleI(Combat, BerserkBaseCritChance);
			critMod = 400;
		}
		else
		{
			critChance += RuleI(Combat, WarBerBaseCritChance);
		}

		critChance = (int)((double)critChance * RuleR(PVPCombat, ReduceCritChanceBerserker));
	}

	bool rogueBehindMob = false;

	if(skill == ARCHERY && GetClass() == RANGER && GetSkill(ARCHERY) >= 65){
		critChance += 6;
	}
	// KaB / Red69 / Zek - Implementation of Deadly Strike for Rogue Throwing
	else if ( skill == THROWING && GetClass() == ROGUE && defender ) {

		rogueBehindMob =  BehindMob(defender, GetX(), GetY());

		if ( rogueBehindMob )
			critChance = 35;
		else
			critChance = 10;

	}

	int CritBonus = GetCriticalChanceBonus(skill);
	if(IsClient())
		critChance += GetCriticalChanceBonus(skill, true); // These add straight on

	if(CritBonus > 0) {
		if(critChance == 0) //If we have a bonus to crit in items or spells but no actual chance to crit
			critChance = (CritBonus / 100) + 1; //Give them a small one so skills and items appear to have some effect.
		else
			critChance += (critChance * CritBonus / 100); //crit chance is a % increase to your reg chance
	}

	if(GetAA(aaSlayUndead)){
		if(defender && defender->GetBodyType() == BT_Undead || defender->GetBodyType() == BT_SummonedUndead || defender->GetBodyType() == BT_Vampire){
			switch(GetAA(aaSlayUndead)){
			case 1:
				critMod += 33;
				break;
			case 2:
				critMod += 66;
				break;
			case 3:
				critMod += 100;
				break;
			}
			slayUndeadCrit = true;
		}
	}

	if(critChance > 0){
		if(MakeRandomInt(0, 99) < critChance)
		{
			if (slayUndeadCrit)
			{
				damage = (damage * (critMod * 2.65)) / 100;
				entity_list.MessageClose(this, false, 200, MT_CritMelee, "%s cleanses %s target!(%d)", GetCleanName(), this->GetGender() == 0 ? "his" : this->GetGender() == 1 ? "her" : "its", damage);
				return true;
			}
			//Veteran's Wrath AA
			//first, find out of we have it (don't multiply by 0 :-\ )
			int32 AAdmgmod = GetAA(aaVeteransWrath);
			if (AAdmgmod > 0) {
				//now, make sure it's not a special attack
				if (skill == _1H_BLUNT
					|| skill == _2H_BLUNT
					|| skill == _1H_SLASHING
					|| skill == _2H_SLASHING
					|| skill == PIERCING
					|| skill == HAND_TO_HAND
					)
					critMod += AAdmgmod * 3; //AndMetal: guessing
			}
			critMod += GetCritDmgMob(skill) * 2; // To account for base crit mod being 200 not 100

			// KaB - Red69 - Zek / Implementation of crit reduction for pvp
			if ( defender->IsClient() && IsClient() )
			{
				sint32 divRule = RuleI(PVPCombat,MeleeCriticalReduction);
				if ( divRule < 1 )
					divRule = 100;
				
				damage = (damage * critMod) / divRule;
				defender->CastToClient()->ApplyPVPMitigation( GetLevel(), GetClass(), (SkillType)skill, itemType, damage);
			}
			else
			{
				damage = (damage * critMod) / 100;
			}

			if(IsClient() && CastToClient()->berserk || crip_success)
			{
				entity_list.MessageClose(this, false, 200, MT_CritMelee, "%s lands a crippling blow!(%d)", GetCleanName(), damage);
				// Crippling blows also have a chance to stun
				if(MakeRandomInt(0,99) < 50) //improbable to stun every hit
					defender->Stun(0);
			}

			else
			{

				// KaB / Red69 / Zek - Implementation of Deadly Strike for Rogue Throwing
				if ( GetClass() == ROGUE && skill == THROWING && rogueBehindMob )
				{
					char dmg[64];
					_snprintf(dmg,64,"%d",damage);
					entity_list.MessageClose_StringID(this, false, 200, MT_CritMelee, DEADLY_STRIKE , GetCleanName(), dmg);
				}
				else
					entity_list.MessageClose(this, false, 200, MT_CritMelee, "%s scores a critical hit!(%d)", GetCleanName(), damage);
			}

			return true; // we did some type of crit hit
		}
	}

	return false;
}

bool Mob::TryFinishingBlow(Mob *defender, SkillType skillinuse, uint8 itemType)
{
	int8 aa_item = GetAA(aaFinishingBlow) + GetAA(aaCoupdeGrace) + GetAA(aaDeathblow);

	if(aa_item && !defender->IsClient() && defender->GetHPRatio() < 10){
		int chance = 0;
		int levelreq = 0;
		switch(aa_item)
		{
		case 1:
			chance = 2;
			levelreq = 50;
			break;
		case 2:
			chance = 5;
			levelreq = 52;
			break;
		case 3:
			chance = 7;
			levelreq = 54;
			break;
		case 4:
			chance = 7;
			levelreq = 55;
			break;
		case 5:
			chance = 7;
			levelreq = 57;
			break;
		case 6:
			chance = 7;
			levelreq = 59;
			break;
		case 7:
			chance = 7;
			levelreq = 61;
			break;
		case 8:
			chance = 7;
			levelreq = 63;
			break;
		case 9:
			chance = 7;
			levelreq = 65;
			break;
		default:
			break;
		}

		if(chance >= MakeRandomInt(0, 100) && defender->GetLevel() <= levelreq){
			mlog(COMBAT__ATTACKS, "Landed a finishing blow: AA at %d, other level %d", aa_item, defender->GetLevel());
			entity_list.MessageClose_StringID(this, false, 200, MT_CritMelee, FINISHING_BLOW, GetName());
			defender->Damage(this, 32000, SPELL_UNKNOWN, skillinuse, itemType);
			return true;
		}
		else
		{
			mlog(COMBAT__ATTACKS, "FAILED a finishing blow: AA at %d, other level %d", aa_item, defender->GetLevel());
			return false;
		}
	}
	return false;
}

void Mob::DoRiposte(Mob* defender) {
	mlog(COMBAT__ATTACKS, "Preforming a riposte");

	defender->Attack(this, SLOT_PRIMARY, true);

	//double riposte
	int DoubleRipChance = 0;
	switch(defender->GetAA(aaDoubleRiposte)) {
	case 1: 
		DoubleRipChance = 15;
		break;
	case 2:
		DoubleRipChance = 35;
		break;
	case 3:
		DoubleRipChance = 50;
		break;
	}

	DoubleRipChance += 10*defender->GetAA(aaFlashofSteel);

	if(DoubleRipChance >= MakeRandomInt(0, 100)) {
		mlog(COMBAT__ATTACKS, "Preforming a double riposed (%d percent chance)", DoubleRipChance);

		defender->Attack(this, SLOT_PRIMARY, true);
	}

	if(defender->GetAA(aaReturnKick)){
		int ReturnKickChance = 0;
		switch(defender->GetAA(aaReturnKick)){
		case 1:
			ReturnKickChance = 25;
			break;
		case 2:
			ReturnKickChance = 35;
			break;
		case 3:
			ReturnKickChance = 50;
			break;
		}

		if(ReturnKickChance >= MakeRandomInt(0, 100)) {
			mlog(COMBAT__ATTACKS, "Preforming a return kick (%d percent chance)", ReturnKickChance);
			defender->MonkSpecialAttack(this, FLYING_KICK);
		}
	}		
}

void Mob::ApplyMeleeDamageBonus(int16 skill, sint32 &damage, Mob* defMob){
	switch(skill)
	{
	case THROWING:
		{
			switch(GetAA(aaThrowingMastery))
			{
			case 1:
				damage = damage * 1.15;
				break;
			case 2:
				damage = damage * 1.25;
				break;
			case 3:
				damage = damage * 1.5;
				break;
			}
			break;
		}
		// Kings & Bandits H2H Monk and 2H Weapon increase
	case HAND_TO_HAND:
		{
			if ( IsClient() )
				damage = damage * 1.5;

			break;
		}
	}

	if(!RuleB(Combat, UseIntervalAC)){
		if(IsNPC()){ //across the board NPC damage bonuses.
			//only account for STR here, assume their base STR was factored into their DB damages
			int dmgbonusmod = 0;
			dmgbonusmod += (100*(itembonuses.STR + spellbonuses.STR))/3;
			dmgbonusmod += (100*(spellbonuses.ATK + itembonuses.ATK))/5;
			mlog(COMBAT__DAMAGE, "Damage bonus: %d percent from ATK and STR bonuses.", (dmgbonusmod/100));
			damage += (damage*dmgbonusmod/10000);
		}
		else if ( skill == ARCHERY )
		{
			// Kings & Bandits - modification for pvp combat
			damage += (GetSTR()+GetATK())/10;
		}
	}

	damage += damage * GetMeleeDamageMod_SE(skill) / 100;

	//Rogue sneak attack disciplines make use of this, they are active for one hit
	uint32 buff_count = GetMaxTotalSlots();
	for(int bs = 0; bs < buff_count; bs++){
		if((buffs[bs].spellid != SPELL_UNKNOWN) && IsEffectInSpell(buffs[bs].spellid, SE_HitChance) && spells[buffs[bs].spellid].numhits > 0){
			if(skill == spells[buffs[bs].spellid].skill)
				CheckHitsRemaining(bs);
		}	
	}	
}

bool Mob::HasDied() {
	bool Result = false;
	sint16 hp_below = 0;

	if(IsClient())
		hp_below = (CastToClient()->GetDelayDeath() * -1);

	if((GetHP()) <= (hp_below))
		Result = true;

	return Result;
}

int16 Mob::GetDamageTable(SkillType skillinuse)
{
	if(GetLevel() <= 51)
	{
		int16 ret_table = 0;
		int str_over_75 = 0;
		if(GetSTR() > 75)
			str_over_75 = GetSTR() - 75;
		if(str_over_75 > 255)
			ret_table = (GetSkill(skillinuse)+255)/2;
		else
			ret_table = (GetSkill(skillinuse)+str_over_75)/2;

		if(ret_table < 100)
			return 100;

		return ret_table;
	}
	else if(GetLevel() >= 90)
	{
		if(GetClass() == MONK)
			return 379;
		else
			return 345;
	}
	else
	{
		int16 dmg_table[] = { 
			275, 275, 275, 275, 275,
			280, 280, 280, 280,	285,
			285, 285, 290, 290, 295,
			295, 300, 300, 300, 305, 
			305, 305, 310, 310, 315,
			315, 320, 320, 320, 325,
			325, 325, 330, 330, 335, 
			335, 340, 340, 340,
		};
		if(GetClass() == MONK)
			return (dmg_table[GetLevel()-51]*(100+RuleI(Combat,MonkDamageTableBonus))/100);
		else
			return dmg_table[GetLevel()-51];
	}
}
