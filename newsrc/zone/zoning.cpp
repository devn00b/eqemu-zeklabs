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
#include "../common/debug.h"

#include "zone.h"
#include "worldserver.h"
#include "masterentity.h"
#include "../common/packet_dump.h"
#include "../common/rulesys.h"
#include "StringIDs.h"
#include "../KingsBandits/KingsBandits.h"

#ifdef EMBPERL
#include "embparser.h"
#endif

extern WorldServer worldserver;
extern Zone* zone;


void Client::Handle_OP_ZoneChange(const EQApplicationPacket *app) {
#ifdef BOTS
	// This block is necessary to clean up any bot objects owned by a Client
	Bot::ProcessClientZoneChange(this);
#endif

	if (app->size != sizeof(ZoneChange_Struct)) {
		LogFile->write(EQEMuLog::Debug, "Wrong size: OP_ZoneChange, size=%d, expected %d", app->size, sizeof(ZoneChange_Struct));
		return;
	}

#if EQDEBUG >= 5
	LogFile->write(EQEMuLog::Debug, "Zone request from %s", GetName());
	DumpPacket(app);
#endif
	ZoneChange_Struct* zc=(ZoneChange_Struct*)app->pBuffer;

	uint16 target_zone_id = 0;
	uint16 target_instance_id = zc->instanceID;

	// Kings & Bandits zone change fix for bind point Image 4/24/2013 - disabled for Red69 as it causes problems
	// for eastwastes bind points

	char tmp[20];
	database.GetVariable("ServerType", tmp, 9);
	int pvpFlag = atoi(tmp);
	if ( pvpFlag == SERVERTYPE_KAB && ( zone_mode == ZoneToBindPoint || zone_mode == GateToBindPoint ) && zc->zoneID == 116)
		target_instance_id = 5574;

	ZonePoint* zone_point = NULL;
	
	time_t now;
	time(&now);
	uint32 time = (uint32)now;
	
	uint32 zoneDeathsLastSeconds = 0;
	// need a better way to go about this
	if ( zc->instanceID < 10000 )
	{
		if ( !RuleB(PVPCombat,LockoutLastKillOnly) )
			database.GetPlayerZoneDeaths(CharacterID(),(char*)database.GetZoneName(zc->zoneID, zc->instanceID),&zoneDeathsLastSeconds, RuleI(PVPCombat,ZoneDeathDelay), true);

		if ( zoneDeathsLastSeconds > 0 || 
			( RuleB(PVPCombat,LockoutLastKillOnly) && zc->zoneID == GetPP().PVPLastDeath.Zone &&
			( time - GetPP().PVPLastDeath.Time) < RuleI(PVPCombat,ZoneDeathDelay) ) )
		{
			zoneLockoutID = zc->zoneID;
			LogFile->write(EQEMuLog::Error, "Zoning %s: Failed, client died recently in target zone id '%d'.", GetName(), zc->zoneID);
			SendZoneError(zc, ZONE_ERROR_NOMSG);
			return;
		}
	}
	//figure out where they are going.
	if(zc->zoneID == 0 || zone_mode == GateToBindPoint || zone_mode == EvacToSafeCoords || zone_mode == GatePVPDeath ) {
		//client dosent know where they are going...
		//try to figure it out for them.
		
		switch(zone_mode) {
		case EvacToSafeCoords:
		case ZoneToSafeCoords:
			//going to safe coords, but client dosent know where?
			//assume it is this zone for now.
			target_zone_id = zone->GetZoneID();
			break;
		case GMSummon:
			target_zone_id = zonesummon_id;
			break;
		case GateToBindPoint:
			target_zone_id = m_pp.binds[0].zoneId;
			break;
		case ZoneToBindPoint:
			target_zone_id = m_pp.binds[0].zoneId;
			break;
		case GatePVPDeath:
			if ( RuleB(PVPCombat,ForceNexusHubOnDeath) )
					target_zone_id = 152;
			else
					target_zone_id = m_pp.binds[0].zoneId;
			break;
		case ZoneSolicited:  //we told the client to zone somewhere, so we know where they are going.
			target_zone_id = zonesummon_id;
			break;
		case ZoneUnsolicited:   //client came up with this on its own.
			zone_point = zone->GetClosestZonePointWithoutZone(GetX(), GetY(), GetZ(), this, ZONEPOINT_NOZONE_RANGE);
			if(zone_point) {
				//we found a zone point, which is a reasonable distance away
				//assume that is the one were going with.
				target_zone_id = zone_point->target_zone_id;
				target_instance_id = zone_point->target_zone_instance;
			} else if ( clearToZone ) {
				if ( target_zone_id == 0 && zonesummon_id != 0 )
					target_zone_id = zonesummon_id;
				else
					target_zone_id = zc->zoneID;
			}
			else
			{
				//unable to find a zone point... is there anything else
				//that can be a valid un-zolicited zone request?
				
				CheatDetected(MQZone, GetX(),GetY(),GetZ(),zc->x,zc->y,zc->z,target_zone_id);
				Message(13, "Invalid unsolicited zone request.");
				LogFile->write(EQEMuLog::Error, "Zoning %s: Invalid unsolicited zone request to zone id '%d'.", GetName(), target_zone_id);
				SendZoneCancel(zc);
				return;
			}
			break;
		}
	}
	else {
		// This is to allow both 6.2 and Titanium clients to perform a proper zoning of the client when evac/succor
		// WildcardX 27 January 2008
		if(zone_mode == EvacToSafeCoords && zonesummon_id > 0)
			target_zone_id = zonesummon_id;
		else if ( zone_mode == ZoneUnsolicited )
			target_zone_id = zc->zoneID;
		else if ( RuleB(Zone,AllowClientZoning) )
			target_zone_id = zc->zoneID;
		
		//if we are zoning to a specific zone unsolicied,
		//then until otherwise determined, they must be zoning
		//on a zone line.
		if(zone_mode == ZoneUnsolicited)
		{
			if(target_zone_id == zone->GetZoneID())
			{
				SendZoneCancel(zc);
				return;
			}

			zone_point = zone->GetClosestZonePoint(GetX(), GetY(), GetZ(), target_zone_id, this, ZONEPOINT_ZONE_RANGE);
			//if we didnt get a zone point, or its to a different zone,
			//then we assume this is invalid.
			if(!zone_point || zone_point->target_zone_id != target_zone_id) {
				LogFile->write(EQEMuLog::Error, "Zoning %s: Invalid unsolicited zone request to zone id '%d'.", GetName(), target_zone_id);
				int16 zonepointID = 0;

				if ( zone_point )
					zonepointID = zone_point->target_zone_id;
				CheatDetected(MQGate, GetX(), GetY(), GetZ(), zc->x, zc->y, zc->z, zonepointID, zc->zone_reason, target_zone_id);				
				SendZoneCancel(zc);
				return;
			}
		}
	}

	// hardcoded 10000 to avoid instance id's less than 10000 triggering.  These are special zone's.
	if(target_instance_id > 10000)
	{
		//make sure we are in it and it's unexpired.
		if(!database.VerifyInstanceAlive(target_instance_id, CharacterID()))
		{
			Message(13, "Instance ID was expired or you were not in it.");
			SendZoneCancel(zc);
			return;
		}

		if(!database.VerifyZoneInstance(target_zone_id, target_instance_id))
		{
			Message(13, "Instance ID was %u does not go with zone id %u", target_instance_id, target_zone_id);
			SendZoneCancel(zc);
			return;
		}
	}

		//make sure its a valid zone.
	const char *target_zone_name = database.GetZoneName(target_zone_id, target_instance_id);
	if(target_zone_name == NULL) {
		//invalid zone...
		Message(13, "Invalid target zone ID.");
		LogFile->write(EQEMuLog::Error, "Zoning %s: Unable to get zone name for zone id '%d'.", GetName(), target_zone_id);
		SendZoneCancel(zc);
		return;
	}

	//load up the safe coords, restrictions, and verify the zone name
	float safe_x, safe_y, safe_z;
	sint16 minstatus = 0;
	int8 minlevel = 0;
	char flag_needed[128];
	if(!database.GetSafePoints(target_zone_name, database.GetInstanceVersion(target_instance_id), &safe_x, &safe_y, &safe_z, &minstatus, &minlevel, flag_needed)) {
		//invalid zone...
		Message(13, "Invalid target zone while getting safe points.");
		LogFile->write(EQEMuLog::Error, "Zoning %s: Unable to get safe coordinates for zone '%s'.", GetName(), target_zone_name);
		SendZoneCancel(zc);
		return;
	}

	float startX = GetX();
	float startY = GetY();
	float startZ = GetZ();
	//handle circumvention of zone restrictions
	//we need the value when creating the outgoing packet as well.
	int8 ignorerestrictions = zonesummon_ignorerestrictions;
	zonesummon_ignorerestrictions = 0;

	float dest_x=0, dest_y=0, dest_z=0, dest_h;
	dest_h = GetHeading();
	switch(zone_mode) {
	case EvacToSafeCoords:
	case ZoneToSafeCoords:
		LogFile->write(EQEMuLog::Debug, "Zoning %s to safe coords (%f,%f,%f) in %s (%d)", GetName(), safe_x, safe_y, safe_z, target_zone_name, target_zone_id);
		dest_x = safe_x;
		dest_y = safe_y;
		dest_z = safe_z;
		break;
	case GMSummon:
		dest_x = zonesummon_x;
		dest_y = zonesummon_y;
		dest_z = zonesummon_z;
		ignorerestrictions = 1;
		break;
	case GateToBindPoint:
		dest_x = m_pp.binds[0].x;
		dest_y = m_pp.binds[0].y;
		dest_z = m_pp.binds[0].z;
		break;
	case GatePVPDeath:
		{
			if ( RuleB(PVPCombat,ForceNexusHubOnDeath) )
			{
				m_pp.zone_id = 152;
				dest_x = 0;
				dest_y = 0;
				dest_z = -20;
				ignorerestrictions = 1;
			}
			else
			{
				dest_x = m_pp.binds[0].x;
				dest_y = m_pp.binds[0].y;
				dest_z = m_pp.binds[0].z;
				ignorerestrictions = 1;	//can always get to our bind point? seems exploitable
			}
			break;
		}
	case ZoneToBindPoint:
		
			// special kings & bandits override to bind
			if ( m_pp.zone_id == 152 )
			{
				dest_x = 0;
				dest_y = 0;
				dest_z = -20;
				ignorerestrictions = 1;
			}
			else
			{
				dest_x = m_pp.binds[0].x;
				dest_y = m_pp.binds[0].y;
				dest_z = m_pp.binds[0].z;
				ignorerestrictions = 1;	//can always get to our bind point? seems exploitable
			}
		break;
	case ZoneSolicited:  //we told the client to zone somewhere, so we know where they are going.
		//recycle zonesummon variables
		dest_x = zonesummon_x;
		dest_y = zonesummon_y;
		dest_z = zonesummon_z;
		break;
	case ZoneUnsolicited:   //client came up with this on its own.
		//client requested a zoning... what are the cases when this could happen?
		
		//Handle zone point case:
		if(zone_point != NULL) {
			//they are zoning using a valid zone point, figure out coords
			
			//999999 is a placeholder for 'same as where they were from'
			if(zone_point->target_x == 999999)
				dest_x = GetX();
			else
				dest_x = zone_point->target_x;
			if(zone_point->target_y == 999999)
				dest_y = GetY();
			else
				dest_y = zone_point->target_y;
			if(zone_point->target_z == 999999)
				dest_z=GetZ();
			else
				dest_z = zone_point->target_z;
			if(zone_point->target_heading == 999)
				dest_h = GetHeading();
			else
				dest_h = zone_point->target_heading;
			
			break;
		}
		
		//for now, there are no other cases...
		
		//could not find a valid reason for them to be zoning, stop it.
		CheatDetected(MQZoneUnknownDest, startX,startY,startZ,dest_x,dest_y,dest_z,target_zone_id);
		LogFile->write(EQEMuLog::Error, "Zoning %s: Invalid unsolicited zone request to zone id '%s'. Not near a zone point.", GetName(), target_zone_name);
		SendZoneCancel(zc);
		return;
	}

	//OK, now we should know where were going...

	//Check some rules first.
	sint8 myerror = 1;		//1 is succes

	//not sure when we would use ZONE_ERROR_NOTREADY

	//enforce min status and level
	if (!ignorerestrictions && (Admin() < minstatus || GetLevel() < minlevel)) 
	{
		myerror = ZONE_ERROR_NOEXPERIENCE;
	}
	
	if(!ignorerestrictions && flag_needed[0] != '\0') {
		//the flag needed string is not empty, meaning a flag is required.
		if(Admin() < minStatusToIgnoreZoneFlags && !HasZoneFlag(target_zone_id)) 
		{
			LogFile->write(EQEMuLog::Error, "Zoning %s: Flag missing (%s) prevents this char from zoning into '%s'",flag_needed, GetName(), target_zone_name);
			Message(13, "You must have the flag %s to enter this zone.");
			myerror = ZONE_ERROR_NOEXPERIENCE;
		}
	}
	
	//TODO: ADVENTURE ENTRANCE CHECK
	// KaB/red69/zek - new zone code to prevent cheating
	//if( myerror && ( ( target_zone_id != zone->GetZoneID() && clearToZone ) || ( target_zone_id == zone->GetZoneID() ) ) ) {
	if(myerror == 1) {
		//we have successfully zoned
		DoZoneSuccess(zc, target_zone_id, target_instance_id, dest_x, dest_y, dest_z, dest_h, ignorerestrictions);
	} else {
		LogFile->write(EQEMuLog::Error, "Zoning %s: Rules prevent this char from zoning into '%s'", GetName(), target_zone_name);
		CheatDetected(MQZone, GetX(), GetY(), GetZ(), zc->x, zc->y, zc->z, zc->zoneID, zc->zone_reason, target_zone_id);		
		SendZoneError(zc, myerror);
	}
}

void Client::SendZoneCancel(ZoneChange_Struct *zc) {
	//effectively zone them right back to where they were
	//unless we find a better way to stop the zoning process.
	SetPortExemption(true);
	EQApplicationPacket *outapp;
	outapp = new EQApplicationPacket(OP_ZoneChange, sizeof(ZoneChange_Struct));
	ZoneChange_Struct *zc2 = (ZoneChange_Struct*)outapp->pBuffer;
	strcpy(zc2->char_name, zc->char_name);
	zc2->zoneID = zone->GetZoneID();
	zc2->success = 1;
	outapp->priority = 6;
	FastQueuePacket(&outapp);
	
	//reset to unsolicited.
	zone_mode = ZoneUnsolicited;
}

void Client::SendZoneError(ZoneChange_Struct *zc, sint8 err) 
{
	LogFile->write(EQEMuLog::Error, "Zone %i is not available because target wasn't found, character insufficent level or some previous error was logged.", zc->zoneID);
	
	SetPortExemption(true);

	EQApplicationPacket *outapp;
	outapp = new EQApplicationPacket(OP_ZoneChange, sizeof(ZoneChange_Struct));
	ZoneChange_Struct *zc2 = (ZoneChange_Struct*)outapp->pBuffer;
	strcpy(zc2->char_name, zc->char_name);
	zc2->zoneID = zc->zoneID;
	zc2->success = err;
	outapp->priority = 6;
	FastQueuePacket(&outapp);
	
	//reset to unsolicited.
	zone_mode = ZoneUnsolicited;
}

void Client::DoZoneSuccess(ZoneChange_Struct *zc, uint16 zone_id, int32 instance_id, float dest_x, float dest_y, float dest_z, float dest_h, sint8 ignore_r) {
	//this is called once the client is fully allowed to zone here
	//it takes care of all the activities which occur when a client zones out
	
	zoning = true;
	instalog = true;

	SendLogoutPackets();
	
	//dont clear aggro until the zone is successful
	entity_list.RemoveFromHateLists(this);
	
	if(this->GetPet())
		entity_list.RemoveFromHateLists(this->GetPet());
	
	LogFile->write(EQEMuLog::Status, "Zoning '%s' to: %s (%i) - (%i) x=%f, y=%f, z=%f",
		m_pp.name, database.GetZoneName(zone_id, instance_id), zone_id, instance_id,
		dest_x, dest_y, dest_z);
	
#ifdef EMBPERL
		char buf[10];
		snprintf(buf, 9, "%d", zone_id);
		buf[9] = '\0';
		((PerlembParser*)parse)->Event(EVENT_ZONE, 0, buf, (NPC*)NULL, this);
#endif

	//set the player's coordinates in the new zone so they have them
	//when they zone into it
	x_pos = dest_x; //these coordinates will now be saved when ~client is called
	y_pos = dest_y;
	z_pos = dest_z;
	heading = dest_h; // Cripp: fix for zone heading
	m_pp.heading = dest_h;

	// Kings & Bandits - fix for zoning into the wrong zone id, make sure if we have a instance_id that doesnt match then we just use the normal zone
	if ( instance_id > 0 && instance_id < 10000 && !database.GetZoneName(zone_id,instance_id,false,false) )
	{
		m_pp.zoneInstance = 0;
		instance_id = 0;
	}
	else
		m_pp.zoneInstance = instance_id;
	
	//Force a save so its waiting for them when they zone
	Save(2);
	
	if (zone_id == zone->GetZoneID() && instance_id == zone->GetInstanceID()) {
		// No need to ask worldserver if we're zoning to ourselves (most
		// likely to a bind point), also fixes a bug since the default response was failure
		EQApplicationPacket* outapp = new EQApplicationPacket(OP_ZoneChange,sizeof(ZoneChange_Struct));
		ZoneChange_Struct* zc2 = (ZoneChange_Struct*) outapp->pBuffer;
		strcpy(zc2->char_name, GetName());
		zc2->zoneID = zone_id;
		zc2->instanceID = instance_id;
		zc2->success = 1;
		outapp->priority = 6;
		FastQueuePacket(&outapp);
		
		zone->StartShutdownTimer(AUTHENTICATION_TIMEOUT * 1000);
	} else {
	// vesuvias - zoneing to another zone so we need to the let the world server
	//handle things with the client for a while
		ServerPacket* pack = new ServerPacket(ServerOP_ZoneToZoneRequest, sizeof(ZoneToZone_Struct));
		ZoneToZone_Struct* ztz = (ZoneToZone_Struct*) pack->pBuffer;
		ztz->response = 0;
		ztz->current_zone_id = zone->GetZoneID();
		ztz->current_instance_id = zone->GetInstanceID();
		ztz->requested_zone_id = zone_id;
		ztz->requested_instance_id = instance_id;
		ztz->admin = admin;
		ztz->ignorerestrictions = ignore_r;
		strcpy(ztz->name, GetName());
		ztz->guild_id = GuildID();
		worldserver.SendPacket(pack);
		safe_delete(pack);
	}
	
	//reset to unsolicited.
	zone_mode = ZoneUnsolicited;
	zonesummon_x = 0;
	zonesummon_y = 0;
	zonesummon_z = 0;
	zonesummon_id = 0;
	zonesummon_ignorerestrictions = 0;
}

void Client::MovePC(const char* zonename, float x, float y, float z, float heading, int8 ignorerestrictions, ZoneMode zm) {
	ProcessMovePC(database.GetZoneID(zonename), 0, x, y, z, heading, ignorerestrictions, zm);
}

//designed for in zone moving
void Client::MovePC(float x, float y, float z, float heading, int8 ignorerestrictions, ZoneMode zm) {
	ProcessMovePC(zone->GetZoneID(), zone->GetInstanceID(), x, y, z, heading, ignorerestrictions, zm);
}

void Client::MovePC(int32 zoneID, float x, float y, float z, float heading, int8 ignorerestrictions, ZoneMode zm) {
	ProcessMovePC(zoneID, 0, x, y, z, heading, ignorerestrictions, zm);
}

void Client::MovePC(int32 zoneID, int32 instanceID, float x, float y, float z, float heading, int8 ignorerestrictions, ZoneMode zm){
	ProcessMovePC(zoneID, instanceID, x, y, z, heading, ignorerestrictions, zm);
}


void Client::ProcessMovePC(int32 zoneID, int32 instance_id, float x, float y, float z, float heading, int8 ignorerestrictions, ZoneMode zm)
{
	if ( GetZoneMode() == SummonPC && zm != SummonPC )
	{
		QueuedZoneEvent queueevent;
		queueevent.zoneID = zoneID;
		queueevent.instID = instance_id;
		queueevent.x = x;
		queueevent.y = y;
		queueevent.z = z;
		queueevent.heading = heading;
		queueevent.ignorerestrictions = ignorerestrictions;
		queueevent.zonemode = zm;
		zone_event_list.push_back(queueevent);
		return;
	}

	// From what I have read, dragged corpses should stay with the player for Intra-zone summons etc, but we can implement that later.
	ClearDraggedCorpses();

	if(zoneID == 0)
		zoneID = zone->GetZoneID();

	if(zoneID == zone->GetZoneID() && instance_id == zone->GetInstanceID()) {
		// TODO: Determine if this condition is necessary.
		if(IsAIControlled()) {
			GMMove(x, y, z);
			return;
		}

		if(GetPetID() != 0) {
			//if they have a pet and they are staying in zone, move with them
			Mob *p = GetPet();
			if(p != NULL){
				p->SetPetOrder(SPO_Follow);
				p->GMMove(x+15, y, z);	//so it dosent have to run across the map.
			}
		}
	}

	if ( zm != Rewind && zm != ZoneSolicited && zm != SummonPC )
		clearToZone = true;

	switch(zm) {
		case GateToBindPoint:
			ZonePC(zoneID, instance_id, x, y, z, heading, ignorerestrictions, zm);
			break;
		case EvacToSafeCoords:
		case ZoneToSafeCoords:
			ZonePC(zoneID, instance_id, x, y, z, heading, ignorerestrictions, zm);
			break;
		case GMSummon:
			Message(15, "You have been summoned by a GM!");
			ZonePC(zoneID, instance_id, x, y, z, heading, ignorerestrictions, zm);
			break;
		case GatePVPDeath:
			ZonePC(zoneID, instance_id, x, y, z, heading, ignorerestrictions, zm);
			break;
		case ZoneToBindPoint:
			ZonePC(zoneID, instance_id, x, y, z, heading, ignorerestrictions, zm);
			break;
		case ZoneSolicited:
			ZonePC(zoneID, instance_id, x, y, z, heading, ignorerestrictions, zm);
			break;
		case SummonPC:
			Message(15, "You have been summoned!");
			ZonePC(zoneID, instance_id, x, y, z, heading, ignorerestrictions, zm);
			break;
		case Rewind:
			Message(15, "Rewinding to previous location.");
			ZonePC(zoneID, instance_id, x, y, z, heading, ignorerestrictions, zm);
			break;
		default:
			LogFile->write(EQEMuLog::Error, "Client::ProcessMovePC received a reguest to perform an unsupported client zone operation.");
			break;
	}

	// LoZ / Red69 / Zek - new log event for tracking zone events
	char zoneEventMsg[256];
	_snprintf(zoneEventMsg,256,"ZoneRequest (Mode: %i): zoneID (%i) InstanceID (%i). X (%f) Y (%f) Z (%f), ZonePermissionStatus: %i",(int)zm,zoneID,instance_id,x,y,z,clearToZone);
	database.logevents(
			AccountName(),
			AccountID(),
			admin,GetName(),
			GetTarget()?GetTarget()->GetName():"None",
			"ZoneChange",
			zoneEventMsg,
			25,
			GetX(),
			GetY(),
			GetZ(),
			(char*)zone->GetShortName(),
			GetTarget()?GetTarget()->GetX():0,
			GetTarget()?GetTarget()->GetY():0,
			GetTarget()?GetTarget()->GetZ():0
			);
}

void Client::ZonePC(int32 zoneID, int32 instance_id, float x, float y, float z, float heading, int8 ignorerestrictions, ZoneMode zm) {
	bool ReadyToZone = true;
	int iZoneNameLength = 0;
	const char*	pShortZoneName = NULL;
	char* pZoneName = NULL;

	pShortZoneName = database.GetZoneName(zoneID, instance_id);
	database.GetZoneLongName(pShortZoneName, &pZoneName);

	SetPortExemption(true);

	if(!pZoneName) {
		Message(13, "Invalid zone number specified");
		safe_delete_array(pZoneName);
		return;
	}
	iZoneNameLength = strlen(pZoneName);
		
	switch(zm) {
		case EvacToSafeCoords:
		case ZoneToSafeCoords:
			x = zone->safe_x();
			y = zone->safe_y();
			z = zone->safe_z();
			SetHeading(heading);		
			zone_mode = zm; // we know this is a legitimate attempt
			ignorerestrictions = 1;
			break;
		case GMSummon:
			zonesummon_x = x_pos = x;
			zonesummon_y = y_pos = y;
			zonesummon_z = z_pos = z;
			SetHeading(heading);
			
			zonesummon_id = zoneID;
			zonesummon_ignorerestrictions = 1;
			break;
		case ZoneSolicited:
			zonesummon_x = x;
			zonesummon_y = y;
			zonesummon_z = z;
			SetHeading(heading);
			
			zonesummon_id = zoneID;
			zonesummon_ignorerestrictions = ignorerestrictions;
			break;
		case GateToBindPoint:
			x = x_pos = m_pp.binds[0].x;
			y = y_pos = m_pp.binds[0].y;
			z = z_pos = m_pp.binds[0].z;
			heading = m_pp.binds[0].heading;
			ignorerestrictions = 1;
			zone_mode = zm;  // we know this is a legitimate attempt
			break;
		case GatePVPDeath:
			{
				ignorerestrictions = 1;
				zonesummon_ignorerestrictions = 1;
				if (  RuleB(PVPCombat,ForceNexusHubOnDeath) )
				{
					m_pp.zone_id = 152;
					x = x_pos = 0;
					y = y_pos = 0;
					z = z_pos = 0;
					heading = 0;
				}
				else
				{
					x = x_pos = m_pp.binds[0].x;
					y = y_pos = m_pp.binds[0].y;
					z = z_pos = m_pp.binds[0].z;
					heading = m_pp.binds[0].heading;
					zone_mode = ZoneToBindPoint;  // we know this is a legitimate attempt
				}
				break;
			}
		case ZoneToBindPoint:
				x = x_pos = m_pp.binds[0].x;
				y = y_pos = m_pp.binds[0].y;
				z = z_pos = m_pp.binds[0].z;
				heading = m_pp.binds[0].heading;
				ignorerestrictions = 1;
				zone_mode = zm;  // we know this is a legitimate attempt
			
			zonesummon_ignorerestrictions = 1;
			LogFile->write(EQEMuLog::Debug, "Player %s has died and will be zoned to bind point in zone: %s at LOC x=%f, y=%f, z=%f, heading=%f", GetName(), pZoneName, m_pp.binds[0].x, m_pp.binds[0].y, m_pp.binds[0].z, m_pp.binds[0].heading);
			break;
		case SummonPC:
			zonesummon_x = x_pos = x;
			zonesummon_y = y_pos = y;
			zonesummon_z = z_pos = z;
			SetHeading(heading);
			break;
		case Rewind:
			LogFile->write(EQEMuLog::Debug, "%s has requested a /rewind from %f, %f, %f, to %f, %f, %f in %s", GetName(), x_pos, y_pos, z_pos, rewind_x, rewind_y, rewind_z, zone->GetShortName());
			zonesummon_x = x_pos = x;
			zonesummon_y = y_pos = y;
			zonesummon_z = z_pos = z;
			SetHeading(heading);
			break;
		default:
			{
			LogFile->write(EQEMuLog::Error, "Client::ZonePC() received a reguest to perform an unsupported client zone operation.");
			ReadyToZone = false;
			break;
			}
	}

	// this might be a bad idea lol
	/*if ( !ReadyToZone && zone_mode != ZoneUnsolicited )
	{
		zm = zone_mode;
		ReadyToZone = true;
	}*/

	if(ReadyToZone) {
		if (IsLooting())
		{
			Entity* entity = entity_list.GetID(entity_id_being_looted);
			if (entity == 0)
			{
				Message(13, "Error: OP_EndLootRequest: Corpse not found (ent = 0)");
				if (GetClientVersion() >= EQClientSoD)
					Corpse::SendEndLootErrorPacket(this);
				else
					Corpse::SendLootReqErrorPacket(this);
			}
			else if (!entity->IsCorpse())
			{
				Message(13, "Error: OP_EndLootRequest: Corpse not found (!entity->IsCorpse())");
				Corpse::SendLootReqErrorPacket(this);
			}
			else
			{
				Corpse::SendEndLootErrorPacket(this);
				entity->CastToCorpse()->EndLoot(this, NULL);
			}
			SetLooting(0);
		}

		zone_mode = zm;
		if( ( zm == ZoneToBindPoint || zm == GatePVPDeath ) && instance_id < 1) {
			EQApplicationPacket* outapp = new EQApplicationPacket(OP_ZonePlayerToBind, sizeof(ZonePlayerToBind_Struct) + iZoneNameLength);
			ZonePlayerToBind_Struct* gmg = (ZonePlayerToBind_Struct*) outapp->pBuffer;

			// If we are SoF and later and are respawning from hover, we want the real zone ID, else zero to use the old hack.
			//
			if((GetClientVersionBit() & BIT_SoFAndLater) && (!RuleB(Character, RespawnFromHover) || !IsHoveringForRespawn()))
				gmg->bind_zone_id = 0;
			else
				gmg->bind_zone_id = zoneID;

			gmg->x = x;
			gmg->y = y;
			gmg->z = z;
			gmg->heading = heading;
			strcpy(gmg->zone_name, pZoneName);

			outapp->priority = 6;
			FastQueuePacket(&outapp);
			safe_delete(outapp);
		}
		else if(instance_id > 10000 || ( zm == ZoneSolicited || zm == ZoneToSafeCoords )) {
			EQApplicationPacket* outapp = new EQApplicationPacket(OP_RequestClientZoneChange, sizeof(RequestClientZoneChange_Struct));
			RequestClientZoneChange_Struct* gmg = (RequestClientZoneChange_Struct*) outapp->pBuffer;

			gmg->zone_id = zoneID;
			gmg->x = x;
			gmg->y = y;
			gmg->z = z;
			gmg->heading = heading;
			gmg->instance_id = instance_id;
			gmg->type = 0x01;				//an observed value, not sure of meaning

			outapp->priority = 6;
			FastQueuePacket(&outapp);
			safe_delete(outapp);
		}
		else if(zm == EvacToSafeCoords) {
			EQApplicationPacket* outapp = new EQApplicationPacket(OP_RequestClientZoneChange, sizeof(RequestClientZoneChange_Struct));
			RequestClientZoneChange_Struct* gmg = (RequestClientZoneChange_Struct*) outapp->pBuffer;

			// if we are in the same zone we want to evac to, client will not send OP_ZoneChange back to do an actual
			// zoning of the client, so we have to send a viable zoneid that the client *could* zone to to make it believe
			// we are leaving the zone, even though we are not. We have to do this because we are missing the correct op code
			// and struct that should be used for evac/succor.
			// 213 is Plane of War
			// 76 is orignial Plane of Hate
			// WildcardX 27 January 2008. Tested this for 6.2 and Titanium clients.

			if(this->GetZoneID() == 1)
				gmg->zone_id = 2;
			else if(this->GetZoneID() == 2)
				gmg->zone_id = 1;
			else
				gmg->zone_id = 1;

			gmg->x = x;
			gmg->y = y;
			gmg->z = z;
			gmg->heading = heading;
			gmg->instance_id = instance_id;
			gmg->type = 0x01;				// '0x01' was an observed value for the type field, not sure of meaning
			
			// we hide the real zoneid we want to evac/succor to here
			zonesummon_id = zoneID;

			outapp->priority = 6;
			FastQueuePacket(&outapp);
			safe_delete(outapp);
		}
		else {
			if(zoneID == this->GetZoneID()) {
				//properly handle proximities
				entity_list.ProcessMove(this, x_pos, y_pos, z_pos);
				proximity_x = x_pos;
				proximity_y = y_pos;
				proximity_z = z_pos;
				
				//send out updates to people in zone.
				SendPosition();
		
#ifdef PACKET_UPDATE_MANAGER   
				//flush our position queues because we dont know where we will end up
				update_manager.FlushQueues();
#endif
			}
	
			EQApplicationPacket* outapp = new EQApplicationPacket(OP_RequestClientZoneChange, sizeof(RequestClientZoneChange_Struct));
			RequestClientZoneChange_Struct* gmg = (RequestClientZoneChange_Struct*) outapp->pBuffer;
			
			gmg->zone_id = zoneID;
			gmg->x = x;
			gmg->y = y;
			gmg->z = z;
			gmg->heading = heading;
			gmg->instance_id = instance_id;
			gmg->type = 0x01;	//an observed value, not sure of meaning
			//outapp->priority = 5;
			if ( zm == SummonPC )
				FastQueuePacket(&outapp);
			else
				QueuePacket(outapp);
			safe_delete(outapp);
		}

		LogFile->write(EQEMuLog::Debug, "Player %s has requested a zoning to LOC x=%f, y=%f, z=%f, heading=%f in zoneid=%i", GetName(), x, y, z, heading, zoneID);
		//Clear zonesummon variables if we're zoning to our own zone
		//Client wont generate a zone change packet to the server in this case so
		//They aren't needed and it keeps behavior on next zone attempt from being undefined.
		if(zoneID == zone->GetZoneID() && instance_id == zone->GetInstanceID())
		{
			if(zm != EvacToSafeCoords && zm != ZoneToSafeCoords && zm != ZoneToBindPoint && zm != GatePVPDeath)
			{
				zonesummon_x = 0;
				zonesummon_y = 0;
				zonesummon_z = 0;
				zonesummon_id = 0;
				zonesummon_ignorerestrictions = 0;
				zone_mode = ZoneUnsolicited;
			}
		}
	}

	safe_delete_array(pZoneName);
}

void Client::GoToSafeCoords(uint16 zone_id, uint16 instance_id) {
	if(zone_id == 0)
		zone_id = zone->GetZoneID();

	MovePC(zone_id, instance_id, 0.0f, 0.0f, 0.0f, 0.0f, 0, ZoneToSafeCoords);
}


void Mob::Gate() {
	GoToBind();
}

void Client::Gate() {
	Mob::Gate();
}

void NPC::Gate() {
	entity_list.MessageClose_StringID(this, true, 200, MT_Spells, GATES, GetCleanName());

	Mob::Gate();
}

void Client::SetBindPoint(int to_zone, float new_x, float new_y, float new_z, bool zonepointCheck) {
	QGlobal res;
	if ( QGlobalCache::GetQGlobal(res,GetQGlobals()->GetBucket(),"nobindallowed",0,CharacterID(),zone->GetZoneID()) ) {
		Message(13,"You cannot rebind right now as you are flagged to not allow rebinding.");
		return;
	}

	ZonePoint* zone_point = NULL;
	
	if ( !RuleB(Character,AllowBindAtZone) && zonepointCheck )
		zone_point = zone->GetClosestZonePointWithoutZone(GetX(), GetY(), GetZ(), this, RuleR(Character,ReqBindDistanceFromZonePoint));

	if ( zone_point && ( zone_point->x != 0 || zone_point->y != 0 ) )
	{
		Message(13,"You cannot bind here, you are too close to a zone point.");
		return;
	}

	if (to_zone == -1) {
		m_pp.binds[0].zoneId = zone->GetZoneID();
		m_pp.binds[0].x = x_pos;
		m_pp.binds[0].y = y_pos;
		m_pp.binds[0].z = z_pos;
	}
	else {
		m_pp.binds[0].zoneId = to_zone;
		m_pp.binds[0].x = new_x;
		m_pp.binds[0].y = new_y;
		m_pp.binds[0].z = new_z;
	}
}

void Client::GoToBind(uint8 bindnum) {
	// if the bind number is invalid, use the primary bind
	if(bindnum > 4) 
		bindnum = 0;
	
	// move the client, which will zone them if needed.
	// ignore restrictions on the zone request..?
	if(bindnum == 0)
		MovePC(m_pp.binds[0].zoneId, 0.0f, 0.0f, 0.0f, 0.0f, 1, GateToBindPoint);
	else
		MovePC(m_pp.binds[bindnum].zoneId, m_pp.binds[bindnum].x, m_pp.binds[bindnum].y, m_pp.binds[bindnum].z, m_pp.binds[bindnum].heading, 1);
}

void Client::GoToDeath() {
	MovePC(m_pp.binds[0].zoneId, 0.0f, 0.0f, 0.0f, 0.0f, 1, ZoneToBindPoint);
}

void Client::SetZoneFlag(uint32 zone_id) {
	if(HasZoneFlag(zone_id))
		return;
	
	zone_flags.insert(zone_id);
	
	//update the DB
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	
    // Retrieve all waypoints for this grid
    if(!database.RunQuery(query,MakeAnyLenString(&query,
    	"INSERT INTO zone_flags (charID,zoneID) VALUES(%d,%d)",
    	CharacterID(),zone_id),errbuf)) {
		LogFile->write(EQEMuLog::Error, "MySQL Error while trying to set zone flag for %s: %s", GetName(), errbuf);
	}
}

void Client::ClearZoneFlag(uint32 zone_id) {
	if(!HasZoneFlag(zone_id))
		return;
	
	zone_flags.erase(zone_id);
	
	//update the DB
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	
    // Retrieve all waypoints for this grid
    if(!database.RunQuery(query,MakeAnyLenString(&query,
    	"DELETE FROM zone_flags WHERE charID=%d AND zoneID=%d",
    	CharacterID(),zone_id),errbuf)) {
		LogFile->write(EQEMuLog::Error, "MySQL Error while trying to clear zone flag for %s: %s", GetName(), errbuf);
	}
}

void Client::LoadZoneFlags() {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	
    // Retrieve all waypoints for this grid
    if(database.RunQuery(query,MakeAnyLenString(&query,
    	"SELECT zoneID from zone_flags WHERE charID=%d",
    	CharacterID()),errbuf,&result))
    {
		while((row = mysql_fetch_row(result))) {
			zone_flags.insert(atoi(row[0]));
		}
		mysql_free_result(result);
    }
    else	// DB query error!
    {
		LogFile->write(EQEMuLog::Error, "MySQL Error while trying to load zone flags for %s: %s", GetName(), errbuf);
    }
    safe_delete_array(query);
}

bool Client::HasZoneFlag(uint32 zone_id) const {
	return(zone_flags.find(zone_id) != zone_flags.end());
}

void Client::SendZoneFlagInfo(Client *to) const {
	if(zone_flags.empty()) {
		to->Message(0, "%s has no zone flags.", GetName());
		return;
	}
	
	set<uint32>::const_iterator cur, end;
	cur = zone_flags.begin();
	end = zone_flags.end();
	char empty[1] = { '\0' };
	
	to->Message(0, "Flags for %s:", GetName());
	
	for(; cur != end; cur++) {
		uint32 zoneid = *cur;
		
		const char *short_name = database.GetZoneName(zoneid);
		
		char *long_name = NULL;
		database.GetZoneLongName(short_name, &long_name);
		if(long_name == NULL)
			long_name = empty;
		
		float safe_x, safe_y, safe_z;
		sint16 minstatus = 0;
		int8 minlevel = 0;
		char flag_name[128];
		if(!database.GetSafePoints(short_name, 0, &safe_x, &safe_y, &safe_z, &minstatus, &minlevel, flag_name)) {
			strcpy(flag_name, "(ERROR GETTING NAME)");
		}
		
		to->Message(0, "Has Flag %s for zone %s (%d,%s)", flag_name, long_name, zoneid, short_name);
		if(long_name != empty)
			delete[] long_name;
	}
}

bool Client::CanBeInZone() {
	//check some critial rules to see if this char needs to be booted from the zone
    //only enforce rules here which are serious enough to warrant being kicked from
    //the zone

	if(Admin() >= RuleI(GM, MinStatusToZoneAnywhere))
		return(true);
	
	float safe_x, safe_y, safe_z;
	sint16 minstatus = 0;
	int8 minlevel = 0;
	char flag_needed[128];
	printf("Checking if safe point exists.\n");
	if(!database.GetSafePoints(zone->GetShortName(), zone->GetInstanceVersion(), &safe_x, &safe_y, &safe_z, &minstatus, &minlevel, flag_needed)) {
		//this should not happen...
		_log(CLIENT__ERROR, "Unable to query zone info for ourself '%s'", zone->GetShortName());
		return(false);
	}
	
	printf("Checking if character meets min level and status requirements.\n");
	if(GetLevel() < minlevel) {
		_log(CLIENT__ERROR, "Character does not meet min level requirement (%d < %d)!", GetLevel(), minlevel);
		return(false);
	}
	if(Admin() < minstatus) {
		_log(CLIENT__ERROR, "Character does not meet min status requirement (%d < %d)!", Admin(), minstatus);
		return(false);
	}
	
	printf("Checking if flag needed.\n");
	if(flag_needed[0] != '\0') {
	printf("Flag needed: %s\n",flag_needed);
		//the flag needed string is not empty, meaning a flag is required.
		if(Admin() < minStatusToIgnoreZoneFlags && !HasZoneFlag(zone->GetZoneID())) {
			_log(CLIENT__ERROR, "Character does not have the flag to be in this zone (%s)!", flag_needed);
			return(false);
		}
	}

	return(true);
}
