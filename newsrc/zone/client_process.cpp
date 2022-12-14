/*  EQEMu:  Everquest Server Emulator
	Copyright (C) 2001-2003  EQEMu Development Team (http://eqemulator.net)
	
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
	
	client_process.cpp:
	Handles client login sequence and packets sent from client to zone
*/
#include "../common/debug.h"
#include <iostream>
#include <iomanip>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <zlib.h>
#include <assert.h>

#ifdef WIN32
	#include <windows.h>
	#include <winsock.h>
	#define snprintf	_snprintf
#if (_MSC_VER < 1500)
	#define vsnprintf	_vsnprintf
#endif
	#define strncasecmp	_strnicmp
	#define strcasecmp  _stricmp
#else
	#include <pthread.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <unistd.h>
#endif

#include "masterentity.h"
#include "zonedb.h"
#include "../common/packet_functions.h"
#include "../common/packet_dump.h"
#include "worldserver.h"
#include "../common/packet_dump_file.h"
#include "../common/MiscFunctions.h"
#include "spdat.h"
#include "petitions.h"
#include "NpcAI.h"
#include "../common/skills.h"
#include "forage.h"
#include "zone.h"
#include "event_codes.h"
#include "faction.h"
#include "../common/crc32.h"
#include "../common/rulesys.h"
#include "StringIDs.h"
#include "map.h"
#include <string>
#include "../KingsBandits/KingsBandits.h"

using namespace std;


extern Zone* zone;
extern volatile bool ZoneLoaded;
extern WorldServer worldserver;
#if !defined(NEW_LoadSPDat) && !defined(DB_LoadSPDat)
	extern SPDat_Spell_Struct spells[SPDAT_RECORDS];
#endif
extern bool spells_loaded;
extern PetitionList petition_list;
extern EntityList entity_list;

bool Client::Process() {
	_ZP(Client_Process);
	adverrorinfo = 1;
	bool ret = true;

	if(Connected() || IsLD())
	{
		
		// LD handling
		if ( IsLD() && RuleB(Character,ZoneInLinkdead) )
		{
			switch(conn_state)
			{
			case ZoneInfoSent:
			case ClientSpawnRequested:
			case ZoneContentsSent:
			case PlayerProfileLoaded:
				{
					EQApplicationPacket *outapp = new EQApplicationPacket();
					CreateSpawnPacket(outapp);
					outapp->priority = 6;
					if (!GetHideMe()) entity_list.QueueClients(this, outapp, true);
					safe_delete(outapp);
					
					if ( RuleI(Character,ZoneInLinkdeadTimer) > 0 )
						linkdead_timer.Start(RuleI(Character,ZoneInLinkdeadTimer), true);
					
					if ( RuleB(Character, ZoneInLDShowFlag) )
						SendAppearancePacket(AT_Linkdead, 1);
					
					if(GetPVP())	//force a PVP update until we fix the spawn struct
							RefreshPVPFlag();
					LogFile->write(EQEMuLog::Status, "Client %s being instantiated in world due to being linkdead prior to making it in zone.", GetName());

						
					uint32 tmpInfamy = database.GetCharacterStatsUnsignedInt(CharacterID(),"infamy");
					infamyLevel = tmpInfamy;
					cout << "Infamy set to: " << infamyLevel << " for charid: " << CharacterID() << endl;
	
					uint32 tmpBounty = database.GetCharacterStatsUnsignedInt(CharacterID(),"bountyset");
					bountySet = tmpBounty;

					// Kings & Bandits - MakePet cross zone.
					// If server is Red69 (1) then we do not want pets cross zone.
					// all other servers it will work
					if (m_epp.pet_id > 1 && !GetPet() && m_epp.pet_id <= SPDAT_RECORDS)
					{
						MakePet(m_epp.pet_id, spells[m_epp.pet_id].teleport_zone, m_epp.pet_name, m_epp.pet_power);
						if (GetPet() && GetPet()->IsNPC()) {
							NPC *pet = GetPet()->CastToNPC();
							pet->SetPetState(m_epp.pet_buffs, m_epp.pet_items);
							pet->CalcBonuses();
							if ( zone->GetInstanceID() < 10000 )
							{
								int16 halfHP = pet->GetMaxHP()/2;
								if ( m_epp.pet_hp > halfHP )
									m_epp.pet_hp = halfHP;
							}

							pet->SetHP(m_epp.pet_hp);
							pet->SetMana(m_epp.pet_mana);
						}
						m_epp.pet_id = 0;
					}

					hpupdate_timer.Start();
					position_timer.Start();
					autosave_timer.Start();

					ResetUnderAttack(); // KaB / Red69 / Zek - Reset the under attack timer when we login

					SetDuelTarget(0);
					SetDueling(false);

					if(IsInAGuild()){
						SendAppearancePacket(AT_GuildID, GuildID(), false);
						SendAppearancePacket(AT_GuildRank, GuildRank(), false);
					}
					
					CheckInfamyLevel(true); // check what our infamy state is

					if ( KingsBandits::PlayerOwnsCity(CharacterID()) || bountySet )
					{
						SetBounty(true);
						UpdateInfamyLevel(999999);
					}

					// Kings & Bandits - Client::CompleteConnect update.  Once here we should update the db to notify the user is back in a zone.
					database.UpdateCharacterStats(CharacterID(), GetPP().STR, GetPP().STA, GetPP().DEX, GetPP().AGI, GetPP().INT, GetPP().WIS, GetPP().CHA, 
						GetPP().cur_hp,GetMaxHP(), GetPP().mana, GetMaxMana(), GetPP().endurance, GetPP().endurance, GetPP().timePlayedMin, infamyLevel, bountySet, 2);

					client_data_loaded = true;
					linkdead_zonein = true;
					conn_state = ClientConnectFinished;
					SendPosUpdate(0);
					break;
				}
			}
		}
		// end LD handling

		if ( conn_state == ClientConnectFinished && cacheClientEntities.size() > 0 )
		{			
			std::list<uint32>::iterator itr,tmp;
			EQApplicationPacket *spawnapp = NULL;
			for(itr = cacheClientEntities.begin(); itr != cacheClientEntities.end(); )
			{
				uint32 entid = *itr;
				Mob* findMob = entity_list.GetMob(entid);

				if ( findMob && findMob->IsClient() && (!findMob->CastToClient()->GetHideMe()) && (!findMob->CastToClient()->isDespawningFromZone()) && (!findMob->CastToClient()->IsHoveringForRespawn()) )
				{
					Client* clientSpawn = findMob->CastToClient();
					if ( !clientSpawn->InZone() )
					{
						// they aren't in zone yet, wait
						itr++;
					}
					else
					{
						LogFile->write(EQEMuLog::Status, "%s: SpawnPlayerCache for client %s (%i), was sent successfully.", GetName(), clientSpawn->GetName(), entid);
						// they are in zone, send the spawn info to the client
						tmp = itr;
						tmp++;

						if ( !spawnapp )
							spawnapp = new EQApplicationPacket();

						clientSpawn->CreateSpawnPacket(spawnapp,this->CastToMob(),true);
						
						spawnapp->priority = 6;
						QueuePacket(spawnapp);

						cacheClientEntities.erase(itr);

						itr = tmp;
					}
				}
				else
				{
					LogFile->write(EQEMuLog::Status, "%s: SpawnPlayerCache for client could not be found by entity id (%i), removed from cache.", GetName(), entid);
					// our entity id target is gone, remove it
						tmp = itr;
						tmp++;
						cacheClientEntities.erase(itr);
						itr = tmp;
				}
			}
			safe_delete(spawnapp);
		}

		if ( checktellqueue_timer.Check() )
		{
			if ( zoneLockoutID > 0 )
			{
				time_t now;
				time(&now);
	
				uint32 time = (uint32)now;
				sint64 secondsCalc = (((sint64)GetPP().PVPLastDeath.Time + RuleI(PVPCombat, ZoneDeathDelay)) - (sint64)time);

				sint64 timeLeft = secondsCalc / 60;
				sint64 seconds = secondsCalc % 60;
				if ( secondsCalc > 0 )
					Message(13, "You died recently in %s and must wait to return, there is roughly %i minute(s) and %i second(s) remaining.  Enter #ignoretimer to stop these updates.", database.GetZoneName(zoneLockoutID), timeLeft, seconds);
				else
					zoneLockoutID = 0;
			}

			LoadMessageQueue(false);
		}
		if ( clientinzone_timer.Check() )
		{
			clientinzone_timer.Disable();

			if ( conn_state < ClientConnectFinished )
			{
				// cause buffs to trigger as soon as we zone in
				tic_timer.Trigger();
				int32 cur_time = Timer::GetCurrentTime();
				LogFile->write(EQEMuLog::Status, "(%i) Client %s charid %i is connected in zone.", cur_time, GetName(),CharacterID());
				zoneinpacket_timer.Start(3000,true);
				conn_state = ClientConnectFinished;
				SendPosUpdate(0);
			}
		}
        // try to send all packets that weren't sent before
		if(conn_state == ClientConnectFinished && !IsLD() && zoneinpacket_timer.Check()){
			LinkedListIterator<BulkZoneSpawnPacket*> iterator(BzspQueue);
			int count = 0;
			for(iterator.Reset(); iterator.MoreElements();count++)
			{
				BulkZoneSpawnPacket* cur = iterator.GetData();
				iterator.RemoveCurrent();
				LogFile->write(EQEMuLog::Status, "Client %s charid %i packet %i sent of %i.", GetName(),CharacterID(),count,BzspQueue.Count());
				if ( count > RuleI(Combat,MaxQueuePacketsSent) )
					break;
			}
			SendAllPackets();
		}
		
#ifdef PACKET_UPDATE_MANAGER
		update_manager.Process();
#endif

		if(adventure_request_timer)
		{
			if(adventure_request_timer->Check())
			{
				safe_delete(adventure_request_timer);
			}
		}

		if(adventure_create_timer)
		{
			if(adventure_create_timer->Check())
			{
				safe_delete(adventure_create_timer);
			}
		}

		if(adventure_leave_timer)
		{
			if(adventure_leave_timer->Check())
			{
				safe_delete(adventure_leave_timer);
			}
		}

		if(adventure_door_timer)
		{
			if(adventure_door_timer->Check())
			{
				safe_delete(adventure_door_timer);
			}
		}

		if(adventure_stats_timer)
		{
			if(adventure_stats_timer->Check())
			{
				safe_delete(adventure_stats_timer);
			}
		}

		if(adventure_leaderboard_timer)
		{
			if(adventure_leaderboard_timer->Check())
			{
				safe_delete(adventure_leaderboard_timer);
			}
		}

		// check status of infamy
		CheckInfamyLevel();

		if ( ReplayEnabled && ReplayTimer.Check() )
		{
			RecordReplayFrame();
		}

		if ( Replaying && ReplayingTimer.Check() )
		{
			PlayFrame();
		}

		if(dead)
		{
			SetHP(-100);
			if(RespawnFromHoverTimer.Check())
				HandleRespawnFromHover(0);
		}
		
		if(IsTracking() && (GetClientVersion() >= EQClientSoD) && TrackingTimer.Check())
			DoTracking();

		if(hpupdate_timer.Check())
		{
			SendHPUpdate();	

			// 10/17/2012 - KaB/Red69/Zek - Image: Zone queue events to avoid getting a client stuck (eg they are summoned then attempt to gate)
			// attached to the hp update timer to add a bit of a delay without requiring another timer
			std::list<QueuedZoneEvent>::iterator itr;
			for(itr = zone_event_list.begin(); itr != zone_event_list.end(); itr++)
			{
				ProcessMovePC(itr->zoneID,itr->instID,itr->x,itr->y,itr->z,itr->heading,itr->ignorerestrictions,itr->zonemode);
				zone_event_list.erase(itr);
				break;
			}
		}

		if(mana_timer.Check())
			SendManaUpdatePacket();
		if(dead && dead_timer.Check()) {
			if ( zone_mode == GatePVPDeath && RuleB(PVPCombat,ForceNexusHubOnDeath) )
			{
				m_pp.zone_id = 152;
				database.MoveCharacterToZone(GetName(),database.GetZoneName(m_pp.zone_id));
				m_pp.zoneInstance = 0;
				m_pp.x = 0;
				m_pp.y = 0;
				m_pp.z = -20;
			}
			else
			{
				database.MoveCharacterToZone(GetName(),database.GetZoneName(m_pp.binds[0].zoneId));
				m_pp.zone_id = m_pp.binds[0].zoneId;
				m_pp.zoneInstance = 0;
				m_pp.x = m_pp.binds[0].x;
				m_pp.y = m_pp.binds[0].y;
				m_pp.z = m_pp.binds[0].z;
			}
				
			Save();
			
			Group *mygroup = GetGroup();
			if (mygroup)	// && zone.GetZoneID() != m_pp.binds[0].zoneId
			{
				entity_list.MessageGroup(this,true,15,"%s died.", GetName());
				mygroup->MemberZoned(this);
			}
			Raid *myraid = entity_list.GetRaidByClient(this);
			if (myraid)
			{
				myraid->MemberZoned(this);
			}
			return(false);
		}
		
		if(charm_update_timer.Check())
		{
			CalcItemScale();
		}

		if(TaskPeriodic_Timer.Check() && taskstate)
			taskstate->TaskPeriodicChecks(this);

		if(linkdead_timer.Check()){

			if ( linkdead_timer.GetDuration() == LinkDeadDelayNotAttacked && 
				 !underattack_timer.Check() )
			{
				int diff = RuleI(Zone,ClientLinkdeadMS)-LinkDeadDelayNotAttacked;
				if ( diff < 1000 )
					diff += 1000;

				linkdead_timer.Start(diff, true);
			}
			else
			{
				LeaveGroup();
				Save(2,false);
				Raid *myraid = entity_list.GetRaidByClient(this);
				if (myraid)
				{
					myraid->MemberZoned(this);
				}
				//AI_Start(CLIENT_LD_TIMEOUT);
				OnDisconnect(true);
				return false;
			}
		}

		if (camp_timer.Check()) {
			LeaveGroup();
			Save();
			instalog = true;

			// Kings & Bandits - use camp timer to disconnect them a second later after packets handle
			if( GetGM() )
				Disconnect();
		}
		
		if (IsStunned() && stunned_timer.Check()) {
			this->stunned = false;
			this->stunned_timer.Disable();
		}
		
		if(!m_CheatDetectMoved)
		{
			m_TimeSinceLastPositionCheck = Timer::GetCurrentTime();
		}

		if (bardsong_timer.Check() && bardsong != 0) {
			if ( bardsong_timer.GetDuration() == 10 ) // default for resetting variables
					ZeroCastingVars();

			//NOTE: this is kinda a heavy-handed check to make sure the mob still exists before
			//doing the next pulse on them...
			Mob *song_target;
			if(bardsong_target_id == GetID()) {
				song_target = this;
			} else {
				song_target = entity_list.GetMob(bardsong_target_id);
			}
			
			if (song_target == NULL) {
				InterruptSpell(SONG_ENDS_ABRUPTLY, 0x121, bardsong);
				// Red69/LoZ/KaB: fix for bard songs with recast time sep 7 2012
			} else if ( spells[bardsong].recast_time != 0 ) {
				SendSpellBarEnable(bardsong);
				MemorizeSpell(bardsong_slot, bardsong, memSpellSpellbar);
				_StopSong();
				//MemSpell(bardsong, bardsong_slot); // this might be a 'hack' (eg im not sure if this is the right way to go about this) - but it greys the bar for recast
			}
			else {
				if(!ApplyNextBardPulse(bardsong, song_target, bardsong_slot))
					InterruptSpell(SONG_ENDS_ABRUPTLY, 0x121, bardsong);
//				SpellFinished(bardsong, bardsong_target, bardsong_slot, spells[bardsong].mana);
			}
		}
		
		if(IsAIControlled())
			AI_Process();
		
		if (bindwound_timer.Check() && bindwound_target != 0) {
		    BindWound(bindwound_target, false);
		}

		if(KarmaUpdateTimer)
		{
			if(KarmaUpdateTimer->Check(false))
			{
				KarmaUpdateTimer->Start(RuleI(Chat, KarmaUpdateIntervalMS));
				database.UpdateKarma(AccountID(), ++TotalKarma);
			}
		}

		if(qGlobals)
		{
			if(qglobal_purge_timer.Check())
			{
				qGlobals->PurgeExpiredGlobals();
			}
		}
		
		bool may_use_attacks = false;
		/*
			Things which prevent us from attacking:
				- being under AI control, the AI does attacks
				- being dead
				- casting a spell (not sure what the rest is doing, prolly bard)
				- not having a target
				- being stunned or mezzed
				- having used a ranged weapon recently
		*/
		if(auto_attack) {
			if(!IsAIControlled() && !dead
				&& !(spellend_timer.Enabled() && (spells[casting_spell_id].classes[7] < 1 && spells[casting_spell_id].classes[7] > 65)) 
				&& !IsStunned() && !IsFeared() && !IsMezzed() && GetAppearance() != eaDead
				)
				may_use_attacks = true;
			
			if(may_use_attacks && ranged_timer.Enabled()) {
				//if the range timer is enabled, we need to consider it
				if(!ranged_timer.Check(false)) {
					//the ranged timer has not elapsed, cannot attack.
					may_use_attacks = false;
				}
			}
		}

		if(AutoFireEnabled()){
			ItemInst *ranged = GetInv().GetItem(SLOT_RANGE);
			if(ranged)
			{
				if(ranged->GetItem() && ranged->GetItem()->ItemType == ItemTypeBow){
					if(ranged_timer.Check(false)){
						if(GetTarget() && (GetTarget()->IsNPC() || GetTarget()->IsClient())){
							if(!GetTarget()->BehindMob(this, GetTarget()->GetX(), GetTarget()->GetY())){
								if(CheckLosFN(GetTarget())){
									//client has built in los check, but auto fire does not.. done last.
									RangedAttack(GetTarget());
								}
								else
									ranged_timer.Start();
							}
							else
								ranged_timer.Start();
						}
						else
							ranged_timer.Start();
					}
				}
				else if(ranged->GetItem() && (ranged->GetItem()->ItemType == ItemTypeThrowing || ranged->GetItem()->ItemType == ItemTypeThrowingv2)){
					if(ranged_timer.Check(false)){
						if(GetTarget() && (GetTarget()->IsNPC() || GetTarget()->IsClient())){
							if(!GetTarget()->BehindMob(this, GetTarget()->GetX(), GetTarget()->GetY())){
								if(CheckLosFN(GetTarget())){
									//client has built in los check, but auto fire does not.. done last.
									ThrowingAttack(GetTarget());
								}
								else
									ranged_timer.Start();
							}
							else
								ranged_timer.Start();
						}
						else
							ranged_timer.Start();
					}				
				}
			}
		}

		Mob *auto_attack_target = GetTarget();
		if (auto_attack && auto_attack_target != NULL && may_use_attacks && attack_timer.Check()) 
		{
			//check if change
			//only check on primary attack.. sorry offhand you gotta wait!
			if(aa_los_them_mob)
			{
				if(auto_attack_target != aa_los_them_mob ||
					aa_los_me.x != GetX() ||
					aa_los_me.y != GetY() ||
					aa_los_me.z != GetZ() ||
					aa_los_them.x != aa_los_them_mob->GetX() ||
					aa_los_them.y != aa_los_them_mob->GetY() ||
					aa_los_them.z != aa_los_them_mob->GetZ())
				{
					aa_los_them_mob = auto_attack_target;
					aa_los_me.x = GetX();
					aa_los_me.y = GetY();
					aa_los_me.z = GetZ();
					aa_los_them.x = aa_los_them_mob->GetX();
					aa_los_them.y = aa_los_them_mob->GetY();
					aa_los_them.z = aa_los_them_mob->GetZ();
					if(CheckLosFN(auto_attack_target))
						los_status = true;
					else
						los_status = false;
				}
			}
			else
			{
				aa_los_them_mob = auto_attack_target;
				aa_los_me.x = GetX();
				aa_los_me.y = GetY();
				aa_los_me.z = GetZ();
				aa_los_them.x = aa_los_them_mob->GetX();
				aa_los_them.y = aa_los_them_mob->GetY();
				aa_los_them.z = aa_los_them_mob->GetZ();
				if(CheckLosFN(auto_attack_target))
					los_status = true;
				else
					los_status = false;
			}

			if (!CombatRange(auto_attack_target)) 
			{
				Message_StringID(13,TARGET_TOO_FAR);
			}
			else if (auto_attack_target == this) 
			{
				Message_StringID(13,TRY_ATTACKING_SOMEONE);
			}
			else if (!los_status)
			{
				//you can't see your target	
			}
			else if (auto_attack_target->GetHP() > -10) // -10 so we can watch people bleed in PvP
			{
				if(CheckAAEffect(aaEffectRampage))
				{
					entity_list.AEAttack(this, 30);
				} else {
					Attack(auto_attack_target, 13); 	// Kaiyodo - added attacking hand to arguments
				}
				ItemInst *wpn = GetInv().GetItem(SLOT_PRIMARY);
				TryWeaponProc(wpn, auto_attack_target, 13);
				
				bool tripleAttackSuccess = false;
				if( auto_attack_target && CanThisClassDoubleAttack() ) {
					
					CheckIncreaseSkill(DOUBLE_ATTACK, auto_attack_target, -10);
					if(CheckDoubleAttack()) {
						//should we allow rampage on double attack?
						if(CheckAAEffect(aaEffectRampage)) {
							entity_list.AEAttack(this, 30);
						} else {
							Attack(auto_attack_target, 13, false);
						}
					}
					
					//triple attack: rangers, monks, warriors, berserkers over level 60
					if((((GetClass() == MONK || GetClass() == WARRIOR || GetClass() == RANGER || GetClass() == BERSERKER)
						&& GetLevel() >= 60) || SpecAttacks[SPECATK_TRIPLE])
					   && CheckDoubleAttack(true))
					{
						tripleAttackSuccess = true;
						Attack(auto_attack_target, 13, false);
					}
					
					//quad attack, does this belong here??
					if(SpecAttacks[SPECATK_QUAD] && CheckDoubleAttack(true)) 
					{
						Attack(auto_attack_target, 13, false);
					}
				}
				if (auto_attack_target && (GetAA(aaFlurry) > 0 || spellbonuses.FlurryChance > 0 || itembonuses.FlurryChance > 0))
				{
					// Assuming Flurry Chance (X) effects = X%
					// Can any class flurry with tribute?
					// Is flurry supposed to have a chance even without a successful triple attack?
					int32 flurrychance = (itembonuses.FlurryChance + spellbonuses.FlurryChance) * 10; 
					switch (GetAA(aaFlurry)) 
					{
					case 1:
						flurrychance += 10;
						break;
					case 2:
						flurrychance += 25;
						break;
					case 3:
						flurrychance += 50;
						break;
					}

					if(tripleAttackSuccess) {
						tripleAttackSuccess = false;
						switch (GetAA(aaRagingFlurry)) {
						case 1:
							flurrychance += 10;
							break;
						case 2:
							flurrychance += 25;
							break;
						case 3:
							flurrychance += 50;
							break;
						}
					}

					if(MakeRandomInt(0, 999) < flurrychance) 
					{
						Message_StringID(MT_Flurry, 128);
						Attack(auto_attack_target, 13, false);
						Attack(auto_attack_target, 13, false);
					}
				}

				if (GetTarget() && GetAA(aaRapidStrikes))
				{
					int chance_xhit1 = 0;
					int chance_xhit2 = 0;
					switch (GetAA(aaRapidStrikes))
					{
					case 1:
						chance_xhit1 = 10;
						chance_xhit2 = 2;
						break;
					case 2:
						chance_xhit1 = 12;
						chance_xhit2 = 4;
						break;
					case 3:
						chance_xhit1 = 14;
						chance_xhit2 = 6;
						break;
					case 4:
						chance_xhit1 = 16;
						chance_xhit2 = 8;
						break;
					case 5:
						chance_xhit1 = 20;
						chance_xhit2 = 10;
						break;
					}
					if (MakeRandomInt(1,100) < chance_xhit1)
						Attack(GetTarget(), 13, false);
					if (MakeRandomInt(1,100) < chance_xhit2)
						Attack(GetTarget(), 13, false);
				}
				
				if (auto_attack_target && (GetAA(aaPunishingBlade) > 0 || GetAA(aaSpeedoftheKnight) > 0)) {
					ItemInst *wpn = GetInv().GetItem(SLOT_PRIMARY);
					if(wpn){
						if(wpn->GetItem()->ItemType == ItemType2HS || 
							wpn->GetItem()->ItemType == ItemType2HB ||
							wpn->GetItem()->ItemType == ItemType2HPierce )
						{
							int extatk = GetAA(aaPunishingBlade)*5;
							extatk += GetAA(aaSpeedoftheKnight)*5;
							if(MakeRandomInt(0, 100) < extatk)
							{
								Attack(auto_attack_target, 13, false);
							}
						}
					}
				}
			}
		}
		
		if (GetClass() == WARRIOR || GetClass() == BERSERKER) {
			if(!dead && !berserk && this->GetHPRatio() < RuleI(Combat, BerserkHPRatio)) {
				entity_list.MessageClose_StringID(this, false, 200, 0, BERSERK_START, GetName());
				this->berserk = true;
			}
			if (berserk && this->GetHPRatio() > RuleI(Combat, BerserkHPRatio)) {
				entity_list.MessageClose_StringID(this, false, 200, 0, BERSERK_END, GetName());
				this->berserk = false;
			}
		}
		
		if(auto_attack && may_use_attacks && auto_attack_target != NULL
			&& CanThisClassDualWield() && attack_dw_timer.Check()) 
		{	
			// Range check
			if(!CombatRange(auto_attack_target)) {
				Message_StringID(13,TARGET_TOO_FAR);
			}
			// Don't attack yourself
			else if(auto_attack_target == this) {
				Message_StringID(13,TRY_ATTACKING_SOMEONE);
			}
			else if (!los_status)
			{
				//you can't see your target	
			}
			else if(auto_attack_target->GetHP() > -10) {
				float DualWieldProbability = (GetSkill(DUAL_WIELD) + GetLevel()) / 400.0f; // 78.0 max
				if(GetAA(aaAmbidexterity))
					DualWieldProbability += 0.1f;
				//discipline effects:
				DualWieldProbability += (spellbonuses.DualWieldChance + itembonuses.DualWieldChance) / 100.0f;
				
				float random = MakeRandomFloat(0, 1);
				CheckIncreaseSkill(DUAL_WIELD, auto_attack_target, -10);
				if (random < DualWieldProbability){ // Max 78% of DW
					if(CheckAAEffect(aaEffectRampage)) {
						entity_list.AEAttack(this, 30, 14);
					} else {
						Attack(auto_attack_target, 14);	// Single attack with offhand
					}
					ItemInst *wpn = GetInv().GetItem(SLOT_SECONDARY);
					TryWeaponProc(wpn, auto_attack_target, 14);
					
					if( CanThisClassDoubleAttack() && CheckDoubleAttack()) {
						if(CheckAAEffect(aaEffectRampage)) {
							entity_list.AEAttack(this, 30, 14);
						} else {
							if(auto_attack_target && auto_attack_target->GetHP() > -10)
								Attack(auto_attack_target, 14);	// Single attack with offhand
						}
					}
				}
			}
		}
		
		adverrorinfo = 2;
		if (position_timer.Check()) {
			if (IsAIControlled())
			{
				if(IsMoving())
					SendPosUpdate(2);
				else
				{
					animation = 0;
					delta_x = 0;
					delta_y = 0;
					delta_z = 0;
					SendPosUpdate(2);
				}
			}
			
			if (m_ShadowStepUpdate)
			{
				entity_list.SendPositionUpdates(this, pLastUpdateWZ, RuleI(Zone,ShadowStepPositionUpdateDistance), GetTarget(), true);
				pLastUpdate = Timer::GetCurrentTime();
				pLastUpdateWZ = pLastUpdate;
				position_timer_counter++;
				if ( position_timer_counter > 50 )
				{
					m_ShadowStepUpdate = false;
					position_timer_counter = 0;
					position_timer.Start(250, true);
				}
			}
			// Send a position packet every 8 seconds - if not done, other clients
			// see this char disappear after 10-12 seconds of inactivity
			else if (position_timer_counter >= 36) { // Approx. 4 ticks per second
				entity_list.SendPositionUpdates(this, pLastUpdateWZ, RuleI(Zone,PositionUpdateDistance), GetTarget(), true);
			/* if (position_timer_counter >= 3) { // Send every 750ms?
				//Image (2k5): The trick of stopping MQ map without screwing up client updates, shorter distances, faster updates, however if its an admin we can send further updates
				if(Admin() > 80)
					entity_list.SendPositionUpdates(this, pLastUpdateWZ, 450, 0, true);
				else
					entity_list.SendPositionUpdates(this, pLastUpdateWZ, 150, 0, true);
				}
			*/
				pLastUpdate = Timer::GetCurrentTime();
				pLastUpdateWZ = pLastUpdate;
				position_timer_counter = 0;
			}
			else {
				pLastUpdate = Timer::GetCurrentTime();
				position_timer_counter++;
			}
		}
		
		if(HasVirus()) {
			if(ViralTimer.Check()) {
				viral_timer_counter++;
				for(int i = 0; i < MAX_SPELL_TRIGGER*2; i+=2) {
					if(viral_spells[i])	{
						if(viral_timer_counter % spells[viral_spells[i]].viral_timer == 0) {
							SpreadVirus(viral_spells[i], viral_spells[i+1]);
						}
					}
				}
			}
			if(viral_timer_counter > 999)
				viral_timer_counter = 0;
		}
		
		if(spellbonuses.GravityEffect == 1) {
			if(GravityTimer.Check())
				DoGravityEffect();
		}

		if (shield_timer.Check())
		{
			if (shield_target)
			{
				if (!CombatRange(shield_target))
				{
					entity_list.MessageClose(this,false,100,0,"%s ceases shielding %s.",GetCleanName(),shield_target->GetCleanName());
					for (int y = 0; y < 2; y++)
					{
						if (shield_target->shielder[y].shielder_id == GetID())
						{
							shield_target->shielder[y].shielder_id = 0;
							shield_target->shielder[y].shielder_bonus = 0;
						}
					}
					shield_target = 0;
					shield_timer.Disable();
				}
			}
			else
			{
				shield_target = 0;
				shield_timer.Disable();
			}
		}
		
		adverrorinfo = 3;
		SpellProcess();
		adverrorinfo = 4;
		if (endupkeep_timer.Check() && !dead){
			DoEnduranceUpkeep();
		}

			if ( ClientFinishedLoading() )
			{
				if ( forceposupdate_timer.Check() )
				{
					if ( animation > 0 )
						animation = 0;
					SetMoving(false);
					EQApplicationPacket* outapp = new EQApplicationPacket(OP_ClientUpdate, sizeof(PlayerPositionUpdateServer_Struct));
					PlayerPositionUpdateServer_Struct* ppu = (PlayerPositionUpdateServer_Struct*)outapp->pBuffer;
					MakeSpawnUpdateNoDelta(ppu);
					if (gmhideme)
						entity_list.QueueClientsStatus(this,outapp,true,Admin(),250);
					else
					#ifdef PACKET_UPDATE_MANAGER
								entity_list.QueueManaged(this,outapp,true,false);
					#else
								entity_list.QueueCloseClients(this,outapp,true,300,NULL,false);
					#endif
					forceposupdate_timer.Start(RuleI(Zone,PositionUpdateIdleTimer), true);
					safe_delete(outapp);
				}
			}

		if (tic_timer.Check() && !dead) {
			CalcMaxHP();
			CalcMaxMana();
			CalcATK();
			CalcMaxEndurance();
			CalcRestState();
			if ( ClientFinishedLoading() )
			{
				DoHPRegen();
				DoManaRegen();
				DoEnduranceRegen();
				BuffProcess();
			}

			DoStaminaUpdate();
		
			if(tribute_timer.Check()) {
				ToggleTribute(true);	//re-activate the tribute.
			}
			
			if (fishing_timer.Check()) {
				GoFish();
			}
			
			if (m_pp.intoxication > 0)
			{
				--m_pp.intoxication;
				CalcBonuses();
			}
			
			if (autosave_timer.Check()) {
				if(petition_list.FindPetitionByAccountName(AccountName()))
				{
					LoadMessageQueue(true);
				}
				Save(0);
			}
		}
	}
	
	if (client_state == CLIENT_KICKED) {
		Save();
		OnDisconnect(true);
		cout << "Client disconnected (cs=k): " << GetName() << endl;
		return false;
	}
	
	if (client_state == DISCONNECTED) {
		OnDisconnect(true);
		cout << "Client disconnected (cs=d): " << GetName() << endl;
		return false;
	}
	
	if (client_state == CLIENT_ERROR) {
		OnDisconnect(true);
		cout << "Client disconnected (cs=e): " << GetName() << endl;
		return false;
	}
	
	if (client_state != CLIENT_LINKDEAD && !eqs->CheckState(ESTABLISHED)) {
		OnDisconnect(true);
		cout << "Client linkdead: " << name << ", connection state: " << conn_state << endl;
		if (GetGM()) {
			return false;
		}
		else if ( !linkdead_timer.Enabled() )
		{
			if ( underattack_timer.Check(false) )
			{
				linkdead_timer.Start(LinkDeadDelayNotAttacked);
			}
			else
			{
				ResetUnderAttack();
				linkdead_timer.Start(RuleI(Zone,ClientLinkdeadMS));
			}
				
			//SendAppearancePacket(AT_Linkdead, 1);
			client_state = CLIENT_LINKDEAD;
		}
	}


	/************ Get all packets from packet manager out queue and process them ************/
	adverrorinfo = 5;
	
	EQApplicationPacket *app = 0;
//	if(eqs->GetState()==CLOSING && eqs->CheckActive())
	if ( client_state != CLIENT_LINKDEAD )
	{
		if(eqs->CheckState(CLOSING))
		{
			//eqs->Close();
			//return false;
			//handled below 
		} else {
			while(ret && (app = (EQApplicationPacket *)eqs->PopPacket())) {
				if(app)
					ret = HandlePacket(app);
				safe_delete(app);
			}
		}
	}
	
#ifdef REVERSE_AGGRO
	//At this point, we are still connected, everything important has taken
	//place, now check to see if anybody wants to aggro us.
	// Everhood 6/15/06 - only if client is not feigned
	if(ret && !GetFeigned() && scanarea_timer.Check()) {
		entity_list.CheckClientAggro(this);
	}
#endif	
	
	if (client_state != CLIENT_LINKDEAD && (client_state == CLIENT_ERROR || client_state == DISCONNECTED || client_state == CLIENT_KICKED || !eqs->CheckState(ESTABLISHED))) {
		//client logged out or errored out
		//ResetTrade();
		if (client_state != CLIENT_KICKED) {
			Save();
		}
		adverrorinfo = 811;
		client_state = CLIENT_LINKDEAD;
		if (/*!loggedin || */zoning || instalog || GetGM())
		{
			adverrorinfo = 811;
			Group *mygroup = GetGroup();
			if (mygroup)
			{
				adverrorinfo = 812;
				if (!zoning) {
					entity_list.MessageGroup(this,true,15,"%s logged out.",GetName());
					LeaveGroup();
				} else {
					entity_list.MessageGroup(this,true,15,"%s left the zone.",GetName());
					mygroup->MemberZoned(this);
				}
				
				adverrorinfo = 813;
			}
			Raid *myraid = entity_list.GetRaidByClient(this);
			if (myraid)
			{
				if (!zoning) {
					//entity_list.MessageGroup(this,true,15,"%s logged out.",GetName());
					//mygroup->DelMember(this);
					myraid->MemberZoned(this);
				} else {
					//entity_list.MessageGroup(this,true,15,"%s left the zone.",GetName());
					myraid->MemberZoned(this);
				}
			}
			OnDisconnect(false);
			return false;
		}
		else
		{
			adverrorinfo = 814;
			LinkDead();
		}
		OnDisconnect(true);
	}
	// EverHood Feign Death 2 minutes and zone forgets you
	if (forget_timer.Check()) {
		forget_timer.Disable();
		entity_list.ClearZoneFeignAggro(this);
		Message(0,"Your enemies have forgotten you!");
	}
	
	return ret;
}

//just a set of actions preformed all over in Client::Process
void Client::OnDisconnect(bool hard_disconnect) {
	if(hard_disconnect) {
		LeaveGroup();

		Raid *MyRaid = entity_list.GetRaidByClient(this);

		if (MyRaid)
			MyRaid->MemberZoned(this);
	}

	Mob *Other = trade->With();

	if(Other)
	{
		mlog(TRADING__CLIENT, "Client disconnected during a trade. Returning their items.");

		FinishTrade(this);

		if(Other->IsClient())
			Other->CastToClient()->FinishTrade(Other);

		trade->Reset();

		Other->trade->Reset();
	}

	//remove ourself from all proximities
	ClearAllProximities();
	
	EQApplicationPacket *outapp = new EQApplicationPacket(OP_LogoutReply);
	FastQueuePacket(&outapp);
	
	Disconnect();
}

// Sends the client complete inventory used in character login
//#ifdef ITEMCOMBINED
void Client::BulkSendInventoryItems()
{
	// Search all inventory buckets for items
	bool deletenorent=database.NoRentExpired(GetName());
	// Worn items and Inventory items
	sint16 slot_id = 0;
	if(deletenorent){//client was offline for more than 30 minutes, delete no rent items
		RemoveNoRent();
	}
	
	//TODO: this function is just retarded... it re-allocates the buffer for every
	//new item. It should be changed to loop through once, gather the
	//lengths, and item packet pointers into an array (fixed length), and
	//then loop again to build the packet.
	//EQApplicationPacket *packets[50];
	//unsigned long buflen = 0;
	//unsigned long pos = 0;
	//memset(packets, 0, sizeof(packets));
	//foreach item in the invendor sections
	//	packets[pos++] = ReturnItemPacket(...)
	//	buflen += temp->size
	//...
	//allocat the buffer
	//for r from 0 to pos
	//	put pos[r]->pBuffer into the buffer
	//for r from 0 to pos
	//	safe_delete(pos[r]);
	int32 size=0;
	int16 i = 0;
	map<int16,string> ser_items;
	map<int16,string>::iterator itr;
	//Inventory items
	for (slot_id=0; slot_id<=30; slot_id++) {
		const ItemInst* inst = m_inv[slot_id];
		if(inst && inst->IsSlotAllowed(slot_id) == 0)
			inst = NULL;
		if (inst){
			string packet = inst->Serialize(slot_id);
			ser_items[i++] = packet;
			size+=packet.length();
		}
	}
	// Power Source
	if(GetClientVersion() >= EQClientSoF)
	{	
		slot_id = 9999;
		const ItemInst* inst = m_inv[slot_id];
		if(inst && inst->IsSlotAllowed(slot_id) == 0)
			inst = NULL;
		if (inst){
			string packet = inst->Serialize(slot_id);
			ser_items[i++] = packet;
			size+=packet.length();
		}
	}
	// Bank items
	for (slot_id=2000; slot_id<=2023; slot_id++) {
		const ItemInst* inst = m_inv[slot_id];
		if(inst && inst->IsSlotAllowed(slot_id) == 0)
			inst = NULL;
		if (inst){
			string packet = inst->Serialize(slot_id);
			ser_items[i++] = packet;
			size+=packet.length();
		}
	}
	// Shared Bank items
	for (slot_id=2500; slot_id<=2501; slot_id++) {
		const ItemInst* inst = m_inv[slot_id];
		if(inst && inst->IsSlotAllowed(slot_id) == 0)
			inst = NULL;
		if (inst){
			string packet = inst->Serialize(slot_id);
			ser_items[i++] = packet;
			size+=packet.length();
		}
	}
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_CharInventory,size);
	uchar* ptr = outapp->pBuffer;
	for(itr=ser_items.begin();itr!=ser_items.end();itr++){
		int length = itr->second.length();
		if(length>5){
			memcpy(ptr,itr->second.c_str(),length);
			ptr+=length;
		}
	}
	//DumpPacket(outapp);
	FastQueuePacket(&outapp);
	//safe_delete(outapp);
	// LINKDEAD TRADE ITEMS
	// If player went LD during a trade, they have items in the trade inventory
	// slots.  These items are now being put into their inventory (then queue up on cursor)
	for (sint16 trade_slot_id=3000; trade_slot_id<=3007; trade_slot_id++) {
		const ItemInst* inst = m_inv[slot_id];
		if (inst) {
			bool is_arrow = (inst->GetItem()->ItemType == ItemTypeArrow) ? true : false;
			sint16 free_slot_id = m_inv.FindFreeSlot(inst->IsType(ItemClassContainer), true, inst->GetItem()->Size, is_arrow);
			DeleteItemInInventory(trade_slot_id, 0, false);
			PutItemInInventory(free_slot_id, *inst, true);
		}
	}
}
/*#else
void Client::BulkSendInventoryItems()
{
	// Search all inventory buckets for items
	bool deletenorent=database.NoRentExpired(GetName());
	// Worn items and Inventory items
	sint16 slot_id = 0;
	if(deletenorent){//client was offline for more than 30 minutes, delete no rent items
		RemoveNoRent();
	}
	for (slot_id=0; slot_id<=30; slot_id++) {
		const ItemInst* inst = m_inv[slot_id];
		if (inst){
			SendItemPacket(slot_id, inst, ItemPacketCharInventory);
		}
	}
	// Bank items
	for (slot_id=2000; slot_id<=2015; slot_id++) {
		const ItemInst* inst = m_inv[slot_id];
		if (inst){
			SendItemPacket(slot_id, inst, ItemPacketCharInventory);
		}
	}
	
	// Shared Bank items
	for (slot_id=2500; slot_id<=2501; slot_id++) {
		const ItemInst* inst = m_inv[slot_id];
		if (inst){
			SendItemPacket(slot_id, inst, ItemPacketCharInventory);
		}
	}
	
	// LINKDEAD TRADE ITEMS
	// If player went LD during a trade, they have items in the trade inventory
	// slots.  These items are now being put into their inventory (then queue up on cursor)
	for (sint16 trade_slot_id=3000; trade_slot_id<=3007; trade_slot_id++) {
		const ItemInst* inst = m_inv[slot_id];
		if (inst) {
			sint16 free_slot_id = m_inv.FindFreeSlot(inst->IsType(ItemClassContainer), true, inst->GetItem()->Size);
			DeleteItemInInventory(trade_slot_id, 0, false);
			PutItemInInventory(free_slot_id, *inst, true);
		}
	}
}
#endif*/

void Client::BulkSendMerchantInventory(int merchant_id, int npcid) {
	const Item_Struct* handyitem = NULL;
	int32 numItemSlots=80;  //The max number of items passed in the transaction.
	const Item_Struct *item;
	std::list<MerchantList> merlist = zone->merchanttable[merchant_id];
	std::list<MerchantList>::const_iterator itr;
	Mob* merch = entity_list.GetMobByNpcTypeID(npcid);
	if(merlist.size()==0){ //Attempt to load the data, it might have been missed if someone spawned the merchant after the zone was loaded
		zone->LoadNewMerchantData(merchant_id);
		merlist = zone->merchanttable[merchant_id];
		if(merlist.size()==0)
			return;
	}
	std::list<TempMerchantList> tmp_merlist = zone->tmpmerchanttable[npcid];
	std::list<TempMerchantList>::iterator tmp_itr;

	int32 i=1;
	int8 handychance = 0;
	for(itr = merlist.begin();itr != merlist.end() && i<numItemSlots;itr++){
		MerchantList ml = *itr;
		handychance = MakeRandomInt(0, merlist.size() + tmp_merlist.size() - 1 );
		
		item = database.GetItem(ml.item);
		if(item) {
			if(handychance==0)
				handyitem=item;
			else
				handychance--;
			int charges=1;

			if(item->ItemClass==ItemClassCommon)
				charges=item->MaxCharges;
			ItemInst* inst = database.CreateItem(item, charges);
			if (inst) {
				if (RuleB(Merchant, UsePriceMod)){
					// Kings & Bandits city tax mod
					uint32 taxPrice = item->Price * Client::CalcCityMod(merch,false);
				inst->SetPrice(taxPrice + (item->Price*(RuleR(Merchant, SellCostMod))*item->SellRate*Client::CalcPriceMod(merch,false)));
				}
				else
					inst->SetPrice((item->Price*(RuleR(Merchant, SellCostMod))*item->SellRate));
				inst->SetMerchantSlot(ml.slot);
				inst->SetMerchantCount(-1);		//unlimited
				if(charges > 0)
					inst->SetCharges(charges);
				else
					inst->SetCharges(1);

				SendItemPacket(ml.slot-1, inst, ItemPacketMerchant);
				safe_delete(inst);
			}
		}
		// Account for merchant lists with gaps.
		if(ml.slot >= i)
			i = ml.slot + 1;
	}
	std::list<TempMerchantList> origtmp_merlist = zone->tmpmerchanttable[npcid];
	tmp_merlist.clear();
	for(tmp_itr = origtmp_merlist.begin();tmp_itr != origtmp_merlist.end() && i<numItemSlots;tmp_itr++){
		TempMerchantList ml = *tmp_itr;
		item=database.GetItem(ml.item);
		ml.slot=i;
		if (item) {
			if(handychance==0)
				handyitem=item;
			else
				handychance--;
			int charges=1;
			//if(item->ItemClass==ItemClassCommon && (sint16)ml.charges <= item->MaxCharges)
			//	charges=ml.charges;
			//else
			sint16 quantity = 1;

			bool setCharges = false;
			if ( !item->Stackable )
			{
				// properly set the charges here and reflect quantity another way eventually
				if ( item->MaxCharges > 0 )
				{
					charges = ml.charges;
					setCharges = true;
					quantity = 1;
				}
			}

			if ( !setCharges )
			{
			if ( ml.charges != 0 )
				charges = ml.charges;
			else
				charges = item->MaxCharges;

			quantity = charges;
			}

			ItemInst* inst = database.CreateItem(item, quantity);
			if (inst) {
				if (RuleB(Merchant, UsePriceMod)){
					
					// Kings & Bandits city tax mod
					uint32 taxPrice = item->Price * Client::CalcCityMod(merch,false);

				inst->SetPrice(taxPrice + (item->Price*(RuleR(Merchant, SellCostMod))*item->SellRate*Client::CalcPriceMod(merch,false)));
				}
				else
					inst->SetPrice((item->Price*(RuleR(Merchant, SellCostMod))*item->SellRate));
				inst->SetMerchantSlot(ml.slot);
				inst->SetMerchantCount(ml.charges);
				if(setCharges)
					inst->SetCharges(charges);
				else if(charges > 0)
					inst->SetCharges(item->MaxCharges);//inst->SetCharges(charges);
				else
					inst->SetCharges(1);
				SendItemPacket(ml.slot-1, inst, ItemPacketMerchant);
				safe_delete(inst);
			}
		}
		tmp_merlist.push_back(ml);
		i++;
	}
	//this resets the slot
	zone->tmpmerchanttable[npcid] = tmp_merlist;
	if(merch != NULL && handyitem){
		char handy_id[8]={0};
		int greeting=MakeRandomInt(0, 4);
		int greet_id=0;
		switch(greeting){
			case 1:
				greet_id=MERCHANT_GREETING;
				break;
			case 2:
				greet_id=MERCHANT_HANDY_ITEM1;
				break;
			case 3:
				greet_id=MERCHANT_HANDY_ITEM2;
				break;
			case 4:
				greet_id=MERCHANT_HANDY_ITEM3;
				break;
			default:
				greet_id=MERCHANT_HANDY_ITEM4;
        }
		sprintf(handy_id,"%i",greet_id);
		
		if(greet_id!=MERCHANT_GREETING)
			Message_StringID(10,GENERIC_STRINGID_SAY,merch->GetCleanName(),handy_id,this->GetName(),handyitem->Name);
        else
			Message_StringID(10,GENERIC_STRINGID_SAY,merch->GetCleanName(),handy_id,this->GetName());
		
		merch->CastToNPC()->FaceTarget(this->CastToMob());
    }
		
//		safe_delete_array(cpi);
}

int8 Client::WithCustomer(int16 NewCustomer){

	if(NewCustomer == 0) {
		CustomerID = 0;
		return 0;
	}

	if(CustomerID == 0) {
		CustomerID = NewCustomer;
		return 1;
	}

	// Check that the player browsing our wares hasn't gone away.

	Client* c = entity_list.GetClientByID(CustomerID);

	if(!c) {
		_log(TRADING__CLIENT, "Previous customer has gone away.");
		CustomerID = NewCustomer;
		return 1;
	}

	return 0;
}

void Client::OPRezzAnswer(int32 Action, int32 SpellID, int16 ZoneID, int16 InstanceID, float x, float y, float z)
{
	if(PendingRezzXP < 0) {
		// pendingrezexp is set to -1 if we are not expecting an OP_RezzAnswer
		_log(SPELLS__REZ, "Unexpected OP_RezzAnswer. Ignoring it.");
		Message(13, "You have already been resurrected.\n");
		return;
	}

	if (Action == 1) {
		_log(SPELLS__REZ, "Player %s got a %i Rezz, spellid %i in zone%i, instance id %i", 
				  this->name, (int16)spells[SpellID].base[0],
				  SpellID, ZoneID, InstanceID);

		this->BuffFadeAll();
		int SpellEffectDescNum = GetSpellEffectDescNum(SpellID);
		// Rez spells with Rez effects have this DescNum (first is Titanium, second is 6.2 Client)
		if((SpellEffectDescNum == 82) || (SpellEffectDescNum == 39067)) {
			SetMana(0);
			SetHP(GetMaxHP()/5);
			SpellOnTarget(756, this); // Rezz effects
		}
		else {
			SetMana(GetMaxMana());
			SetHP(GetMaxHP());
		}
		if(spells[SpellID].base[0] < 100 && spells[SpellID].base[0] > 0 && PendingRezzXP > 0) 
		{
				SetEXP(((int)(GetEXP()+((float)((PendingRezzXP / 100) * spells[SpellID].base[0])))),
				       GetAAXP(),true);
		}
		else if (spells[SpellID].base[0] == 100 && PendingRezzXP > 0) {
			SetEXP((GetEXP() + PendingRezzXP), GetAAXP(), true);
		}
		
		// Red69 / Zek / KaB - Sep 28 2012 - Modified to take in the Ressurect_Struct from the other zone to properly set the client destination for a rez.
		MovePC(resStruct.zone_id, resStruct.instance_id, resStruct.x, resStruct.y, resStruct.z, GetHeading(), 0, ZoneSolicited);
	}
	
	// Red69 / Zek / KaB - Sep 28 2012 - Replaced clearing of rezz data
	SetPendingRezzData(-1,0,"",NULL);
}

void Client::OPTGB(const EQApplicationPacket *app)
{
	if(!app) return;
	if(!app->pBuffer) return;

	int32 tgb_flag = *(int32 *)app->pBuffer;
	if(tgb_flag == 2)
		Message_StringID(0, TGB() ? TGB_ON : TGB_OFF);
	else
		tgb = tgb_flag;
}

void Client::OPMemorizeSpell(const EQApplicationPacket* app)
{
	if(app->size != sizeof(MemorizeSpell_Struct))
	{
		LogFile->write(EQEMuLog::Error,"Wrong size on OP_MemorizeSpell. Got: %i, Expected: %i", app->size, sizeof(MemorizeSpell_Struct));
		DumpPacket(app);
		return;
	}
	
	const MemorizeSpell_Struct* memspell = (const MemorizeSpell_Struct*) app->pBuffer;
	
	if(!IsValidSpell(memspell->spell_id))
	{
		Message(13, "Unexpected error: spell id out of range");
		return;
	}

	if
	(
		GetClass() > 16 ||
		GetLevel() < spells[memspell->spell_id].classes[GetClass()-1]
	)
	{
		char val1[20]={0};
		Message_StringID(13,SPELL_LEVEL_TO_LOW,ConvertArray(spells[memspell->spell_id].classes[GetClass()-1],val1),spells[memspell->spell_id].name);
		//Message(13, "Unexpected error: Class cant use this spell at your level!");
		return;
	}

	switch(memspell->scribing)
	{
		case memSpellScribing:	{	// scribing spell to book
			const ItemInst* inst = m_inv[SLOT_CURSOR];

			if(inst && inst->IsType(ItemClassCommon))
			{
				const Item_Struct* item = inst->GetItem();
				
				if(item && item->Scroll.Effect == (sint32)(memspell->spell_id))
				{
					ScribeSpell(memspell->spell_id, memspell->slot);
					DeleteItemInInventory(SLOT_CURSOR, 1, true);
				}
				else 
					Message(0,"Scribing spell: inst exists but item does not or spell ids do not match.");
			}
			else
				Message(0,"Scribing a spell without an inst on your cursor?");
			break;
		}
		case memSpellMemorize:	{	// memming spell
			if(HasSpellScribed(memspell->spell_id))
			{
				MemSpell(memspell->spell_id, memspell->slot);
			}
			else
			{
				database.SetMQDetectionFlag(AccountName(), GetName(), "OP_MemorizeSpell but we don't have this spell scribed...", zone->GetShortName());
			}
			break;
		}
		case memSpellForget:	{	// unmemming spell
			UnmemSpell(memspell->slot);
			break;
		}
	}

	Save();
}

void Client::CancelSneakHide()
{
	if (hidden || improved_hidden) {
		EQApplicationPacket* app = new EQApplicationPacket(OP_CancelSneakHide, 0);
		FastQueuePacket(&app);
		
		BreakMobHide();
	}
}

void Client::BreakInvis()
{
	// trade does not break sneak: http://everquest.allakhazam.com/db/skills.html?skill=31
	//CancelSneakHide();
	if (invisible)
	{
		EQApplicationPacket* outapp = new EQApplicationPacket(OP_SpawnAppearance, sizeof(SpawnAppearance_Struct));
		SpawnAppearance_Struct* sa_out = (SpawnAppearance_Struct*)outapp->pBuffer;
		sa_out->spawn_id = GetID();
		sa_out->type = 0x03;
		sa_out->parameter = 0;
		entity_list.QueueClients(this, outapp, true);
		safe_delete(outapp);
		invisible = false;
		invisible_undead = false;
		invisible_animals = false;
		hidden = false;
		improved_hidden = false;
	}

}

static int64 CoinTypeCoppers(uint32 type) {
	switch(type) {
	case COINTYPE_PP:
		return(1000);
	case COINTYPE_GP:
		return(100);
	case COINTYPE_SP:
		return(10);
	case COINTYPE_CP:
	default:
		break;
	}
	return(1);
}

void Client::OPMoveCoin(const EQApplicationPacket* app)
{
	MoveCoin_Struct* mc = (MoveCoin_Struct*)app->pBuffer;
	int64 value = 0, amount_to_take = 0, amount_to_add = 0;
	sint32 *from_bucket = 0, *to_bucket = 0;
	Mob* trader = trade->With();
	
	if ( mc->amount < 0 )
		return;

	//DumpPacket(app);

	// could just do a range, but this is clearer and explicit
	if
	(
		(
			mc->cointype1 != COINTYPE_PP &&
			mc->cointype1 != COINTYPE_GP &&
			mc->cointype1 != COINTYPE_SP &&
			mc->cointype1 != COINTYPE_CP
		) ||
		(
			mc->cointype2 != COINTYPE_PP &&
			mc->cointype2 != COINTYPE_GP &&
			mc->cointype2 != COINTYPE_SP &&
			mc->cointype2 != COINTYPE_CP
		)
	)
	{
		return;
	}

	switch(mc->from_slot)
	{
		case -1:	// destroy
		{
			// solar: I don't think you can move coin from the void, 
			// but need to check this
			break;
		}
		case 0:	// cursor
		{
			switch(mc->cointype1)
			{
				case COINTYPE_PP:
					from_bucket = (sint32 *) &m_pp.platinum_cursor; break;
				case COINTYPE_GP:
					from_bucket = (sint32 *) &m_pp.gold_cursor; break;
				case COINTYPE_SP:
					from_bucket = (sint32 *) &m_pp.silver_cursor; break;
				case COINTYPE_CP:
					from_bucket = (sint32 *) &m_pp.copper_cursor; break;
			}
			break;
		}
		case 1:	// inventory
		{
			switch(mc->cointype1)
			{
				case COINTYPE_PP:
					from_bucket = (sint32 *) &m_pp.platinum; break;
				case COINTYPE_GP:
					from_bucket = (sint32 *) &m_pp.gold; break;
				case COINTYPE_SP:
					from_bucket = (sint32 *) &m_pp.silver; break;
				case COINTYPE_CP:
					from_bucket = (sint32 *) &m_pp.copper; break;
			}
			break;
		}
		case 2:	// bank
		{
			uint32 distance = 0;
			NPC *banker = entity_list.GetClosestBanker(this, distance);
			if(!banker || distance > USE_NPC_RANGE2)
			{
				char *hacked_string = NULL;
				MakeAnyLenString(&hacked_string, "Player tried to make use of a banker(coin move) but %s is non-existant or too far away (%u units).", 
					banker ? banker->GetName() : "UNKNOWN NPC", distance);
				database.SetMQDetectionFlag(AccountName(), GetName(), hacked_string, zone->GetShortName());
				safe_delete_array(hacked_string);
				return;
			}

			switch(mc->cointype1)
			{
				case COINTYPE_PP:
					from_bucket = (sint32 *) &m_pp.platinum_bank; break;
				case COINTYPE_GP:
					from_bucket = (sint32 *) &m_pp.gold_bank; break;
				case COINTYPE_SP:
					from_bucket = (sint32 *) &m_pp.silver_bank; break;
				case COINTYPE_CP:
					from_bucket = (sint32 *) &m_pp.copper_bank; break;
			}
			break;
		}
		case 3:	// trade
		{
			// can't move coin from trade
			break;
		}
		case 4:	// shared bank
		{
			uint32 distance = 0;
			NPC *banker = entity_list.GetClosestBanker(this, distance);
			if(!banker || distance > USE_NPC_RANGE2)
			{
				char *hacked_string = NULL;
				MakeAnyLenString(&hacked_string, "Player tried to make use of a banker(shared coin move) but %s is non-existant or too far away (%u units).", 
					banker ? banker->GetName() : "UNKNOWN NPC", distance);
				database.SetMQDetectionFlag(AccountName(), GetName(), hacked_string, zone->GetShortName());
				safe_delete_array(hacked_string);
				return;
			}
			if(mc->cointype1 == COINTYPE_PP)	// there's only platinum here
				from_bucket = (sint32 *) &m_pp.platinum_shared;
			break;
		}
	}

	switch(mc->to_slot)
	{
		case -1:	// destroy
		{
			// no action required
			break;
		}
		case 0:	// cursor
		{
			switch(mc->cointype2)
			{
				case COINTYPE_PP:
					to_bucket = (sint32 *) &m_pp.platinum_cursor; break;
				case COINTYPE_GP:
					to_bucket = (sint32 *) &m_pp.gold_cursor; break;
				case COINTYPE_SP:
					to_bucket = (sint32 *) &m_pp.silver_cursor; break;
				case COINTYPE_CP:
					to_bucket = (sint32 *) &m_pp.copper_cursor; break;
			}
			break;
		}
		case 1:	// inventory
		{
			switch(mc->cointype2)
			{
				case COINTYPE_PP:
					to_bucket = (sint32 *) &m_pp.platinum; break;
				case COINTYPE_GP:
					to_bucket = (sint32 *) &m_pp.gold; break;
				case COINTYPE_SP:
					to_bucket = (sint32 *) &m_pp.silver; break;
				case COINTYPE_CP:
					to_bucket = (sint32 *) &m_pp.copper; break;
			}
			break;
		}
		case 2:	// bank
		{
			uint32 distance = 0;
			NPC *banker = entity_list.GetClosestBanker(this, distance);
			if(!banker || distance > USE_NPC_RANGE2)
			{
				char *hacked_string = NULL;
				MakeAnyLenString(&hacked_string, "Player tried to make use of a banker(coin move) but %s is non-existant or too far away (%u units).", 
					banker ? banker->GetName() : "UNKNOWN NPC", distance);
				database.SetMQDetectionFlag(AccountName(), GetName(), hacked_string, zone->GetShortName());
				safe_delete_array(hacked_string);
				return;
			}
			switch(mc->cointype2)
			{
				case COINTYPE_PP:
					to_bucket = (sint32 *) &m_pp.platinum_bank; break;
				case COINTYPE_GP:
					to_bucket = (sint32 *) &m_pp.gold_bank; break;
				case COINTYPE_SP:
					to_bucket = (sint32 *) &m_pp.silver_bank; break;
				case COINTYPE_CP:
					to_bucket = (sint32 *) &m_pp.copper_bank; break;
			}
			break;
		}
		case 3:	// trade
		{
			if(trader)
			{
				switch(mc->cointype2)
				{
					case COINTYPE_PP:
						to_bucket = (sint32 *) &trade->pp; break;
					case COINTYPE_GP:
						to_bucket = (sint32 *) &trade->gp; break;
					case COINTYPE_SP:
						to_bucket = (sint32 *) &trade->sp; break;
					case COINTYPE_CP:
						to_bucket = (sint32 *) &trade->cp; break;
				}
			}
			break;
		}
		case 4:	// shared bank
		{
			uint32 distance = 0;
			NPC *banker = entity_list.GetClosestBanker(this, distance);
			if(!banker || distance > USE_NPC_RANGE2)
			{
				char *hacked_string = NULL;
				MakeAnyLenString(&hacked_string, "Player tried to make use of a banker(shared coin move) but %s is non-existant or too far away (%u units).", 
					banker ? banker->GetName() : "UNKNOWN NPC", distance);
				database.SetMQDetectionFlag(AccountName(), GetName(), hacked_string, zone->GetShortName());
				safe_delete_array(hacked_string);
				return;
			}
			if(mc->cointype2 == COINTYPE_PP)	// there's only platinum here
				to_bucket = (sint32 *) &m_pp.platinum_shared;
			break;
		}
	}

	if(!from_bucket)
	{
		return;
	}

	// don't allow them to go into negatives (from our point of view)
	amount_to_take = *from_bucket < mc->amount ? *from_bucket : mc->amount;

	// solar: if you move 11 gold into a bank platinum location, the packet
	// will say 11, but the client will have 1 left on their cursor, so we have
	// to figure out the conversion ourselves

	amount_to_add = amount_to_take * ((float)CoinTypeCoppers(mc->cointype1) / (float)CoinTypeCoppers(mc->cointype2));

	// the amount we're adding could be different than what was requested, so
	// we have to adjust the amount we take as well
	amount_to_take = amount_to_add * ((float)CoinTypeCoppers(mc->cointype2) / (float)CoinTypeCoppers(mc->cointype1));

	// solar: now we should have a from_bucket, a to_bucket, an amount_to_take
	// and an amount_to_add

#ifdef SOLAR
	printf("taking %d coins, adding %d coins\n", amount_to_take, amount_to_add);
#endif

	// solar: now we actually take it from the from bucket.  if there's an error
	// with the destination slot, they lose their money
	*from_bucket -= amount_to_take;
	// why are intentionally inducing a crash here rather than letting the code attempt to stumble on?
	// assert(*from_bucket >= 0);

	if(to_bucket)
	{
		if(*to_bucket + amount_to_add > *to_bucket)	// overflow check
			*to_bucket += amount_to_add;

		//shared bank plat
		if (RuleB(Character, SharedBankPlat))
		{
			if (to_bucket == &m_pp.platinum_shared || from_bucket == &m_pp.platinum_shared)
			{
				if (from_bucket == &m_pp.platinum_shared) 
					amount_to_add = 0 - amount_to_take;

				database.SetSharedPlatinum(AccountID(),amount_to_add);
			}
		}
	}

#ifdef SOLAR
	printf("from bucket = %d  ", *from_bucket);
	if(to_bucket)
		printf("to bucket = %d", *to_bucket);
	printf("\n");
#endif

	// if this is a trade move, inform the person being traded with
	if(mc->to_slot == 3 && trader && trader->IsClient())
	{

		// If one party accepted the trade then some coin was added, their state needs to be reset
		trade->state = Trading;
		Mob* with = trade->With();
		if (with)
			with->trade->state = Trading;
	
		Client* recipient = trader->CastToClient();
		recipient->Message(15, "%s adds some coins to the trade.", GetName());
		recipient->Message(15, "The total trade is: %i PP, %i GP, %i SP, %i CP",
			trade->pp, trade->gp,
			trade->sp, trade->cp
		);

		EQApplicationPacket* outapp = new EQApplicationPacket(OP_TradeCoins,sizeof(TradeCoin_Struct));
		TradeCoin_Struct* tcs = (TradeCoin_Struct*)outapp->pBuffer;
		tcs->trader = trader->GetID();
		tcs->slot = mc->cointype2;
		tcs->unknown5 = 0x4fD2;
		tcs->unknown7 = 0;
		tcs->amount = amount_to_add;
		recipient->QueuePacket(outapp);
		safe_delete(outapp);
	}

	Save();
}

void Client::OPGMTraining(const EQApplicationPacket *app)
{

	EQApplicationPacket* outapp = app->Copy();
	GMTrainee_Struct* gmtrain = (GMTrainee_Struct*) outapp->pBuffer;

	Mob* pTrainer = entity_list.GetMob(gmtrain->npcid);

	if(!pTrainer || !pTrainer->IsNPC() || pTrainer->GetClass() < WARRIORGM || pTrainer->GetClass() > BERSERKERGM)
		return;
	
	//you can only use your own trainer, client enforces this, but why trust it
	int trains_class = pTrainer->GetClass() - (WARRIORGM - WARRIOR);
	if(GetClass() != trains_class)
		return;
	
	//you have to be somewhat close to a trainer to be properly using them
	if(DistNoRoot(*pTrainer) > USE_NPC_RANGE2)
		return;

 	SkillType sk;
 	for (sk = _1H_BLUNT; sk <= HIGHEST_SKILL; sk = (SkillType)(sk+1)) {
		if(sk == TINKERING && GetRace() != GNOME) {
			gmtrain->skills[sk] = 0; //Non gnomes can't tinker!
		} else {
			gmtrain->skills[sk] = GetMaxSkillAfterSpecializationRules(sk, MaxSkill(sk, GetClass(), RuleI(Character, MaxLevel)+1)); 
			//this is the highest level that the trainer can train you to, this is enforced clientside so we can't just
			//Set it to 1 with CanHaveSkill or you wont be able to train past 1.
		}
	}
	
	uchar ending[]={0x34,0x87,0x8a,0x3F,0x01
		,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9
		,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9,0xC9
		,0x76,0x75,0x3f};
	memcpy(&outapp->pBuffer[outapp->size-40],ending,sizeof(ending));
	FastQueuePacket(&outapp);

	// welcome message
	if (pTrainer && pTrainer->IsNPC())
	{
		pTrainer->Say_StringID(MakeRandomInt(1204, 1207), GetCleanName());
	}
}

void Client::OPGMEndTraining(const EQApplicationPacket *app)
{
	EQApplicationPacket *outapp = new EQApplicationPacket(OP_GMEndTrainingResponse, 0);
	GMTrainEnd_Struct *p = (GMTrainEnd_Struct *)app->pBuffer;

	FastQueuePacket(&outapp);

	Mob* pTrainer = entity_list.GetMob(p->npcid);
	if(!pTrainer || !pTrainer->IsNPC() || pTrainer->GetClass() < WARRIORGM || pTrainer->GetClass() > BERSERKERGM)
		return;
	
	//you can only use your own trainer, client enforces this, but why trust it
	int trains_class = pTrainer->GetClass() - (WARRIORGM - WARRIOR);
	if(GetClass() != trains_class)
		return;

	//you have to be somewhat close to a trainer to be properly using them
	if(DistNoRoot(*pTrainer) > USE_NPC_RANGE2)
		return;
	
	// goodbye message
	if (pTrainer->IsNPC())
	{
		pTrainer->Say_StringID(MakeRandomInt(1208, 1211), GetCleanName());
	}
}

void Client::OPGMTrainSkill(const EQApplicationPacket *app)
{
	if(!m_pp.points)
		return;

	int Cost = 0;

	GMSkillChange_Struct* gmskill = (GMSkillChange_Struct*) app->pBuffer;
	
	Mob* pTrainer = entity_list.GetMob(gmskill->npcid);
	if(!pTrainer || !pTrainer->IsNPC() || pTrainer->GetClass() < WARRIORGM || pTrainer->GetClass() > BERSERKERGM)
		return;
	
	//you can only use your own trainer, client enforces this, but why trust it
	int trains_class = pTrainer->GetClass() - (WARRIORGM - WARRIOR);
	if(GetClass() != trains_class)
		return;
	
	//you have to be somewhat close to a trainer to be properly using them
	if(DistNoRoot(*pTrainer) > USE_NPC_RANGE2)
		return;
	
	if (gmskill->skillbank == 0x01)
	{
		// languages go here
		if (gmskill->skill_id > 25)
		{
			cout << "Wrong Training Skill (languages)" << endl;
			DumpPacket(app);
			return;
		}
		int AdjustedSkillLevel = GetLanguageSkill(gmskill->skill_id) - 10;
		if(AdjustedSkillLevel > 0)
			Cost = AdjustedSkillLevel * AdjustedSkillLevel * AdjustedSkillLevel / 100;

		IncreaseLanguageSkill(gmskill->skill_id);
	}
	else if (gmskill->skillbank == 0x00)
	{
		// normal skills go here
		if (gmskill->skill_id > HIGHEST_SKILL)
		{
			cout << "Wrong Training Skill (abilities)" << endl;
			DumpPacket(app);
			return;
		}

		SkillType skill = (SkillType) gmskill->skill_id;

		if(!CanHaveSkill(skill)) {
			mlog(CLIENT__ERROR, "Tried to train skill %d, which is not allowed.", skill);
			return;
		}
		
		int16 skilllevel = GetRawSkill(skill);
		if(skilllevel == 0) {
			//this is a new skill..
			int16 t_level = SkillTrainLevel(skill, GetClass());
			if (t_level == 0)
			{
				return;
			}
			SetSkill(skill, t_level);
		} else {
			switch(skill) {
			case BREWING:
			case MAKE_POISON:
			case TINKERING:
			case RESEARCH:
			case ALCHEMY:
			case BAKING:
			case TAILORING:
			case BLACKSMITHING:
			case FLETCHING:
			case JEWELRY_MAKING:
			case POTTERY:
				if(skilllevel >= RuleI(Skills, MaxTrainTradeskills)) {
					Message_StringID(13, MORE_SKILLED_THAN_I, pTrainer->GetCleanName());
					return;
				}
			default:
				break;
			}
            		// Client train a valid skill
	    		//
			int AdjustedSkillLevel = skilllevel - 10;

			if(AdjustedSkillLevel > 0)
				Cost = AdjustedSkillLevel * AdjustedSkillLevel * AdjustedSkillLevel / 100;

			SetSkill(skill, skilllevel + 1);


		}
	}

	if(GetClientVersion() >= EQClientSoF) {
		// The following packet decreases the skill points left in the Training Window and
		// produces the 'You have increased your skill / learned the basics of' message.
		//
		EQApplicationPacket* outapp = new EQApplicationPacket(OP_GMTrainSkillConfirm, sizeof(GMTrainSkillConfirm_Struct));

		GMTrainSkillConfirm_Struct *gmtsc = (GMTrainSkillConfirm_Struct *)outapp->pBuffer;
		gmtsc->SkillID = gmskill->skill_id;

		if(gmskill->skillbank == 1) {
			gmtsc->NewSkill = (GetLanguageSkill(gmtsc->SkillID) == 1);
			gmtsc->SkillID += 100;
		}
		else
			gmtsc->NewSkill = (GetRawSkill((SkillType)gmtsc->SkillID) == 1);

		gmtsc->Cost = Cost;

		strcpy(gmtsc->TrainerName, pTrainer->GetName());
		QueuePacket(outapp);
		safe_delete(outapp);
	}
	
	if(Cost)
		TakeMoneyFromPP(Cost);

	m_pp.points--;
}

// this is used for /summon and /corpse
void Client::OPGMSummon(const EQApplicationPacket *app)
{
	GMSummon_Struct* gms = (GMSummon_Struct*) app->pBuffer;
	Mob* st = entity_list.GetMob(gms->charname);
	if(st && st->IsCorpse())
	{
		st->CastToCorpse()->Summon(this, false, true);
	}
	else
	{
		if(admin < 80)
		{
			return;
		}
		if(st)
		{
			Message(0, "Local: Summoning %s to %f, %f, %f", gms->charname, gms->x, gms->y, gms->z);
			if (st->IsClient() && (st->CastToClient()->GetAnon() != 1 || this->Admin() >= st->CastToClient()->Admin()))
				st->CastToClient()->MovePC(zone->GetZoneID(), zone->GetInstanceID(), (float)gms->x, (float)gms->y, (float)gms->z, this->GetHeading(), true);
			else
				st->GMMove(this->GetX(), this->GetY(), this->GetZ(),this->GetHeading());
		}
		else
		{
			int8 tmp = gms->charname[strlen(gms->charname)-1];
			if (!worldserver.Connected())
			{
				Message(0, "Error: World server disconnected");
			}
			else if (tmp < '0' || tmp > '9') // dont send to world if it's not a player's name
			{
				ServerPacket* pack = new ServerPacket(ServerOP_ZonePlayer, sizeof(ServerZonePlayer_Struct));
				ServerZonePlayer_Struct* szp = (ServerZonePlayer_Struct*) pack->pBuffer;
				strcpy(szp->adminname, this->GetName());
				szp->adminrank = this->Admin();
				strcpy(szp->name, gms->charname);
				strcpy(szp->zone, zone->GetShortName());
				szp->x_pos = (float)gms->x;
				szp->y_pos = (float)gms->y;
				szp->z_pos = (float)gms->z;
				szp->ignorerestrictions = 2;
				worldserver.SendPacket(pack);
				safe_delete(pack);
			}
			else {
				//all options have been exhausted
				//summon our target...
				if(GetTarget() && GetTarget()->IsCorpse()){
					GetTarget()->CastToCorpse()->Summon(this, false, true);
				}
			}
		}
	}
}

void Client::DoHPRegen() {
	SetHP(GetHP() + CalcHPRegen() + RestRegenHP);
	SendHPUpdate();
}

void Client::DoManaRegen() {
	if (GetMana() >= max_mana)
		return;
	
	SetMana(GetMana() + CalcManaRegen() + RestRegenMana);
	SendManaUpdatePacket();
}


void Client::DoStaminaUpdate() {			
	if(!stamina_timer.Check())
		return;
	
	EQApplicationPacket* outapp = new EQApplicationPacket(OP_Stamina, sizeof(Stamina_Struct));
	Stamina_Struct* sta = (Stamina_Struct*)outapp->pBuffer;

	if(zone->GetZoneID() != 151) {
		if (m_pp.hunger_level > 0)
			m_pp.hunger_level-=35;
		if (m_pp.thirst_level > 0)
			m_pp.thirst_level-=35;
		sta->food = m_pp.hunger_level > 6000 ? 6000 : m_pp.hunger_level;
		sta->water = m_pp.thirst_level> 6000 ? 6000 : m_pp.thirst_level;
	}
	else {
		// No auto food/drink consumption in the Bazaar
		sta->food = 6000;
		sta->water = 6000;
	}
	FastQueuePacket(&outapp);
}

void Client::DoEnduranceRegen()
{
	if(GetEndurance() >= GetMaxEndurance())
		return;

	SetEndurance(GetEndurance() + CalcEnduranceRegen() + RestRegenEndurance);
}

void Client::DoEnduranceUpkeep() {
	int upkeep_sum = 0;

	int cost_redux = spellbonuses.EnduranceReduction + itembonuses.EnduranceReduction;

	uint32 buffs_i;
	uint32 buff_count = GetMaxTotalSlots();
	for (buffs_i = 0; buffs_i < buff_count; buffs_i++) {
		if (buffs[buffs_i].spellid != SPELL_UNKNOWN) {
			int upkeep = spells[buffs[buffs_i].spellid].EndurUpkeep;
			if(upkeep > 0) {
				if(cost_redux > 0) {
					if(upkeep <= cost_redux)
						continue;	//reduced to 0
					upkeep -= cost_redux;
				}
				if((upkeep+upkeep_sum) > GetEndurance()) {
					//they do not have enough to keep this one going.
					BuffFadeBySlot(buffs_i);
				} else {
					upkeep_sum += upkeep;
				}
			}
		}
	}
	
	if(upkeep_sum != 0)
		SetEndurance(GetEndurance() - upkeep_sum);
}

void Client::CalcRestState() {

	// This method calculates rest state HP and mana regeneration.
	// The client must have been out of combat for RuleI(Character, RestRegenTimeToActivate) seconds,
	// must be sitting down, and must not have any detrimental spells affecting them.
	//
	if(!RuleI(Character, RestRegenPercent))
		return;

	RestRegenHP = RestRegenMana = RestRegenEndurance = 0;

	if(AggroCount || !IsSitting())
		return;

	if(!rest_timer.Check(false))
		return;

	uint32 buff_count = GetMaxTotalSlots();
	for (unsigned int j = 0; j < buff_count; j++) {
		if(buffs[j].spellid != SPELL_UNKNOWN) {
			if(IsDetrimentalSpell(buffs[j].spellid) && (buffs[j].ticsremaining > 0))
				if(!DetrimentalSpellAllowsRest(buffs[j].spellid))
					return;
		}
	}

	RestRegenHP = (GetMaxHP() * RuleI(Character, RestRegenPercent) / 100);

	RestRegenMana = (GetMaxMana() * RuleI(Character, RestRegenPercent) / 100);
	
	if(RuleB(Character, RestRegenEndurance))
		RestRegenEndurance = (GetMaxEndurance() * RuleI(Character, RestRegenPercent) / 100);
}

void Client::DoTracking()
{
	if(TrackingID == 0)
		return;

	Mob *m = entity_list.GetMob(TrackingID);

	if(!m || m->IsCorpse())
	{
		Message_StringID(MT_Skills, TRACK_LOST_TARGET);

		TrackingID = 0;

		return;
	}

	float RelativeHeading = GetHeading() - CalculateHeadingToTarget(m->GetX(), m->GetY());

	if(RelativeHeading < 0)
		RelativeHeading += 256;

	if((RelativeHeading <= 16) || (RelativeHeading >= 240))
	{
		Message_StringID(MT_Skills, TRACK_STRAIGHT_AHEAD, m->GetCleanName());
	}
	else if((RelativeHeading > 16) && (RelativeHeading <= 48))
	{
		Message_StringID(MT_Skills, TRACK_AHEAD_AND_TO, m->GetCleanName(), "right");
	}
	else if((RelativeHeading > 48) && (RelativeHeading <= 80))
	{
		Message_StringID(MT_Skills, TRACK_TO_THE, m->GetCleanName(), "right");
	}
	else if((RelativeHeading > 80) && (RelativeHeading <= 112))
	{
		Message_StringID(MT_Skills, TRACK_BEHIND_AND_TO, m->GetCleanName(), "right");
	}
	else if((RelativeHeading > 112) && (RelativeHeading <= 144))
	{
		Message_StringID(MT_Skills, TRACK_BEHIND_YOU, m->GetCleanName());
	}
	else if((RelativeHeading > 144) && (RelativeHeading <= 176))
	{
		Message_StringID(MT_Skills, TRACK_BEHIND_AND_TO, m->GetCleanName(), "left");
	}
	else if((RelativeHeading > 176) && (RelativeHeading <= 208))
	{
		Message_StringID(MT_Skills, TRACK_TO_THE, m->GetCleanName(), "left");
	}
	else if((RelativeHeading > 208) && (RelativeHeading < 240))
	{
		Message_StringID(MT_Skills, TRACK_AHEAD_AND_TO, m->GetCleanName(), "left");
	}
}

void Client::HandleRespawnFromHover(uint32 Option)
{
	RespawnFromHoverTimer.Disable();

	if(Option == 1)	// Resurrect
	{
		QGlobal res;
		if((PendingRezzXP < 0) || (PendingRezzSpellID == 0))
		{
			_log(SPELLS__REZ, "Unexpected Rezz from hover request.");
			return;
		}
		else if(QGlobalCache::GetQGlobal(res,CastToClient()->GetQGlobals()->GetBucket(),"noressurectallowed",0,CastToClient()->CharacterID(),zone->GetZoneID()))
		{
			Message(0, "You are not able to accept a ressurection at this time.");
			return;
		}
		SetHP(GetMaxHP() / 5);

		Corpse* corpse = entity_list.GetCorpseByName(PendingRezzCorpseName.c_str());

		if(corpse)
		{
			x_pos = corpse->GetX();
			y_pos = corpse->GetY();
			z_pos = corpse->GetZ();
		}

		EQApplicationPacket* outapp = new EQApplicationPacket(OP_ZonePlayerToBind, sizeof(ZonePlayerToBind_Struct) + 10);
		ZonePlayerToBind_Struct* gmg = (ZonePlayerToBind_Struct*) outapp->pBuffer;
		
		gmg->bind_zone_id = zone->GetZoneID();
		gmg->bind_instance_id = zone->GetInstanceID();
		gmg->x = GetX();
		gmg->y = GetY();
		gmg->z = GetZ();
		gmg->heading = GetHeading();
		strcpy(gmg->zone_name, "Resurrect");

		FastQueuePacket(&outapp);

		ClearHover();
		SendHPUpdate();
		OPRezzAnswer(1, PendingRezzSpellID, zone->GetZoneID(), zone->GetInstanceID(), GetX(), GetY(), GetZ());

		if (corpse && corpse->IsCorpse()) {
			_log(SPELLS__REZ, "Hover Rez in zone %s for corpse %s",
					  zone->GetShortName(), PendingRezzCorpseName.c_str());

			_log(SPELLS__REZ, "Found corpse. Marking corpse as rezzed.");

			corpse->Rezzed(true);
			corpse->CompleteRezz();
		}
		return;
	}

	// Respawn at Bind Point.
	//
	if(m_pp.binds[0].zoneId == zone->GetZoneID())
	{
		PendingRezzSpellID = 0;

		EQApplicationPacket* outapp = new EQApplicationPacket(OP_ZonePlayerToBind, sizeof(ZonePlayerToBind_Struct) + 14);
		ZonePlayerToBind_Struct* gmg = (ZonePlayerToBind_Struct*) outapp->pBuffer;
		
		gmg->bind_zone_id = m_pp.binds[0].zoneId;
		gmg->x = m_pp.binds[0].x;
		gmg->y = m_pp.binds[0].y;
		gmg->z = m_pp.binds[0].z;
		gmg->heading = 0;
		strcpy(gmg->zone_name, "Bind Location");

		FastQueuePacket(&outapp);

		CalcBonuses();
		SetHP(GetMaxHP());
		SetMana(GetMaxMana());
		SetEndurance(GetMaxEndurance());

		x_pos = m_pp.binds[0].x;
		y_pos = m_pp.binds[0].y;
		z_pos = m_pp.binds[0].z;

		ClearHover();

		SendHPUpdate();

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

		m_pp.zone_id = m_pp.binds[0].zoneId;
		m_pp.zoneInstance = 0;
		database.MoveCharacterToZone(this->CharacterID(), database.GetZoneName(m_pp.zone_id, m_pp.zoneInstance));
		
		Save();
	
		GoToDeath();
	}
}

void Client::ClearHover()
{
	// Our Entity ID is currently zero, set in Client::Death
	SetID(entity_list.GetFreeID());

	EQApplicationPacket *outapp = new EQApplicationPacket(OP_ZoneEntry, sizeof(ServerZoneEntry_Struct));
	ServerZoneEntry_Struct* sze = (ServerZoneEntry_Struct*)outapp->pBuffer;

	FillSpawnStruct(&sze->player,CastToMob());

	sze->player.spawn.NPC = 0;
	sze->player.spawn.z += 6;	//arbitrary lift, seems to help spawning under zone.
	
	entity_list.QueueClients(this, outapp, false);
	safe_delete(outapp);

	if(IsClient() && CastToClient()->GetClientVersionBit() & BIT_UnderfootAndLater)
	{
		EQApplicationPacket *outapp = MakeBuffsPacket(false);
		CastToClient()->FastQueuePacket(&outapp);
	}

	dead = false;
}
