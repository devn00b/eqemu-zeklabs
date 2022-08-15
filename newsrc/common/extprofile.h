/*  EQEMu:  Everquest Server Emulator
	Copyright (C) 2001-2004  EQEMu Development Team (http://eqemulator.net)

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
#ifndef EXTENDED_PROFILE_H
#define EXTENDED_PROFILE_H 

#include "eq_packet_structs.h"
#include "Item.h"

#pragma pack(1)


/*
	This is a place to store data that dosent have a home in the
	official player profile.
	
	Try not to expand or re-arrange thing in this struct
	to make conversion between sizes easier.
	if something gets removed from here, just turn it into an unused
	item, so conversion is not needed. Then reuse old unused values if 
	possible later instead of adding more, so long as old values left
	over would not be extrememly unpleasent side effects.
*/
#define CURRENTPLAYERPROFILE_VERSION 4 // increment when adding a switch case to UpdatePlayerProfile

struct ExtendedProfile_Procs
{
    	int16 spellID;
    	int16 chance;
	int32 timerRemaining;
	bool  isPoison;
};

struct ExtendedProfile_Struct {
	// Pet stuff
	int16					pet_id;
	int16					pet_hp;
	int16					pet_mana;
	SpellBuff_Struct		pet_buffs[BUFF_COUNT];
	int32					pet_items[MAX_MATERIALS];
	char					pet_name[64];
	
	uint32					aa_effects;
	uint32					perAA;		//% of exp going to AAs
	uint32					expended_aa;		// Total of expended AA
	uint32					profile_version; // used to issue updates to player profile

	ExtendedProfile_Procs			PermaProcs[4];
	ExtendedProfile_Procs			SpellProcs[4];
	float					pet_power; // cross zone pet power from previous focus effects
	Buffs_Struct				buffs[BUFF_COUNT];

	ExtendedProfile_Procs	PetPermaProcs[4];
	ExtendedProfile_Procs	PetSpellProcs[4];

	bool					isBecomeNPC;
	sint32					npcMaxLevel;
	uint32					npcSpawnID;
	char					npcName[64];
	uint8					npcClass;
	uint16					npcRace;
	uint8					npcGender;
	uint8					npcLevel;
};

#pragma pack()

void InitExtendedProfile(ExtendedProfile_Struct *p, bool recreate=false);
bool SetExtendedProfile(ExtendedProfile_Struct *to, char *old, unsigned int len);


#endif
