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
#ifndef NPC_H
#define NPC_H

class NPC;
#include "zonedb.h"
#include "mob.h"
//#include "spawn.h"

#include <list>
using namespace std;

#include "spawn2.h"
#include "loottable.h"
#include "zonedump.h"
#include "QGlobals.h"
#include "../common/rulesys.h"

#ifdef WIN32
	#define  M_PI	3.141592
#endif

typedef struct {
	float min_x;
	float max_x;
	float min_y;
	float max_y;
	float min_z;
	float max_z;
	bool say;
} NPCProximity;

struct AISpells_Struct {
	int16	type;			// 0 = never, must be one (and only one) of the defined values
	uint16	spellid;		// <= 0 = no spell
	sint16	manacost;		// -1 = use spdat, -2 = no cast time
	int32	time_cancast;	// when we can cast this spell next
	sint32	recast_delay;
	sint16	priority;
	sint16  resist_adjust;
};

class NPC : public Mob
{
public:
	static NPC* SpawnNPC(const char* spawncommand, float in_x, float in_y, float in_z, float in_heading = 0, Client* client = 0);
	static sint8 GetAILevel(bool iForceReRead = false);
	
	NPC(const NPCType* data, Spawn2* respawn, float x, float y, float z, float heading, int iflymode, bool IsCorpse = false, bool isTempPet = false);
	
	virtual ~NPC();

	//abstract virtual function implementations requird by base abstract class
	virtual void Death(Mob* killerMob, sint32 damage, int16 spell_id, SkillType attack_skill, int32 player_id=0);
	virtual void Damage(Mob* from, sint32 damage, int16 spell_id, SkillType attack_skill, uint8 itemType, bool avoidable = true, sint8 buffslot = -1, bool iBuffTic = false, bool isDmgShield=false, bool isCritHit = false, int32 player_id=0, bool isDoT=false);
	virtual bool Attack(Mob* other, int Hand = 13, bool FromRiposte = false, bool IsStrikethrough = false);
	virtual bool HasRaid() { return false; }
	virtual bool HasGroup() { return false; }
	virtual Raid* GetRaid() { return 0; }
	virtual Group* GetGroup() { return 0; }

	virtual bool IsNPC() const { return true; }

	virtual bool Process();
	virtual void	AI_Init();
	virtual void	AI_Start(int32 iMoveDelay = 0);
	virtual void	AI_Stop();
	void			AI_DoMovement();
	bool			AI_AddNPCSpells(int32 iDBSpellsID, bool resetSpellList=true, int32 levelCap=0);
	virtual bool	AI_EngagedCastCheck();
	bool			AI_HasSpells() { return HasAISpell; }

	virtual bool	AI_PursueCastCheck();
	virtual bool	AI_IdleCastCheck();
	virtual void	AI_Event_SpellCastFinished(bool iCastSucceeded, int8 slot);

	void LevelScale();
	void CalcNPCResists();
	sint16 CalcNPCManaRegen();
	sint16 CalcNPCHPRegen();
	void CalcNPCRegen(sint16 hpRegenBonus=0,sint16 manaRegenBonus=0, bool addToDefaults=false);
	
	void CalcNPCDamage();
	
	virtual void SetTarget(Mob* mob);
	virtual uint16 GetSkill(SkillType skill_num) const { if (skill_num <= HIGHEST_SKILL) { return skills[skill_num]; } return 0; }

	void CalcItemBonuses(StatBonuses *newbon);
	virtual void CalcBonuses();
	virtual int GetCurrentBuffSlots() const { return RuleI(Spells, MaxBuffSlotsNPC); }
	virtual int GetCurrentSongSlots() const { return RuleI(Spells, MaxSongSlotsNPC); }
	virtual int GetCurrentDiscSlots() const { return RuleI(Spells, MaxDiscSlotsNPC); }
	virtual int GetMaxBuffSlots() const { return RuleI(Spells, MaxBuffSlotsNPC); }
	virtual int GetMaxSongSlots() const { return RuleI(Spells, MaxSongSlotsNPC); }
	virtual int GetMaxDiscSlots() const { return RuleI(Spells, MaxDiscSlotsNPC); }
	virtual int GetMaxTotalSlots() const { return RuleI(Spells, MaxTotalSlotsNPC); }
	virtual void InitializeBuffSlots();
	virtual void UninitializeBuffSlots();

	virtual void	RangedAttack(Mob* other, bool bypassTimer=false);
	virtual void	ThrowingAttack(Mob* other) { }

	//void	Damage(Mob* other, sint32 damage, int16 spell_id, SkillType attack_skill, bool avoidable = true, sint8 buffslot = -1, bool iBuffTic = false);
	//void	Death(Mob* other, sint32 damage, int16 spell_id, SkillType attack_skill);
	bool	DatabaseCastAccepted(int spell_id);
	bool	IsFactionListAlly(uint32 other_faction);
	FACTION_VALUE CheckNPCFactionAlly(sint32 other_faction);
	virtual FACTION_VALUE GetReverseFactionCon(Mob* iOther);

	void	GoToBind(uint8 bindnum = 0)	{ GMMove(org_x, org_y, org_z, org_heading); }
	void	Gate();

	void	GetPetState(SpellBuff_Struct *buffs, int32 *items, char *name);
	void	GetPetProcs(ExtendedProfile_Procs* permaprocs, ExtendedProfile_Procs* spellprocs);
	void	SetPetState(SpellBuff_Struct *buffs, int32 *items, ExtendedProfile_Procs* permaprocs=NULL, ExtendedProfile_Procs* spellprocs=NULL);
	void	InteractiveChat(int8 chan_num, int8 language, const char * message, const char* targetname,Mob* sender);
	void	TakenAction(int8 action,Mob* actiontaker);
	virtual void SpellProcess();
	virtual void FillSpawnStruct(NewSpawn_Struct* ns, Mob* ForWho);

	void	AddItem(const Item_Struct* item, int8 charges, bool equipitem = true);
	void	AddItem(int32 itemid, int8 charges, bool equipitem = true);
	void	AddItem(int32 itemid);
	void	AddLootTable();
	
	// Kings & Bandits
	void	AddRandomLootTable();
/*
	void	NPCSpecialAttacks(const char* parse, int permtag);
	void	NPCDatabaseSpells(const char* parse);
	void	NPCUnharmSpell(int spell_id);
	void	CheckFriendlySpellStatus();

	void	CheckEnemySpellStatus();
	void	NPCHarmSpell(int target,int type);
	void    HateSummon();
*/	
	void	DescribeAggro(Client *towho, Mob *mob, bool verbose);
	void    RemoveItem(uint32 item_id, int16 quantity = 0, int16 slot = 0);
//	bool	AddNPCSpells(int32 iDBSpellsID, AISpells_Struct* AIspells);
	void	ClearItemList();
	int		HasItem(uint32 itemid);
	ServerLootItem_Struct*	GetItem(int slot_id);
	void	AddCash(int16 in_copper, int16 in_silver, int16 in_gold, int16 in_platinum);
	void	AddCash(uint64 copper);
	void	AddCash();
	void	RemoveCash();
	void	QueryLoot(Client* to);
	int32	CountLoot();
	void	DumpLoot(int32 npcdump_index, ZSDump_NPC_Loot* npclootdump, int32* NPCLootindex);
	inline int32	GetLoottableID()	const { return loottable_id; }
//	void	SetPetType(int16 in_type)	{ typeofpet = in_type; } // put this here because only NPCs can be anything but charmed pets

	inline uint32	GetCopper()		const { return copper; }
	inline uint32	GetSilver()		const { return silver; }
	inline uint32	GetGold()		const { return gold; }
	inline uint32	GetPlatinum()	const { return platinum; }

	inline void	SetCopper(uint32 amt)		{ copper = amt; }
	inline void	SetSilver(uint32 amt)		{ silver = amt; }
	inline void	SetGold(uint32 amt)			{ gold = amt; }
	inline void	SetPlatinum(uint32 amt)		{ platinum = amt; }



	void SetGrid(sint32 grid_){ grid=grid_; }
	void SetSp2(int32 sg2){ spawn_group=sg2; }
	void SetWaypointMax(int16 wp_){ wp_m=wp_; }
	void SetSaveWaypoint(int16 wp_){ save_wp=wp_; }

	int16 GetWaypointMax() const { return wp_m; }
	sint32 GetGrid() const { return grid; }
	int32 GetSp2() const { return spawn_group; }
	int32 GetSpawnPointID() const;

	float GetSpawnPointX()	const { return org_x; }
	float GetSpawnPointY()	const { return org_y; }
	float GetSpawnPointZ()	const { return org_z; }
	float GetSpawnPointH()	const { return org_heading; }
	float GetGuardPointX()	const { return guard_x; }
	float GetGuardPointY()	const { return guard_y; }
	float GetGuardPointZ()	const { return guard_z; }

	void SetFlyMode(int8 FlyMode){ flymode=FlyMode; }
	int32 GetFlyMode() const { return flymode; }

	int8 GetPrimSkill()	const { return prim_melee_type; }
	int8 GetSecSkill()	const { return sec_melee_type; }
	void SetPrimSkill(int8 skill_type)	{ prim_melee_type = skill_type; }
	void SetSecSkill(int8 skill_type)	{ sec_melee_type = skill_type; }
	
	bool IsTemporaryPet() { return TemporaryPet; }
	
	uint32	MerchantType;

	uint32	MerchantMinLevel;

	void	Depop(bool StartSpawnTimer = true);
	void	Stun(int duration, bool randInterrupt=false);
	void	UnStun();
	int32	GetSwarmOwner();
	int32	GetSwarmTarget();
	void	SetSwarmTarget(int target_id = 0);
	
	inline void SignalNPC(int _signal_id) { signaled = true; signal_id = _signal_id; }
	
	inline sint32	GetNPCFactionID()	const { return npc_faction_id; }
	inline sint32			GetPrimaryFaction()	const { return primary_faction; }
	sint32	GetNPCHate(Mob* in_ent)  {return hate_list.GetEntHate(in_ent);}
	
	// Kings & Bandits - hatelist reference to top hate value
    int32    GetTopHateValue(Mob*p) { return hate_list.GetHateTopValue(p);}

    bool    IsOnHatelist(Mob*p) { return hate_list.IsOnHateList(p);}

	void	SetNPCFactionID(sint32 in) { npc_faction_id = in; database.GetFactionIdsForNPC(npc_faction_id, &faction_list, &primary_faction); }

	float   org_x, org_y, org_z, org_heading;
	
	int32	GetMaxDMG() const {return max_dmg;}
	int32	GetMinDMG() const {return min_dmg;}
	bool	IsAnimal() const { return(bodytype == BT_Animal); }
	int16   GetPetSpellID() const {return pet_spell_id;}
	void    SetPetSpellID(int16 amt) {pet_spell_id = amt;}
	int32	GetMaxDamage(int8 tlevel);
	void    SetTaunting(bool tog) {taunting = tog;}
	void	PickPocket(Client* thief);
	void	StartSwarmTimer(int32 duration) { swarm_timer.Start(duration); }
	void	AddLootDrop(const Item_Struct*dbitem, ItemList* itemlistconst, sint8 charges, bool equipit, bool wearchange = false);
	virtual void DoClassAttacks(Mob *target);
	void	CheckSignal();
	
	//waypoint crap
	int                 GetMaxWp() const { return max_wp; }
	void				DisplayWaypointInfo(Client *to, bool dispWaypoints=true);
	void				DisplayLootInfo(Client* to, bool condenseList=true);
	void				CalculateNewWaypoint();
//	int8				CalculateHeadingToNextWaypoint();
//	float				CalculateDistanceToNextWaypoint();
	void				AssignWaypoints(sint32 grid);
	void				SetWaypointPause();
	void				UpdateWaypoint(int wp_index);
	// quest wandering commands
	void				StopWandering();
	void				ResumeWandering();
	void				PauseWandering(int pausetime);
	void				MoveTo(float mtx, float mty, float mtz, float mth, bool saveguardspot);
	void                GetClosestWaypoint(list<wplist> &wp_list, int count, float m_x, float m_y, float m_z);
	void                GetImageWaypoint(list<wplist> &wp_list);
	
	void				SetEquipment(int8 material_slot, int32 itemid);
	int32				GetEquipment(int8 material_slot) const;	// returns item id
	sint32				GetEquipmentMaterial(int8 material_slot) const;
	
	void				NextGuardPosition();
	void				SaveGuardSpot(bool iClearGuardSpot = false);
	inline bool			IsGuarding() const { return(guard_heading != 0); }
	void				SaveGuardSpotCharm();
	void				RestoreGuardSpotCharm();
	void				AI_SetRoambox(float iDist, float iRoamDist, int32 iDelay = 2500);
	void				AI_SetRoambox(float iDist, float iMaxX, float iMinX, float iMaxY, float iMinY, int32 iDelay = 2500);
	
	
	inline bool WillAggroNPCs() const { return(npc_aggro); }
	
	inline void GiveNPCTypeData(NPCType *ours) { NPCTypedata_ours = ours; }
	inline const int32 GetNPCSpellsID()	const { return npc_spells_id; }
	
	ItemList	itemlist; //kathgar - why is this public?  Doing other things or I would check the code
	
	NPCProximity* proximity;
	Spawn2*	respawn2;
	QGlobalCache *GetQGlobals() { return qGlobals; }
	QGlobalCache *CreateQGlobals() { qGlobals = new QGlobalCache(); return qGlobals; }

	AA_SwarmPetInfo *GetSwarmInfo() { return (swarmInfoPtr); }
	void SetSwarmInfo(AA_SwarmPetInfo *mSwarmInfo) { safe_delete(swarmInfoPtr); swarmInfoPtr = mSwarmInfo; }

	sint32	GetAccuracyRating() const { return (accuracy_rating); }
	void	SetAccuracyRating(sint32 d) { accuracy_rating = d;}
	sint32 GetRawAC() const { return AC; }

	void	ModifyNPCStat(const char *identifier, const char *newValue);
	virtual void SetLevel(uint8 in_level, bool command = false);

	bool IsLDoNTrapped() const { return (ldon_trapped); }
	void SetLDoNTrapped(bool n) { ldon_trapped = n; }

	int8 GetLDoNTrapType() const { return (ldon_trap_type); }
	void SetLDoNTrapType(int8 n) { ldon_trap_type = n; }

	int16 GetLDoNTrapSpellID() const { return (ldon_spell_id); }
	void SetLDoNTrapSpellID(int16 n) { ldon_spell_id = n; }

	bool IsLDoNLocked() const { return (ldon_locked); }
	void SetLDoNLocked(bool n) { ldon_locked = n; }

	int16 GetLDoNLockedSkill() const { return (ldon_locked_skill); }
	void SetLDoNLockedSkill(int16 n) { ldon_locked_skill = n; }

	bool IsLDoNTrapDetected() const { return (ldon_trap_detected); }
	void SetLDoNTrapDetected(bool n) { ldon_trap_detected = n; }

	const bool GetCombatEvent() const { return combat_event; }
	void SetCombatEvent(bool b) { combat_event = b; }

	//The corpse we make can only be looted by people who got credit for the kill
	const bool HasPrivateCorpse() const { return NPCTypedata->private_corpse; }
	const char* GetRawNPCTypeName() const { return NPCTypedata->name; }

	bool GetDepop() { return p_depop; }

	void NPCSlotTexture(int8 slot, int16 texture);	// Sets new material values for slots

	uint32 GetAdventureTemplate() const { return adventure_template_id; }
	void AddSpellToNPCList(sint16 iPriority, sint16 iSpellID, uint16 iType, sint16 iManaCost, sint32 iRecastDelay, sint16 iResistAdjust);
	bool HasSpell(sint16 spellID);
	void RemoveSpellFromNPCList(sint16 spell_id);
	void RemoveAllSpellsFromNPCList();

	// Kings & Bandits
	
	bool TemporaryPet;
	bool AddedLootGroupInPerl;
	uint32 teamID;
	uint32 npcID;

	std::list<int32> spellOmitList; // by spell id
	std::list<int32> spellGroupOmitList; // by effect type 


	bool isGuildNPC;
	void LoadRandomLootGroups();

	uint32 GetRandomNameGroupID() { if ( NPCTypedata ) return NPCTypedata->randomNameGroupID; else return 0; }
	
	// Kings & Bandits - npc melee texture fix on attack anim
	int16 GetMeleeTexture1() { return d_meele_texture1; }
	int16 GetMeleeTexture2() { return d_meele_texture2; }

	int GetFactionCount() { return faction_list.size(); }

	// KaB - Zek - Red69 / Multiquest additions
	void AddQuestItem(ItemInst* inst) { questItems.Insert(inst); }

	void ClearQuestLists()
	{
		ClearQuestItems(true);
		ClearQuestDeleteItems(true);
	}

	void ResetQuestDeleteList()
	{
		ClearQuestDeleteItems(true);
	}
	
	
	void ClearQuestItems(bool delete_=false)
	{
		LinkedListIterator<ItemInst*> iterator(questItems);
		iterator.Reset();
		while(iterator.MoreElements())
		{
			ItemInst* inst = iterator.GetData();
			iterator.RemoveCurrent(delete_);
		}

		questItems.Clear();
	}

	void ClearQuestDeleteItems(bool delete_=false)
	{
		LinkedListIterator<ItemInst*> iterator(questDeletionItems);
		iterator.Reset();
		while(iterator.MoreElements())
		{
			ItemInst* inst = iterator.GetData();
			iterator.RemoveCurrent(delete_);
		}

		questDeletionItems.Clear();
	}
	
	ItemInst* FindQuestItemByID(int32 itmID, int charges, bool flagItemForDeletion=false)
	{
		LinkedListIterator<ItemInst*> iterator(questItems);
		iterator.Reset();
		int totalCharges = 0;
		while(iterator.MoreElements())
		{
			if ( iterator.GetData()->GetItem()->ID == itmID )
			{
				totalCharges += 1;

				if ( flagItemForDeletion )
					questDeletionItems.Insert(iterator.GetData()->Clone());
				if ( charges > totalCharges )
				{
					iterator.Advance();
					continue;
				}

				return iterator.GetData();
			}
			iterator.Advance();
		}
		return NULL;
	}

	bool DoesQuestItemExist(int32 itmID, int charges, bool flagItemForDeletion=false) { 	
		ItemInst* inst = FindQuestItemByID(itmID,charges,flagItemForDeletion);
		if ( inst != NULL )
		{
			return true;
		}
		else
			return false;
	}

	void ClearQuestItem(ItemInst* inst, bool delete_=true)
	{
		LinkedListIterator<ItemInst*> iterator(questItems);
		iterator.Reset();

		while(iterator.MoreElements())
		{
			if ( iterator.GetData ()->GetItem()->ID == inst->GetItem()->ID )
			{
				iterator.RemoveCurrent(delete_);
				break;
			}
			iterator.Advance();
		}
	}

	void RemoveQuestDeleteItems()
	{
		LinkedListIterator<ItemInst*> iterator(questDeletionItems);
		iterator.Reset();
		while(iterator.MoreElements())
		{
			ClearQuestItem(iterator.GetData(),true);
			iterator.RemoveCurrent(true);
		}

		questDeletionItems.Clear();
	}

	void PrintOutQuestItems(Client* c);
	
	void AddSpellList(int32 spell_list)
	{
		if ( !HasSpellList(spell_list) )
			npc_spell_list_ids.push_back(spell_list);
	}

	bool HasSpellList(int32 spell_list)
	{
		for(std::list<int32>::iterator iter = npc_spell_list_ids.begin();
			iter != npc_spell_list_ids.end();
			++iter)
		{
			if(*iter == spell_list)
				return true;
		}
		return false;
	}

	bool HasSpellListInRange(int32 minID, int32 maxID)
	{
		for(std::list<int32>::iterator iter = npc_spell_list_ids.begin();
			iter != npc_spell_list_ids.end();
			++iter)
		{
			if(*iter <= maxID && *iter >= minID)
				return true;
		}
		return false;
	}
	
	uint32 CountSpellListInRange(int32 minID, int32 maxID)
	{
		uint32 count = 0;
		for(std::list<int32>::iterator iter = npc_spell_list_ids.begin();
			iter != npc_spell_list_ids.end();
			++iter)
		{
			if(*iter <= maxID && *iter >= minID)
				count++;
		}
		return count;
	}
	
	void AddSpellGroupOmit(int32 effect_type)
	{
		spellGroupOmitList.push_back(effect_type);
	}
	
	bool IsSpellGroupOmit(int32 spell_id)
	{
		for(std::list<int32>::iterator iter = spellGroupOmitList.begin();
			iter != spellGroupOmitList.end();
			++iter)
		{
			if(IsEffectInSpell(spell_id,*iter))
			{
				return true;
			}
		}
		return false;
	}
	
	bool HasSpellGroupOmit(int32 effecttype)
	{
		for(std::list<int32>::iterator iter = spellGroupOmitList.begin();
			iter != spellGroupOmitList.end();
			++iter)
		{
			if(*iter == effecttype)
				return true;
		}
		return false;
	}
	
	void AddSpellOmit(int32 effect_type)
	{
		spellOmitList.push_back(effect_type);
	}
	
	bool HasSpellOmit(int32 spellid)
	{
		for(std::list<int32>::iterator iter = spellOmitList.begin();
			iter != spellOmitList.end();
			++iter)
		{
			if(*iter == spellid)
				return true;
		}
		return false;
	}
	
	void ReloadSpellList();

	void ClearSpellList()
	{
		npc_spell_list_ids.clear();
	}
	
	void ClearSpellGroupOmitList()
	{
		spellGroupOmitList.clear();
	}
	
	void ClearSpellOmitList()
	{
		spellOmitList.clear();
	}

protected:
	
	std::list<int32> npc_spell_list_ids;

	const NPCType*	NPCTypedata;
	NPCType*	NPCTypedata_ours;	//special case for npcs with uniquely created data.

	friend class EntityList;
	list<struct NPCFaction*> faction_list;
	uint32	copper;
	uint32	silver;
	uint32	gold;
	uint32	platinum;
	sint32   grid;
	int32   spawn_group;
	int16	wp_m;

	sint32	npc_faction_id;
	sint32	primary_faction;
	
	Timer	attacked_timer;		//running while we are being attacked (damaged)
    Timer	swarm_timer;
    Timer	classattack_timer;
	Timer	knightattack_timer;
    Timer	assist_timer;		//ask for help from nearby mobs
	Timer	qglobal_purge_timer;

	bool	combat_event;	//true if we are in combat, false otherwise
    Timer	sendhpupdate_timer;
	Timer	enraged_timer;

	int32	npc_spells_id;
	int8	casting_spell_AIindex;
	Timer*	AIautocastspell_timer;
	int32*	pDontCastBefore_casting_spell;
	std::vector<AISpells_Struct> AIspells;

	bool HasAISpell;
	virtual bool AICastSpell(Mob* tar, int8 iChance, int16 iSpellTypes);
	virtual bool AIDoSpellCast(int8 i, Mob* tar, sint32 mana_cost, int32* oDontDoAgainBefore = 0);
	
	
	int32	max_dmg;
	int32	min_dmg;
	sint32	accuracy_rating;
	
	//pet crap:
	int16	pet_spell_id;
	bool	taunting;
    Timer	taunt_timer;		//for pet taunting
	
	bool npc_aggro;
	
	int		signal_id;
	bool	signaled;	// used by quest signal() command
	
	//waypoint crap:
	vector<wplist> Waypoints;
	void _ClearWaypints();
	int max_wp;
	int save_wp;
    float guard_x, guard_y, guard_z, guard_heading;
	float guard_x_saved, guard_y_saved, guard_z_saved, guard_heading_saved;
	float roambox_max_x;
	float roambox_max_y;
	float roambox_min_x;
	float roambox_min_y;
	float roambox_distance;
	float roambox_movingto_x;
	float roambox_movingto_y;
	int32 roambox_delay;
	
	int16   skills[HIGHEST_SKILL+1];
	int32   equipment[MAX_MATERIALS];	//this is an array of item IDs
	int16	d_meele_texture1;			//this is an item Material value
	int16	d_meele_texture2;			//this is an item Material value (offhand)
	int8	prim_melee_type;			//Sets the Primary Weapon attack message and animation
	int8	sec_melee_type;				//Sets the Secondary Weapon attack message and animation
	AA_SwarmPetInfo *swarmInfoPtr;
	
	bool ldon_trapped;
	int8 ldon_trap_type;
	int16 ldon_spell_id;
	bool ldon_locked;
	int16 ldon_locked_skill;
	bool ldon_trap_detected;
	QGlobalCache *qGlobals;
	uint32 adventure_template_id;

	// Kings & Bandits Additions

	std::vector<uint32> RandomLootGroups;
	
	LinkedList<ItemInst*> questItems;
	LinkedList<ItemInst*> questDeletionItems;
private:
	int32	loottable_id;
	bool	p_depop;
};

#endif

