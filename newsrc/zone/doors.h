#ifndef DOORS_H
#define DOORS_H
#include "../common/types.h"
#include "../common/linked_list.h"
#include "../common/timer.h"
#include "../common/emu_opcodes.h"
#include "../common/eq_packet_structs.h"
#include "entity.h"
#include "mob.h"
#include "zonedump.h"

class Doors : public Entity
{
public:
	Doors(const Door* door);
	~Doors();
	bool	IsDoor() const { return true; }
	void	HandleClick(Client* sender, int8 trigger);
	bool	Process();
	int8	GetDoorID() { return door_id; }
	int32	GetDoorDBID() { return db_id; }
	int32	GetGuildID() { return guild_id; }
	int8	GetOpenType() { return opentype; }
	char*	GetDoorName() { return door_name; }
	int32	GetDoorParam() { return door_param; }
	int		GetInvertState() { return invert_state; }
	float	GetX() { return pos_x; }
	float	GetY() { return pos_y; }
	float	GetZ() { return pos_z; }
	float	GetHeading() { return heading; }
	int		GetIncline() { return incline; }
	bool	triggered;
	void	SetOpenState(bool st) { isopen = st; }
	// Kings & Bandits - Door open modified to fix the up/down for the elevators for faydark.  They sync properly now
	bool	IsDoorOpen() { if ( ( !strcasecmp(GetDoorName(),"FELE2") || !strcasecmp(GetDoorName(),"FAYLEVATOR") ) && door_param == 1 ) return false; else return isopen; }

	int8	GetTriggerDoorID() { return trigger_door; }
	int8	GetTriggerType() { return trigger_type; }
 
	uint32	GetKeyItem() { return keyitem; }
	int8	GetNoKeyring() { return nokeyring; }
	int16	GetLockpick() { return lockpick; } 
	int16	GetSize() { return size; }
	void	SetGuildID(int32 guild_id) { guild_id = guild_id; }

	int32	GetEntityID() { return entity_id; }
	void	SetEntityID(int32 entity) { entity_id = entity; }
	
	void	DumpDoor();
	float	GetDestX() { return dest_x; }
	float	GetDestY() { return dest_y; }
	float	GetDestZ() { return dest_z; }
	float	GetDestHeading() { return dest_heading; }

	int8	IsLDoNDoor() { return is_ldon_door; }
	int32	GetClientVersionMask() { return client_version_mask; }
	
	void	NPCOpen(NPC* sender);
	void	ForceOpen(Mob *sender);
	void	ForceClose(Mob *sender);

private:
	
	int32	db_id;
	uint8	door_id;
	char	zone_name[16];
	char	door_name[32];
	float	pos_x;
	float	pos_y;
	float	pos_z;
	float	heading;
	int		incline;
	int8	opentype;
	int32	guild_id;
	int16	lockpick;
	uint32	keyitem;
	int8	nokeyring;
	int8	trigger_door;
	int8	trigger_type;
	int32	door_param;
	int16	size;
	int		invert_state;
	int32	entity_id;
	bool	isopen;
	Timer	close_timer;
	//Timer	trigger_timer;

	char	dest_zone[16];
	int		dest_instance_id;
	float	dest_x;
	float	dest_y;
	float	dest_z;
	float	dest_heading;

	int8	is_ldon_door;
	int32	client_version_mask;
};
#endif
