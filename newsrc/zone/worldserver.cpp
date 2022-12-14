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
#include <iostream>
using namespace std;
#include <string.h>
#include <stdio.h>
#include <iomanip>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../KingsBandits/KingsBandits.h"
#ifdef WIN32
#include <process.h>

#define snprintf	_snprintf
#if (_MSC_VER < 1500)
#define vsnprintf	_vsnprintf
#endif
#define strncasecmp	_strnicmp
#define strcasecmp	_stricmp
#endif

#include "../common/servertalk.h"
#include "worldserver.h"
#include "../common/eq_packet_structs.h"
#include "../common/packet_dump.h"
#include "../common/MiscFunctions.h"
#include "zonedb.h"
#include "zone.h"
#include "entity.h"
#include "masterentity.h"
#include "net.h"
#include "petitions.h"
#include "../common/packet_functions.h"
#include "../common/md5.h"
#include "../common/files.h"
#include "ZoneConfig.h"
#include "StringIDs.h"
#include "guild_mgr.h"
#include "../common/rulesys.h"
#include "titles.h"
#include "QGlobals.h"

extern EntityList    entity_list;
extern Zone* zone;
extern volatile bool ZoneLoaded;
extern void CatchSignal(int);
extern WorldServer worldserver;
extern NetConnection net;
extern PetitionList petition_list;
extern int32 numclients;
extern uint32 staticZones;
extern uint32 bootedZones;
extern uint32 availZones;
extern volatile bool RunLoops;

WorldServer::WorldServer()
	: WorldConnection(EmuTCPConnection::packetModeZone)
{
	cur_groupid = 0;
	last_groupid = 0;
	oocmuted = false;
}

WorldServer::~WorldServer() {
}

/*void WorldServer::SendGuildJoin(GuildJoin_Struct* gj){
ServerPacket* pack = new ServerPacket(ServerOP_GuildJoin, sizeof(GuildJoin_Struct));
GuildJoin_Struct* wgj = (GuildJoin_Struct*)pack->pBuffer;
wgj->class_=gj->class_;
wgj->guild_id=gj->guild_id;
wgj->level=gj->level;
strcpy(wgj->name,gj->name);
wgj->rank=gj->rank;
wgj->zoneid=gj->zoneid;
SendPacket(pack);
safe_delete(pack);
}*/

void WorldServer::SetZone(int32 iZoneID, int32 iInstanceID) {
	ServerPacket* pack = new ServerPacket(ServerOP_SetZone, sizeof(SetZone_Struct));
	SetZone_Struct* szs = (SetZone_Struct*) pack->pBuffer;
	szs->zoneid = iZoneID;
	szs->instanceid = iInstanceID;
	if (zone) {
		szs->staticzone = zone->IsStaticZone();
	}
	SendPacket(pack);
	safe_delete(pack);
}

void WorldServer::OnConnected() {
	WorldConnection::OnConnected();

	ServerPacket* pack;

	//tell the launcher what name we were started with.
	pack = new ServerPacket(ServerOP_SetLaunchName,sizeof(LaunchName_Struct));
	LaunchName_Struct* ln = (LaunchName_Struct*)pack->pBuffer;
	strn0cpy(ln->launcher_name, m_launcherName.c_str(), 32);
	strn0cpy(ln->zone_name, m_launchedName.c_str(), 16);
	SendPacket(pack);
	safe_delete(pack);

	pack = new ServerPacket(ServerOP_SetConnectInfo, sizeof(ServerConnectInfo));
	ServerConnectInfo* sci = (ServerConnectInfo*) pack->pBuffer;
	sci->port = ZoneConfig::get()->ZonePort;
	SendPacket(pack);
	safe_delete(pack);

	if (ZoneLoaded) {
		this->SetZone(zone->GetZoneID(), zone->GetInstanceID());
		entity_list.UpdateWho(true);
		this->SendEmoteMessage(0, 0, 15, "Zone connect: %s", zone->GetLongName());
		zone->GetTimeSync();
	} else {
		this->SetZone(0);
	}

	pack = new ServerPacket(ServerOP_LSZoneBoot,sizeof(ZoneBoot_Struct));
	ZoneBoot_Struct* zbs = (ZoneBoot_Struct*)pack->pBuffer;
	strcpy(zbs->compile_time,LAST_MODIFIED);
	SendPacket(pack);
	safe_delete(pack);
}

void WorldServer::Process() {
	_ZP(WorldServer_Process);

	WorldConnection::Process();

	if (!Connected())
		return;

	ServerPacket *pack = 0;
	while((pack = tcpc.PopPacket())) {
		_log(ZONE__WORLD_TRACE,"Got 0x%04x from world:",pack->opcode);
		_hex(ZONE__WORLD_TRACE,pack->pBuffer,pack->size);
		switch(pack->opcode) {
		case 0: {
			break;
				}
		case ServerOP_KeepAlive: {
			// ignore this
			break;
								 }
								 // World is tellins us what port to use.
		case ServerOP_SetConnectInfo: {
			if (pack->size != sizeof(ServerConnectInfo))
				break; 
			ServerConnectInfo* sci = (ServerConnectInfo*) pack->pBuffer;
			_log(ZONE__WORLD,"World indicated port %d for this zone.",sci->port);
			ZoneConfig::SetZonePort(sci->port);
			break;
									  }
		case ServerOP_ZAAuthFailed: {
			cout << "World server responded 'Not Authorized', disabling reconnect" << endl;
			pTryReconnect = false;
			Disconnect();
			break;
									}
		case ServerOP_ChannelMessage: {
			if (!ZoneLoaded) break;
			ServerChannelMessage_Struct* scm = (ServerChannelMessage_Struct*) pack->pBuffer;
			if (scm->deliverto[0] == 0) {
				entity_list.ChannelMessageFromWorld(scm->from, scm->to, scm->chan_num, scm->guilddbid, scm->language, scm->message);
			}
			else {
				Client* client;
				client = entity_list.GetClientByName(scm->deliverto);
				if (client != 0) {
					if (client->Connected()) {
						client->ChannelMessageSend(scm->from, scm->to, scm->chan_num, scm->language, scm->message);
						if (!scm->noreply && scm->chan_num!=2) { //dont echo on group chat
							// if it's a tell, echo back so it shows up
							scm->noreply = true;
							scm->chan_num = 14;
							memset(scm->deliverto, 0, sizeof(scm->deliverto));
							strcpy(scm->deliverto, scm->from);
							pack->Deflate();
							SendPacket(pack);
						}
					}
				}
			}
			break;
									  }
		case ServerOP_VoiceMacro: {

			if (!ZoneLoaded) 
				break;

			ServerVoiceMacro_Struct* svm = (ServerVoiceMacro_Struct*) pack->pBuffer;

			EQApplicationPacket* outapp = new EQApplicationPacket(OP_VoiceMacroOut,sizeof(VoiceMacroOut_Struct));
			VoiceMacroOut_Struct* vmo = (VoiceMacroOut_Struct*)outapp->pBuffer;

			strcpy(vmo->From, svm->From);
			vmo->Type = svm->Type;
			vmo->Voice =svm->Voice;
			vmo->MacroNumber = svm->MacroNumber;

			switch(svm->Type) {
			case VoiceMacroTell: {
				Client* c = entity_list.GetClientByName(svm->To);
				if(!c) 
					break;

				c->QueuePacket(outapp);
				break;
								 }

			case VoiceMacroGroup: {
				Group* g = entity_list.GetGroupByID(svm->GroupID);

				if(!g) 
					break;

				for(unsigned int i = 0; i < MAX_GROUP_MEMBERS; i++) {
					if(g->members[i] && g->members[i]->IsClient())
						g->members[i]->CastToClient()->QueuePacket(outapp);

				}
				break;
								  }

			case VoiceMacroRaid: {
				Raid *r = entity_list.GetRaidByID(svm->RaidID);

				if(!r) 
					break;

				for(int i = 0; i < MAX_RAID_MEMBERS; i++)
					if(r->members[i].member)
						r->members[i].member->QueuePacket(outapp);

				break;
								 }
			}
			safe_delete(outapp);
			break;
								  }

		case ServerOP_SpawnCondition: {
			if(pack->size != sizeof(ServerSpawnCondition_Struct))
				break;
			if (!ZoneLoaded)
				break;
			ServerSpawnCondition_Struct* ssc = (ServerSpawnCondition_Struct*) pack->pBuffer;

			zone->spawn_conditions.SetCondition(zone->GetShortName(), zone->GetInstanceID(), ssc->condition_id, ssc->value, true);
			break;		
									  }
		case ServerOP_SpawnEvent: {
			if(pack->size != sizeof(ServerSpawnEvent_Struct))
				break;
			if (!ZoneLoaded)
				break;
			ServerSpawnEvent_Struct* sse = (ServerSpawnEvent_Struct*) pack->pBuffer;

			zone->spawn_conditions.ReloadEvent(sse->event_id);

			break;		
								  }
		case ServerOP_StatAuth: {
			
			if(pack->size != sizeof(ZoneStatusAuth_Struct))
				break;
			ZoneStatusAuth_Struct* zsa = (ZoneStatusAuth_Struct*) pack->pBuffer;
			
			staticZones = zsa->staticZones;
			bootedZones = zsa->bootedZones;
			availZones = zsa->availZones;

			break;
								}
		case ServerOP_AcceptWorldEntrance: {
			if(pack->size != sizeof(WorldToZone_Struct))
				break;
			if (!ZoneLoaded)
				break;
			WorldToZone_Struct* wtz = (WorldToZone_Struct*) pack->pBuffer;

			if(zone->GetMaxClients() != 0 && numclients >= zone->GetMaxClients())
				wtz->response = -1;
			else
				wtz->response = 1;

			SendPacket(pack);
			break;		
										   }
		case ServerOP_ZoneToZoneRequest: {
			if(pack->size != sizeof(ZoneToZone_Struct))
				break;
			if (!ZoneLoaded)
				break;
			ZoneToZone_Struct* ztz = (ZoneToZone_Struct*) pack->pBuffer;

			if(ztz->current_zone_id == zone->GetZoneID() 
				&& ztz->current_instance_id == zone->GetInstanceID()) {
					// it's a response
					Entity* entity = entity_list.GetClientByName(ztz->name);
					if(entity == 0)
						break;

					EQApplicationPacket *outapp;
					outapp = new EQApplicationPacket(OP_ZoneChange,sizeof(ZoneChange_Struct));
					ZoneChange_Struct* zc2=(ZoneChange_Struct*)outapp->pBuffer;

					if(ztz->response <= 0) {
						zc2->success = ZONE_ERROR_NOTREADY;
						entity->CastToMob()->SetZone(ztz->current_zone_id, ztz->current_instance_id);
					}
					else {
						entity->CastToClient()->UpdateWho(1);
						strn0cpy(zc2->char_name,entity->CastToMob()->GetName(),64);
						zc2->zoneID=ztz->requested_zone_id;
						zc2->instanceID=ztz->requested_instance_id;
						zc2->success = 1;

						entity->CastToMob()->SetZone(ztz->requested_zone_id, ztz->requested_instance_id);

						if(ztz->ignorerestrictions == 3)
							entity->CastToClient()->GoToSafeCoords(ztz->requested_zone_id, ztz->requested_instance_id);
					}

					outapp->priority = 6;
					entity->CastToClient()->QueuePacket(outapp);
					safe_delete(outapp);

					switch(ztz->response)
					{
					case -2: {
						entity->CastToClient()->Message(13,"You do not own the required locations to enter this zone.");
						break;
							 }
					case -1: {
						entity->CastToClient()->Message(13,"The zone is currently full, please try again later.");
						break;
							 }
					case 0:	{
						entity->CastToClient()->Message(13,"All zone servers are taken at this time, please try again later.");
						break;
							}
					}
			}
			else {
				// it's a request
				ztz->response = 0;

				if(zone->GetMaxClients() != 0 && numclients >= zone->GetMaxClients())
					ztz->response = -1;
				else {
					ztz->response = 1;
					// since they asked about comming, lets assume they are on their way and not shut down.
					zone->StartShutdownTimer(AUTHENTICATION_TIMEOUT * 1000);
				}

				SendPacket(pack);
				break;
			}
			break;
										 }
		case ServerOP_WhoAllReply:{
			if(!ZoneLoaded)
				break;


			WhoAllReturnStruct* wars= (WhoAllReturnStruct*)pack->pBuffer;
			if (wars && wars->id!=0 && wars->id<0xFFFFFFFF){
				Client* client = entity_list.GetClientByID(wars->id);
				if (client) {
					if(pack->size==64)//no results
						client->Message_StringID(0,WHOALL_NO_RESULTS);
					else{
						EQApplicationPacket* outapp = new EQApplicationPacket(OP_WhoAllResponse, pack->size);
						memcpy(outapp->pBuffer, pack->pBuffer, pack->size);
						client->QueuePacket(outapp);
						safe_delete(outapp);
					}
				}
				else {
#ifdef _EQDEBUG
					_log(ZONE__WORLD, "Error: WhoAllReturnStruct did not point to a valid client!  "
						"id=%i, playerineqstring=%i, playersinzonestring=%i.  Dumping WhoAllReturnStruct:",
						wars->id, wars->playerineqstring, wars->playersinzonestring);
					//DumpPacket(pack);
#endif
				}
			}
			else
				_log(ZONE__WORLD_ERR, "WhoAllReturnStruct: Could not get return struct!");
			break;
								  }
		case ServerOP_EmoteMessage: {
			if (!ZoneLoaded)
				break;
			ServerEmoteMessage_Struct* sem = (ServerEmoteMessage_Struct*) pack->pBuffer;
			if (sem->to[0] != 0) {
				if (strcasecmp(sem->to, zone->GetShortName()) == 0)
					entity_list.MessageStatus(sem->guilddbid, sem->minstatus, sem->type, (char*)sem->message);
				else {
					Client* client = entity_list.GetClientByName(sem->to);
					if (client != 0){
						char* newmessage=0;
						if(strstr(sem->message,"^")==0)
							client->Message(sem->type, (char*)sem->message);
						else{
							for(newmessage = strtok((char*)sem->message,"^");newmessage!=NULL;newmessage=strtok(NULL, "^"))
								client->Message(sem->type, newmessage);
						}
					}
				}
			}
			else{
				char* newmessage=0;
				if(strstr(sem->message,"^")==0)
					entity_list.MessageStatus(sem->guilddbid, sem->minstatus, sem->type, sem->message);
				else{
					for(newmessage = strtok((char*)sem->message,"^");newmessage!=NULL;newmessage=strtok(NULL, "^"))
						entity_list.MessageStatus(sem->guilddbid, sem->minstatus, sem->type, newmessage);
				}
			}
			break;
									}
		case ServerOP_Motd: {
			ServerMotd_Struct* smotd = (ServerMotd_Struct*) pack->pBuffer;
			EQApplicationPacket *outapp;
			outapp = new EQApplicationPacket(OP_MOTD);
			char tmp[500] = {0};
			sprintf(tmp, "%s", smotd->motd);

			outapp->size = strlen(tmp)+1;
			outapp->pBuffer = new uchar[outapp->size];
			memset(outapp->pBuffer,0,outapp->size);
			strcpy((char*)outapp->pBuffer, tmp);

			entity_list.QueueClients(0, outapp);
			safe_delete(outapp);

			break;
							}
		case ServerOP_ShutdownAll: {
			entity_list.Save();
			CatchSignal(2);
			break;
								   }
		case ServerOP_ZoneShutdown: {
			if (pack->size != sizeof(ServerZoneStateChange_struct)) {
				cout << "Wrong size on ServerOP_ZoneShutdown. Got: " << pack->size << ", Expected: " << sizeof(ServerZoneStateChange_struct) << endl;
				break;
			}
			// Annouce the change to the world
			if (!ZoneLoaded) {
				SetZone(0);
			}
			else {
				SendEmoteMessage(0, 0, 15, "Zone shutdown: %s", zone->GetLongName());

				ServerZoneStateChange_struct* zst = (ServerZoneStateChange_struct *) pack->pBuffer;
				cout << "Zone shutdown by " << zst->adminname << endl;
				Zone::Shutdown();
			}
			break;
									}
		case ServerOP_ZoneBootup: {
			if (pack->size != sizeof(ServerZoneStateChange_struct)) {
				cout << "Wrong size on ServerOP_ZoneBootup. Got: " << pack->size << ", Expected: " << sizeof(ServerZoneStateChange_struct) << endl;
				break;
			}
			ServerZoneStateChange_struct* zst = (ServerZoneStateChange_struct *) pack->pBuffer;
			if (ZoneLoaded) {
				SetZone(zone->GetZoneID(), zone->GetInstanceID());
				if (zst->zoneid == zone->GetZoneID()) {
					// This packet also doubles as "incomming client" notification, lets not shut down before they get here
					zone->StartShutdownTimer(AUTHENTICATION_TIMEOUT * 1000);
				}
				else {
					SendEmoteMessage(zst->adminname, 0, 0, "Zone bootup failed: Already running '%s'", zone->GetShortName());
				}
				break;
			}

			if (zst->adminname[0] != 0)
				cout << "Zone bootup by " << zst->adminname << endl;

			if (!(Zone::Bootup(zst->zoneid, zst->instanceid, zst->makestatic))) {
				SendChannelMessage(0, 0, 10, 0, 0, "%s:%i Zone::Bootup failed: %s", net.GetZoneAddress(), net.GetZonePort(), database.GetZoneName(zst->zoneid));
			}
			// Moved annoucement to ZoneBootup() - Quagmire
			//			else
			//				SendEmoteMessage(0, 0, 15, "Zone bootup: %s", zone->GetLongName());
			break;
								  }
		case ServerOP_ZoneIncClient: {
			if (pack->size != sizeof(ServerZoneIncommingClient_Struct)) {
				cout << "Wrong size on ServerOP_ZoneIncClient. Got: " << pack->size << ", Expected: " << sizeof(ServerZoneIncommingClient_Struct) << endl;
				break;
			}
			ServerZoneIncommingClient_Struct* szic = (ServerZoneIncommingClient_Struct*) pack->pBuffer;
			if (ZoneLoaded) {
				SetZone(zone->GetZoneID(), zone->GetInstanceID());
				if (szic->zoneid == zone->GetZoneID()) {
					zone->AddAuth(szic);
					// This packet also doubles as "incomming client" notification, lets not shut down before they get here
					zone->StartShutdownTimer(AUTHENTICATION_TIMEOUT * 1000);
				}
			}
			else {
				if ((Zone::Bootup(szic->zoneid, szic->instanceid))) {
					zone->AddAuth(szic);
				}
				else
					SendEmoteMessage(0, 0, 100, 0, "%s:%i Zone::Bootup failed: %s (%i)", net.GetZoneAddress(), net.GetZonePort(), database.GetZoneName(szic->zoneid, true), szic->zoneid);
			}
			break;
									 }
		case ServerOP_ZonePlayer: {
			ServerZonePlayer_Struct* szp = (ServerZonePlayer_Struct*) pack->pBuffer;
			Client* client = entity_list.GetClientByName(szp->name);
			printf("Zoning %s to %s(%u) - %u\n", client != NULL ? client->GetCleanName() : "Unknown", szp->zone, database.GetZoneID(szp->zone), szp->instance_id);
			if (client != 0) {
				if (strcasecmp(szp->adminname, szp->name) == 0)
					client->Message(0, "Zoning to: %s", szp->zone);
				else if (client->GetAnon() == 1 && client->Admin() > szp->adminrank)
					break;
				else {
					SendEmoteMessage(szp->adminname, 0, 0, "Summoning %s to %s %1.1f, %1.1f, %1.1f", szp->name, szp->zone, szp->x_pos, szp->y_pos, szp->z_pos);
				}
				client->MovePC(database.GetZoneID(szp->zone), szp->instance_id, szp->x_pos, szp->y_pos, szp->z_pos, client->GetHeading(), szp->ignorerestrictions, GMSummon);
			}
			break;
								  }
		case ServerOP_KickPlayer: {
			ServerKickPlayer_Struct* skp = (ServerKickPlayer_Struct*) pack->pBuffer;
			Client* client = entity_list.GetClientByName(skp->name);
			if (client != 0) {
				if (skp->adminrank >= client->Admin()) {
					client->WorldKick();
					if (ZoneLoaded)
						SendEmoteMessage(skp->adminname, 0, 0, "Remote Kick: %s booted in zone %s.", skp->name, zone->GetShortName());
					else
						SendEmoteMessage(skp->adminname, 0, 0, "Remote Kick: %s booted.", skp->name);
				}
				else if (client->GetAnon() != 1)
					SendEmoteMessage(skp->adminname, 0, 0, "Remote Kick: Your avatar level is not high enough to kick %s", skp->name);
			}
			break;
								  }
		case ServerOP_KillPlayer: {
			ServerKillPlayer_Struct* skp = (ServerKillPlayer_Struct*) pack->pBuffer;
			Client* client = entity_list.GetClientByName(skp->target);
			if (client != 0) {
				if (skp->admin >= client->Admin()) {
					client->GMKill();
					if (ZoneLoaded)
						SendEmoteMessage(skp->gmname, 0, 0, "Remote Kill: %s killed in zone %s.", skp->target, zone->GetShortName());
					else
						SendEmoteMessage(skp->gmname, 0, 0, "Remote Kill: %s killed.", skp->target);
				}
				else if (client->GetAnon() != 1)
					SendEmoteMessage(skp->gmname, 0, 0, "Remote Kill: Your avatar level is not high enough to kill %s", skp->target);
			}
			break;
								  }

								  //hand all the guild related packets to the guild manager for processing.
		case ServerOP_OnlineGuildMembersResponse:
		case ServerOP_RefreshGuild:
			//		case ServerOP_GuildInvite:
		case ServerOP_DeleteGuild:
		case ServerOP_GuildCharRefresh:
		case ServerOP_GuildMemberUpdate:
		case ServerOP_GuildRankUpdate:
			//		case ServerOP_GuildGMSet:
			//		case ServerOP_GuildGMSetRank:
			//		case ServerOP_GuildJoin:
			guild_mgr.ProcessWorldPacket(pack);
			break;

		case ServerOP_FlagUpdate: {
			Client* client = entity_list.GetClientByAccID(*((int32*) pack->pBuffer));
			if (client != 0) {
				client->UpdateAdmin();
			}
			break;
								  }
		case ServerOP_GMGoto: {
			if (pack->size != sizeof(ServerGMGoto_Struct)) {
				cout << "Wrong size on ServerOP_GMGoto. Got: " << pack->size << ", Expected: " << sizeof(ServerGMGoto_Struct) << endl;
				break;
			}
			if (!ZoneLoaded)
				break;
			ServerGMGoto_Struct* gmg = (ServerGMGoto_Struct*) pack->pBuffer;
			Client* client = entity_list.GetClientByName(gmg->gotoname);
			if (client != 0) {
				SendEmoteMessage(gmg->myname, 0, 13, "Summoning you to: %s @ %s, %1.1f, %1.1f, %1.1f", client->GetName(), zone->GetShortName(), client->GetX(), client->GetY(), client->GetZ());
				ServerPacket* outpack = new ServerPacket(ServerOP_ZonePlayer, sizeof(ServerZonePlayer_Struct));
				ServerZonePlayer_Struct* szp = (ServerZonePlayer_Struct*) outpack->pBuffer;
				strcpy(szp->adminname, gmg->myname);
				strcpy(szp->name, gmg->myname);
				strcpy(szp->zone, zone->GetShortName());
				szp->instance_id = zone->GetInstanceID();
				szp->x_pos = client->GetX();
				szp->y_pos = client->GetY();
				szp->z_pos = client->GetZ();
				SendPacket(outpack);
				safe_delete(outpack);
			}
			else {
				SendEmoteMessage(gmg->myname, 0, 13, "Error: %s not found", gmg->gotoname);
			}
			break;
							  }
		case ServerOP_MultiLineMsg: {
			ServerMultiLineMsg_Struct* mlm = (ServerMultiLineMsg_Struct*) pack->pBuffer;
			Client* client = entity_list.GetClientByName(mlm->to);
			if (client) {
				EQApplicationPacket* outapp = new EQApplicationPacket(OP_MultiLineMsg, strlen(mlm->message));
				strcpy((char*) outapp->pBuffer, mlm->message);
				client->QueuePacket(outapp);
				safe_delete(outapp);
			}
			break;
									}
		case ServerOP_Uptime: {
			if (pack->size != sizeof(ServerUptime_Struct)) {
				cout << "Wrong size on ServerOP_Uptime. Got: " << pack->size << ", Expected: " << sizeof(ServerUptime_Struct) << endl;
				break;
			}
			ServerUptime_Struct* sus = (ServerUptime_Struct*) pack->pBuffer;
			int32 ms = Timer::GetCurrentTime();
			int32 d = ms / 86400000;
			ms -= d * 86400000;
			int32 h = ms / 3600000;
			ms -= h * 3600000;
			int32 m = ms / 60000;
			ms -= m * 60000;
			int32 s = ms / 1000;
			if (d)
				this->SendEmoteMessage(sus->adminname, 0, 0, "Zone #%i Uptime: %02id %02ih %02im %02is", sus->zoneserverid, d, h, m, s);
			else if (h)
				this->SendEmoteMessage(sus->adminname, 0, 0, "Zone #%i Uptime: %02ih %02im %02is", sus->zoneserverid, h, m, s);
			else
				this->SendEmoteMessage(sus->adminname, 0, 0, "Zone #%i Uptime: %02im %02is", sus->zoneserverid, m, s);
							  }
		case ServerOP_Petition: {
			cout << "Got Server Requested Petition List Refresh" << endl;
			ServerPetitionUpdate_Struct* sus = (ServerPetitionUpdate_Struct*) pack->pBuffer;
			// solar: this was typoed to = instead of ==, not that it acts any different now though..
			if (sus->status == 0) petition_list.ReadDatabase();
			else if (sus->status == 1) petition_list.ReadDatabase(); // Until I fix this to be better....
			break;
								}
		case ServerOP_RezzPlayer: {
			RezzPlayer_Struct* srs = (RezzPlayer_Struct*) pack->pBuffer;
			if (srs->rezzopcode == OP_RezzRequest)
			{
				// The Rezz request has arrived in the zone the player to be rezzed is currently in,
				// so we send the request to their client which will bring up the confirmation box.
				Client* client = entity_list.GetClientByName(srs->rez.your_name);
				if (client)
				{
					if(client->IsRezzPending())
					{
						ServerPacket * Response = new ServerPacket(ServerOP_RezzPlayerReject, strlen(srs->rez.rezzer_name) + 1);

						char *Buffer = (char *)Response->pBuffer;
						sprintf(Buffer, "%s", srs->rez.rezzer_name);
						worldserver.SendPacket(Response);
						safe_delete(Response);
						break;
					}
					//pendingrezexp is the amount of XP on the corpse. Setting it to a value >= 0
					//also serves to inform Client::OPRezzAnswer to expect a packet.
					// Red69 / Zek / KaB - Sep 28 2012 - Modified SetPendingRezzData to take in the Ressurect_Struct from the other zone to properly set the client destination for a rez.
					client->SetPendingRezzData(srs->exp, srs->rez.spellid, srs->rez.corpse_name, &srs->rez);
					_log(SPELLS__REZ, "OP_RezzRequest in zone %s for %s, spellid:%i", 
						zone->GetShortName(), client->GetName(), srs->rez.spellid);
					EQApplicationPacket* outapp = new EQApplicationPacket(OP_RezzRequest, 
						sizeof(Resurrect_Struct));
					memcpy(outapp->pBuffer, &srs->rez, sizeof(Resurrect_Struct));
					client->QueuePacket(outapp);
					_pkt(SPELLS__REZ, outapp);
					safe_delete(outapp);
					break;	
				}
			}
			if (srs->rezzopcode == OP_RezzComplete){
				// We get here when the Rezz complete packet has come back via the world server
				// to the zone that the corpse is in.
				Corpse* corpse = entity_list.GetCorpseByName(srs->rez.corpse_name);
				if (corpse && corpse->IsCorpse()) {
					_log(SPELLS__REZ, "OP_RezzComplete received in zone %s for corpse %s",
						zone->GetShortName(), srs->rez.corpse_name);

					_log(SPELLS__REZ, "Found corpse. Marking corpse as rezzed.");
					// I don't know why Rezzed is not set to true in CompleteRezz().
					corpse->Rezzed(true);
					corpse->CompleteRezz();
				}
			}

			break;
								  }
		case ServerOP_RezzPlayerReject:
			{
				char *Rezzer = (char *)pack->pBuffer;

				Client* c = entity_list.GetClientByName(Rezzer);

				if (c)
					c->Message_StringID(MT_WornOff, REZZ_ALREADY_PENDING);

				break;
			}
		case ServerOP_ZoneReboot: {
			cout << "Got Server Requested Zone reboot" << endl;
			ServerZoneReboot_Struct* zb = (ServerZoneReboot_Struct*) pack->pBuffer;
			//	printf("%i\n",zb->zoneid);
			struct in_addr	in;
			in.s_addr = GetIP();
#ifdef WIN32
			char buffer[200];
			snprintf(buffer,200,". %s %i %s",zb->ip2, zb->port, inet_ntoa(in));
			if(zb->zoneid != 0) {
				snprintf(buffer,200,"%s %s %i %s",database.GetZoneName(zb->zoneid),zb->ip2, zb->port ,inet_ntoa(in));
				cout << "executing: " << buffer; 
				ShellExecute(0,"Open",net.GetZoneFileName(), buffer, 0, SW_SHOWDEFAULT);
			}
			else
			{
				cout << "executing: " << net.GetZoneFileName() << " " << buffer; 
				ShellExecute(0,"Open",net.GetZoneFileName(), buffer, 0, SW_SHOWDEFAULT);
			}
#else
			char buffer[5];
			snprintf(buffer,5,"%i",zb->port); //just to be sure that it will work on linux	
			if(zb->zoneid != 0)
				execl(net.GetZoneFileName(),net.GetZoneFileName(),database.GetZoneName(zb->zoneid),zb->ip2, buffer,inet_ntoa(in), NULL);
			else
				execl(net.GetZoneFileName(),net.GetZoneFileName(),".",zb->ip2, buffer,inet_ntoa(in), NULL);
#endif
			break;
								  }
		case ServerOP_SyncWorldTime: {
			if(zone!=0) {
				cout << "Received Message SyncWorldTime" << endl;
				eqTimeOfDay* newtime = (eqTimeOfDay*) pack->pBuffer;
				zone->zone_time.setEQTimeOfDay(newtime->start_eqtime, newtime->start_realtime);
				EQApplicationPacket* outapp = new EQApplicationPacket(OP_TimeOfDay, sizeof(TimeOfDay_Struct));
				TimeOfDay_Struct* tod = (TimeOfDay_Struct*)outapp->pBuffer;
				zone->zone_time.getEQTimeOfDay(time(0), tod);
				entity_list.QueueClients(0, outapp, false);
				safe_delete(outapp);
				//TEST
				char timeMessage[255];
				time_t timeCurrent = time(NULL);
				TimeOfDay_Struct eqTime;
				zone->zone_time.getEQTimeOfDay( timeCurrent, &eqTime);
				//if ( eqTime.hour >= 0 && eqTime.minute >= 0 )
				//{
				sprintf(timeMessage,"EQTime [%02d:%s%d %s]",
					((eqTime.hour - 1) % 12) == 0 ? 12 : ((eqTime.hour - 1) % 12),
					(eqTime.minute < 10) ? "0" : "",
					eqTime.minute,
					(eqTime.hour >= 13) ? "pm" : "am"
					);
				cout << "Time Broadcast Packet: " << timeMessage << endl;
				zone->GotCurTime(true);
				//}
				//Test
			}
			break;
									 }
		case ServerOP_ChangeWID: {
			if (pack->size != sizeof(ServerChangeWID_Struct)) {
				cout << "Wrong size on ServerChangeWID_Struct. Got: " << pack->size << ", Expected: " << sizeof(ServerChangeWID_Struct) << endl;
				break;
			}
			ServerChangeWID_Struct* scw = (ServerChangeWID_Struct*) pack->pBuffer;
			Client* client = entity_list.GetClientByCharID(scw->charid);
			if (client)
				client->SetWID(scw->newwid);
			break;
								 }
		case ServerOP_ItemStatus: {
			if (pack->size != 5) {
				cout << "Wrong size on ServerChangeWID_Struct. Got: " << pack->size << ", Expected: 5" << endl;
				break;
			}
			database.SetItemStatus(*((int32*) &pack->pBuffer[0]), *((int8*) &pack->pBuffer[4]));
			break;
								  }
		case ServerOP_OOCMute: {
			oocmuted = *(pack->pBuffer);
			break;
							   }
		case ServerOP_Revoke: {					
			RevokeStruct* rev = (RevokeStruct*) pack->pBuffer;
			Client* client = entity_list.GetClientByName(rev->name);
			if (client)
			{
				SendEmoteMessage(rev->adminname, 0, 0, "%s: %srevoking %s", zone->GetShortName(), rev->toggle?"":"un", client->GetName());
				client->SetRevoked(rev->toggle);
			}
#if EQDEBUG >= 6
			else
				SendEmoteMessage(rev->adminname, 0, 0, "%s: Can't find %s", zone->GetShortName(), rev->name);
#endif
			break;
							  }
		case ServerOP_GroupIDReply: {
			ServerGroupIDReply_Struct* ids = (ServerGroupIDReply_Struct*) pack->pBuffer;
			cur_groupid = ids->start;
			last_groupid = ids->end;
#ifdef _EQDEBUG
			printf("Got new group id set: %lu -> %lu\n", (unsigned long)cur_groupid, (unsigned long)last_groupid);
#endif
			break;
									}
		case ServerOP_GroupLeave: {
			ServerGroupLeave_Struct* gl = (ServerGroupLeave_Struct*)pack->pBuffer;
			if(zone){
				if(gl->zoneid == zone->GetZoneID() && gl->instance_id == zone->GetInstanceID())
					break;

				entity_list.SendGroupLeave(gl->gid, gl->member_name);
			}
			break;
								  }

		case ServerOP_GroupJoin: {
			ServerGroupJoin_Struct* gj = (ServerGroupJoin_Struct*)pack->pBuffer;
			if(zone){
				if(gj->zoneid == zone->GetZoneID() && gj->instance_id == zone->GetInstanceID())
					break;

				entity_list.SendGroupJoin(gj->gid, gj->member_name);
			}
			break;
								 }

		case ServerOP_ForceGroupUpdate: {
			ServerForceGroupUpdate_Struct* fgu = (ServerForceGroupUpdate_Struct*)pack->pBuffer;
			if(zone){
				if(fgu->origZoneID == zone->GetZoneID()	&& fgu->instance_id == zone->GetInstanceID())
					break;

				entity_list.ForceGroupUpdate(fgu->gid);
			}
			break;
										}

		case ServerOP_OOZGroupMessage: {
			ServerGroupChannelMessage_Struct* gcm = (ServerGroupChannelMessage_Struct*)pack->pBuffer;
			if(zone){
				if(gcm->zoneid == zone->GetZoneID() && gcm->instanceid == zone->GetInstanceID())
					break;

				entity_list.GroupMessage(gcm->groupid, gcm->from, gcm->message);
			}
			break;							   
									   }
		case ServerOP_DisbandGroup: {
			ServerDisbandGroup_Struct* sd = (ServerDisbandGroup_Struct*)pack->pBuffer;
			if(zone){
				if(sd->zoneid == zone->GetZoneID() && sd->instance_id == zone->GetInstanceID())
					break;

				Group *g = entity_list.GetGroupByID(sd->groupid);
				if(g)
					g->DisbandGroup();
			}
			break;
									}
		case ServerOP_RaidAdd:{
			ServerRaidGeneralAction_Struct* rga = (ServerRaidGeneralAction_Struct*)pack->pBuffer;
			if(zone){
				if(rga->zoneid == zone->GetZoneID() && rga->instance_id == zone->GetInstanceID())
					break;

				Raid *r = entity_list.GetRaidByID(rga->rid);
				if(r){
					r->LearnMembers();
					r->VerifyRaid();
					r->SendRaidAddAll(rga->playername);
				}
			}
			break;
							  }

		case ServerOP_RaidRemove:{
			ServerRaidGeneralAction_Struct* rga = (ServerRaidGeneralAction_Struct*)pack->pBuffer;
			if(zone){
				if(rga->zoneid == zone->GetZoneID() && rga->instance_id == zone->GetInstanceID())
					break;

				Raid *r = entity_list.GetRaidByID(rga->rid);
				if(r){
					r->SendRaidRemoveAll(rga->playername);
					Client *rem = entity_list.GetClientByName(rga->playername);
					if(rem){
						r->SendRaidDisband(rem);
					}
					r->LearnMembers();
					r->VerifyRaid();					
				}
			}
			break;
								 }

		case ServerOP_RaidDisband:{
			ServerRaidGeneralAction_Struct* rga = (ServerRaidGeneralAction_Struct*)pack->pBuffer;
			if(zone){
				if(rga->zoneid == zone->GetZoneID() && rga->instance_id == zone->GetInstanceID())
					break;

				Raid *r = entity_list.GetRaidByID(rga->rid);
				if(r){
					r->SendRaidDisbandAll();
					r->LearnMembers();
					r->VerifyRaid();
				}
			}
			break;
								  }

		case ServerOP_RaidLockFlag:{
			ServerRaidGeneralAction_Struct* rga = (ServerRaidGeneralAction_Struct*)pack->pBuffer;
			if(zone){
				if(rga->zoneid == zone->GetZoneID() && rga->instance_id == zone->GetInstanceID())
					break;

				Raid *r = entity_list.GetRaidByID(rga->rid);
				if(r){
					r->GetRaidDetails(); //update our details
					if(rga->gid)
						r->SendRaidLock();
					else
						r->SendRaidUnlock();
				}
			}
			break;
								   }

		case ServerOP_RaidChangeGroup:{
			ServerRaidGeneralAction_Struct* rga = (ServerRaidGeneralAction_Struct*)pack->pBuffer;
			if(zone){
				if(rga->zoneid == zone->GetZoneID() && rga->instance_id == zone->GetInstanceID())
					break;

				Raid *r = entity_list.GetRaidByID(rga->rid);
				if(r){
					r->LearnMembers();
					r->VerifyRaid();
					Client *c = entity_list.GetClientByName(rga->playername);
					if(c){
						r->SendRaidDisband(c);
						r->SendRaidRemoveAll(rga->playername);
						r->SendRaidCreate(c);
						r->SendMakeLeaderPacketTo(r->leadername, c);
						r->SendBulkRaid(c);
						r->SendRaidAddAll(rga->playername);
						if(r->IsLocked()) { r->SendRaidLockTo(c); }
					}
					else{
						r->SendRaidRemoveAll(rga->playername);
						r->SendRaidAddAll(rga->playername);
					}
				}
			}
			break;
									  }

		case ServerOP_UpdateGroup:{
			ServerRaidGeneralAction_Struct* rga = (ServerRaidGeneralAction_Struct*)pack->pBuffer;
			if(zone){
				if(rga->zoneid == zone->GetZoneID() && rga->instance_id == zone->GetInstanceID())
					break;

				Raid *r = entity_list.GetRaidByID(rga->rid);
				if(r){
					r->GroupUpdate(rga->gid, false);
				}
			}
			break;
								  }

		case ServerOP_RaidGroupLeader:{
			ServerRaidGeneralAction_Struct* rga = (ServerRaidGeneralAction_Struct*)pack->pBuffer;
			if(zone){
				if(rga->zoneid == zone->GetZoneID() && rga->instance_id == zone->GetInstanceID())
					break;
			}
			break;
									  }

		case ServerOP_RaidLeader:{
			ServerRaidGeneralAction_Struct* rga = (ServerRaidGeneralAction_Struct*)pack->pBuffer;
			if(zone){
				if(rga->zoneid == zone->GetZoneID() && rga->instance_id == zone->GetInstanceID())
					break;

				Raid *r = entity_list.GetRaidByID(rga->rid);
				if(r){
					Client *c = entity_list.GetClientByName(rga->playername);
					strn0cpy(r->leadername, rga->playername, 64);
					if(c){
						r->SetLeader(c);
					}
					r->LearnMembers();
					r->VerifyRaid();
					r->SendMakeLeaderPacket(rga->playername);
				}
			}
			break;
								 }

		case ServerOP_DetailsChange:{
			ServerRaidGeneralAction_Struct* rga = (ServerRaidGeneralAction_Struct*)pack->pBuffer;
			if(zone){
				if(rga->zoneid == zone->GetZoneID() && rga->instance_id == zone->GetInstanceID())
					break;

				Raid *r = entity_list.GetRaidByID(rga->rid);
				if(r){
					r->GetRaidDetails();
					r->LearnMembers();
					r->VerifyRaid();
				}
			}
			break;
									}

		case ServerOP_RaidGroupDisband:{
			ServerRaidGeneralAction_Struct* rga = (ServerRaidGeneralAction_Struct*)pack->pBuffer;
			if(zone){
				if(rga->zoneid == zone->GetZoneID() && rga->instance_id == zone->GetInstanceID())
					break;

				Client *c = entity_list.GetClientByName(rga->playername);
				if(c)
				{
					EQApplicationPacket* outapp = new EQApplicationPacket(OP_GroupUpdate,sizeof(GroupUpdate_Struct));
					GroupUpdate_Struct* gu = (GroupUpdate_Struct*) outapp->pBuffer;
					gu->action = groupActDisband;
					strn0cpy(gu->leadersname, c->GetName(), 64);
					strn0cpy(gu->yourname, c->GetName(), 64);
					c->FastQueuePacket(&outapp);
				}
			}
			break;
									   }

		case ServerOP_RaidGroupAdd:{
			ServerRaidGroupAction_Struct* rga = (ServerRaidGroupAction_Struct*)pack->pBuffer;	
			if(zone){
				Raid *r = entity_list.GetRaidByID(rga->rid);
				if(r){
					r->LearnMembers();
					r->VerifyRaid();
					EQApplicationPacket* outapp = new EQApplicationPacket(OP_GroupUpdate, sizeof(GroupJoin_Struct));
					GroupJoin_Struct* gj = (GroupJoin_Struct*) outapp->pBuffer;	
					strn0cpy(gj->membername, rga->membername, 64);
					gj->action = groupActJoin;

					for(int x = 0; x < MAX_RAID_MEMBERS; x++)
					{
						if(r->members[x].member)
						{
							if(strcmp(r->members[x].member->GetName(), rga->membername) != 0){
								if((rga->gid < 12) && rga->gid == r->members[x].GroupNumber)
								{
									strn0cpy(gj->yourname, r->members[x].member->GetName(), 64);
									r->members[x].member->QueuePacket(outapp);
								}
							}
						}
					}
					safe_delete(outapp);
				}
			}
			break;
								   }

		case ServerOP_RaidGroupRemove:{
			ServerRaidGroupAction_Struct* rga = (ServerRaidGroupAction_Struct*)pack->pBuffer;
			if(zone){
				Raid *r = entity_list.GetRaidByID(rga->rid);
				if(r){
					r->LearnMembers();
					r->VerifyRaid();
					EQApplicationPacket* outapp = new EQApplicationPacket(OP_GroupUpdate, sizeof(GroupJoin_Struct));
					GroupJoin_Struct* gj = (GroupJoin_Struct*) outapp->pBuffer;	
					strn0cpy(gj->membername, rga->membername, 64);
					gj->action = groupActLeave;

					for(int x = 0; x < MAX_RAID_MEMBERS; x++)
					{
						if(r->members[x].member)
						{
							if(strcmp(r->members[x].member->GetName(), rga->membername) != 0){
								if((rga->gid < 12) && rga->gid == r->members[x].GroupNumber)
								{
									strn0cpy(gj->yourname, r->members[x].member->GetName(), 64);
									r->members[x].member->QueuePacket(outapp);
								}
							}
						}
					}
					safe_delete(outapp);
				}
			}
			break;
									  }

		case ServerOP_RaidGroupSay:{
			ServerRaidMessage_Struct* rmsg = (ServerRaidMessage_Struct*)pack->pBuffer;	
			if(zone){
				Raid *r = entity_list.GetRaidByID(rmsg->rid);
				if(r)
				{
					for(int x = 0; x < MAX_RAID_MEMBERS; x++)
					{
						if(r->members[x].member){
							if(strcmp(rmsg->from, r->members[x].member->GetName()) != 0)
							{
								if(r->members[x].GroupNumber == rmsg->gid){
									if(r->members[x].member->GetFilter(FILTER_GROUP)!=0)
									{
										r->members[x].member->ChannelMessageSend(rmsg->from, r->members[x].member->GetName(), 2, 0, rmsg->message);
									}
								}
							}
						}
					}
				}
			}
			break;
								   }

		case ServerOP_RaidSay:{
			ServerRaidMessage_Struct* rmsg = (ServerRaidMessage_Struct*)pack->pBuffer;
			if(zone)
			{
				Raid *r = entity_list.GetRaidByID(rmsg->rid);
				if(r)
				{
					for(int x = 0; x < MAX_RAID_MEMBERS; x++)
					{
						if(r->members[x].member){
							if(strcmp(rmsg->from, r->members[x].member->GetName()) != 0)
							{
								if(r->members[x].member->GetFilter(FILTER_GROUP)!=0)
								{
									r->members[x].member->ChannelMessageSend(rmsg->from, r->members[x].member->GetName(), 15, 0, rmsg->message);
								}
							}
						}
					}
				}
			}
			break;
							  }

		case ServerOP_SpawnPlayerCorpse: {
			SpawnPlayerCorpse_Struct* s = (SpawnPlayerCorpse_Struct*)pack->pBuffer;
			Corpse* NewCorpse = database.LoadPlayerCorpse(s->player_corpse_id);
			if(NewCorpse)
				NewCorpse->Spawn();
			else 
				LogFile->write(EQEMuLog::Error,"Unable to load player corpse id %u for zone %s.", s->player_corpse_id, zone->GetShortName());

			break;
										 }
		case ServerOP_Consent: {
			ServerOP_Consent_Struct* s = (ServerOP_Consent_Struct*)pack->pBuffer;
			Client* client = entity_list.GetClientByName(s->grantname);
			if(client) {
				if(s->permission == 1)
					client->consent_list.push_back(s->ownername);
				else
					client->consent_list.remove(s->ownername);

				EQApplicationPacket* outapp = new EQApplicationPacket(OP_ConsentResponse, sizeof(ConsentResponse_Struct));
				ConsentResponse_Struct* crs = (ConsentResponse_Struct*)outapp->pBuffer;
				strcpy(crs->grantname, s->grantname);
				strcpy(crs->ownername, s->ownername);
				crs->permission = s->permission;
				strcpy(crs->zonename,"all zones");
				client->QueuePacket(outapp);
				safe_delete(outapp);
			}
			else {
				// target not found

				// Message string id's likely to be used here are:
				// CONSENT_YOURSELF = 399
				// CONSENT_INVALID_NAME = 397
				// TARGET_NOT_FOUND = 101

				safe_delete(pack);
				pack = new ServerPacket(ServerOP_Consent_Response, sizeof(ServerOP_Consent_Struct));
				ServerOP_Consent_Struct* scs = (ServerOP_Consent_Struct*)pack->pBuffer;
				strcpy(scs->grantname, s->grantname);
				strcpy(scs->ownername, s->ownername);
				scs->permission = s->permission;
				scs->zone_id = s->zone_id;
				scs->instance_id = s->instance_id;
				scs->message_string_id = TARGET_NOT_FOUND;
				worldserver.SendPacket(pack);
				safe_delete(pack);
			}
			break;
							   }
		case ServerOP_Consent_Response: {
			ServerOP_Consent_Struct* s = (ServerOP_Consent_Struct*)pack->pBuffer;
			Client* client = entity_list.GetClientByName(s->ownername);
			if(client) {
				client->Message_StringID(0, s->message_string_id);
			}
			break;
										}
		case ServerOP_ReloadTasks: {
			if(RuleB(Tasks,EnableTaskSystem)) {
				HandleReloadTasks(pack);
			}
			break;
								   }
		case ServerOP_LFGMatches: {
			HandleLFGMatches(pack);
			break;
								  }
		case ServerOP_LFPMatches: {
			HandleLFPMatches(pack);
			break;
								  }

		case ServerOP_UpdateSpawn: {
			if(zone)
			{
				UpdateSpawnTimer_Struct *ust = (UpdateSpawnTimer_Struct*)pack->pBuffer;
				LinkedListIterator<Spawn2*> iterator(zone->spawn2_list);
				iterator.Reset();
				while (iterator.MoreElements()) 
				{
					if(iterator.GetData()->GetID() == ust->id)
					{
						if(!iterator.GetData()->NPCPointerValid())
						{
							iterator.GetData()->SetTimer(ust->duration);
						}
						break;
					}
					iterator.Advance();
				}
			}
			break;
								   }

		case ServerOP_InstanceUpdateTime:
			{
				ServerInstanceUpdateTime_Struct *iut = (ServerInstanceUpdateTime_Struct*)pack->pBuffer;
				if(zone)
				{
					if(zone->GetInstanceID() == iut->instance_id)
					{
						zone->SetInstanceTimer(iut->new_duration);
					}
				}
				break;
			}

		case ServerOP_DepopAllPlayersCorpses:
			{
				ServerDepopAllPlayersCorpses_Struct *sdapcs = (ServerDepopAllPlayersCorpses_Struct *)pack->pBuffer;

				if(zone && !((zone->GetZoneID() == sdapcs->ZoneID) && (zone->GetInstanceID() == sdapcs->InstanceID)))
					entity_list.RemoveAllCorpsesByCharID(sdapcs->CharacterID);

				break;

			}
		case ServerOP_ReloadTitles:
			{
				title_manager.LoadTitles();
				break;
			}

		case ServerOP_SpawnStatusChange:
			{
				if(zone)
				{
					ServerSpawnStatusChange_Struct *ssc = (ServerSpawnStatusChange_Struct*)pack->pBuffer;
					LinkedListIterator<Spawn2*> iterator(zone->spawn2_list);
					iterator.Reset();
					Spawn2 *found_spawn = NULL;
					while(iterator.MoreElements())
					{
						Spawn2* cur = iterator.GetData();
						if(cur->GetID() == ssc->id)
						{
							found_spawn = cur;
							break;
						}
						iterator.Advance();
					}

					if(found_spawn)
					{
						if(ssc->new_status == 0)
						{
							found_spawn->Disable();
						}
						else
						{
							found_spawn->Enable();
						}
					}
				}
				break;
			}

		case ServerOP_QGlobalUpdate:
			{
				if(pack->size != sizeof(ServerQGlobalUpdate_Struct))
				{
					break;
				}

				if(zone)
				{
					ServerQGlobalUpdate_Struct *qgu = (ServerQGlobalUpdate_Struct*)pack->pBuffer;
					if(qgu->from_zone_id != zone->GetZoneID() || qgu->from_instance_id != zone->GetInstanceID())
					{
						QGlobal temp;
						temp.npc_id = qgu->npc_id;
						temp.char_id = qgu->char_id;
						temp.zone_id = qgu->zone_id;
						temp.expdate = qgu->expdate;
						temp.name.assign(qgu->name);
						temp.value.assign(qgu->value);
						entity_list.UpdateQGlobal(qgu->id, temp);
						zone->UpdateQGlobal(qgu->id, temp);
					}
				}
				break;
			}

		case ServerOP_QGlobalDelete:
			{
				if(pack->size != sizeof(ServerQGlobalDelete_Struct))
				{
					break;
				}

				if(zone)
				{
					ServerQGlobalDelete_Struct *qgd = (ServerQGlobalDelete_Struct*)pack->pBuffer;
					if(qgd->from_zone_id != zone->GetZoneID() || qgd->from_instance_id != zone->GetInstanceID())
					{
						entity_list.DeleteQGlobal(std::string((char*)qgd->name), qgd->npc_id, qgd->char_id, qgd->zone_id);
						zone->DeleteQGlobal(std::string((char*)qgd->name), qgd->npc_id, qgd->char_id, qgd->zone_id);
					}
				}
				break;
			}

		case ServerOP_AdventureRequestAccept:
			{
				ServerAdventureRequestAccept_Struct *ars = (ServerAdventureRequestAccept_Struct*)pack->pBuffer;
				Client *c = entity_list.GetClientByName(ars->leader);
				if(c)
				{
					c->NewAdventure(ars->id, ars->theme, ars->text, ars->member_count, (const char*)(pack->pBuffer + sizeof(ServerAdventureRequestAccept_Struct)));
					c->ClearPendingAdventureRequest();
				}
				break;
			}

		case ServerOP_AdventureRequestDeny:
			{
				ServerAdventureRequestDeny_Struct *ars = (ServerAdventureRequestDeny_Struct*)pack->pBuffer;
				Client *c = entity_list.GetClientByName(ars->leader);
				if(c)
				{
					c->SendAdventureError(ars->reason);
					c->ClearPendingAdventureRequest();
				}
				break;
			}

		case ServerOP_AdventureCreateDeny:
			{
				Client *c = entity_list.GetClientByName((const char*)pack->pBuffer);
				if(c)
				{
					c->ClearPendingAdventureData();
					c->ClearPendingAdventureCreate();
				}
				break;
			}

		case ServerOP_AdventureData:
			{
				Client *c = entity_list.GetClientByName((const char*)pack->pBuffer);
				if(c)
				{
					c->ClearAdventureData();
					char * adv_data = new char[pack->size];
					memcpy(adv_data, pack->pBuffer, pack->size);
					c->SetAdventureData(adv_data);
					c->ClearPendingAdventureData();
					c->ClearPendingAdventureCreate();
					c->SendAdventureDetails();
				}
				break;
			}

		case ServerOP_AdventureDataClear:
			{
				Client *c = entity_list.GetClientByName((const char*)pack->pBuffer);
				if(c)
				{
					if(c->HasAdventureData())
					{
						c->ClearAdventureData();
						c->SendAdventureError("You are not currently assigned to an adventure.");
					}
				}
				break;
			}

		case ServerOP_AdventureClickDoorReply:
			{
				ServerPlayerClickedAdventureDoorReply_Struct *adr = (ServerPlayerClickedAdventureDoorReply_Struct*)pack->pBuffer;
				Client *c = entity_list.GetClientByName(adr->player);
				if(c)
				{
					c->ClearPendingAdventureDoorClick();
					c->MovePC(adr->zone_id, adr->instance_id, adr->x, adr->y, adr->z, adr->h, 0);
				}
				break;
			}

		case ServerOP_AdventureClickDoorError:
			{
				Client *c = entity_list.GetClientByName((const char*)pack->pBuffer);
				if(c)
				{
					c->ClearPendingAdventureDoorClick();
					c->Message_StringID(13, 5141);
				}
				break;
			}

		case ServerOP_AdventureLeaveReply:
			{
				Client *c = entity_list.GetClientByName((const char*)pack->pBuffer);
				if(c)
				{
					c->ClearPendingAdventureLeave();
					c->ClearCurrentAdventure();
				}
				break;
			}

		case ServerOP_AdventureLeaveDeny:
			{
				Client *c = entity_list.GetClientByName((const char*)pack->pBuffer);
				if(c)
				{
					c->ClearPendingAdventureLeave();
					c->Message(13, "You cannot leave this adventure at this time.");
				}
				break;
			}

		case ServerOP_AdventureCountUpdate:
			{
				ServerAdventureCountUpdate_Struct *ac = (ServerAdventureCountUpdate_Struct*)pack->pBuffer;
				Client *c = entity_list.GetClientByName(ac->player);
				if(c)
				{
					c->SendAdventureCount(ac->count, ac->total);
				}
				break;
			}

		case ServerOP_AdventureZoneData:
			{
				if(zone)
				{
					safe_delete(zone->adv_data);
					zone->adv_data = new char[pack->size];
					memcpy(zone->adv_data, pack->pBuffer, pack->size);
					ServerZoneAdventureDataReply_Struct* ds = (ServerZoneAdventureDataReply_Struct*)zone->adv_data;
				}
				break;
			}

		case ServerOP_AdventureFinish:
			{
				ServerAdventureFinish_Struct *af = (ServerAdventureFinish_Struct*)pack->pBuffer;
				Client *c = entity_list.GetClientByName(af->player);
				if(c)
				{
					c->AdventureFinish(af->win, af->theme, af->points);
				}
				break;
			}

		case ServerOP_AdventureLeaderboard:
			{
				Client *c = entity_list.GetClientByName((const char*)pack->pBuffer);
				if(c)
				{
					EQApplicationPacket* outapp = new EQApplicationPacket(OP_AdventureLeaderboardReply, sizeof(AdventureLeaderboard_Struct));
					memcpy(outapp->pBuffer, pack->pBuffer+64, sizeof(AdventureLeaderboard_Struct));
					c->FastQueuePacket(&outapp);
				}
				break;
			}
		case ServerOP_Command:
			{
				if ( pack->size != sizeof(Command_Struct) )
					break;

				Command_Struct *cs = (Command_Struct*)pack->pBuffer;

				switch(cs->commandID)
				{
				case RELOAD_ZONE_RULES:
					{
						rules->LoadRules(&database, rules->GetActiveRuleset());
						database.LoadVariables();
						break;
					}
				case RELOAD_HOTZONES:
					{
						if ( zone && zone->GetShortName() != NULL && strlen((char*)zone->GetShortName()) > 0 )
							zone->SetIsHotZone(database.getZoneHotZone((char*)zone->GetShortName()));
						break;
					}
				case RELOAD_PVPHOTZONES:
					{
						if ( zone && zone->GetShortName() != NULL && strlen((char*)zone->GetShortName()) > 0 )
							zone->SetIsPVPHotZone(database.getPVPHotZone((char*)zone->GetShortName()));
						break;
					}
				case RELOAD_ZONE_QUESTS:
					{
						if ( parse != NULL )
							parse->ReloadQuests();
						break;
					}
				case CAST_SELF_ON_CLIENTS:
					{
						int16 spellid = (int16)cs->commandValue;
						if ( zone && spellid < SPDAT_RECORDS)
						{
							list<Client*> targets;
							list<Client*>::iterator iter;

							entity_list.GetClientList(targets);
							iter = targets.begin();
							while(iter != targets.end())
							{
								bool isGM = (*iter)->GetGM();
								if ( !isGM )
									(*iter)->SpellFinished(spellid, (*iter), 10, 0, -1);

								iter++;
							}
						}
						break;
					}
				case UPDATE_CLIENT_LOSTLEADERSHIP:
					{
						int32 charid = (int32)cs->commandValue;
						Client* c = entity_list.GetClientByCharID(charid);
						if ( c && c->GetInv().cityBankLoaded )
						{
							// evac player to reset
							c->CastSpell(1628, c->GetID(), 0, 0);
						}
						break;
					}
				case UPDATE_CLIENT_BANKREVENUE:
					{
						int32 charid = (int32)cs->commandValue;
						Client* c = entity_list.GetClientByCharID(charid);
						if ( c && c->GetInv().cityBankCoinLoaded )
						{
							// evac player to reset
							KingsBandits::UpdatePlayerCityBank(c);
						}
						break;
					}
				case UPDATE_CLIENT_GIVELEADERSHIP:
					{
						if ( strlen(cs->player) > 0 )
						{
							int32 charid = atoi(cs->player);
							Client* c = entity_list.GetClientByCharID(charid);
							if ( c )
							{
								// value passed is the points earned for taking the city
								//c->AddPVPPoints(cs->commandValue);
								c->Message(13,"Congratulations!  Leadership has also been provided to you of the city!");
								c->Message(13,"If you are in a city bank zone you must zone out of the current zone and back in to view the city bank.");
								//c->Message(13,"City NPC's will respawn on their normal 60 minute timer which will also include the banker.");
							}
						}
						break;
					}
				case UPDATE_CITY_MOBS:
					{
						if ( zone == NULL )
							break;

						bool triggerReload = false;
						switch(cs->commandValue)
						{
						case QEYNOSTEAMID:
							{
								// qeynos 1
								// qeynos2 2
								// qeytoqrg 4
								if ( zone->GetZoneID() == 1 || zone->GetZoneID() == 2 || zone->GetZoneID() == 4 )
									triggerReload = true;
								break;
							}
						case HALASTEAMID:
							{
								// halas 29
								// everfrost 30
								if ( zone->GetZoneID() == 29 || zone->GetZoneID() == 30 )
									triggerReload = true;
								break;
							}
						case FREEPORTTEAMID:
							{
								// freportn 8
								// freportw 9
								// freporte 10

								if ( zone->GetZoneID() == 8 || zone->GetZoneID() == 9 || zone->GetZoneID() == 10 )
									triggerReload = true;
								break;
							}
						case RIVERVALETEAMID:
							{
								// rivervale 19
								// misty 33

								if ( zone->GetZoneID() == 19 || zone->GetZoneID() == 33 )
									triggerReload = true;

								break;
							}
						case ERUDINTEAMID:
							{
								// erudnint 23
								// erudnext 24
								// tox 38

								if ( zone->GetZoneID() == 23 || zone->GetZoneID() == 24 || zone->GetZoneID() == 38 )
									triggerReload = true;

								break;
							}
						case FELWITHETEAMID:
							{
								// felwithea 61
								// felwitheb 62
								// gfaydark 54

								if ( zone->GetZoneID() == 61 || zone->GetZoneID() == 62 || zone->GetZoneID() == 54 )
									triggerReload = true;

								break;
							}
						case THURGADINTEAMID:
							{
								// thurgadina 115
								// thurgadinb 129

								if ( zone->GetZoneID() == 115 || zone->GetZoneID() == 129 )
									triggerReload = true;

								break;
							}
						}

						if ( triggerReload )
							KingsBandits::ReloadCityGuards(cs->commandValue);
						break;
					}
				case UPDATE_CLIENT_SETBOUNTY:
					{
						if ( strlen(cs->player) > 0 )
						{
							Client* c = entity_list.GetClientByName(cs->player);
							if ( c )
							{
								if ( !c->IsBountySet() && cs->commandValue > 0 )
								{
									c->SetBounty(true);
									//c->AddPVPPoints(1);
									c->UpdateInfamyLevel(999999);
									c->Message(13,"[BOUNTY] You have a red target painted on your back, Good luck.");
									SendEmoteMessage(0,0,0,13,"Bounty has been set on %s!",c->GetName());
								}
								else if ( cs->commandValue == 0 && c->IsBountySet() )
								{
									c->SetBounty(false);
									c->UpdateInfamyLevel(-999999);
									c->Message(13,"[BOUNTY] Bounty was cancelled.");
								}
							}
						}
						break;
					}
				}
				break;
			}
		case ServerOP_PlayerCounts:
			{
				if ( pack->size != sizeof(PlayerInfo_Struct) )
					break;
				PlayerInfo_Struct *pis = (PlayerInfo_Struct*)pack->pBuffer;
				if ( zone )
					zone->SetServerPlayerCounts(pis->neutralCount,pis->teamOneCount,pis->teamTwoCount,pis->teamThreeCount,pis->teamFourCount,pis->teamFiveCount);

				break;
			}
		default: {
			cout << " Unknown ZSopcode:" << (int)pack->opcode;
			cout << " size:" << pack->size << endl;
			//DumpPacket(pack->pBuffer, pack->size);
			break;
				 }
		}
		safe_delete(pack);
	}

	return;
}

bool WorldServer::SendChannelMessage(Client* from, const char* to, int8 chan_num, int32 guilddbid, int8 language, const char* message, ...) {
	if(!worldserver.Connected())
		return false;
	va_list argptr;
	char buffer[512];

	va_start(argptr, message);
	vsnprintf(buffer, 512, message, argptr);
	va_end(argptr);
	buffer[511] = '\0';

	ServerPacket* pack = NULL;
	ServerChannelMessage_Struct* scm = NULL;
	if ( chan_num == 5 && RuleB(Chat,OOCReportZone) && from && !from->GetGM()) // ooc
	{
		char* zoneNameShort = (char*)zone->GetShortName();
		int length = strlen(zoneNameShort) + 3 + strlen(buffer) + 1;
		pack = new ServerPacket(ServerOP_ChannelMessage, sizeof(ServerChannelMessage_Struct) + length);
		scm = (ServerChannelMessage_Struct*) pack->pBuffer;
		_snprintf(scm->message,length,"[%s] %s",zoneNameShort,buffer);
	}
	else
	{
		pack = new ServerPacket(ServerOP_ChannelMessage, sizeof(ServerChannelMessage_Struct) + strlen(buffer) + 1);
		scm = (ServerChannelMessage_Struct*) pack->pBuffer;
		strcpy(scm->message, buffer);
	}

	if (from == 0) {
		strcpy(scm->from, "ZServer");
		scm->fromadmin = 0;
	} else {
		strcpy(scm->from, from->GetName());
		scm->fromadmin = from->Admin();
	}
	if (to == 0) {
		scm->to[0] = 0;
		scm->deliverto[0] = '\0';
	} else {
		strn0cpy(scm->to, to, sizeof(scm->to));
		strn0cpy(scm->deliverto, to, sizeof(scm->deliverto));
	}
	scm->noreply = false;
	scm->chan_num = chan_num;
	scm->guilddbid = guilddbid;
	scm->language = language;

	pack->Deflate();
	bool ret = SendPacket(pack);
	safe_delete(pack);
	return ret;
}

bool WorldServer::SendEmoteMessage(const char* to, int32 to_guilddbid, int32 type, const char* message, ...) {
	va_list argptr;
	char buffer[256];

	va_start(argptr, message);
	vsnprintf(buffer, 256, message, argptr);
	va_end(argptr);

	return SendEmoteMessage(to, to_guilddbid, 0, type, buffer);
}

bool WorldServer::SendEmoteMessage(const char* to, int32 to_guilddbid, sint16 to_minstatus, int32 type, const char* message, ...) {
	va_list argptr;
	char buffer[256];

	va_start(argptr, message);
	vsnprintf(buffer, 256, message, argptr);
	va_end(argptr);

	if (!Connected() && to == 0) {
		entity_list.MessageStatus(to_guilddbid, to_minstatus, type, buffer);
		return false;
	}

	ServerPacket* pack = new ServerPacket(ServerOP_EmoteMessage, sizeof(ServerEmoteMessage_Struct)+strlen(buffer)+1);
	ServerEmoteMessage_Struct* sem = (ServerEmoteMessage_Struct*) pack->pBuffer;
	sem->type = type;
	if (to != 0)
		strcpy(sem->to, to);
	sem->guilddbid = to_guilddbid;
	sem->minstatus = to_minstatus;
	strcpy(sem->message, buffer);

	pack->Deflate();
	bool ret = SendPacket(pack);
	safe_delete(pack);
	return ret;
}

bool WorldServer::SendVoiceMacro(Client* From, int32 Type, char* Target, int32 MacroNumber, int32 GroupOrRaidID) {

	if(!worldserver.Connected() || !From)
		return false;

	ServerPacket* pack = new ServerPacket(ServerOP_VoiceMacro, sizeof(ServerVoiceMacro_Struct));

	ServerVoiceMacro_Struct* svm = (ServerVoiceMacro_Struct*) pack->pBuffer;

	strcpy(svm->From, From->GetName());

	switch(Type) {

	case VoiceMacroTell:
		strcpy(svm->To, Target);
		break;

	case VoiceMacroGroup:
		svm->GroupID = GroupOrRaidID;
		break;

	case VoiceMacroRaid:
		svm->RaidID = GroupOrRaidID;
		break;
	}

	svm->Type = Type;

	svm->Voice = (GetArrayRace(From->GetRace()) * 2) + From->GetGender();

	svm->MacroNumber = MacroNumber;

	pack->Deflate();

	bool Ret = SendPacket(pack);

	safe_delete(pack);

	return Ret;
}

bool WorldServer::RezzPlayer(EQApplicationPacket* rpack,int32 rezzexp, int16 opcode) {
	_log(SPELLS__REZ, "WorldServer::RezzPlayer rezzexp is %i (0 is normal for RezzComplete", rezzexp);
	ServerPacket* pack = new ServerPacket(ServerOP_RezzPlayer, sizeof(RezzPlayer_Struct));
	RezzPlayer_Struct* sem = (RezzPlayer_Struct*) pack->pBuffer;
	sem->rezzopcode = opcode;
	sem->rez = *(Resurrect_Struct*) rpack->pBuffer;
	sem->exp = rezzexp;
	bool ret = SendPacket(pack);
	if (ret)
		_log(SPELLS__REZ, "Sending player rezz packet to world spellid:%i", sem->rez.spellid);
	else
		_log(SPELLS__REZ, "NOT Sending player rezz packet to world");

	safe_delete(pack);
	return ret;
}

void WorldServer::SendReloadTasks(int Command, int TaskID) {
	ServerPacket* pack = new ServerPacket(ServerOP_ReloadTasks, sizeof(ReloadTasks_Struct));
	ReloadTasks_Struct* rts = (ReloadTasks_Struct*) pack->pBuffer;

	rts->Command = Command;
	rts->Parameter = TaskID;

	SendPacket(pack);
}

void WorldServer::HandleReloadTasks(ServerPacket *pack) 
{	
	ReloadTasks_Struct* rts = (ReloadTasks_Struct*) pack->pBuffer;

	_log(TASKS__GLOBALLOAD, "Zone received ServerOP_ReloadTasks from World, Command %i", rts->Command);

	switch(rts->Command) {
	case RELOADTASKS:
		entity_list.SaveAllClientsTaskState();

		if(rts->Parameter == 0) {
			_log(TASKS__GLOBALLOAD, "Reload ALL tasks");
			safe_delete(taskmanager);
			taskmanager = new TaskManager;
			taskmanager->LoadTasks();
			if(zone) 
				taskmanager->LoadProximities(zone->GetZoneID());
			entity_list.ReloadAllClientsTaskState();
		}
		else {
			_log(TASKS__GLOBALLOAD, "Reload only task %i", rts->Parameter);
			taskmanager->LoadTasks(rts->Parameter);
			entity_list.ReloadAllClientsTaskState(rts->Parameter);
		}

		break;

	case RELOADTASKPROXIMITIES:
		if(zone) {
			_log(TASKS__GLOBALLOAD, "Reload task proximities");
			taskmanager->LoadProximities(zone->GetZoneID());
		}
		break;

	case RELOADTASKGOALLISTS:
		_log(TASKS__GLOBALLOAD, "Reload task goal lists");
		taskmanager->ReloadGoalLists();
		break;

	case RELOADTASKSETS:
		_log(TASKS__GLOBALLOAD, "Reload task sets");
		taskmanager->LoadTaskSets();
		break;

	default:
		_log(TASKS__GLOBALLOAD, "Unhandled ServerOP_ReloadTasks command %i", rts->Command);

	}


}


int32 WorldServer::NextGroupID() {
	//this system wastes a lot of potential group IDs (~5%), but
	//if you are creating 2 billion groups in 1 run of the emu,
	//something else is wrong...
	if(cur_groupid >= last_groupid) {
		//this is an error... This means that 50 groups were created before
		//1 packet could make the zone->world->zone trip... so let it error.
		_log(ZONE__WORLD_ERR, "Ran out of group IDs before the server sent us more.");
		return(0);
	}
	if(cur_groupid > (last_groupid - /*50*/995)) {
		//running low, request more
		ServerPacket* pack = new ServerPacket(ServerOP_GroupIDReq);
		SendPacket(pack);
		safe_delete(pack);
	}
	printf("Handing out new group id %d\n", cur_groupid);
	return(cur_groupid++);
}

void WorldServer::UpdateLFP(int32 LeaderID, uint8 Action, uint8 MatchFilter, uint32 FromLevel, uint32 ToLevel, uint32 Classes, 
	const char *Comments, GroupLFPMemberEntry *LFPMembers) {

		ServerPacket* pack = new ServerPacket(ServerOP_LFPUpdate, sizeof(ServerLFPUpdate_Struct));
		ServerLFPUpdate_Struct* sus = (ServerLFPUpdate_Struct*) pack->pBuffer;

		sus->LeaderID = LeaderID;
		sus->Action = Action;
		sus->MatchFilter = MatchFilter;
		sus->FromLevel = FromLevel;
		sus->ToLevel = ToLevel;
		sus->Classes = Classes;
		strcpy(sus->Comments, Comments);
		memcpy(sus->Members, LFPMembers, sizeof(sus->Members));
		SendPacket(pack);
		safe_delete(pack);

}

void WorldServer::UpdateLFP(int32 LeaderID, GroupLFPMemberEntry *LFPMembers) {

	UpdateLFP(LeaderID, LFPMemberUpdate, 0, 0, 0, 0, "", LFPMembers);
}

void WorldServer::StopLFP(int32 LeaderID) {

	GroupLFPMemberEntry LFPMembers;
	UpdateLFP(LeaderID, LFPOff, 0, 0, 0, 0, "", &LFPMembers);
}

void WorldServer::HandleLFGMatches(ServerPacket *pack) {

	char *Buffer = (char *)pack->pBuffer;

	int PacketLength = 4;

	int Entries = (pack->size - 4) / sizeof(ServerLFGMatchesResponse_Struct);

	uint32 EntityID = VARSTRUCT_DECODE_TYPE(uint32, Buffer);

	Client* client = entity_list.GetClientByID(EntityID);

	if(client) {
		ServerLFGMatchesResponse_Struct* smrs = (ServerLFGMatchesResponse_Struct*)Buffer;

		for(int i=0; i<Entries; i++) {
			PacketLength = PacketLength + 12 + strlen(smrs->Name) + strlen(smrs->Comments);
			smrs++;
		}

		EQApplicationPacket* outapp = new EQApplicationPacket(OP_LFGGetMatchesResponse, PacketLength);

		smrs = (ServerLFGMatchesResponse_Struct*)Buffer;

		char *OutBuffer = (char *)outapp->pBuffer;

		VARSTRUCT_ENCODE_TYPE(uint32, OutBuffer, 0x00074af); // Unknown

		for(int i=0; i<Entries; i++) {
			VARSTRUCT_ENCODE_STRING(OutBuffer, smrs->Comments);
			VARSTRUCT_ENCODE_STRING(OutBuffer, smrs->Name);
			VARSTRUCT_ENCODE_TYPE(uint16, OutBuffer, smrs->Class_);
			VARSTRUCT_ENCODE_TYPE(uint16, OutBuffer, smrs->Level);
			VARSTRUCT_ENCODE_TYPE(uint16, OutBuffer, smrs->Zone);
			VARSTRUCT_ENCODE_TYPE(uint16, OutBuffer, smrs->GuildID);
			VARSTRUCT_ENCODE_TYPE(uint16, OutBuffer, smrs->Anon);
			smrs++;
		}
		client->QueuePacket(outapp);
		safe_delete(outapp);
	}
}				

void WorldServer::HandleLFPMatches(ServerPacket *pack) {

	char *Buffer = (char *)pack->pBuffer;

	int PacketLength = 4;

	int Entries = (pack->size - 4) / sizeof(ServerLFPMatchesResponse_Struct);

	uint32 EntityID = VARSTRUCT_DECODE_TYPE(uint32, Buffer);

	ServerLFPMatchesResponse_Struct* smrs = (ServerLFPMatchesResponse_Struct*)Buffer;

	Client* client = entity_list.GetClientByID(EntityID);

	if(client) {
		for(int i=0; i<Entries; i++) {
			PacketLength += strlen(smrs->Comments) + 11;
			for(unsigned int j=0; j<MAX_GROUP_MEMBERS; j++) {

				if(smrs->Members[j].Name[0] != '\0') 
					PacketLength += strlen(smrs->Members[j].Name) + 9;
			}
			smrs++;
		}
		EQApplicationPacket* outapp = new EQApplicationPacket(OP_LFPGetMatchesResponse, PacketLength);

		smrs = (ServerLFPMatchesResponse_Struct*)Buffer;

		char *OutBuffer = (char *)outapp->pBuffer;

		VARSTRUCT_ENCODE_TYPE(uint32, OutBuffer, 0x00074af); // Unknown

		for(int i=0; i<Entries; i++) {

			int MemberCount = 0;

			for(unsigned int j=0; j<MAX_GROUP_MEMBERS; j++) 
				if(smrs->Members[j].Name[0] != '\0')
					MemberCount++;

			VARSTRUCT_ENCODE_STRING(OutBuffer, smrs->Comments);
			VARSTRUCT_ENCODE_TYPE(uint16, OutBuffer, smrs->FromLevel);
			VARSTRUCT_ENCODE_TYPE(uint16, OutBuffer, smrs->ToLevel);
			VARSTRUCT_ENCODE_TYPE(uint32, OutBuffer, smrs->Classes);
			VARSTRUCT_ENCODE_TYPE(uint16, OutBuffer, MemberCount);

			for(unsigned int j=0; j<MAX_GROUP_MEMBERS; j++) {
				if(smrs->Members[j].Name[0] != '\0') {
					VARSTRUCT_ENCODE_STRING(OutBuffer, smrs->Members[j].Name);
					VARSTRUCT_ENCODE_TYPE(uint16, OutBuffer, smrs->Members[j].Class);
					VARSTRUCT_ENCODE_TYPE(uint16, OutBuffer, smrs->Members[j].Level);
					VARSTRUCT_ENCODE_TYPE(uint16, OutBuffer, smrs->Members[j].Zone);
					VARSTRUCT_ENCODE_TYPE(uint16, OutBuffer, smrs->Members[j].GuildID);
				}
			}
			smrs++;
		}
		client->QueuePacket(outapp);
		safe_delete(outapp);
	}
}


void WorldServer::SendCommandRequest(int32 cmdID, int32 cmdValue, char* senderName) {

	ServerPacket* pack = new ServerPacket(ServerOP_Command, sizeof(Command_Struct));
	Command_Struct* cs = (Command_Struct*) pack->pBuffer;

	cs->commandID = cmdID;
	cs->commandValue = cmdValue;
	strncpy(cs->player,senderName,64);
	SendPacket(pack);
	safe_delete(pack);
}