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
#ifndef SPAWN2_H
#define SPAWN2_H

#include "../common/timer.h"
#include "npc.h"

#include <string>
using namespace std;

#define SC_AlwaysEnabled 0

class SpawnCondition;
class NPC;

class Spawn2
{
public:
	Spawn2(int32 spawn2_id, int32 spawngroup_id, 
		float x, float y, float z, float heading, 
		int32 respawn, int32 variance, 
		int32 timeleft = 0, int32 grid = 0,
		uint16 cond_id = SC_AlwaysEnabled, sint16 min_value = 0, bool in_enabled = true);
	~Spawn2();

	void	LoadGrid();
	void	Enable() { enabled = true; }
	void	Disable(bool depoponly=false);
	bool	Enabled() { return enabled; }
	bool	Process();
	void	Reset();
	void	Depop();
	void	Repop(int32 delay = 0);

	void	DeathReset(float modRespawnTime=0.0f); //resets the spawn in the case the npc dies, also updates db if needed

	void	SpawnConditionChanged(const SpawnCondition &c, sint16 old_value);
	int32	GetID()		{ return spawn2_id; }
	float	GetX()		{ return x; }
	float	GetY()		{ return y; }
	float	GetZ()		{ return z; }
	float	GetHeading() { return heading; }
	void	SetRespawnTimer(int32 newrespawntime) { respawn_ = newrespawntime; };
	void	SetVariance(int32 newvariance) { variance_ = newvariance; }
	const int32 GetVariance() const { return variance_; }
	int32	RespawnTimer() { return respawn_; }
	int32	SpawnGroupID() { return spawngroup_id_; }
	int32	CurrentNPCID() { return currentnpcid; }
	void	SetCurrentNPCID(int32 nid) { currentnpcid = nid; }
	int32	GetSpawnCondition() { return condition_id; }
	int32	spawn2_id;
	int32	respawn_;

	bool	NPCPointerValid() { return (npcthis!=NULL); }
	void	SetNPCPointer(NPC* n) { npcthis = n; }
	void	SetTimer(int32 duration) { timer.Start(duration); }

	bool	isTimerEnabled() { return timer.Enabled(); }

	int32	getDeathCount() { return deathcount; }
	void	setDeathCount(int32 count) { deathcount = count; }

	void	SetX(float newX) { x = newX; };
	void	SetY(float newY) { y = newY; };
	void	SetZ(float newZ) { z = newZ; };
	void	SetHeading(float newHeading) { heading = newHeading; };
protected:
	friend class Zone;
	Timer	timer;
	Timer	resetDeathTimer;
private:
	int32	resetTimer();

	int32	spawngroup_id_;
	int32	currentnpcid;
	NPC*	npcthis;
	float	x;
	float	y;
	float	z;
	float	heading;
	int32	variance_;
	int32	grid_;
	uint16	condition_id;
	sint16	condition_min_value;
	int32	deathcount;
	bool enabled;
};

class SpawnCondition {
public:
	typedef enum {
		DoNothing = 0,
		DoDepop = 1,
		DoRepop = 2,
		//... 3...9 reserved for future use
		DoSignalMin = 10	//any number above this value is used as
							//a base for the signal ID sent. e.g.
							// value 12 sends signal id 2
	} OnChange;
	
	SpawnCondition();
	
	uint16		condition_id;
	sint16		value;
	OnChange 	on_change;
};

class SpawnEvent {
public:
	typedef enum {
		ActionSet = 0,
		ActionAdd = 1,
		ActionSubtract = 2,
		ActionMultiply = 3,
		ActionDivide = 4
	} Action;
	
	SpawnEvent();
	
	uint32	id;
	uint16	condition_id;
	string	zone_name;
	
	bool	enabled;
	Action	action;
	sint16	argument;
	
	uint32	period;	//eq minutes (3 seconds) between events
	TimeOfDay_Struct next;	//next time this event triggers
};

class SpawnConditionManager {
public:
	SpawnConditionManager();
	
	void Process();
	bool LoadSpawnConditions(const char* zone_name, uint32 instance_id);
	
	sint16 GetCondition(const char *zone_short, uint32 instance_id, uint16 condition_id);
	void SetCondition(const char *zone_short, uint32 instance_id, uint16 condition_id, sint16 new_value, bool world_update = false);
	void ToggleEvent(uint32 event_id, bool enabled, bool reset_base);
	bool Check(uint16 condition, sint16 min_value);
	void ReloadEvent(uint32 event_id);
	
protected:
	map<uint16, SpawnCondition> spawn_conditions;
	vector<SpawnEvent> spawn_events;
	
	void ExecEvent(SpawnEvent &e, bool send_update);
	void UpdateDBEvent(SpawnEvent &e);
	bool LoadDBEvent(uint32 event_id, SpawnEvent &e, string &zone_name);
	void UpdateDBCondition(const char* zone_name, uint32 instance_id, uint16 cond_id, sint16 value);
	void FindNearestEvent();
	
	Timer minute_timer;
	TimeOfDay_Struct next_event;
};

#endif
