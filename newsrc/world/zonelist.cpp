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
#include "zonelist.h"
#include "zoneserver.h"
#include "clientlist.h"
#include "WorldTCPConnection.h"
#include "worlddb.h"
#include "console.h"
#include "WorldConfig.h"
#include "../common/servertalk.h"
#include "../common/MiscFunctions.h"

extern uint32			numzones;
extern bool holdzones;
extern ConsoleList		console_list;
extern ClientList		client_list;



ZSList::ZSList()
{
	NextID = 1;
	CurGroupID = 1;
	LastAllocatedPort=0;
	memset(pLockedZones, 0, sizeof(pLockedZones));
}

ZSList::~ZSList() {
}

void ZSList::ShowUpTime(WorldTCPConnection* con, const char* adminname) {
	int32 ms = Timer::GetCurrentTime();
	int32 d = ms / 86400000;
	ms -= d * 86400000;
	int32 h = ms / 3600000;
	ms -= h * 3600000;
	int32 m = ms / 60000;
	ms -= m * 60000;
	int32 s = ms / 1000;
	if (d)
		con->SendEmoteMessage(adminname, 0, 0, 0, "Worldserver Uptime: %02id %02ih %02im %02is", d, h, m, s);
	else if (h)
		con->SendEmoteMessage(adminname, 0, 0, 0, "Worldserver Uptime: %02ih %02im %02is", h, m, s);
	else
		con->SendEmoteMessage(adminname, 0, 0, 0, "Worldserver Uptime: %02im %02is", m, s);
}

void ZSList::Add(ZoneServer* zoneserver) {
	list.Insert(zoneserver);
	zoneserver->SendGroupIDs();	//send its initial set of group ids
}

void ZSList::KillAll() {
	LinkedListIterator<ZoneServer*> iterator(list);

	iterator.Reset();
	while(iterator.MoreElements()) {
		iterator.GetData()->Disconnect();
		iterator.RemoveCurrent();
		numzones--;
	}
}

void ZSList::Process() {
	if(shutdowntimer && shutdowntimer->Check()){
		_log(WORLD__ZONELIST, "Shutdown timer has expired. Telling all zones to shut down and exiting. (fake sigint)");
		ServerPacket* pack2 = new ServerPacket;
		pack2->opcode = ServerOP_ShutdownAll;
		pack2->size=0;
		SendPacket(pack2);
		safe_delete(pack2);
		Process();
		CatchSignal(2);
	}
	if(reminder && reminder->Check()){
		sint64 timeLeftMin = shutdowntimer->GetRemainingTime() / 60000;
		sint64 seconds = (shutdowntimer->GetRemainingTime()/1000)  - (timeLeftMin * 60);
		SendEmoteMessage(0,0,0,15,"<SYSTEMWIDE MESSAGE>:SYSTEM MSG:World coming down, everyone log out now.  World will shut down in %i minutes and %i seconds...",timeLeftMin,seconds);
	}
	LinkedListIterator<ZoneServer*> iterator(list);

	iterator.Reset();
	while(iterator.MoreElements()) {
		if (!iterator.GetData()->Process()) {
			ZoneServer* zs = iterator.GetData();
			struct in_addr  in;
			in.s_addr = zs->GetIP();
			_log(WORLD__ZONELIST,"Removing zoneserver #%d at %s:%d",zs->GetID(),zs->GetCAddress(),zs->GetCPort());
			zs->LSShutDownUpdate(zs->GetZoneID());
			if (holdzones){
				_log(WORLD__ZONELIST,"Hold Zones mode is ON - rebooting lost zone");
				if(!zs->IsStaticZone())
					RebootZone(inet_ntoa(in),zs->GetCPort(),zs->GetCAddress(),zs->GetID());
				else
					RebootZone(inet_ntoa(in),zs->GetCPort(),zs->GetCAddress(),zs->GetID(),database.GetZoneID(zs->GetZoneName()));
			}

			iterator.RemoveCurrent();
			numzones--;
		}
		else {
			iterator.Advance();
		}
	}
}

bool ZSList::SendPacket(ServerPacket* pack) {
	LinkedListIterator<ZoneServer*> iterator(list);
	
	iterator.Reset();
	while(iterator.MoreElements()) {
		iterator.GetData()->SendPacket(pack);
		iterator.Advance();
	}
	return true;
}

bool ZSList::SendPacket(uint32 ZoneID, ServerPacket* pack) {
	LinkedListIterator<ZoneServer*> iterator(list);

	iterator.Reset();
	while(iterator.MoreElements()) {
		if (iterator.GetData()->GetZoneID() == ZoneID) {
			ZoneServer* tmp = iterator.GetData();
			return(tmp->SendPacket(pack));
		}
		iterator.Advance();
	}
	return(false);
}

bool ZSList::SendPacket(uint32 ZoneID, uint16 instanceID, ServerPacket* pack) {
	LinkedListIterator<ZoneServer*> iterator(list);

	iterator.Reset();
	if(instanceID != 0)
	{
		while(iterator.MoreElements()) {
			if(iterator.GetData()->GetInstanceID() == instanceID) {
				ZoneServer* tmp = iterator.GetData();
				return(tmp->SendPacket(pack));
			}
			iterator.Advance();
		}
	}
	else
	{
		while(iterator.MoreElements()) {
			if (iterator.GetData()->GetZoneID() == ZoneID 
				&& iterator.GetData()->GetInstanceID() == 0) {
				ZoneServer* tmp = iterator.GetData();
				return(tmp->SendPacket(pack));
			}
			iterator.Advance();
		}
	}
	return(false);
}

ZoneServer* ZSList::FindByName(const char* zonename) {
	LinkedListIterator<ZoneServer*> iterator(list);

	iterator.Reset();
	while(iterator.MoreElements())
	{
		if (strcasecmp(iterator.GetData()->GetZoneName(), zonename) == 0) {
			ZoneServer* tmp = iterator.GetData();
			return tmp;
		}
		iterator.Advance();
	}
	return 0;
}

ZoneServer* ZSList::FindByID(int32 ZoneID) {
	LinkedListIterator<ZoneServer*> iterator(list);

	iterator.Reset();
	while(iterator.MoreElements()) {
		if (iterator.GetData()->GetID() == ZoneID) {
			ZoneServer* tmp = iterator.GetData();
			return tmp;
		}
		iterator.Advance();
	}
	return 0;
}

ZoneServer* ZSList::FindByZoneID(int32 ZoneID) {
	LinkedListIterator<ZoneServer*> iterator(list);
	iterator.Reset();
	while(iterator.MoreElements())
	{
		ZoneServer* tmp = iterator.GetData();
		if (tmp->GetZoneID() == ZoneID  && tmp->GetInstanceID() == 0) {
			return tmp;
		}
		iterator.Advance();
	}
	return 0;
}

ZoneServer* ZSList::FindByPort(int16 port) {
	LinkedListIterator<ZoneServer*> iterator(list);

	iterator.Reset();
	while(iterator.MoreElements())
	{
		if (iterator.GetData()->GetCPort() == port) {
			ZoneServer* tmp = iterator.GetData();
			return tmp;
		}
		iterator.Advance();
	}
	return 0;
}

ZoneServer* ZSList::FindByInstanceID(int32 InstanceID)
{
	LinkedListIterator<ZoneServer*> iterator(list);

	iterator.Reset();
	while(iterator.MoreElements())
	{
		if (iterator.GetData()->GetInstanceID() == InstanceID) {
			ZoneServer* tmp = iterator.GetData();
			return tmp;
		}
		iterator.Advance();
	}
	return 0;
}

bool ZSList::SetLockedZone(int16 iZoneID, bool iLock) {
	for (int i=0; i<MaxLockedZones; i++) {
		if (iLock) {
			if (pLockedZones[i] == 0) {
				pLockedZones[i] = iZoneID;
				return true;
			}
		}
		else {
			if (pLockedZones[i] == iZoneID) {
				pLockedZones[i] = 0;
				return true;
			}
		}
	}
	return false;
}

bool ZSList::IsZoneLocked(int16 iZoneID) {
	for (int i=0; i<MaxLockedZones; i++) {
		if (pLockedZones[i] == iZoneID)
			return true;
	}
	return false;
}

void ZSList::ListLockedZones(const char* to, WorldTCPConnection* connection) {
	int x = 0;
	for (int i=0; i<MaxLockedZones; i++) {
		if (pLockedZones[i]) {
			connection->SendEmoteMessageRaw(to, 0, 0, 0, database.GetZoneName(pLockedZones[i], true));
			x++;
		}
	}
	connection->SendEmoteMessage(to, 0, 0, 0, "%i zones locked.", x);
}

void ZSList::SendZoneStatus(const char* to, sint16 admin, WorldTCPConnection* connection) {
	LinkedListIterator<ZoneServer*> iterator(list);
	struct in_addr  in;
	
	iterator.Reset();
	char locked[4];
	if (WorldConfig::get()->Locked == true)
		strcpy(locked, "Yes");
	else
		strcpy(locked, "No");

	char* output = 0;
	int32 outsize = 0, outlen = 0;
	if (connection->IsConsole())
		AppendAnyLenString(&output, &outsize, &outlen, "World Locked: %s\r\n", locked);
	else
		AppendAnyLenString(&output, &outsize, &outlen, "World Locked: %s^", locked);
	if (connection->IsConsole())
		AppendAnyLenString(&output, &outsize, &outlen, "Zoneservers online:\r\n");
	else
		AppendAnyLenString(&output, &outsize, &outlen, "Zoneservers online:^");
//	connection->SendEmoteMessage(to, 0, 0, 0, "World Locked: %s", locked);
//	connection->SendEmoteMessage(to, 0, 0, 0, "Zoneservers online:");
	int v=0, w=0, x=0, y=0, z=0;
	char tmpStatic[2] = { 0, 0 }, tmpZone[64];
	memset(tmpZone, 0, sizeof(tmpZone));
	ZoneServer* zs = 0;
	while(iterator.MoreElements()) {
		zs = iterator.GetData();
		in.s_addr = zs->GetIP();

		if(zs->IsStaticZone())
			z++;
		else if (zs->GetZoneID() != 0)
			w++;
		else if(zs->GetZoneID() == 0 && !zs->IsBootingUp())
			v++;

		if (zs->IsStaticZone())
			tmpStatic[0] = 'S';
		else
			tmpStatic[0] = ' ';

		if (admin >= 150) {
			if (zs->GetZoneID())
				snprintf(tmpZone, sizeof(tmpZone), "%s (%i)", zs->GetZoneName(), zs->GetZoneID());
			else if (zs->IsBootingUp())
				strcpy(tmpZone, "...");
			else
				tmpZone[0] = 0;

			AppendAnyLenString(&output, &outsize, &outlen, "  #%-3i %s %15s:%-5i %2i  %s:%i  %s", zs->GetID(), tmpStatic, inet_ntoa(in), zs->GetPort(), zs->NumPlayers(), zs->GetCAddress(), zs->GetCPort(), tmpZone);
			if (outlen >= 3584) {
				connection->SendEmoteMessageRaw(to, 0, 0, 10, output);
				safe_delete(output);
				outsize = 0;
				outlen = 0;
			}
			else {
				if (connection->IsConsole())
					AppendAnyLenString(&output, &outsize, &outlen, "\r\n");
				else
					AppendAnyLenString(&output, &outsize, &outlen, "^");
			}
			x++;
		}
		else if (zs->GetZoneID() != 0) {
			if (zs->GetZoneID())
				strcpy(tmpZone, zs->GetZoneName());
			else
				tmpZone[0] = 0;
			AppendAnyLenString(&output, &outsize, &outlen, "  #%i %s  %s", zs->GetID(), tmpStatic, tmpZone);
			if (outlen >= 3584) {
				connection->SendEmoteMessageRaw(to, 0, 0, 10, output);
				safe_delete(output);
				outsize = 0;
				outlen = 0;
			}
			else {
				if (connection->IsConsole())
					AppendAnyLenString(&output, &outsize, &outlen, "\r\n");
				else
					AppendAnyLenString(&output, &outsize, &outlen, "^");
			}
			x++;
		}
		y++;
		iterator.Advance();
	}
	if (connection->IsConsole())
		AppendAnyLenString(&output, &outsize, &outlen, "%i servers listed. %i servers online.\r\n", x, y);
	else
		AppendAnyLenString(&output, &outsize, &outlen, "%i servers listed. %i servers online.^", x, y);
	AppendAnyLenString(&output, &outsize, &outlen, "%i zones are static zones, %i zones are booted zones, %i zones available.",z,w,v);
//	connection->SendEmoteMessage(to, 0, 0, "%i servers listed. %i servers online.", x, y);
//	connection->SendEmoteMessage(to,0,0,"%i zones are static zones, %i zones are booted zones, %i zones available.",z,w,v);
	if (output)
		connection->SendEmoteMessageRaw(to, 0, 0, 10, output);
	safe_delete(output);
}

void ZSList::SendChannelMessage(const char* from, const char* to, int8 chan_num, int8 language, const char* message, ...) {
	if (!message)
		return;
	va_list argptr;
	char buffer[1024];

	va_start(argptr, message);
	vsnprintf(buffer, sizeof(buffer), message, argptr);
	va_end(argptr);

	SendChannelMessageRaw(from, to, chan_num, language, buffer);
}

void ZSList::SendChannelMessageRaw(const char* from, const char* to, int8 chan_num, int8 language, const char* message) {
	if (!message)
		return;
	ServerPacket* pack = new ServerPacket;

	pack->opcode = ServerOP_ChannelMessage;
	pack->size = sizeof(ServerChannelMessage_Struct)+strlen(message)+1;
	pack->pBuffer = new uchar[pack->size];
	memset(pack->pBuffer, 0, pack->size);
	ServerChannelMessage_Struct* scm = (ServerChannelMessage_Struct*) pack->pBuffer;
	if (from == 0) {
		strcpy(scm->from, "WServer");
		scm->noreply = true;
	}
	else if (from[0] == 0) {
		strcpy(scm->from, "WServer");
		scm->noreply = true;
	}
	else
		strcpy(scm->from, from);
	if (to != 0) {
		strcpy((char *) scm->to, to);
		strcpy((char *) scm->deliverto, to);
	}
	else {
		scm->to[0] = 0;
		scm->deliverto[0] = 0;
	}

	scm->language = language;
	scm->chan_num = chan_num;
	strcpy(&scm->message[0], message);
	if (scm->chan_num == 5 || scm->chan_num == 6 || scm->chan_num == 11) {
		console_list.SendChannelMessage(scm);
	}
	pack->Deflate();
	SendPacket(pack);
	delete pack;
}


void ZSList::SendEmoteMessage(const char* to, int32 to_guilddbid, sint16 to_minstatus, int32 type, const char* message, ...) {
	if (!message)
		return;
	va_list argptr;
	char buffer[1024];

	va_start(argptr, message);
	vsnprintf(buffer, sizeof(buffer), message, argptr);
	va_end(argptr);

	SendEmoteMessageRaw(to, to_guilddbid, to_minstatus, type, buffer);
}

void ZSList::SendEmoteMessageRaw(const char* to, int32 to_guilddbid, sint16 to_minstatus, int32 type, const char* message) {
	if (!message)
		return;
	ServerPacket* pack = new ServerPacket;

	pack->opcode = ServerOP_EmoteMessage;
	pack->size = sizeof(ServerEmoteMessage_Struct)+strlen(message)+1;
	pack->pBuffer = new uchar[pack->size];
	memset(pack->pBuffer, 0, pack->size);
	ServerEmoteMessage_Struct* sem = (ServerEmoteMessage_Struct*) pack->pBuffer;
	
	if (to) {
		if (to[0] == '*') {
			Console* con = console_list.FindByAccountName(&to[1]);
			if (con)
				con->SendEmoteMessageRaw(to, to_guilddbid, to_minstatus, type, message);
			delete pack;
			return;
		}
		strcpy((char *) sem->to, to);
	}
	else {
		sem->to[0] = 0;
	}

	sem->guilddbid = to_guilddbid;
	sem->minstatus = to_minstatus;
	sem->type = type;
	strcpy(&sem->message[0], message);
	char tempto[64]={0};
	if(to)
		strn0cpy(tempto,to,64);
	pack->Deflate();
	if (tempto[0] == 0) {
		SendPacket(pack);
		if (to_guilddbid == 0)
			console_list.SendEmoteMessageRaw(type, message);
	}
	else {
		ZoneServer* zs = FindByName(to);

		if (zs != 0)
			zs->SendPacket(pack);
		else
			SendPacket(pack);
	}
	delete pack;
}

void ZSList::SendTimeSync() {
	ServerPacket* pack = new ServerPacket(ServerOP_SyncWorldTime, sizeof(eqTimeOfDay));
	eqTimeOfDay* tod = (eqTimeOfDay*) pack->pBuffer;
	tod->start_eqtime=worldclock.getStartEQTime();
	tod->start_realtime=worldclock.getStartRealTime();
	SendPacket(pack);
	delete pack;
}

void ZSList::SendPlayerCounts() {
	ServerPacket* pack = new ServerPacket(ServerOP_PlayerCounts, sizeof(PlayerInfo_Struct));
	PlayerInfo_Struct* pis = (PlayerInfo_Struct*) pack->pBuffer;
	pis->neutralCount = client_list.GetNeutralCount();
	pis->teamOneCount = client_list.GetTeamOneCount();
	pis->teamTwoCount = client_list.GetTeamTwoCount();
	pis->teamThreeCount = client_list.GetTeamThreeCount();
	pis->teamFourCount = client_list.GetTeamFourCount();
	pis->teamFiveCount = client_list.GetTeamFiveCount();

	SendPacket(pack);
	delete pack;
}

void ZSList::NextGroupIDs(int32 &start, int32 &end) {
	start = CurGroupID;
	CurGroupID += 1000;	//hand them out 1000 at a time...
	if(CurGroupID < start) {	//handle overflow
		start = 1;
		CurGroupID = 1001;
	}
	end = CurGroupID - 1;
}

void ZSList::SOPZoneBootup(const char* adminname, int32 ZoneServerID, const char* zonename, bool iMakeStatic) {
	ZoneServer* zs = 0;
	ZoneServer* zs2 = 0;
	int32 zoneid;
	// Kings & Bandits - zone bootup Changed to use the captured zoneid we get back instead.
	if (!(zoneid = database.GetZoneID(zonename)))
		SendEmoteMessage(adminname, 0, 0, 0, "Error: SOP_ZoneBootup: zone '%s' not found in 'zone' table. Typo protection=ON.", zonename);
	else {
		if (zoneid != 0)
			zs = FindByID(zoneid);
		else
			SendEmoteMessage(adminname, 0, 0, 0, "Error: SOP_ZoneBootup: ServerID must be specified");

		if (zs == 0)
			SendEmoteMessage(adminname, 0, 0, 0, "Error: SOP_ZoneBootup: zoneserver not found");
		else {
			zs2 = FindByName(zonename);
			if (zs2 != 0)
				SendEmoteMessage(adminname, 0, 0, 0, "Error: SOP_ZoneBootup: zone '%s' already being hosted by ZoneServer #%i", zonename, zs2->GetID());
			else {
				zs->TriggerBootup(zoneid, 0, adminname, iMakeStatic);
			}
		}
	}
}

void ZSList::RebootZone(const char* ip1,int16 port,const char* ip2, int32 skipid, int32 zoneid){
// get random zone
	LinkedListIterator<ZoneServer*> iterator(list);
	int32 x = 0;
	iterator.Reset();
	while(iterator.MoreElements()) {
		x++;
		iterator.Advance();
	}
	if (x == 0)
		return;
	ZoneServer** tmp = new ZoneServer*[x];
	int32 y = 0;
	iterator.Reset();
	while(iterator.MoreElements()) {
		if (!strcmp(iterator.GetData()->GetCAddress(),ip2) && !iterator.GetData()->IsBootingUp() && iterator.GetData()->GetID() != skipid) {
			tmp[y++] = iterator.GetData();
		}
		iterator.Advance();
	}
	if (y == 0) {
		safe_delete(tmp);
		return;
	}
	int32 z = MakeRandomInt(0, y-1);
	
	ServerPacket* pack = new ServerPacket(ServerOP_ZoneReboot, sizeof(ServerZoneReboot_Struct));
	ServerZoneReboot_Struct* s = (ServerZoneReboot_Struct*) pack->pBuffer;
//	strcpy(s->ip1,ip1);
	strcpy(s->ip2,ip2);
	s->port = port;
	s->zoneid = zoneid;
	if(zoneid != 0)
		_log(WORLD__ZONELIST,"Rebooting static zone with the ID of: %i",zoneid);
	tmp[z]->SendPacket(pack);
	delete pack;
	safe_delete_array(tmp);
}

uint16	ZSList::GetAvailableZonePort()
{
	const WorldConfig *Config=WorldConfig::get();
	int i;
	uint16 port=0;

	if (LastAllocatedPort==0)
		i=Config->ZonePortLow;
	else
		i=LastAllocatedPort+1;

	while(i!=LastAllocatedPort && port==0) {
		if (i>Config->ZonePortHigh) 
			i=Config->ZonePortLow;

		if (!FindByPort(i)) {
			port=i;
			break;
		}
		i++;
	}
	LastAllocatedPort=port;

	return port;
}

int32 ZSList::TriggerBootup(int32 iZoneID, int32 iInstanceID) {
	if(iInstanceID > 0)
	{
		LinkedListIterator<ZoneServer*> iterator(list);
		iterator.Reset();
		while(iterator.MoreElements()) {
			if(iterator.GetData()->GetInstanceID() == iInstanceID)
			{
				return iterator.GetData()->GetID();
			}
			iterator.Advance();
		}

		iterator.Reset();
		while(iterator.MoreElements()) {
			if (iterator.GetData()->GetZoneID() == 0 && !iterator.GetData()->IsBootingUp()) {
				ZoneServer* zone=iterator.GetData();
				zone->TriggerBootup(iZoneID, iInstanceID);
				return zone->GetID();
			}
			iterator.Advance();
		}
		return 0;
	}
	else
	{
		LinkedListIterator<ZoneServer*> iterator(list);
		iterator.Reset();
		while(iterator.MoreElements()) {
			if(iterator.GetData()->GetZoneID() == iZoneID && iterator.GetData()->GetInstanceID() == 0)
			{
				return iterator.GetData()->GetID();
			}
			iterator.Advance();
		}

		iterator.Reset();
		while(iterator.MoreElements()) {
			if (iterator.GetData()->GetZoneID() == 0 && !iterator.GetData()->IsBootingUp()) {
				ZoneServer* zone=iterator.GetData();
				zone->TriggerBootup(iZoneID);
				return zone->GetID();
			}
			iterator.Advance();
		}
		return 0;
	}
	/*Old Random boot zones use this if your server is distributed across computers.
	LinkedListIterator<ZoneServer*> iterator(list);

	srand(time(NULL));
	int32 x = 0;
	iterator.Reset();
	while(iterator.MoreElements()) {
		x++;
		iterator.Advance();
	}
	if (x == 0) {
		return 0;
	}

	ZoneServer** tmp = new ZoneServer*[x];
	int32 y = 0;

	iterator.Reset();
	while(iterator.MoreElements()) {
		if (iterator.GetData()->GetZoneID() == 0 && !iterator.GetData()->IsBootingUp()) {
			tmp[y++] = iterator.GetData();
		}
		iterator.Advance();
	}
	if (y == 0) {
		safe_delete(tmp);
		return 0;
	}

	int32 z = rand() % y;
	
	tmp[z]->TriggerBootup(iZoneID);
	int32 ret = tmp[z]->GetID();
	safe_delete(tmp);
	return ret;
	*/
}

void ZSList::SendLSZones(){
	LinkedListIterator<ZoneServer*> iterator(list);
	iterator.Reset();
	while(iterator.MoreElements()) {
		ZoneServer* zs = iterator.GetData();
		zs->LSBootUpdate(zs->GetZoneID(),true);
		iterator.Advance();
	}
}

int ZSList::GetZoneCount() {
	return(numzones);
}

void ZSList::GetZoneIDList(vector<int32> &zones) {
	LinkedListIterator<ZoneServer*> iterator(list);
	iterator.Reset();
	while(iterator.MoreElements()) {
		ZoneServer* zs = iterator.GetData();
		zones.push_back(zs->GetID());
		iterator.Advance();
	}
	
}

uint32 ZSList::GetZoneCount(ZONE_TYPES zoneType) {
	uint32 a = 0,b = 0,s = 0; // available, booted, static
	LinkedListIterator<ZoneServer*> iterator(list);
	iterator.Reset();
	while(iterator.MoreElements()) {
		ZoneServer* zs = iterator.GetData();

		if(zs->IsStaticZone())
			s++;
		else if (zs->GetZoneID() != 0)
			b++;
		else if(zs->GetZoneID() == 0 && !zs->IsBootingUp())
			a++;
		
		iterator.Advance();
	}
	uint32 result = 0;
	switch(zoneType)
	{
		case ZONE_STATIC:
		{
			result = s;
			break;
		}
		case ZONE_BOOTED:
		{
			result = b;
			break;
		}
		case ZONE_AVAILABLE:
		{
			result = a;
			break;
		}
		case ZONE_ALL:
		{
			result = s + b + a;
			break;
		}
	}
	return result;
}

void ZSList::SendServerStats() {
	ServerPacket* pack = new ServerPacket(ServerOP_StatAuth, sizeof(ZoneStatusAuth_Struct));
	ZoneStatusAuth_Struct* zsa = (ZoneStatusAuth_Struct*) pack->pBuffer;
	zsa->staticZones=GetZoneCount(ZONE_STATIC);
	zsa->bootedZones=GetZoneCount(ZONE_BOOTED);
	zsa->availZones=GetZoneCount(ZONE_AVAILABLE);
	SendPacket(pack);
	delete pack;
}