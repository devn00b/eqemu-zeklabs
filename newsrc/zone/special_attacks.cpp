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

#include "../common/debug.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "masterentity.h"
#include "StringIDs.h"
#include "../common/MiscFunctions.h"
#include "../common/rulesys.h"

int Mob::GetKickDamage() const {
	int multiple=(GetLevel()*100/5);
	multiple += 100;
	int dmg=(
			    (
				 (GetSkill(KICK) + GetSTR() + GetLevel())*100 / 9000
				) * multiple
			  )
			  + 600;	//Set a base of 6 damage, 1 seemed too low at the sub level 30 level.
	if(GetClass() == WARRIOR || GetClass() == WARRIORGM
	 ||GetClass() == BERSERKER || GetClass() == BERSERKERGM) {
		dmg*=12/10;//small increase for warriors
	}
	dmg /= 100;

	switch (GetAA(aaStrengthenedStrike))
	{
	case 1:
		dmg *= 1.05;
		break;
	case 2:
		dmg *= 1.15;
		break;
	case 3:
		dmg *= 1.30;
		break;
	}

	return(dmg);
}

int Mob::GetBashDamage() const {
	int multiple=(GetLevel()*100/5);
	multiple += 100;

	//this is complete shite
	int dmg=(
			    (
				 ((GetSkill(BASH) + GetSTR())*100 + GetLevel()*100/2) / 10000
				) * multiple
			  )
			  + 600;	//Set a base of 6 damage, 1 seemed too low at the sub level 30 level.
	dmg /= 100;

	switch (GetAA(aaStrengthenedStrike))
	{
	case 1:
		dmg *= 1.05;
		break;
	case 2:
		dmg *= 1.15;
		break;
	case 3:
		dmg *= 1.30;
		break;
	}

	switch (GetAA(aaViciousSmash))
	{
	case 1:
		dmg *= 1.05;
		break;
	case 2:
		dmg *= 1.15;
		break;
	case 3:
		dmg *= 1.30;
		break;
	}

	return(dmg);
}

void Mob::DoSpecialAttackDamage(Mob *who, SkillType skill, uint8 itemType, sint32 max_damage, sint32 min_damage, sint32 hate_override) {
	//this really should go through the same code as normal melee damage to
	//pick up all the special behavior there

	sint32 hate = max_damage;
	if(hate_override > -1)
		hate = hate_override;

	if(skill == BASH)
	{
		if(IsClient())
		{
			ItemInst *item = CastToClient()->GetInv().GetItem(SLOT_SECONDARY);
			if(item)
			{
				itemType = item->GetItem()->ItemType;

				if(item->GetItem()->ItemType == ItemTypeShield)
				{
					hate += item->GetItem()->AC;
				}
				const Item_Struct *itm = item->GetItem();
				hate = hate * (100 + GetFuriousBash(itm->Focus.Effect)) / 100;
			}
		}
	}

	if(max_damage > 0) {
		if(skill != THROWING && skill != ARCHERY && skill != BACKSTAB)
		{
			who->AvoidDamage(this, max_damage);
		}
		who->MeleeMitigation(this, max_damage, min_damage);
		ApplyMeleeDamageBonus(skill, max_damage);
		TryCriticalHit(who, skill, itemType, max_damage);
	}
	who->Damage(this, max_damage, SPELL_UNKNOWN, skill, itemType, false);
			
	// KaB / Red69 / Zek - client stun code
	if ( ( IsClient() && skill == BASH ) || ( GetClass() == WARRIOR && skill == KICK && GetLevel() > RuleI(Combat,WarriorKickStunLevel) ) ) // KaB - Red69 - Zek / July 27 - lvl 55 and higher kick for warrior stuns
	{
		int stun_resist_mod = 0;
			if ( who->HasRune() )
			{
				stun_resist_mod = -80; // makes it a 20% chance to be stunned
			}

			if(who->GetBaseRace() == OGRE && !BehindMob(who, GetX(), GetY())) 
			{
					mlog(COMBAT__HITS, "Stun Resisted. Ogres are immune to frontal melee stuns.");
			}
			else if ( max_damage > 0 && (MakeRandomInt(0,99) < (stun_resist_mod+RuleI(Combat, RandomStunChance))))
			{
				
				bool stunTarget = false;
				// KaB - Red69 - Zek / Epic checks for SK/Pally bash/slam
				
				switch(GetClass())
				{
				case WARRIOR:
					{
						if ( skill == KICK )
							stunTarget = true;

						break;
					}
				case SHADOWKNIGHT:
				case PALADIN:
					{
						const ItemInst *wpn = CastToClient()->GetInv().GetItem(SLOT_PRIMARY);
							if ( wpn && ( wpn->GetItem()->ID == 10099 || wpn->GetItem()->ID == 48147 || // Pally Epic
										wpn->GetItem()->ID == 14383 || wpn->GetItem()->ID == 48136 )  ) // SK Epic
								stunTarget = true;
						break;
					}
				}
					switch(GetBaseRace())
					{
					case BARBARIAN:
					case TROLL:
					case OGRE:
						{
							stunTarget = true;
							break;
						}
					default:
						{
							// Image (2014): this will allow non big races when they bash to stun if they have a shield
							if ( IsClient() && skill == BASH )
							{
								ItemInst* inst = CastToClient()->GetInv().GetItem(SLOT_SECONDARY);
								if ( ( inst != NULL && inst->GetItem()->AC > 0 ) || CastToClient()->GetAA(aa2HandBash) > 0 ) // Image: 5/28/2015, two handed bash (aa2HandBash) addition to stun fix for bash
									stunTarget = true;
							}
							break;
						}
					}

				
				if ( stunTarget )
				{
					// these guys can bash and stun
					if ( who->IsClient() )
						who->CastToClient()->Stun(MakeRandomInt(500,1000), true);
					else if ( who->IsNPC() && !who->CastToNPC()->SpecAttacks[UNSTUNABLE] )
					{
						who->CastToNPC()->Stun(MakeRandomInt(500,1000), true);
					}
				}
			}
	}

	if(max_damage >= 0)
		who->AddToHateList(this, hate);
	
	if(max_damage == -3)
		DoRiposte(who);	
}


void Client::OPCombatAbility(const EQApplicationPacket *app) {
	if(!GetTarget())
		return;
	CombatAbility_Struct* ca_atk = (CombatAbility_Struct*) app->pBuffer;

	//make sure were actually able to use such an attack.
	if( ( spellend_timer.Enabled() && GetClass() != BARD ) 
		|| IsFeared() || IsStunned() || IsMezzed() || DivineAura() || dead )
		return;
	
	if(GetTarget()->GetID() != ca_atk->m_target)
		return;	//invalid packet.
	
	if(!IsAttackAllowed(GetTarget()))
		return;
	
	//These two are not subject to the combat ability timer, as they
	//allready do their checking in conjunction with the attack timer
	//throwing weapons
	if(ca_atk->m_atk == 11) {
		if (ca_atk->m_skill == THROWING) {
			SetAttackTimer();
			ThrowingAttack(GetTarget());
			return;
		}
		//ranged attack (archery)
		if (ca_atk->m_skill == ARCHERY) {
			SetAttackTimer();
			
			RangedAttack(GetTarget());
			if ( GetClass() == RANGER && GetLevel() > 50 && GetTarget() != NULL )
			{
				if ( !GetTarget()->CastToNPC()->IsMoving() )
					RangedAttack(GetTarget(),true);
			}
			return;
		}
		//could we return here? Im not sure is m_atk 11 is used for real specials
	}
	
	//check range for all these abilities, they are all close combat stuff
	if(!CombatRange(GetTarget()))
		return;
	
	if(!p_timers.Expired(&database, pTimerCombatAbility, false)) {
		Message(13,"Ability recovery time not yet met.");
		return;
	}
	
	int ReuseTime = 0;
	int ClientHaste = GetHaste();
	int HasteMod = 0;

	if(ClientHaste >= 0){
		HasteMod = (10000/(100+ClientHaste)); //+100% haste = 2x as many attacks
	}
	else{
		HasteMod = (100-ClientHaste); //-100% haste = 1/2 as many attacks
	}
	sint32 dmg = 0;

	sint32 skill_reduction = this->GetSkillReuseTime(ca_atk->m_skill);
	
	if (/*(ca_atk->m_atk == 100) // commented this out because we don't care if its slam/bash they both use same code except slam checks shield
	  &&*/ (ca_atk->m_skill == BASH)) {    // SLAM - Bash without a shield equipped
		if (GetTarget() != this) {
			
			CheckIncreaseSkill(BASH, GetTarget(), 10);
			DoAnim(animTailRake);

			sint32 ht = 0;

			if( ( GetWeaponDamage(GetTarget(), GetInv().GetItem(SLOT_SECONDARY) ) <= 0 &&
				GetWeaponDamage(GetTarget(), GetInv().GetItem(SLOT_SHOULDER)) <= 0 ) ){
				dmg = -5;
			}
			else{
				if(!GetTarget()->CheckHitChance(this, BASH, 0)) {
					dmg = 0;
					ht = GetBashDamage();
				}
				else{
					if(RuleB(Combat, UseIntervalAC))
						ht = dmg = GetBashDamage();
					else
						ht = dmg = MakeRandomInt(1, GetBashDamage());

				}
			}

			DoSpecialAttackDamage(GetTarget(), BASH, 0, dmg, 1, ht);
			ReuseTime = BashReuseTime-1-skill_reduction;
			ReuseTime = (ReuseTime*HasteMod)/100;
			if(ReuseTime > 0)
			{
				p_timers.Start(pTimerCombatAbility, ReuseTime);
			}
		}
		return;
	}

	if ((ca_atk->m_atk == 100) && (ca_atk->m_skill == FRENZY)) 
	{
		CheckIncreaseSkill(FRENZY, GetTarget(), 10);

		int dmg = 1 + (GetSkill(FRENZY) / 100);

		switch (GetAA(aaBlurofAxes)) {
			case 1:
				dmg *= 1.15;
				break;
			case 2:
				dmg *= 1.30;
				break;
			case 3:
				dmg *= 1.50;
				break;
		}

		switch (GetAA(aaViciousFrenzy)) {
			case 1:
				dmg *= 1.05;
				break;
			case 2:
				dmg *= 1.10;
				break;
			case 3:
				dmg *= 1.15;
				break;
			case 4:
				dmg *= 1.20;
				break;
			case 5:
				dmg *= 1.25;
				break;
		}

		while(dmg > 0 && GetTarget()) {
			if(Attack(GetTarget()))
				dmg--;
			else
				dmg = 0;
		}

		ReuseTime = FrenzyReuseTime-1-skill_reduction;
		ReuseTime = (ReuseTime*HasteMod)/100;
		if(ReuseTime > 0) {
			p_timers.Start(pTimerCombatAbility, ReuseTime);
		}
		return;
	}

	switch(GetClass())
	{
		case BERSERKER:
		case WARRIOR:
		case RANGER:
		case BEASTLORD:
			if (ca_atk->m_atk != 100 || ca_atk->m_skill != KICK) {
				break;
			}
			if (GetTarget() != this) {
				CheckIncreaseSkill(KICK, GetTarget(), 10);
				DoAnim(animKick);

				sint32 ht = 0;
				if(GetWeaponDamage(GetTarget(), GetInv().GetItem(SLOT_FEET)) <= 0){
					dmg = -5;
				}
				else{
					if(!GetTarget()->CheckHitChance(this, KICK, 0)) {
						dmg = 0;
						ht = GetKickDamage();
					}
					else{
						if(RuleB(Combat, UseIntervalAC))
							ht = dmg = GetKickDamage();
						else
							ht = dmg = MakeRandomInt(1, GetKickDamage());
					}
				}

				DoSpecialAttackDamage(GetTarget(), KICK, 0, dmg, 1, ht);
				ReuseTime = KickReuseTime-1-skill_reduction;
			}
			break;
		case MONK: {
			bool override_ = false;
				int8 satype = KICK;
			if(level < 30 && level > 24 && ca_atk->m_skill == FLYING_KICK) {
					satype = DRAGON_PUNCH;
					override_ = true;
			} else if(level < 25 && level > 19 && ( ca_atk->m_skill == FLYING_KICK || ca_atk->m_skill == DRAGON_PUNCH ) ) {
					satype = EAGLE_STRIKE;
					override_ = true;
			} else if(level < 20 && level > 9 && ( ca_atk->m_skill == FLYING_KICK || ca_atk->m_skill == DRAGON_PUNCH || ca_atk->m_skill == EAGLE_STRIKE ) ) {
					satype = TIGER_CLAW;
					override_ = true;
			} else if(level < 10 && level > 4 && ( ca_atk->m_skill == FLYING_KICK || ca_atk->m_skill == DRAGON_PUNCH || ca_atk->m_skill == EAGLE_STRIKE || ca_atk->m_skill == TIGER_CLAW ) ) {
					satype = ROUND_KICK;
					override_ = true;
			} else if ( level < 5 && ( ca_atk->m_skill == FLYING_KICK || ca_atk->m_skill == DRAGON_PUNCH || ca_atk->m_skill == EAGLE_STRIKE || ca_atk->m_skill == TIGER_CLAW 
				|| ca_atk->m_skill == ROUND_KICK ) )
			{
				override_ = true;
			}

			if ( override_ )
			{
				// override the attack type
				ca_atk->m_skill = satype;
				char hString[250];
				sprintf(hString, "Monk Hack, Tried to use ability %i at level %i", ca_atk->m_skill, GetLevel());

				database.logevents(this->account_name,this->AccountID(), 0, this->name, GetTarget() != NULL ? GetTarget()->GetName() : "", "OPCombatAbility", hString, 15,GetX(),
					GetY(),GetZ(), (char*)database.GetZoneName(GetZoneID(), GetPP().zoneInstance, true) );
			}



			ReuseTime = MonkSpecialAttack(GetTarget(), ca_atk->m_skill) - 1 - skill_reduction;

			int specl = GetAA(aaTechniqueofMasterWu) * 20;
			if(specl == 100 || specl > MakeRandomInt(0,100)) {
				ReuseTime = MonkSpecialAttack(GetTarget(), ca_atk->m_skill) - 1 - skill_reduction;
				if(20 > MakeRandomInt(0,100)) {
					ReuseTime = MonkSpecialAttack(GetTarget(), ca_atk->m_skill) - 1 - skill_reduction;
				}
			}

			if(ReuseTime < 100) {
				//hackish... but we return a huge reuse time if this is an 
				// invalid skill, otherwise, we can safely assume it is a 
				// valid monk skill and just cast it to a SkillType
				CheckIncreaseSkill((SkillType) ca_atk->m_skill, GetTarget(), 10);
			}
			break;
		}
		case ROGUE: {
			if (ca_atk->m_atk != 100 || ca_atk->m_skill != BACKSTAB) {
				break;
			}
			TryBackstab(GetTarget());
			ReuseTime = BackstabReuseTime-1 - skill_reduction;
			break;
		}
		default:
			//they have no abilities... wtf? make em wait a bit
			ReuseTime = 9 - skill_reduction;
			break;
	}
	
	ReuseTime = (ReuseTime*HasteMod)/100;
	if(ReuseTime > 0)
	{
		p_timers.Start(pTimerCombatAbility, ReuseTime);
	}	
}

//returns the reuse time in sec for the special attack used.
int Mob::MonkSpecialAttack(Mob* other, int8 unchecked_type)
{
	if(!other)
		return 0;

	sint32 ndamage = 0;
	sint32 max_dmg = 0;
	sint32 min_dmg = 1;
	int reuse = 0;
	SkillType skill_type;	//to avoid casting... even though it "would work"
	int8 itemslot = SLOT_FEET;
	
	switch(unchecked_type)
	{
	case FLYING_KICK:{
		skill_type = FLYING_KICK;
		max_dmg = ((GetSTR()+GetSkill(skill_type)) * RuleI(Combat, FlyingKickBonus) / 100) + 35;
		min_dmg = ((level*8)/10);

		if (IsClient() && CastToClient()->GetAA(aaKickMastery))
		{
			switch (CastToClient()->GetAA(aaKickMastery))
			{
			case 1:
				min_dmg += MakeRandomFloat(10,50);
				break;
			case 2:
				min_dmg += MakeRandomFloat(20,60);
				break;
			case 3:
				min_dmg += MakeRandomFloat(30,70);
				break;
			}
		}

		DoAnim(animFlyingKick);
		reuse = FlyingKickReuseTime;
		break;
		}
	case DRAGON_PUNCH:{
		skill_type = DRAGON_PUNCH;
		max_dmg = ((GetSTR()+GetSkill(skill_type)) * RuleI(Combat, DragonPunchBonus) / 100) + 26;
		itemslot = SLOT_HANDS;

		DoAnim(animTailRake);
		reuse = TailRakeReuseTime;
		break;
		}

	case EAGLE_STRIKE:{
		skill_type = EAGLE_STRIKE;
		max_dmg = ((GetSTR()+GetSkill(skill_type)) * RuleI(Combat, EagleStrikeBonus) / 100) + 19;
		itemslot = SLOT_HANDS;

		DoAnim(animEagleStrike);
		reuse = EagleStrikeReuseTime;
		break;
		}

	case TIGER_CLAW:{
		skill_type = TIGER_CLAW;
		max_dmg = ((GetSTR()+GetSkill(skill_type)) * RuleI(Combat, TigerClawBonus) / 100) + 12;
		itemslot = SLOT_HANDS;

		DoAnim(animTigerClaw);
		reuse = TigerClawReuseTime;
		break;
		}

	case ROUND_KICK:{
		skill_type = ROUND_KICK;
		max_dmg = ((GetSTR()+GetSkill(skill_type)) * RuleI(Combat, RoundKickBonus) / 100) + 10;

		DoAnim(animRoundKick);
		reuse = RoundKickReuseTime;
		break;
		}

	case KICK:{
		skill_type = KICK;
		max_dmg = GetKickDamage();

		DoAnim(animKick);
		reuse = KickReuseTime;
		break;
			  }
	default:
		mlog(CLIENT__ERROR, "Invalid special attack type %d attempted", unchecked_type);
		return(1000); /* nice long delay for them, the caller depends on this! */
	}

	if(IsClient()){
		if(GetWeaponDamage(other, CastToClient()->GetInv().GetItem(itemslot)) <= 0){
			ndamage = -5;
		}
	}
	else{
		if(GetWeaponDamage(other, (const Item_Struct*)NULL) <= 0){
			ndamage = -5;
		}
	}

	sint32 ht = 0;
	if(ndamage == 0){
		if(other->CheckHitChance(this, skill_type, 0)){
			if(RuleB(Combat, UseIntervalAC))
				ht = ndamage = max_dmg;
			else
				ht = ndamage = MakeRandomInt(min_dmg, max_dmg);
		}
		else
		{
			ht = max_dmg;
		}
	}
	
	DoSpecialAttackDamage(other, skill_type, 0, ndamage, min_dmg, ht);

	if(unchecked_type == DRAGON_PUNCH && GetAA(aaDragonPunch) && MakeRandomInt(0, 99) < 25){
		SpellFinished(904, other, 10, 0, -1, spells[904].ResistDiff);
		other->Stun(100);
	}	
	return(reuse);
}

void Mob::TryBackstab(Mob *other) {
	if(!other)
		return;
	
	//make sure we have a proper weapon if we are a client.
	if(IsClient()) {
		const ItemInst *wpn = CastToClient()->GetInv().GetItem(SLOT_PRIMARY);
		if(!wpn || (wpn->GetItem()->ItemType != ItemTypePierce)){
			Message_StringID(13, BACKSTAB_WEAPON);
			return;
		}
	}
	
	bool tripleBackstab = false;
	int tripleChance = 0;
	if (IsClient() && CastToClient()->GetAA(aaTripleBackstab) > 0) {
		switch (CastToClient()->GetAA(aaTripleBackstab)) {
		case 1:
			tripleChance = 10;
			break;
		case 2:
			tripleChance = 20;
			break;
		case 3:
			tripleChance = 30;
			break;
		}
		if (tripleChance > MakeRandomInt(0, 100)) {
			tripleBackstab = true;
		}
	}

	bool seizedOpportunity = false;
	int seizedChance = 0;
	if (IsClient() && CastToClient()->GetAA(aaSeizedOpportunity) > 0) {
		switch (CastToClient()->GetAA(aaSeizedOpportunity)) {
			case 1:
				seizedChance = 10;
				break;
			case 2:
				seizedChance = 20;
				break;
			case 3:
				seizedChance = 30;
				break;
		}
		if (seizedChance > MakeRandomInt(0, 100)) {
			seizedOpportunity = true;
		}
	}

	if (BehindMob(other, GetX(), GetY()) || seizedOpportunity) // Player is behind other
	{
		if (seizedOpportunity) {
			CastToClient()->Message(0,"Your fierce attack is executed with such grace, your target did not see it coming!");
		}

		// solar - chance to assassinate
		int chance = 10 + (GetDEX()/10); //18.5% chance at 85 dex 40% chance at 300 dex
		if(
			level >= 60 && // player is 60 or higher
			other->GetLevel() <= 45 && // mob 45 or under
			!other->CastToNPC()->IsEngaged() && // not aggro
			other->GetHP()<=32000
			&& other->IsNPC()
			&& MakeRandomFloat(0, 99) < chance // chance
			) {
			entity_list.MessageClose_StringID(this, false, 200, MT_CritMelee, ASSASSINATES, GetName());
			if(IsClient())
				CastToClient()->CheckIncreaseSkill(BACKSTAB, other, 10);
			RogueAssassinate(other);
		}
		else {
			RogueBackstab(other);
			// KAB / Red69 / Zek - rogues require lvl 55 or higher
			// http://www.thesafehouse.org/forums/showthread.php?t=28135
			if ( GetLevel() > 54 )
			{
				float DoubleAttackProbability = (GetSkill(DOUBLE_ATTACK) + GetLevel()) / RuleR(Combat,DoubleBackstabDivision); // 62.4 max
				// Check for double attack with main hand assuming maxed DA Skill (MS)
				
				if(MakeRandomFloat(0, 1) < DoubleAttackProbability)	// Max 62.4 % chance of DA
				{
					if(other->GetHP() > 0)
						RogueBackstab(other);

					/*if (tripleBackstab && other->GetHP() > 0) 
					{
						RogueBackstab(other);
					}*/
				}
			}
			if(IsClient())
				CastToClient()->CheckIncreaseSkill(BACKSTAB, other, 10);
		}
	}
	else if(GetAA(aaChaoticStab) > 0) {
		//we can stab from any angle, we do min damage though.
		RogueBackstab(other, true);
		if (level > 54) {
			float DoubleAttackProbability = (GetSkill(DOUBLE_ATTACK) + GetLevel()) / 500.0f; // 62.4 max
			if(IsClient())
				CastToClient()->CheckIncreaseSkill(BACKSTAB, other, 10);
			// Check for double attack with main hand assuming maxed DA Skill (MS)
			if(MakeRandomFloat(0, 1) < DoubleAttackProbability)		// Max 62.4 % chance of DA
				if(other->GetHP() > 0)
					RogueBackstab(other, true);

			if (tripleBackstab && other->GetHP() > 0) {
					RogueBackstab(other);
				}
		}
	}
	else { //We do a single regular attack if we attack from the front without chaotic stab
		Attack(other, 13);
	}
}

//heko: backstab
void Mob::RogueBackstab(Mob* other, bool min_damage)
{
	sint32 ndamage = 0;
	sint32 max_hit = 0;
	sint32 min_hit = 0;
	sint32 hate = 0;
	sint32 primaryweapondamage = 0;
	sint32 backstab_dmg = 0;

	if(IsClient()){
		const ItemInst *wpn = NULL;
		wpn = CastToClient()->GetInv().GetItem(SLOT_PRIMARY);
		primaryweapondamage = GetWeaponDamage(other, wpn);
		backstab_dmg = wpn->GetItem()->BackstabDmg;
		for(int i = 0; i < MAX_AUGMENT_SLOTS; ++i)
		{
			ItemInst *aug = wpn->GetAugment(i);
			if(aug)
			{
				backstab_dmg += aug->GetItem()->BackstabDmg;
			}
		}
	}
	else{
		primaryweapondamage = (GetLevel()/7)+1; // fallback incase it's a npc without a weapon, 2 dmg at 10, 10 dmg at 65
		backstab_dmg = primaryweapondamage;
	}
	
	if(primaryweapondamage > 0){
		if(level > 25){
			max_hit = (((2*backstab_dmg) * GetDamageTable(BACKSTAB) / 100) * 10 * GetSkill(BACKSTAB) / 255)  + ((level-15)/2) + 1;
			hate = 20 * backstab_dmg * GetSkill(BACKSTAB) / 255;
		}
		else{
			max_hit = (((2*backstab_dmg) * GetDamageTable(BACKSTAB) / 100) * 10 * GetSkill(BACKSTAB) / 200) + (level/3) + 1;
			hate = 20 * backstab_dmg * GetSkill(BACKSTAB) / 200;
		}

		// determine minimum hits
		if (level < 51)
		{
			min_hit = (level*15/10);
		}
		else
		{
			// Trumpcard:  Replaced switch statement with formula calc.  This will give minhit increases all the way to 65.
			min_hit = (level * ( level*4)) / 100;
		}

		if(!other->CheckHitChance(this, BACKSTAB, 0))	{
			ndamage = 0;
		}
		else{
			if(min_damage){
				ndamage = min_hit;
			}
			else
			{
				if (max_hit < min_hit)
					max_hit = min_hit;

				if(RuleB(Combat, UseIntervalAC))
					ndamage = max_hit; 
				else
					ndamage = MakeRandomInt(min_hit, max_hit);

			}
		}
	}
	else{
		ndamage = -5;
	}

	if ( IsClient() && other && other->IsClient() )
		max_hit += (sint32)(float)max_hit * RuleR(PVPCombat, PVPBackstabMaxHitIncrease);
	else
		max_hit += (sint32)(float)max_hit * RuleR(Combat, BackstabMaxHitIncrease);

	DoSpecialAttackDamage(other, BACKSTAB, 0, ndamage, min_hit, hate);
	DoAnim(animPiercing);
}

// solar - assassinate
void Mob::RogueAssassinate(Mob* other)
{
	//can you dodge, parry, etc.. an assassinate??
	//if so, use DoSpecialAttackDamage(other, BACKSTAB, 32000); instead
	uint8 itemType = 0;
	ItemInst* weapon = NULL;
	if ( IsClient() )
	{
		weapon = CastToClient()->GetInv().GetItem(SLOT_PRIMARY);
		if ( weapon != NULL )
			itemType = weapon->GetItem()->ItemType;
	}
	if(GetWeaponDamage(other, weapon) > 0){
		other->Damage(this, 32000, SPELL_UNKNOWN, BACKSTAB, itemType);
	}else{
		other->Damage(this, -5, SPELL_UNKNOWN, BACKSTAB, itemType);
	}
	DoAnim(animPiercing);	//piercing animation
}

void Client::RangedAttack(Mob* other, bool bypassTimer) {
	//conditions to use an attack checked before we are called

	//make sure the attack and ranged timers are up
	//if the ranged timer is disabled, then they have no ranged weapon and shouldent be attacking anyhow
	if(!bypassTimer && ((attack_timer.Enabled() && !attack_timer.Check(false)) || (ranged_timer.Enabled() && !ranged_timer.Check())) ) {
		mlog(COMBAT__RANGED, "Throwing attack canceled. Timer not up. Attack %d, ranged %d", attack_timer.GetRemainingTime(), ranged_timer.GetRemainingTime());
		// The server and client timers are not exact matches currently, so this would spam too often if enabled
		//Message(0, "Error: Timer not up. Attack %d, ranged %d", attack_timer.GetRemainingTime(), ranged_timer.GetRemainingTime());
		return;
	}
	const ItemInst* RangeWeapon = m_inv[SLOT_RANGE];
	
	//locate ammo
	int ammo_slot = SLOT_AMMO;
	const ItemInst* Ammo = m_inv[SLOT_AMMO];
	
	if (!RangeWeapon || !RangeWeapon->IsType(ItemClassCommon)) {
		mlog(COMBAT__RANGED, "Ranged attack canceled. Missing or invalid ranged weapon (%d) in slot %d", GetItemIDAt(SLOT_RANGE), SLOT_RANGE);
		Message(0, "Error: Rangeweapon: GetItem(%i)==0, you have no bow!", GetItemIDAt(SLOT_RANGE));
		return;
	}
	if (!Ammo || !Ammo->IsType(ItemClassCommon)) {
		mlog(COMBAT__RANGED, "Ranged attack canceled. Missing or invalid ammo item (%d) in slot %d", GetItemIDAt(SLOT_AMMO), SLOT_AMMO);
		Message(0, "Error: Ammo: GetItem(%i)==0, you have no ammo!", GetItemIDAt(SLOT_AMMO));
		return;
	}
	
	const Item_Struct* RangeItem = RangeWeapon->GetItem();
	const Item_Struct* AmmoItem = Ammo->GetItem();
	
	if(RangeItem->ItemType != ItemTypeBow) {
		mlog(COMBAT__RANGED, "Ranged attack canceled. Ranged item is not a bow. type %d.", RangeItem->ItemType);
		Message(0, "Error: Rangeweapon: Item %d is not a bow.", RangeWeapon->GetID());
		return;
	}
	if(AmmoItem->ItemType != ItemTypeArrow) {
		mlog(COMBAT__RANGED, "Ranged attack canceled. Ammo item is not an arrow. type %d.", AmmoItem->ItemType);
		Message(0, "Error: Ammo: type %d != %d, you have the wrong type of ammo!", AmmoItem->ItemType, ItemTypeArrow);
		return;
	}
	
	mlog(COMBAT__RANGED, "Shooting %s with bow %s (%d) and arrow %s (%d)", GetTarget()->GetName(), RangeItem->Name, RangeItem->ID, AmmoItem->Name, AmmoItem->ID);
	
	//look for ammo in inventory if we only have 1 left...
	if(Ammo->GetCharges() == 1) {
		//first look for quivers
		int r;
		bool found = false;
		for(r = SLOT_PERSONAL_BEGIN; r <= SLOT_PERSONAL_END; r++) {
			const ItemInst *pi = m_inv[r];
			if(pi == NULL || !pi->IsType(ItemClassContainer))
				continue;
			const Item_Struct* bagitem = pi->GetItem();
			if(!bagitem || bagitem->BagType != bagTypeQuiver)
				continue;
			
			//we found a quiver, look for the ammo in it
			int i;
			for (i = 0; i < bagitem->BagSlots; i++) {
				ItemInst* baginst = pi->GetItem(i);
				if(!baginst)
					continue;	//empty
				if(baginst->GetID() == Ammo->GetID()) {
					//we found it... use this stack
					//the item wont change, but the instance does
					Ammo = baginst;
					ammo_slot = m_inv.CalcSlotId(r, i);
					found = true;
					mlog(COMBAT__RANGED, "Using ammo from quiver stack at slot %d. %d in stack.", ammo_slot, Ammo->GetCharges());
					break;
				}
			}
			if(found)
				break;
		}
		
		if(!found) {
			//if we dont find a quiver, look through our inventory again
			//not caring if the thing is a quiver.
			sint32 aslot = m_inv.HasItem(AmmoItem->ID, 1, invWherePersonal);
			if(aslot != SLOT_INVALID) {
				ammo_slot = aslot;
				Ammo = m_inv[aslot];
				mlog(COMBAT__RANGED, "Using ammo from inventory stack at slot %d. %d in stack.", ammo_slot, Ammo->GetCharges());
			}
		}
	}
	
	float range = RangeItem->Range + AmmoItem->Range + 5; //Fudge it a little, client will let you hit something at 0 0 0 when you are at 205 0 0
	mlog(COMBAT__RANGED, "Calculated bow range to be %.1f", range);
	range *= range;
	if(DistNoRootNoZ(*GetTarget()) > range) {
		mlog(COMBAT__RANGED, "Ranged attack out of range... client should catch this. (%f > %f).\n", DistNoRootNoZ(*GetTarget()), range);
		//target is out of range, client does a message
		return;
	}
	else if(DistNoRootNoZ(*GetTarget()) < (RuleI(Combat, MinRangedAttackDist)*RuleI(Combat, MinRangedAttackDist))){
		return;
	}

	if(!IsAttackAllowed(GetTarget()) || 
		IsCasting() || 
		IsSitting() || 
		(DivineAura() && !GetGM()) ||
		IsStunned() ||
		IsFeared() ||
		IsMezzed() ||
		(GetAppearance() == eaDead)){
		return;
	}
	
	SendItemAnimation(GetTarget(), AmmoItem, ARCHERY);
		
	// Hit?
	if (!GetTarget()->CheckHitChance(this, ARCHERY, 13)) {
		mlog(COMBAT__RANGED, "Ranged attack missed %s.", GetTarget()->GetName());
		GetTarget()->Damage(this, 0, SPELL_UNKNOWN, ARCHERY, RangeItem->ItemType);
	} else {
		mlog(COMBAT__RANGED, "Ranged attack hit %s.", GetTarget()->GetName());
		
		if(!TryHeadShot(GetTarget(), ARCHERY, RangeItem->ItemType)) 
		{
			sint16 WDmg = GetWeaponDamage(GetTarget(), RangeWeapon);
			sint16 ADmg = GetWeaponDamage(GetTarget(), Ammo);
			if((WDmg > 0) || (ADmg > 0))
			{
				if(WDmg < 0)
					WDmg = 0;
				if(ADmg < 0)
					ADmg = 0;

				uint32 MaxDmg = (RuleR(Combat, ArcheryBaseDamageBonus)*(WDmg+ADmg)*GetDamageTable(ARCHERY)) / 100;
				sint32 hate = ((WDmg+ADmg));
							
				switch(GetAA(aaArcheryMastery)) {
					case 1:
						MaxDmg = MaxDmg * 130/100;
						break;
					case 2:
						MaxDmg = MaxDmg * 160/100;
						break;
					case 3:
						MaxDmg = MaxDmg * 2;
						break;
				}
				
				mlog(COMBAT__RANGED, "Bow DMG %d, Arrow DMG %d, Max Damage %d.", WDmg, ADmg, MaxDmg);

				if(GetClass() == RANGER && GetLevel() > 50)
				{
					if(RuleB(Combat, ArcheryBonusRequiresStationary))
					{
						if(GetTarget()->IsNPC() && !GetTarget()->IsMoving() && !GetTarget()->IsRooted())
						{
							MaxDmg *= 2;
							hate *= 2;
							mlog(COMBAT__RANGED, "Ranger. Double damage success roll, doubling damage to %d", MaxDmg);
							Message_StringID(MT_CritMelee, BOW_DOUBLE_DAMAGE);
						}
					}
					else
					{
						MaxDmg *= 2;
						hate *= 2;
						mlog(COMBAT__RANGED, "Ranger. Double damage success roll, doubling damage to %d", MaxDmg);
						Message_StringID(MT_CritMelee, BOW_DOUBLE_DAMAGE);
					}
				}

				sint32 TotalDmg = 0;
				
				if (MaxDmg == 0)
					MaxDmg = 1;

				if(RuleB(Combat, UseIntervalAC))
					TotalDmg = MaxDmg;
				else
					TotalDmg = MakeRandomInt(1, MaxDmg);

				// Kings & Bandits - fixed ranged attack for GetDEX not heroic dex
				TotalDmg += GetDEX()/5; //think this adds like this since archery hits less often than melee
				
				int minDmg = 1;
				if(GetLevel() > 25){
					//twice, for ammo and weapon
					TotalDmg += (2*((GetLevel()-25)/3));
					minDmg += (2*((GetLevel()-25)/3));
					hate += (2*((GetLevel()-25)/3));
				}

				GetTarget()->MeleeMitigation(this, TotalDmg, minDmg);
				ApplyMeleeDamageBonus(ARCHERY, TotalDmg);
				TryCriticalHit(GetTarget(), ARCHERY, RangeItem->ItemType, TotalDmg);

				if ( !GetTarget()->IsClient() || (GetTarget()->IsClient() && GetTarget()->IsCharmed()) )
					GetTarget()->AddToHateList(this, hate, 0, false);

				GetTarget()->Damage(this, TotalDmg, SPELL_UNKNOWN, ARCHERY, RangeItem->ItemType);
			}
			else {
				GetTarget()->Damage(this, -5, SPELL_UNKNOWN, ARCHERY, RangeItem->ItemType);
			}
		}
	}
	
	//try proc on hits and misses
	if(GetTarget() && (GetTarget()->GetHP() > -10))
	{
		TryWeaponProc(RangeWeapon, GetTarget());
	}
	
	//consume ammo (should stay at the end, after we are done with everything)
	if(!GetAA(aaEndlessQuiver)) {
		DeleteItemInInventory(ammo_slot, 1, true);
		mlog(COMBAT__RANGED, "Consumed one arrow from slot %d", ammo_slot);
	} else {
		mlog(COMBAT__RANGED, "Endless Quiver prevented ammo consumption.");
	}
	
	CheckIncreaseSkill(ARCHERY, GetTarget(), 0);

	//break invis when you attack
	if(invisible) {
		mlog(COMBAT__ATTACKS, "Removing invisibility due to melee attack.");
		BuffFadeByEffect(SE_Invisibility);
		BuffFadeByEffect(SE_Invisibility2);
		invisible = false;
	}
	if(invisible_undead) {
		mlog(COMBAT__ATTACKS, "Removing invisibility vs. undead due to melee attack.");
		BuffFadeByEffect(SE_InvisVsUndead);
		BuffFadeByEffect(SE_InvisVsUndead2);
		invisible_undead = false;
	}
	if(invisible_animals){
		mlog(COMBAT__ATTACKS, "Removing invisibility vs. animals due to melee attack.");
		BuffFadeByEffect(SE_InvisVsAnimals);
		invisible_animals = false;
	}

	if(hidden || improved_hidden){
		BreakMobHide();
	}
}

void NPC::RangedAttack(Mob* other, bool bypassTimer) 
{
	//make sure the attack and ranged timers are up
	//if the ranged timer is disabled, then they have no ranged weapon and shouldent be attacking anyhow
	if(!bypassTimer && ((attack_timer.Enabled() && !attack_timer.Check(false)) || (ranged_timer.Enabled() && !ranged_timer.Check())) ) 
	{
		mlog(COMBAT__RANGED, "Archery canceled. Timer not up. Attack %d, ranged %d", attack_timer.GetRemainingTime(), ranged_timer.GetRemainingTime());
		return;
	}

	//if we have SPECATK_RANGED_ATK set then we range attack without weapon or ammo
	const Item_Struct* weapon = NULL;
	const Item_Struct* ammo = NULL;
	if(!SpecAttacks[SPECATK_RANGED_ATK])
	{
		//find our bow and ammo return if we can't find them...
		return;
	}

	float range = 250; // needs to be longer than 200(most spells)
	mlog(COMBAT__RANGED, "Calculated bow range to be %.1f", range);
	range *= range;
	if(DistNoRootNoZ(*GetTarget()) > range) {
		mlog(COMBAT__RANGED, "Ranged attack out of range...%.2f vs %.2f", DistNoRootNoZ(*GetTarget()), range);
		//target is out of range, client does a message
		return;
	}
	else if(DistNoRootNoZ(*GetTarget()) < (RuleI(Combat, MinRangedAttackDist)*RuleI(Combat, MinRangedAttackDist))){
		return;
	}

	if(!IsAttackAllowed(GetTarget()) || 
		IsCasting() || 
		DivineAura() ||
		IsStunned() ||
		IsFeared() ||
		IsMezzed() ||
		(GetAppearance() == eaDead)){
		return;
	}
	
	if(!ammo)
	{
		ammo = database.GetItem(8005);
	}

	if(ammo)
		SendItemAnimation(GetTarget(), ammo, ARCHERY);
		
	// Face the Target
	FaceTarget(GetTarget());
		
	// Hit?
	if (!GetTarget()->CheckHitChance(this, ARCHERY, 13)) 
	{
		mlog(COMBAT__RANGED, "Ranged attack missed %s.", GetTarget()->GetName());
		GetTarget()->Damage(this, 0, SPELL_UNKNOWN, ARCHERY, 0);
	} 
	else 
	{
		sint16 WDmg = GetWeaponDamage(GetTarget(), weapon);
		sint16 ADmg = GetWeaponDamage(GetTarget(), ammo);
		if(WDmg > 0 || ADmg > 0)
		{
			mlog(COMBAT__RANGED, "Ranged attack hit %s.", GetTarget()->GetName());
			sint32 TotalDmg = 0;
			
			sint32 MaxDmg = max_dmg * RuleR(Combat, ArcheryNPCMultiplier); // should add a field to npc_types
			sint32 MinDmg = min_dmg * RuleR(Combat, ArcheryNPCMultiplier);

			if(RuleB(Combat, UseIntervalAC))
				TotalDmg = MaxDmg;
			else
				TotalDmg = MakeRandomInt(MinDmg, MaxDmg);

			sint32 hate = TotalDmg;

			GetTarget()->MeleeMitigation(this, TotalDmg, MinDmg);
			ApplyMeleeDamageBonus(ARCHERY, TotalDmg);
			TryCriticalHit(GetTarget(), ARCHERY, 0, TotalDmg);
			GetTarget()->AddToHateList(this, hate, 0, false);
			GetTarget()->Damage(this, TotalDmg, SPELL_UNKNOWN, ARCHERY, 0);
		}
		else 
		{
			GetTarget()->Damage(this, -5, SPELL_UNKNOWN, ARCHERY, 0);
		}
	}

	//break invis when you attack
	if(invisible) {
		mlog(COMBAT__ATTACKS, "Removing invisibility due to melee attack.");
		BuffFadeByEffect(SE_Invisibility);
		BuffFadeByEffect(SE_Invisibility2);
		invisible = false;
	}
	if(invisible_undead) {
		mlog(COMBAT__ATTACKS, "Removing invisibility vs. undead due to melee attack.");
		BuffFadeByEffect(SE_InvisVsUndead);
		BuffFadeByEffect(SE_InvisVsUndead2);
		invisible_undead = false;
	}
	if(invisible_animals){
		mlog(COMBAT__ATTACKS, "Removing invisibility vs. animals due to melee attack.");
		BuffFadeByEffect(SE_InvisVsAnimals);
		invisible_animals = false;
	}

	if(hidden || improved_hidden){
		BreakMobHide();
	}
}

uint16 Mob::GetThrownDamage(sint16 wDmg, sint32& TotalDmg, int& minDmg)
{
	uint16 MaxDmg = (((2 * wDmg) * GetDamageTable(THROWING)) / 100);

	switch(GetAA(aaThrowingMastery))
	{
		case 1:
			MaxDmg = MaxDmg * 115/100;
			break;
		case 2:
			MaxDmg = MaxDmg * 125/100;
			break;
		case 3:
			MaxDmg = MaxDmg * 150/100;
			break;
	}

	if (MaxDmg == 0)
		MaxDmg = 1;
			
	if(RuleB(Combat, UseIntervalAC))
		TotalDmg = MaxDmg;
	else
		TotalDmg = MakeRandomInt(1, MaxDmg);

	minDmg = 1;
	if(GetLevel() > 25)
	{
		TotalDmg += ((GetLevel()-25)/3);
		minDmg += ((GetLevel()-25)/3);
	}

	return MaxDmg;
}

void Client::ThrowingAttack(Mob* other) { //old was 51
	//conditions to use an attack checked before we are called
	
	//make sure the attack and ranged timers are up
	//if the ranged timer is disabled, then they have no ranged weapon and shouldent be attacking anyhow
	if((attack_timer.Enabled() && !attack_timer.Check(false)) || (ranged_timer.Enabled() && !ranged_timer.Check())) {
		mlog(COMBAT__RANGED, "Throwing attack canceled. Timer not up. Attack %d, ranged %d", attack_timer.GetRemainingTime(), ranged_timer.GetRemainingTime());
		// The server and client timers are not exact matches currently, so this would spam too often if enabled
		//Message(0, "Error: Timer not up. Attack %d, ranged %d", attack_timer.GetRemainingTime(), ranged_timer.GetRemainingTime());
		return;
	}
		
	int ammo_slot = SLOT_RANGE;
	const ItemInst* RangeWeapon = m_inv[SLOT_RANGE];
	
	if (!RangeWeapon || !RangeWeapon->IsType(ItemClassCommon)) {
		mlog(COMBAT__RANGED, "Ranged attack canceled. Missing or invalid ranged weapon (%d) in slot %d", GetItemIDAt(SLOT_RANGE), SLOT_RANGE);
		Message(0, "Error: Rangeweapon: GetItem(%i)==0, you have nothing to throw!", GetItemIDAt(SLOT_RANGE));
		return;
	}
	
	const Item_Struct* item = RangeWeapon->GetItem();
	if(item->ItemType != ItemTypeThrowing && item->ItemType != ItemTypeThrowingv2) {
		mlog(COMBAT__RANGED, "Ranged attack canceled. Ranged item %d is not a throwing weapon. type %d.", item->ItemType);
		Message(0, "Error: Rangeweapon: GetItem(%i)==0, you have nothing useful to throw!", GetItemIDAt(SLOT_RANGE));
		return;
	}
	
	mlog(COMBAT__RANGED, "Throwing %s (%d) at %s", item->Name, item->ID, GetTarget()->GetName());
	
	if(RangeWeapon->GetCharges() == 1) {
		//first check ammo
		const ItemInst* AmmoItem = m_inv[SLOT_AMMO];
		if(AmmoItem != NULL && AmmoItem->GetID() == RangeWeapon->GetID()) {
			//more in the ammo slot, use it
			RangeWeapon = AmmoItem;
			ammo_slot = SLOT_AMMO;
			mlog(COMBAT__RANGED, "Using ammo from ammo slot, stack at slot %d. %d in stack.", ammo_slot, RangeWeapon->GetCharges());
		} else {
			//look through our inventory for more
			sint32 aslot = m_inv.HasItem(item->ID, 1, invWherePersonal);
			if(aslot != SLOT_INVALID) {
				//the item wont change, but the instance does, not that it matters
				ammo_slot = aslot;
				RangeWeapon = m_inv[aslot];
				mlog(COMBAT__RANGED, "Using ammo from inventory slot, stack at slot %d. %d in stack.", ammo_slot, RangeWeapon->GetCharges());
			}
		}
	}
	
	int range = item->Range +50/*Fudge it a little, client will let you hit something at 0 0 0 when you are at 205 0 0*/;
	mlog(COMBAT__RANGED, "Calculated bow range to be %.1f", range);
	range *= range;
	if(DistNoRootNoZ(*GetTarget()) > range) {
		mlog(COMBAT__RANGED, "Throwing attack out of range... client should catch this. (%f > %f).\n", DistNoRootNoZ(*GetTarget()), range);
		//target is out of range, client does a message
		return;
	}
	else if(DistNoRootNoZ(*GetTarget()) < (RuleI(Combat, MinRangedAttackDist)*RuleI(Combat, MinRangedAttackDist))){
		return;
	}

	if(!IsAttackAllowed(GetTarget()) || 
		IsCasting() || 
		IsSitting() || 
		(DivineAura() && !GetGM()) ||
		IsStunned() ||
		IsFeared() ||
		IsMezzed() ||
		(GetAppearance() == eaDead)){
		return;
	}
	//send item animation, also does the throw animation
	SendItemAnimation(GetTarget(), item, THROWING);

	
	// Hit?
	if (!GetTarget()->CheckHitChance(this, THROWING, 13)) {
		mlog(COMBAT__RANGED, "Ranged attack missed %s.", GetTarget()->GetName());
		GetTarget()->Damage(this, 0, SPELL_UNKNOWN, THROWING, item->ItemType);
	} else {
		mlog(COMBAT__RANGED, "Throwing attack hit %s.", GetTarget()->GetName());
		
		sint16 WDmg = GetWeaponDamage(GetTarget(), item);

		if(WDmg > 0)
		{
			sint32 TotalDmg = 0;
			int minDmg = 1;
			uint16 MaxDmg = GetThrownDamage(WDmg, TotalDmg, minDmg);

			mlog(COMBAT__RANGED, "Item DMG %d. Max Damage %d. Hit for damage %d", WDmg, MaxDmg, TotalDmg);

			GetTarget()->MeleeMitigation(this, TotalDmg, minDmg);
			ApplyMeleeDamageBonus(THROWING, TotalDmg);
			TryCriticalHit(GetTarget(), THROWING, item->ItemType, TotalDmg);

			if(TotalDmg > 0) {
				sint32 hate = (2*WDmg);
				GetTarget()->AddToHateList(this, hate, 0, false);
			}

			GetTarget()->Damage(this, TotalDmg, SPELL_UNKNOWN, THROWING, item->ItemType);
		}
		else
			GetTarget()->Damage(this, -5, SPELL_UNKNOWN, THROWING, item->ItemType);
	}
	
	if(GetTarget() && (GetTarget()->GetHP() > -10))
		TryWeaponProc(RangeWeapon, GetTarget());
	
	//consume ammo
	DeleteItemInInventory(ammo_slot, 1, true);
	CheckIncreaseSkill(THROWING, GetTarget());

	//break invis when you attack
	if(invisible) {
		mlog(COMBAT__ATTACKS, "Removing invisibility due to melee attack.");
		BuffFadeByEffect(SE_Invisibility);
		BuffFadeByEffect(SE_Invisibility2);
		invisible = false;
	}
	if(invisible_undead) {
		mlog(COMBAT__ATTACKS, "Removing invisibility vs. undead due to melee attack.");
		BuffFadeByEffect(SE_InvisVsUndead);
		BuffFadeByEffect(SE_InvisVsUndead2);
		invisible_undead = false;
	}
	if(invisible_animals){
		mlog(COMBAT__ATTACKS, "Removing invisibility vs. animals due to melee attack.");
		BuffFadeByEffect(SE_InvisVsAnimals);
		invisible_animals = false;
	}

	if(hidden || improved_hidden){
		BreakMobHide();
	}
}

void Mob::SendItemAnimation(Mob *to, const Item_Struct *item, SkillType skillInUse) {
	EQApplicationPacket *outapp = new EQApplicationPacket(OP_SomeItemPacketMaybe, sizeof(Arrow_Struct));
	Arrow_Struct *as = (Arrow_Struct *) outapp->pBuffer;
	as->type = 1;
	as->src_x = GetX();
	as->src_y = GetY();
	as->src_z = GetZ();
	as->source_id = GetID();
	as->target_id = to->GetID();
	as->item_id = item->ID;

	as->item_type = item->ItemType;
	as->skill = (uint8)skillInUse;

	strn0cpy(as->model_name, item->IDFile, 16);
	

	/*
		The angular field affects how the object flies towards the target.
		A low angular (10) makes it circle the target widely, where a high 
		angular (20000) makes it go straight at them.

		The tilt field causes the object to be tilted flying through the air
		and also seems to have an effect on how it behaves when circling the
		target based on the angular field.

		Arc causes the object to form an arc in motion. A value too high will
	*/
	as->velocity = 4.0;

	//these angle and tilt used together seem to make the arrow/knife throw as straight as I can make it

	as->launch_angle = CalculateHeadingToTarget(to->GetX(), to->GetY()) * 2;
	as->tilt = 125;
	as->arc = 50;
	
	
	//fill in some unknowns, we dont know their meaning yet
	//neither of these seem to change the behavior any
	as->unknown088 = 125;
	as->unknown092 = 16;
	
	entity_list.QueueCloseClients(this, outapp);
	safe_delete(outapp);
}

void Mob::ProjectileAnimation(Mob* to, int16 item_id, bool IsArrow, float speed, float angle, float tilt, float arc) {

	const Item_Struct* item = NULL;
	int8 item_type = 0;

	if(!item_id) {
		item = database.GetItem(8005);   // Arrow will be default
	}
	else {
		item = database.GetItem(item_id);   // Use the item input into the command
	}

	if(!item) {
		return;
	}
	if(IsArrow) {
		item_type = 27;
	}
	if(!item_type) {
		item_type = item->ItemType;
	}
	if(!speed) {
		speed = 4.0;
	}
	if(!angle) {
		angle = CalculateHeadingToTarget(to->GetX(), to->GetY()) * 2;
	}
	if(!tilt) {
		tilt = 125;
	}
	if(!arc) {
		arc = 50;
	}


	// See SendItemAnimation() for some notes on this struct
	EQApplicationPacket *outapp = new EQApplicationPacket(OP_SomeItemPacketMaybe, sizeof(Arrow_Struct));
	Arrow_Struct *as = (Arrow_Struct *) outapp->pBuffer;
	as->type = 1;
	as->src_x = GetX();
	as->src_y = GetY();
	as->src_z = GetZ();
	as->source_id = GetID();
	as->target_id = to->GetID();
	as->item_id = item->ID;
	as->item_type = item_type;
	as->skill = 0;	// Doesn't seem to have any effect
	strn0cpy(as->model_name, item->IDFile, 16);
	as->velocity = speed;
	as->launch_angle = angle;
	as->tilt = tilt;
	as->arc = arc;
	as->unknown088 = 125;
	as->unknown092 = 16;
	
	entity_list.QueueCloseClients(this, outapp);
	safe_delete(outapp);

}


void NPC::DoClassAttacks(Mob *target) {
	if(target == NULL)
		return;	//gotta have a target for all these
	
	bool taunt_time = taunt_timer.Check();
	bool ca_time = classattack_timer.Check(false);
	bool ka_time = knightattack_timer.Check(false);

	//only check attack allowed if we are going to do something
	if((taunt_time || ca_time || ka_time) && !IsAttackAllowed(target))
		return;

	if(ka_time){
		int knightreuse = 1000; //lets give it a small cooldown actually.
		switch(GetClass()){
			case SHADOWKNIGHT: case SHADOWKNIGHTGM:{
				CastSpell(SPELL_NPC_HARM_TOUCH, target->GetID());
				knightreuse = HarmTouchReuseTime * 1000;
				break;
			}
			case PALADIN: case PALADINGM:{
				if(GetHPRatio() < 20) {
					CastSpell(SPELL_LAY_ON_HANDS, GetID());
					knightreuse = LayOnHandsReuseTime * 1000;
				} else {
					knightreuse = 2000; //Check again in two seconds.
				}
				break;
			}
		}
		knightattack_timer.Start(knightreuse); 
	}
	
	//general stuff, for all classes....
	//only gets used when their primary ability get used too
	if (taunting && HasOwner() && target->IsNPC() && target->GetBodyType() != BT_Undead && taunt_time) {
		Taunt(target->CastToNPC(), false);
	}
	
	if(!ca_time)
		return;
	
	float HasteModifier = 0;
	if(GetHaste() > 0)
		HasteModifier = 10000 / (100 + GetHaste());
	else if(GetHaste() < 0)
		HasteModifier = (100 - GetHaste());
	else
		HasteModifier = 100;

	int level = GetLevel();
	int reuse = TauntReuseTime * 1000;	//make this very long since if they dont use it once, they prolly never will
	bool did_attack = false;
	//class specific stuff...
	switch(GetClass()) {
		case ROGUE: case ROGUEGM:
			if(level >= 10) {
				TryBackstab(target);
				reuse = BackstabReuseTime * 1000;
				did_attack = true;
			}
			break;
		case MONK: case MONKGM: {
			int8 satype = KICK;
			if(level > 29) {
				satype = FLYING_KICK;
			} else if(level > 24) {
				satype = DRAGON_PUNCH;
			} else if(level > 19) {
				satype = EAGLE_STRIKE;
			} else if(level > 9) {
				satype = TIGER_CLAW;
			} else if(level > 4) {
				satype = ROUND_KICK;
			}
			reuse = MonkSpecialAttack(target, satype);

			reuse *= 1000;
			did_attack = true;
			break;
		}
		case WARRIOR: case WARRIORGM:{
			if(level >= RuleI(Combat, NPCBashKickLevel)){
				if(MakeRandomInt(0, 100) > 25) //tested on live, warrior mobs both kick and bash, kick about 75% of the time, casting doesn't seem to make a difference.
				{
					DoAnim(animKick);
					sint32 dmg = 0;

					if(GetWeaponDamage(target, (const Item_Struct*)NULL) <= 0){
						dmg = -5;
					}
					else{
						if(target->CheckHitChance(this, KICK, 0)) {
							if(RuleB(Combat, UseIntervalAC))
								dmg = GetKickDamage();
							else
								dmg = MakeRandomInt(1, GetKickDamage());

						}
					}

					if ( IsPet() && IsNPC() )
						dmg /= 2;

					DoSpecialAttackDamage(target, KICK,0, 0, dmg);
					reuse = KickReuseTime * 1000;
					did_attack = true;
				}
				else
				{
					DoAnim(animTailRake);
					sint32 dmg = 0;

					if(GetWeaponDamage(target, (const Item_Struct*)NULL) <= 0){
						dmg = -5;
					}
					else{
						if(target->CheckHitChance(this, BASH, 0)) {
							if(RuleB(Combat, UseIntervalAC))
								dmg = GetBashDamage();
							else
								dmg = MakeRandomInt(1, GetBashDamage());
						}
					}

					DoSpecialAttackDamage(target, BASH,0, 0, dmg);
					reuse = BashReuseTime * 1000;
					did_attack = true;
				}
			}
			break;
		}
		case BERSERKER: case BERSERKERGM:
		{
			int32 num_attacks = 1 + (GetSkill(FRENZY) / 100);
			while(num_attacks > 0 && target) 
			{
				if(Attack(target))
					num_attacks--;
				else
					num_attacks = 0;
			}
			reuse = FrenzyReuseTime * 1000;
			did_attack = true;
			break;
		}
		case RANGER: case RANGERGM:
		case BEASTLORD: case BEASTLORDGM: {
			//kick
			if(level >= RuleI(Combat, NPCBashKickLevel)){
				DoAnim(animKick);
				sint32 dmg = 0;

				if(GetWeaponDamage(target, (const Item_Struct*)NULL) <= 0){
					dmg = -5;
				}
				else{
					if(target->CheckHitChance(this, KICK, 0)) {
						if(RuleB(Combat, UseIntervalAC))
							dmg = GetKickDamage();
						else
							dmg = MakeRandomInt(1, GetKickDamage());
					}
				}

				DoSpecialAttackDamage(target, KICK,0, 0, dmg);
				reuse = KickReuseTime * 1000;
				did_attack = true;
			}
			break;
		}
		case CLERIC: case CLERICGM: //clerics can bash too.
		case SHADOWKNIGHT: case SHADOWKNIGHTGM:
		case PALADIN: case PALADINGM:
		{
			if(level >= RuleI(Combat, NPCBashKickLevel)){
				DoAnim(animTailRake);
				sint32 dmg = 0;

				if(GetWeaponDamage(target, (const Item_Struct*)NULL) <= 0){
					dmg = -5;
				}
				else{
					if(target->CheckHitChance(this, BASH, 0)) {
						if(RuleB(Combat, UseIntervalAC))
							dmg = GetBashDamage();
						else
							dmg = MakeRandomInt(1, GetBashDamage());
					}
				}

				DoSpecialAttackDamage(target, BASH,0, 0, dmg);
				reuse = BashReuseTime * 1000;
				did_attack = true;
			}
			break;
		}
	}
	
	classattack_timer.Start(reuse*HasteModifier/100);
}

void Client::DoClassAttacks(Mob *ca_target)
{
	if(!ca_target)
		return;

	if( ( spellend_timer.Enabled() && GetClass() != BARD ) 
		|| IsFeared() || IsStunned() || IsMezzed() || DivineAura() || dead)
		return;
	
	if(!IsAttackAllowed(ca_target))
		return;
	
	//check range for all these abilities, they are all close combat stuff
	if(!CombatRange(ca_target))
	{
		return;
	}
	
	if(!p_timers.Expired(&database, pTimerCombatAbility, false)) {
		return;
	}
	
	int ReuseTime = 0;
	int ClientHaste = GetHaste();
	int HasteMod = 0;

	if(ClientHaste >= 0){
		HasteMod = (10000/(100+ClientHaste)); //+100% haste = 2x as many attacks
	}
	else{
		HasteMod = (100-ClientHaste); //-100% haste = 1/2 as many attacks
	}
	sint32 dmg = 0;

	sint32 skill_to_use = -1;
	
	switch(GetClass())
	{
	case WARRIOR:
	case RANGER:
	case BEASTLORD:
		skill_to_use = KICK;
		break;
	case BERSERKER:
		skill_to_use = FRENZY;
		break;
	case SHADOWKNIGHT:
	case PALADIN:
		skill_to_use = BASH;
		break;
	case MONK:
		if(GetLevel() >= 30)
		{
			skill_to_use = FLYING_KICK;
		}
		else if(GetLevel() >= 25)
		{
			skill_to_use = DRAGON_PUNCH;
		}
		else if(GetLevel() >= 20)
		{
			skill_to_use = EAGLE_STRIKE;
		}
		else if(GetLevel() >= 10)
		{
			skill_to_use = TIGER_CLAW;
		}
		else if(GetLevel() >= 5)
		{
			skill_to_use = ROUND_KICK;
		}
		else
		{
			skill_to_use = KICK;
		}
		break;
	case ROGUE:
		skill_to_use = BACKSTAB;
		break;
	}
	
	if(skill_to_use == -1)
		return;
	
	if(skill_to_use == BASH) 
	{
		if (ca_target!=this) 
		{
			DoAnim(animTailRake);

			if(GetWeaponDamage(ca_target, GetInv().GetItem(SLOT_SECONDARY)) <= 0 &&
				GetWeaponDamage(ca_target, GetInv().GetItem(SLOT_SHOULDER)) <= 0){
				dmg = -5;
			}
			else{
				if(!ca_target->CheckHitChance(this, BASH, 0)) {
					dmg = 0;
				}
				else{
					if(RuleB(Combat, UseIntervalAC))
						dmg = GetBashDamage();
					else
						dmg = MakeRandomInt(1, GetBashDamage());

				}
			}

			DoSpecialAttackDamage(ca_target, BASH,0, 0, dmg);
			ReuseTime = BashReuseTime-1;
			ReuseTime = (ReuseTime*HasteMod)/100;
			if(ReuseTime > 0)
			{
				p_timers.Start(pTimerCombatAbility, ReuseTime);
			}
		}
		return;
	}

	if(skill_to_use == FRENZY)
	{
		int dmg = 1 + (GetSkill(FRENZY) / 100);

		switch (GetAA(aaBlurofAxes)) {
		case 1:
			dmg *= 1.15;
			break;
		case 2:
			dmg *= 1.30;
			break;
		case 3:
			dmg *= 1.50;
			break;
		}

		switch (GetAA(aaViciousFrenzy)) 
		{
		case 1:
			dmg *= 1.05;
			break;
		case 2:
			dmg *= 1.10;
			break;
		case 3:
			dmg *= 1.15;
			break;
		case 4:
			dmg *= 1.20;
			break;
		case 5:
			dmg *= 1.25;
			break;
		}

		while(dmg > 0 && ca_target) {
			if(Attack(ca_target))
				dmg--;
			else
				dmg = 0;
		}

		ReuseTime = FrenzyReuseTime-1;
		ReuseTime = (ReuseTime*HasteMod)/100;
		if(ReuseTime > 0) {
			p_timers.Start(pTimerCombatAbility, ReuseTime);
		}
		return;
	}

	if(skill_to_use == KICK)
	{
		if(ca_target!=this)
		{
			DoAnim(animKick);

			if(GetWeaponDamage(ca_target, GetInv().GetItem(SLOT_FEET)) <= 0){
				dmg = -5;
			}
			else{
				if(!ca_target->CheckHitChance(this, KICK, 0)) {
					dmg = 0;
				}
				else{
					if(RuleB(Combat, UseIntervalAC))
						dmg = GetKickDamage();
					else
						dmg = MakeRandomInt(1, GetKickDamage());
				}
			}

			DoSpecialAttackDamage(ca_target, KICK,0, 0, dmg);
			ReuseTime = KickReuseTime-1;
		}
	}

	if(skill_to_use == FLYING_KICK ||
		skill_to_use == DRAGON_PUNCH ||
		skill_to_use == EAGLE_STRIKE ||
		skill_to_use == TIGER_CLAW ||
		skill_to_use == ROUND_KICK)
	{
		ReuseTime = MonkSpecialAttack(ca_target, skill_to_use) - 1;

		int specl = GetAA(aaTechniqueofMasterWu) * 20;
		if(specl == 100 || specl > MakeRandomInt(0,100)) {
			ReuseTime = MonkSpecialAttack(ca_target, skill_to_use) - 1;
			if(20 > MakeRandomInt(0,100)) {
				ReuseTime = MonkSpecialAttack(ca_target, skill_to_use) - 1;
			}
		}
	}
	
	if(skill_to_use == BACKSTAB)
	{
		TryBackstab(ca_target);
		ReuseTime = BackstabReuseTime-1;
	}
	
	ReuseTime = (ReuseTime*HasteMod)/100;
	if(ReuseTime > 0)
	{
		p_timers.Start(pTimerCombatAbility, ReuseTime);
	}	
}
void Mob::Taunt(NPC* who, bool always_succeed) {
	if (who == NULL)
		return;
	
	if(DivineAura())
		return;
	
	if ( !IsAttackAllowed(who) )
		return;

	if(!CombatRange(who))
		return;

	if(!always_succeed && IsClient())
		CastToClient()->CheckIncreaseSkill(TAUNT, who, 10);
	
	int level = GetLevel();
	
	Mob *hate_top = who->GetHateMost();
	
	// Check to see if we're already at the top of the target's hate list
	// a mob will not be taunted if its target's health is below 20%
	if ((hate_top != this) 
	&& (who->GetLevel() < level) 
	&& (hate_top == NULL || hate_top && hate_top->GetHPRatio() >= 20) ) {
		sint32 newhate, tauntvalue;

		float tauntchance;
		if(always_succeed) {
			tauntchance = 101;
		} else {
			
			// no idea how taunt success is actually calculated
			// TODO: chance for level 50+ mobs should be lower
			int level_difference = level - who->GetLevel();
			if (level_difference <= 5) {
				tauntchance = 25.0;	// minimum
				tauntchance += tauntchance * (float)GetSkill(TAUNT) / 200.0;	// skill modifier
				if (tauntchance > 65.0)
					tauntchance = 65.0;
			}
			else if (level_difference <= 10) {
				tauntchance = 30.0;	// minimum
				tauntchance += tauntchance * (float)GetSkill(TAUNT) / 200.0;	// skill modifier
				if (tauntchance > 85.0)
					tauntchance = 85.0;
			}
			else if (level_difference <= 15) {
				tauntchance = 40.0;	// minimum
				tauntchance += tauntchance * (float)GetSkill(TAUNT) / 200.0;	// skill modifier
				if (tauntchance > 90.0)
					tauntchance = 90.0;
			}
			else {
				tauntchance = 50.0;	// minimum
				tauntchance += tauntchance * (float)GetSkill(TAUNT) / 200.0;	// skill modifier
				if (tauntchance > 95.0)
					tauntchance = 95.0;
			}
		}
		if (tauntchance > MakeRandomFloat(0, 100)) {
			// this is the max additional hate added per succesfull taunt
			tauntvalue = (MakeRandomInt(4, 6) * level);
			//tauntvalue = (sint32) ((float)level * 10.0 * (float)rand()/(float)RAND_MAX + 1);
			// new hate: find diff of player's hate and whoever's at top of list, add that plus tauntvalue to players hate
			newhate = who->GetNPCHate(hate_top) - who->GetNPCHate(this) + tauntvalue;
			// add the hate
			who->CastToNPC()->AddToHateList(this, newhate);
		}
		else{
			//generate at least some hate reguardless of the outcome.
			sint32 hate = 0;
			// Red69 Kings & Bandits - warrior taunt modification
			switch(who->GetClass())
			{
			case WARRIOR:
				{
					sint32 incRnd = 0;
					if ( who->GetLevel() > 40 )
						incRnd += 1;

					if ( IsClient() )
						hate = MakeRandomInt(3+incRnd,5+incRnd)*level;
					else
						hate = MakeRandomInt(2,4)*level;

					break;
				}
			default:
				{
					hate = MakeRandomInt(2,3)*level;
					break;
				}
			}
			who->CastToNPC()->AddToHateList(this, hate);
		}
	}
		
	//generate at least some hate reguardless of the outcome.
	who->CastToNPC()->AddToHateList(this, (MakeRandomInt(2, 4)*level));
}

void Mob::InstillDoubt(Mob *who) {
	//make sure we can use this skill
	/*int skill = GetSkill(INTIMIDATION);*/	//unused
	
	//make sure our target is an NPC
	if(!who || !who->IsNPC())
		return;
	
	if(DivineAura())
		return;
	
	//range check
	if(!CombatRange(who))
		return;
	
	if(IsClient())
	{
		CastToClient()->CheckIncreaseSkill(INTIMIDATION, who, 10);
	}

	//I think this formula needs work
	int value = 0;
	
	//user's bonus
	value += GetSkill(INTIMIDATION) + GetCHA()/4;
	
	//target's counters
	value -= target->GetLevel()*4 + who->GetWIS()/4;
	
	if (MakeRandomInt(0,99) < value) {
		//temporary hack...
		//cast fear on them... should prolly be a different spell
		//and should be un-resistable.
		SpellOnTarget(229, who, false, true, -2000);
		//is there a success message?
	} else {
		Message_StringID(4,NOT_SCARING);
		//Idea from WR:
		/* if (target->IsNPC() && MakeRandomInt(0,99) < 10 ) {
			entity_list.MessageClose(target, false, 50, MT_Rampage, "%s lashes out in anger!",target->GetName());
			//should we actually do this? and the range is completely made up, unconfirmed
			entity_list.AEAttack(target, 50);
		}*/
	}
}

bool Mob::TryHeadShot(Mob* defender, SkillType skillInUse, uint8 itemType) {
	bool Result = false;

	if(defender && skillInUse == ARCHERY && IsClient()) {
		if(CastToClient()->GetAA(aaHeadshot) && defender->GetBodyType() == BT_Humanoid) {
			if((GetLevelCon(GetLevel(), defender->GetLevel()) == CON_LIGHTBLUE || GetLevelCon(GetLevel(), defender->GetLevel()) == CON_GREEN) && defender->GetLevel() <= 60 && !defender->IsClient()) {
				// WildcardX: These chance formula's below are arbitrary. If someone has a better formula that is more
				// consistent with live, feel free to update these.
				int AttackerChance = 20 + ((GetLevel() - 51) / 2) + (itembonuses.DEX / 10);
				int DefenderChance = MakeRandomInt(0, 100);
				if(AttackerChance > DefenderChance) {
					mlog(COMBAT__ATTACKS, "Landed a headshot: Attacker chance was %f and Defender chance was %f.", AttackerChance, DefenderChance);
					// WildcardX: At the time I wrote this, there wasnt a string id for something like HEADSHOT_BLOW
					//entity_list.MessageClose_StringID(this, false, 200, MT_CritMelee, FINISHING_BLOW, GetName());
					entity_list.MessageClose(this, false, 200, MT_CritMelee, "%s has scored a HEADSHOT!", GetName());
					defender->Damage(this, 32000, SPELL_UNKNOWN, skillInUse, itemType);
					Result = true;
				}
				else {
					mlog(COMBAT__ATTACKS, "FAILED a headshot: Attacker chance was %f and Defender chance was %f.", AttackerChance, DefenderChance);
				}
			}
		}
	}

	return Result;
}
