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
#include "clientlist.h"
#include "zoneserver.h"
#include "zonelist.h"
#include "client.h"
#include "console.h"
#include "worlddb.h"
#include "../common/guilds.h"
#include "../common/races.h"
#include "../common/classes.h"
#include "../common/packet_dump.h"
#include "wguild_mgr.h"

#include <set>
using namespace std;

extern ConsoleList		console_list;
extern ZSList			zoneserver_list;
int32 numplayers = 0;	//this really wants to be a member variable of ClientList...


ClientList::ClientList()
: CLStale_timer(30000)
{
	NextCLEID = 1;
	mNeutralCount = 0;
	mTeamOneCount = 0;
	mTeamTwoCount = 0;
	mTeamThreeCount = 0;
	mTeamFourCount = 0;
	mTeamFiveCount = 0;
}

ClientList::~ClientList() {
}


void ClientList::Process() {
	
	if (CLStale_timer.Check())
		CLCheckStale();
	
	
	LinkedListIterator<Client*> iterator(list);

	iterator.Reset();
	while(iterator.MoreElements()) {
		if (!iterator.GetData()->Process()) {
			struct in_addr  in;
			in.s_addr = iterator.GetData()->GetIP();
			_log(WORLD__CLIENTLIST,"Removing client from %s:%d", inet_ntoa(in), iterator.GetData()->GetPort());
//the client destructor should take care of this.
//			iterator.GetData()->Free();
			iterator.RemoveCurrent();
		}
		else
			iterator.Advance();
	}
}


void ClientList::CLERemoveZSRef(ZoneServer* iZS) {
	LinkedListIterator<ClientListEntry*> iterator(clientlist);

	iterator.Reset();
	while(iterator.MoreElements()) {
		if (iterator.GetData()->Server() == iZS) {
			iterator.GetData()->ClearServer(); // calling this before LeavingZone() makes CLE not update the number of players in a zone
			iterator.GetData()->LeavingZone();
		}
		iterator.Advance();
	}
}

ClientListEntry* ClientList::GetCLE(int32 iID) {
	LinkedListIterator<ClientListEntry*> iterator(clientlist);


	iterator.Reset();
	while(iterator.MoreElements()) {
		if (iterator.GetData()->GetID() == iID) {
			return iterator.GetData();
		}
		iterator.Advance();
	}
	return 0;
}

//Account Limiting Code to limit the number of characters allowed on from a single account at once.
void ClientList::EnforceSessionLimit(int32 iLSAccountID) {

	ClientListEntry* ClientEntry = 0;

	LinkedListIterator<ClientListEntry*> iterator(clientlist, BACKWARD);

	int CharacterCount = 0;

	iterator.Reset();

	while(iterator.MoreElements()) {

		ClientEntry = iterator.GetData();

		if ((ClientEntry->LSAccountID() == iLSAccountID) &&
		    ((ClientEntry->Admin() <= (RuleI(World, ExemptAccountLimitStatus))) || (RuleI(World, ExemptAccountLimitStatus) < 0))) {

			CharacterCount++;

			if (CharacterCount >= (RuleI(World, AccountSessionLimit))){
				// If we have a char name, they are in a zone, so send a kick to the zone server
				if(strlen(ClientEntry->name())) {

					ServerPacket* pack = new ServerPacket(ServerOP_KickPlayer, sizeof(ServerKickPlayer_Struct));
					ServerKickPlayer_Struct* skp = (ServerKickPlayer_Struct*) pack->pBuffer;
					strcpy(skp->adminname, "SessionLimit");
					strcpy(skp->name, ClientEntry->name());
					skp->adminrank = 255;
					zoneserver_list.SendPacket(pack);
					safe_delete(pack);
				}

				ClientEntry->SetOnline(CLE_Status_Offline);

				iterator.RemoveCurrent();

				continue;
			}
		}
		iterator.Advance();
	}
}


//Check current CLE Entry IPs against incoming connection

// countCharacterID = true means we count the instances of that charid and nothing else
int ClientList::GetCLEIP(int32 iIP,int32 charID,int32 acctID, ClientListEntry* origEntry, bool countCharacterID, bool byAccountID) {

	ClientListEntry* countCLEIPs = 0;
	LinkedListIterator<ClientListEntry*> iterator(clientlist);

	int IPInstances = 0;
	iterator.Reset();

	while(iterator.MoreElements()) {

		countCLEIPs = iterator.GetData();

		if ( origEntry == countCLEIPs )
		{
			iterator.Advance();
			continue;
		}

		// If the IP matches, and the connection admin status is below the exempt status,
		// or exempt status is less than 0 (no-one is exempt)
		if ( byAccountID && countCLEIPs->AccountID() == acctID && countCLEIPs != origEntry )
			IPInstances++;
		else if (!byAccountID && (countCLEIPs->GetOnlineStatus() == CLE_Status_Zoning || 
			countCLEIPs->GetOnlineStatus() == CLE_Status_InZone) && (countCLEIPs->GetIP() == iIP) && 
			((countCLEIPs->Admin() < (RuleI(World, ExemptMaxClientsStatus))) ||
			(RuleI(World, ExemptMaxClientsStatus) < 0))) {

				if ( countCharacterID )
				{
					if ( ( iterator.GetData()->CharID() == charID ) )
						IPInstances++;

				} // if we aren't counting matching char ids then just add up those that don't match the charid passed
				else if ( ( iterator.GetData()->CharID() != charID ) )
					IPInstances++;
		}
		iterator.Advance();
	}

	return IPInstances;
}

ClientListEntry* ClientList::FindCharacter(const char* name) {
	LinkedListIterator<ClientListEntry*> iterator(clientlist);

	iterator.Reset();
	while(iterator.MoreElements())
	{
		if (strcasecmp(iterator.GetData()->name(), name) == 0) {
			return iterator.GetData();
		}
		iterator.Advance();
	}
	return 0;
}


ClientListEntry* ClientList::FindCLEByAccountID(int32 iAccID) {
	LinkedListIterator<ClientListEntry*> iterator(clientlist);

	iterator.Reset();
	while(iterator.MoreElements()) {
		if (iterator.GetData()->AccountID() == iAccID) {
			return iterator.GetData();
		}
		iterator.Advance();
	}
	return 0;
}

ClientListEntry* ClientList::FindCLEByCharacterID(int32 iCharID) {
	LinkedListIterator<ClientListEntry*> iterator(clientlist);
 
	iterator.Reset();
	while(iterator.MoreElements()) {
		if (iterator.GetData()->CharID() == iCharID) {
			return iterator.GetData();
		}
		iterator.Advance();
	}
	return 0;
}

void ClientList::SendCLEList(const sint16& admin, const char* to, WorldTCPConnection* connection, const char* iName) {
	LinkedListIterator<ClientListEntry*> iterator(clientlist);
	char* output = 0;
	int32 outsize = 0, outlen = 0;
	int x = 0, y = 0;
	int namestrlen = iName == 0 ? 0 : strlen(iName);
	bool addnewline = false;
	char newline[3];
	if (connection->IsConsole())
		strcpy(newline, "\r\n");
	else
		strcpy(newline, "^");

	iterator.Reset();
	while(iterator.MoreElements()) {
		ClientListEntry* cle = iterator.GetData();
		if (admin >= cle->Admin() && (iName == 0 || namestrlen == 0 || strncasecmp(cle->name(), iName, namestrlen) == 0 || strncasecmp(cle->AccountName(), iName, namestrlen) == 0 || strncasecmp(cle->LSName(), iName, namestrlen) == 0)) {
			struct in_addr  in;
			in.s_addr = cle->GetIP();
			if (addnewline) {
				AppendAnyLenString(&output, &outsize, &outlen, newline);
			}
			AppendAnyLenString(&output, &outsize, &outlen, "ID: %i  Acc# %i  AccName: %s  IP: %s", cle->GetID(), cle->AccountID(), cle->AccountName(), inet_ntoa(in));
			AppendAnyLenString(&output, &outsize, &outlen, "%s  Stale: %i  Online: %i  Admin: %i", newline, cle->GetStaleCounter(), cle->Online(), cle->Admin());
			if (cle->LSID())
				AppendAnyLenString(&output, &outsize, &outlen, "%s  LSID: %i  LSName: %s  WorldAdmin: %i", newline, cle->LSID(), cle->LSName(), cle->WorldAdmin());
			if (cle->CharID())
				AppendAnyLenString(&output, &outsize, &outlen, "%s  CharID: %i  CharName: %s  Zone: %s (%i)", newline, cle->CharID(), cle->name(), database.GetZoneName(cle->zone()), cle->zone());
			if (outlen >= 3072) {
				connection->SendEmoteMessageRaw(to, 0, 0, 10, output);
				safe_delete(output);
				outsize = 0;
				outlen = 0;
				addnewline = false;
			}
			else
				addnewline = true;
			y++;
		}
		iterator.Advance();
		x++;
	}
	AppendAnyLenString(&output, &outsize, &outlen, "%s%i CLEs in memory. %i CLEs listed. numplayers = %i.", newline, x, y, numplayers);
	connection->SendEmoteMessageRaw(to, 0, 0, 10, output);
	safe_delete(output);
}


void ClientList::CLEAdd(int32 iLSID, const char* iLoginName, const char* iLoginKey, sint16 iWorldAdmin, int32 ip, uint8 local, bool localLoginServer) {
	ClientListEntry* tmp = new ClientListEntry(GetNextCLEID(), iLSID, iLoginName, iLoginKey, iWorldAdmin, ip, local, localLoginServer);

	clientlist.Append(tmp);
}

void ClientList::CLCheckStale() {
	LinkedListIterator<ClientListEntry*> iterator(clientlist);

	iterator.Reset();
	while(iterator.MoreElements()) {
		if (iterator.GetData()->CheckStale()) {
			iterator.RemoveCurrent();
		}
		else
			iterator.Advance();
	}
}

void ClientList::RemoveCLE(ClientListEntry* cle) {
	LinkedListIterator<ClientListEntry*> iterator(clientlist);

	iterator.Reset();
	while(iterator.MoreElements()) {
		if (iterator.GetData() == cle) {
			cle->SetOnline(CLE_Status_Offline);
			iterator.RemoveCurrent();
			break;
		}
		else
			iterator.Advance();
	}
}

void ClientList::ClientUpdate(ZoneServer* zoneserver, ServerClientList_Struct* scl) {
	LinkedListIterator<ClientListEntry*> iterator(clientlist);
	ClientListEntry* cle;
	iterator.Reset();
	while(iterator.MoreElements()) {
		if (iterator.GetData()->GetID() == scl->wid) {
			cle = iterator.GetData();
			if (scl->remove == 2){
				cle->LeavingZone(zoneserver, CLE_Status_Offline);
			}
			else if (scl->remove == 1)
				cle->LeavingZone(zoneserver, CLE_Status_Zoning);
			else
				cle->Update(zoneserver, scl);
			return;
		}
		iterator.Advance();
	}
	if (scl->remove == 2)
		cle = new ClientListEntry(GetNextCLEID(), zoneserver, scl, CLE_Status_Online);
	else if (scl->remove == 1)
	{
		cle = new ClientListEntry(GetNextCLEID(), zoneserver, scl, CLE_Status_Zoning);
		whotimestamps[scl->charid] = Timer::GetCurrentTime();
	}
	else
		cle = new ClientListEntry(GetNextCLEID(), zoneserver, scl, CLE_Status_InZone);
	clientlist.Insert(cle);
	zoneserver->ChangeWID(scl->charid, cle->GetID());
}

void ClientList::CLEKeepAlive(int32 numupdates, int32* wid) {
	LinkedListIterator<ClientListEntry*> iterator(clientlist);
	int32 i;

	iterator.Reset();
	while(iterator.MoreElements()) {
		for (i=0; i<numupdates; i++) {
			if (wid[i] == iterator.GetData()->GetID())
				iterator.GetData()->KeepAlive();
		}
		iterator.Advance();
	}
}


ClientListEntry* ClientList::CheckAuth(int32 id, const char* iKey, int32 ip ) {
  LinkedListIterator<ClientListEntry*> iterator(clientlist);

	iterator.Reset();
	while(iterator.MoreElements()) {
		if (iterator.GetData()->CheckAuth(id, iKey, ip))
			return iterator.GetData();
		iterator.Advance();
	}
	return 0;
}
ClientListEntry* ClientList::CheckAuth(int32 iLSID, const char* iKey) {
  LinkedListIterator<ClientListEntry*> iterator(clientlist);

	iterator.Reset();
	while(iterator.MoreElements()) {
		if (iterator.GetData()->CheckAuth(iLSID, iKey))
			return iterator.GetData();
		iterator.Advance();
	}
	return 0;
}

ClientListEntry* ClientList::CheckAuth(const char* iName, const char* iPassword) {
	LinkedListIterator<ClientListEntry*> iterator(clientlist);
	MD5 tmpMD5(iPassword);

	iterator.Reset();
	while(iterator.MoreElements()) {
		if (iterator.GetData()->CheckAuth(iName, tmpMD5))
			return iterator.GetData();
		iterator.Advance();
	}
	sint16 tmpadmin;
	
	_log(WORLD__ZONELIST,"Login with '%s' and '%s'", iName, iPassword);
	
	bool remoteLogin;

	int32 accid = database.CheckLogin(iName, iPassword, &tmpadmin, &remoteLogin);
	bool localServer = true;
	
	if ( remoteLogin )
		localServer = false;

	if (accid) {
		ClientListEntry* tmp = new ClientListEntry(GetNextCLEID(), accid, iName, tmpMD5, tmpadmin, localServer);
		clientlist.Append(tmp);
		return tmp;
	}
	return 0;
}

void ClientList::SendOnlineGuildMembers(uint32 FromID, uint32 GuildID)
{
	int PacketLength = 8;

	uint32 Count = 0;
	ClientListEntry* from = this->FindCLEByCharacterID(FromID);

	if(!from)
	{
		_log(WORLD__CLIENT_ERR,"Invalid client. FromID=%i GuildID=%i", FromID, GuildID);
		return;
	}

	LinkedListIterator<ClientListEntry*> Iterator(clientlist);

	Iterator.Reset();

	while(Iterator.MoreElements())
	{
		ClientListEntry* CLE = Iterator.GetData();

		if(CLE && (CLE->GuildID() == GuildID))
		{
			PacketLength += (strlen(CLE->name()) + 5);
			++Count;
		}

		Iterator.Advance();

	}

	Iterator.Reset();

	ServerPacket* pack = new ServerPacket(ServerOP_OnlineGuildMembersResponse, PacketLength);

	char *Buffer = (char *)pack->pBuffer;

	VARSTRUCT_ENCODE_TYPE(uint32, Buffer, FromID);
	VARSTRUCT_ENCODE_TYPE(uint32, Buffer, Count);

	while(Iterator.MoreElements())
	{
		ClientListEntry* CLE = Iterator.GetData();

		if(CLE && (CLE->GuildID() == GuildID))
		{
			VARSTRUCT_ENCODE_STRING(Buffer, CLE->name());
			if ( RuleB(Guild,ShowGuildMembersZone) )
			{
				VARSTRUCT_ENCODE_TYPE(uint32, Buffer, CLE->zone());
			}
			else
			{
				VARSTRUCT_ENCODE_TYPE(uint32, Buffer, 0);
			}
		}

		Iterator.Advance();
	}
	zoneserver_list.SendPacket(from->zone(), from->instance(), pack);
	safe_delete(pack);
}


void ClientList::SendWhoAll(int32 fromid,const char* to, sint16 admin, Who_All_Struct* whom, WorldTCPConnection* connection, int32 charid) {
	try{
	LinkedListIterator<ClientListEntry*> iterator(clientlist);
	LinkedListIterator<ClientListEntry*> countclients(clientlist);
	ClientListEntry* cle = 0;
	ClientListEntry* countcle = 0;

	//char tmpgm[25] = "";
	//char accinfo[150] = "";
	char line[300] = "";
	char remoteClientAppend[] = { " - EQEMULOGIN" };
	//char tmpguild[50] = "";
	//char LFG[10] = "";
	//int32 x = 0;
	int whomlen = 0;
	if (whom) {
		whomlen = strlen(whom->whom);
		if(whom->wrace == 0x001A) // 0x001A is the old Froglok race number and is sent by the client for /who all froglok
			whom->wrace = FROGLOK; // This is what EQEmu uses for the Froglok Race number.
	}

	char* output = 0;
	int32 outsize = 0, outlen = 0;
	int32 totalusers=0;
	int32 totallength=0;
	AppendAnyLenString(&output, &outsize, &outlen, "Players on server:");
	if (connection->IsConsole())
		AppendAnyLenString(&output, &outsize, &outlen, "\r\n");
	else
		AppendAnyLenString(&output, &outsize, &outlen, "\n");
	countclients.Reset();
	
	bool zoneNameStale = false; // if they have used it recently or not
	if ( whotimestamps[charid] > 0 )
	{
		int32 diffTime = Timer::GetCurrentTime() - whotimestamps[charid];
		if ( diffTime < RuleI(World, WhoAllZoneRequestMS) )
			zoneNameStale = true;
	}
	bool zoneNameTriggered = false;
	while(countclients.MoreElements()){
		countcle = countclients.GetData();
		const char* tmpZone = database.GetZoneName(countcle->zone(),countcle->instance());
		if (
	(countcle->Online() >= CLE_Status_Zoning) &&
	(!countcle->GetGM() || countcle->Anon() != 1 || admin >= countcle->Admin()) && 
	(whom == 0 || (
		((countcle->Admin() >= 80 && countcle->GetGM()) || whom->gmlookup == 0xFFFF) && 
		(whom->lvllow == 0xFFFF || (countcle->level() >= whom->lvllow && countcle->level() <= whom->lvlhigh && (countcle->Anon()==0 || admin > countcle->Admin()))) && 
		(whom->wclass == 0xFFFF || (countcle->class_() == whom->wclass && (countcle->Anon()==0 || admin > countcle->Admin()))) && 
		(whom->wrace == 0xFFFF || (countcle->race() == whom->wrace && (countcle->Anon()==0 || admin > countcle->Admin()))) &&
		(whomlen == 0 || (
			(admin > 80 && tmpZone != 0 && strncasecmp(tmpZone, whom->whom, whomlen) == 0) || 
			// Image: took out on 4/18/2013 -- changing to limit how often this can be triggered, zoneNameTriggered is set to true if we match all these cases
			//(countcle->Admin() == 0 && countcle->level() > 45 && tmpZone != 0 && strncasecmp(tmpZone, whom->whom, whomlen) == 0 && (zoneNameTriggered = true)) || 
			strncasecmp(countcle->name(),whom->whom, whomlen) == 0 || 
			(strncasecmp(guild_mgr.GetGuildName(countcle->GuildID()), whom->whom, whomlen) == 0) || 
			(admin >= 100 && strncasecmp(countcle->AccountName(), whom->whom, whomlen) == 0)
		))
	))
) {

	if ( admin == 0 && zoneNameTriggered )
	{
		if ( zoneNameStale )
		{
			ServerPacket* repPack = new ServerPacket(ServerOP_WhoAllReply, 64);
			memset(repPack->pBuffer,0,repPack->size);
			WhoAllReturnStruct *WARS = (WhoAllReturnStruct *)repPack->pBuffer;

			WARS->id = fromid;
			WARS->playerineqstring = 0xffffffff;
			strcpy(WARS->line, "");
			SendPacket(to,repPack);
			safe_delete(repPack);
			safe_delete(output);
			return;
		}
		else
			whotimestamps[charid] = Timer::GetCurrentTime();
	}
			if((countcle->Anon()>0 && admin>=countcle->Admin() && admin>0) || countcle->Anon()==0 ){
				totalusers++;
				if(totalusers<=20 || admin>=100)
				{
					totallength=totallength+strlen(countcle->name())+strlen(countcle->AccountName())+strlen(guild_mgr.GetGuildName(countcle->GuildID()))+5;
				}
			}
			else if((countcle->Anon()>0 && admin<=countcle->Admin()) || countcle->Anon()==0 && !countcle->GetGM()){
				totalusers++;
				if(totalusers<=20 || admin>=100)
					totallength=totallength+strlen(countcle->name())+strlen(guild_mgr.GetGuildName(countcle->GuildID()))+5;
			}
		}
		countclients.Advance();
	}
	int32 plid=fromid;
	int32 playerineqstring=5001;
	const char line2[]="---------------------------";
	int8 unknown35=0x0A;
	int32 unknown36=0;
	int32 playersinzonestring=5028;
	if(totalusers>20 && admin<100){
		totalusers=20;
		playersinzonestring=5033;
	}
	else if(totalusers>1)
		playersinzonestring=5036;
	int32 unknown44[2];
	unknown44[0]=0;
	unknown44[1]=0;
	int32 unknown52=totalusers;
	int32 unknown56=1;
	ServerPacket* pack2 = new ServerPacket(ServerOP_WhoAllReply,64+totallength+(49*totalusers));
	memset(pack2->pBuffer,0,pack2->size);
	uchar *buffer=pack2->pBuffer;
	uchar *bufptr=buffer;
	//memset(buffer,0,pack2->size);
	memcpy(bufptr,&plid, sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&playerineqstring, sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&line2, strlen(line2));
	bufptr+=strlen(line2);
	memcpy(bufptr,&unknown35, sizeof(int8));
	bufptr+=sizeof(int8);
	memcpy(bufptr,&unknown36, sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&playersinzonestring, sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&unknown44[0], sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&unknown44[1], sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&unknown52, sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&unknown56, sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&totalusers, sizeof(int32));
	bufptr+=sizeof(int32);

	iterator.Reset();
	int idx=-1;
	while(iterator.MoreElements()) {
		cle = iterator.GetData();
		
		const char* tmpZone = database.GetZoneName(cle->zone(),cle->instance());
		if (
	(cle->Online() >= CLE_Status_Zoning) &&
	(!cle->GetGM() || cle->Anon() != 1 || admin >= cle->Admin()) && 
	(whom == 0 || (
		((cle->Admin() >= 80 && cle->GetGM()) || whom->gmlookup == 0xFFFF) && 
		(whom->lvllow == 0xFFFF || (cle->level() >= whom->lvllow && cle->level() <= whom->lvlhigh && (cle->Anon()==0 || admin>cle->Admin()))) && 
		(whom->wclass == 0xFFFF || (cle->class_() == whom->wclass && (cle->Anon()==0 || admin>cle->Admin()))) && 
		(whom->wrace == 0xFFFF || (cle->race() == whom->wrace && (cle->Anon()==0 || admin>cle->Admin()))) &&
		(whomlen == 0 || (
			(admin > 80 && tmpZone != 0 && strncasecmp(tmpZone, whom->whom, whomlen) == 0) || 
			// Image: took out on 4/18/2013 -- changing to limit how often this can be triggered, zoneNameTriggered is set to true if we match all these cases
			//(cle->Admin() == 0 && cle->level() > 45 && tmpZone != 0 && strncasecmp(tmpZone, whom->whom, whomlen) == 0 && (zoneNameTriggered = true)) || 
			strncasecmp(cle->name(),whom->whom, whomlen) == 0 || 
			(strncasecmp(guild_mgr.GetGuildName(cle->GuildID()), whom->whom, whomlen) == 0) || 
			(admin >= 100 && strncasecmp(cle->AccountName(), whom->whom, whomlen) == 0)
		))
	))
) {
	if ( admin == 0 && zoneNameTriggered )
	{
		if ( zoneNameStale )
		{
			ServerPacket* repPack = new ServerPacket(ServerOP_WhoAllReply, 64);
			memset(repPack->pBuffer,0,repPack->size);
			WhoAllReturnStruct *WARS = (WhoAllReturnStruct *)repPack->pBuffer;

			WARS->id = fromid;
			WARS->playerineqstring = 0xffffffff;
			strcpy(WARS->line, "");
			SendPacket(to,repPack);
			safe_delete(repPack);
			safe_delete(pack2);
			safe_delete(output);
			return;
		}
		else
			whotimestamps[charid] = Timer::GetCurrentTime();
	}
			line[0] = 0;
			int32 rankstring=0xFFFFFFFF;
				if((cle->Anon()==1 && cle->GetGM() && cle->Admin()>admin) || (idx>=20 && admin<100)){ //hide gms that are anon from lesser gms and normal players, cut off at 20
					rankstring=0;
					iterator.Advance();
					continue;
				} else if (cle->GetGM()) {
					if (cle->Admin() >=250)
						rankstring=5021;
					else if (cle->Admin() >= 200)
						rankstring=5020;
					else if (cle->Admin() >= 180)
						rankstring=5019;
					else if (cle->Admin() >= 170)
						rankstring=5018;
					else if (cle->Admin() >= 160)
						rankstring=5017;
					else if (cle->Admin() >= 150)
						rankstring=5016;
					else if (cle->Admin() >= 100)
						rankstring=5015;
					else if (cle->Admin() >= 95)
						rankstring=5014;
					else if (cle->Admin() >= 90)
						rankstring=5013;
					else if (cle->Admin() >= 85)
						rankstring=5012;
					else if (cle->Admin() >= 81)
						rankstring=5011;
					else if (cle->Admin() >= 80)
						rankstring=5010;
					else if (cle->Admin() >= 50)
						rankstring=5009;
					else if (cle->Admin() >= 20)
						rankstring=5008;
					else if (cle->Admin() >= 10)
						rankstring=5007;
				}
			idx++;
			char guildbuffer[67]={0};
			if (cle->GuildID() != GUILD_NONE && cle->GuildID()>0)
				sprintf(guildbuffer,"<%s>", guild_mgr.GetGuildName(cle->GuildID()));
			int32 formatstring=5025;
			if(cle->Anon()==1 && (admin<cle->Admin() || admin==0))
				formatstring=5024;
			else if(cle->Anon()==1 && admin>=cle->Admin() && admin>0)
				formatstring=5022;
			else if(cle->Anon()==2 && (admin<cle->Admin() || admin==0))
				formatstring=5023;//display guild
			else if(cle->Anon()==2 && admin>=cle->Admin() && admin>0)
				formatstring=5022;//display everything
	
	//war* wars2 = (war*)pack2->pBuffer;

	int32 plclass_=0;
	int32 pllevel=0;
	int32 pidstring=0xFFFFFFFF;//5003;
	int32 plrace=0;
	int32 zonestring=0xFFFFFFFF;
	int32 plzone=0;
	int32 unknown80[2];
	if(cle->Anon()==0 || (admin>=cle->Admin() && admin>0)){
		plclass_=cle->class_();
		pllevel=cle->level();
		if(admin>=100)
			pidstring=5003;
		plrace=cle->race();
		zonestring=5006;
		plzone=cle->zone();
	}
	
	
	if(admin>=cle->Admin() && admin>0)
		unknown80[0]=cle->Admin();
	else
		unknown80[0]=0xFFFFFFFF;
	unknown80[1]=0xFFFFFFFF;//1035
	

	//char plstatus[20]={0};
	//sprintf(plstatus, "Status %i",cle->Admin());
	char plname[64]={0};
	strcpy(plname,cle->name());
	
	char placcount[48]={0};
	if(admin>=cle->Admin() && admin>0)
	{
		strcpy(placcount,cle->AccountName());
	}

	memcpy(bufptr,&formatstring, sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&pidstring, sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&plname, strlen(plname)+1);
	bufptr+=strlen(plname)+1;
	memcpy(bufptr,&rankstring, sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&guildbuffer, strlen(guildbuffer)+1);
	bufptr+=strlen(guildbuffer)+1;
	memcpy(bufptr,&unknown80[0], sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&unknown80[1], sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&zonestring, sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&plzone, sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&plclass_, sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&pllevel, sizeof(int32));
	bufptr+=sizeof(int32);
	memcpy(bufptr,&plrace, sizeof(int32));
	bufptr+=sizeof(int32);
	int32 ending=0;
	memcpy(bufptr,&placcount, strlen(placcount)+1);
	bufptr+=strlen(placcount)+1;
	ending=207;
	memcpy(bufptr,&ending, sizeof(int32));
	bufptr+=sizeof(int32);
		}
		iterator.Advance();
	}
	pack2->Deflate();
	//zoneserver_list.SendPacket(pack2); // NO NO NO WHY WOULD YOU SEND IT TO EVERY ZONE SERVER?!?
	SendPacket(to,pack2);
	safe_delete(pack2);
	safe_delete(output);
	}
	catch(...){
		_log(WORLD__ZONELIST_ERR,"Unknown error in world's SendWhoAll (probably mem error), ignoring...");
		return;
	}
}

void ClientList::SendFriendsWho(ServerFriendsWho_Struct *FriendsWho, WorldTCPConnection* connection) {

	vector<ClientListEntry*> FriendsCLEs;
	FriendsCLEs.reserve(100);

	char Friend_[65];

	char *FriendsPointer = FriendsWho->FriendsString;

	// FriendsString is a comma delimited list of names.

	char *Seperator = NULL;

	Seperator = strchr(FriendsPointer, ',');
	if(!Seperator) Seperator = strchr(FriendsPointer, '\0');

	int32 TotalLength=0;

	while(Seperator != NULL) {

		if((Seperator - FriendsPointer) > 64) return;

		strncpy(Friend_, FriendsPointer, Seperator - FriendsPointer);
		Friend_[Seperator - FriendsPointer] = 0;

		ClientListEntry* CLE = FindCharacter(Friend_);
		if(CLE && CLE->name() && (CLE->Online() >= CLE_Status_Zoning) && !(CLE->GetGM() && CLE->Anon())) {
			FriendsCLEs.push_back(CLE);
			TotalLength += strlen(CLE->name());
			int GuildNameLength = strlen(guild_mgr.GetGuildName(CLE->GuildID()));
			if(GuildNameLength>0)
				TotalLength += (GuildNameLength + 2);
		}

		if(Seperator[0] == '\0') break;

		FriendsPointer = Seperator + 1;
		Seperator = strchr(FriendsPointer, ',');
		if(!Seperator) Seperator = strchr(FriendsPointer, '\0');
	}


	try{
		ClientListEntry* cle;
		int FriendsOnline = FriendsCLEs.size();
		int PacketLength = sizeof(WhoAllReturnStruct) + (47 * FriendsOnline) + TotalLength; 
		ServerPacket* pack2 = new ServerPacket(ServerOP_WhoAllReply, PacketLength);
		memset(pack2->pBuffer,0,pack2->size);
		uchar *buffer=pack2->pBuffer;
		uchar *bufptr=buffer;

		WhoAllReturnStruct *WARS = (WhoAllReturnStruct *)bufptr;

		WARS->id = FriendsWho->FromID;
		WARS->playerineqstring = 0xffffffff;
		strcpy(WARS->line, "");
		WARS->unknown35 = 0x0a;
		WARS->unknown36 = 0x00;

		if(FriendsCLEs.size() == 1)
			WARS->playersinzonestring = 5028; // 5028 There is %1 player in EverQuest.
		else
			WARS->playersinzonestring = 5036; // 5036 There are %1 players in EverQuest.

		WARS->unknown44[0] = 0;
		WARS->unknown44[1] = 0;
		WARS->unknown52 = FriendsOnline;
		WARS->unknown56 = 1;
		WARS->playercount = FriendsOnline;

		bufptr+=sizeof(WhoAllReturnStruct);

		for(int CLEEntry = 0; CLEEntry < FriendsOnline; CLEEntry++) {

			cle = FriendsCLEs[CLEEntry];
		
			char GuildName[67]={0};
			if (cle->GuildID() != GUILD_NONE && cle->GuildID()>0)
				sprintf(GuildName,"<%s>", guild_mgr.GetGuildName(cle->GuildID()));
			int32 FormatMSGID=5025; // 5025 %T1[%2 %3] %4 (%5) %6 %7 %8 %9
			if(cle->Anon()==1)
				FormatMSGID=5024; // 5024 %T1[ANONYMOUS] %2 %3
			else if(cle->Anon()==2)
				FormatMSGID=5023; // 5023 %T1[ANONYMOUS] %2 %3 %4

			int32 PlayerClass=0;
			int32 PlayerLevel=0;
			int32 PlayerRace=0;
			int32 ZoneMSGID=0xffffffff;
			int32 PlayerZone=0;

			if(cle->Anon()==0) {
				PlayerClass=cle->class_();
				PlayerLevel=cle->level();
				PlayerRace=cle->race();
				ZoneMSGID=5006; // 5006 ZONE: %1
				PlayerZone=cle->zone();
			}				

			char PlayerName[64]={0};
			strcpy(PlayerName,cle->name());
	
			WhoAllPlayerPart1* WAPP1 = (WhoAllPlayerPart1*)bufptr;

			WAPP1->FormatMSGID = FormatMSGID;
			WAPP1->PIDMSGID = 0xffffffff;
			strcpy(WAPP1->Name, PlayerName);

			bufptr += sizeof(WhoAllPlayerPart1) + strlen(PlayerName);
			WhoAllPlayerPart2* WAPP2 = (WhoAllPlayerPart2*)bufptr;

			WAPP2->RankMSGID = 0xffffffff;
			strcpy(WAPP2->Guild, GuildName);

			bufptr += sizeof(WhoAllPlayerPart2) + strlen(GuildName);
			WhoAllPlayerPart3* WAPP3 = (WhoAllPlayerPart3*)bufptr;

			WAPP3->Unknown80[0] = 0xffffffff;
			WAPP3->Unknown80[1] = 0xffffffff;
			WAPP3->ZoneMSGID = ZoneMSGID;
			WAPP3->Zone = PlayerZone;
			WAPP3->Class_ = PlayerClass;
			WAPP3->Level =  PlayerLevel;
			WAPP3->Race = PlayerRace;
			WAPP3->Account[0] = 0;

			bufptr += sizeof(WhoAllPlayerPart3);

			WhoAllPlayerPart4* WAPP4 = (WhoAllPlayerPart4*)bufptr;
			WAPP4->Unknown100 = 207;

			bufptr += sizeof(WhoAllPlayerPart4);

		}
		pack2->Deflate();
		SendPacket(FriendsWho->FromName,pack2);
		safe_delete(pack2);
	}	
	catch(...){
		_log(WORLD__ZONELIST_ERR,"Unknown error in world's SendFriendsWho (probably mem error), ignoring...");
		return;
	}
}

void ClientList::SendLFGMatches(ServerLFGMatchesRequest_Struct *smrs) {

	// Send back matches when someone searches player's Looking For A Group.

	LinkedListIterator<ClientListEntry*> Iterator(clientlist);
	ClientListEntry* CLE = 0;
	int Matches = 0;

	Iterator.Reset();

	// We run the ClientList twice. The first time is to determine how big the outgoing packet needs to be.
	while(Iterator.MoreElements()) {
		CLE = Iterator.GetData();
		if(CLE->LFG()) {
			unsigned int BitMask = 1 << CLE->class_();
			// First we check that the player meets the level and class criteria of the person
			// doing the search.
			if((CLE->level() >= smrs->FromLevel) && (CLE->level() <= smrs->ToLevel) &&
			   (BitMask & smrs->Classes))
			   	// Then we check if if the player doing the search meets the level criteria specified
				// by the player who is LFG.
				//
				// GetLFGMatchFilter returns the setting of the 'Only players who match my posted filters
				//						 can query me' checkbox.
				//
				// FromLevel and ToLevel are the settings of the 'Want group levels:' boxes.
			   	if(!CLE->GetLFGMatchFilter() || ((smrs->QuerierLevel >= CLE->GetLFGFromLevel()) &&
								 (smrs->QuerierLevel <= CLE->GetLFGToLevel())))
					Matches++;
		}
		Iterator.Advance();
	}
	ServerPacket* Pack = new ServerPacket(ServerOP_LFGMatches, (sizeof(ServerLFGMatchesResponse_Struct) * Matches) + 4);

	char *Buf = (char *)Pack->pBuffer;
	// FromID is the Entity ID of the player doing the search.
	VARSTRUCT_ENCODE_TYPE(int32, Buf, smrs->FromID);

	ServerLFGMatchesResponse_Struct* Buffer = (ServerLFGMatchesResponse_Struct*)Buf;

	Iterator.Reset();

	if(Matches) {
		while(Iterator.MoreElements() && (Matches > 0)) {
			CLE = Iterator.GetData();
			if(CLE->LFG()) {
				unsigned int BitMask = 1 << CLE->class_();
				if((CLE->level() >= smrs->FromLevel) && (CLE->level() <= smrs->ToLevel) &&
				   (BitMask & smrs->Classes)) {
					Matches--;
					strcpy(Buffer->Name, CLE->name());
					Buffer->Class_ = CLE->class_();
					Buffer->Level = CLE->level();
					Buffer->Zone = CLE->zone();
					// If the LFG player is anon, level and class are still displayed, but
					// zone shows as UNAVAILABLE.
					Buffer->Anon = (CLE->Anon() != 0);
					// The client can filter on Guildname
					Buffer->GuildID = CLE->GuildID();
					strcpy(Buffer->Comments, CLE->GetLFGComments());
					Buffer++;
				}
			}
			Iterator.Advance();
		}
		Pack->Deflate();
	}
	SendPacket(smrs->FromName,Pack);
	safe_delete(Pack);
}

void ClientList::ConsoleSendWhoAll(const char* to, sint16 admin, Who_All_Struct* whom, WorldTCPConnection* connection) {
	LinkedListIterator<ClientListEntry*> iterator(clientlist);
	ClientListEntry* cle = 0;
	char tmpgm[25] = "";
	char accinfo[150] = "";
	char line[300] = "";
	char tmpguild[50] = "";
	char LFG[10] = "";
	int32 x = 0;
	int whomlen = 0;
	if (whom)
		whomlen = strlen(whom->whom);

	char* output = 0;
	int32 outsize = 0, outlen = 0;
	AppendAnyLenString(&output, &outsize, &outlen, "Players on server:");
	if (connection->IsConsole())
		AppendAnyLenString(&output, &outsize, &outlen, "\r\n");
	else
		AppendAnyLenString(&output, &outsize, &outlen, "\n");
	iterator.Reset();
	while(iterator.MoreElements()) {
		cle = iterator.GetData();
		const char* tmpZone = database.GetZoneName(cle->zone(),cle->instance());
		if (
	(cle->Online() >= CLE_Status_Zoning)
	&& (whom == 0 || (
		((cle->Admin() >= 80 && cle->GetGM()) || whom->gmlookup == 0xFFFF) && 
		(whom->lvllow == 0xFFFF || (cle->level() >= whom->lvllow && cle->level() <= whom->lvlhigh)) && 
		(whom->wclass == 0xFFFF || cle->class_() == whom->wclass) && 
		(whom->wrace == 0xFFFF || cle->race() == whom->wrace) && 
		(whomlen == 0 || (
			(tmpZone != 0 && strncasecmp(tmpZone, whom->whom, whomlen) == 0) || 
			strncasecmp(cle->name(),whom->whom, whomlen) == 0 || 
			(strncasecmp(guild_mgr.GetGuildName(cle->GuildID()), whom->whom, whomlen) == 0) || 
			(admin >= 100 && strncasecmp(cle->AccountName(), whom->whom, whomlen) == 0)
		))
	))
) {
			line[0] = 0;
// MYRA - use new (5.x) Status labels in who for telnet connection			
			if (cle->Admin() >=250)
				strcpy(tmpgm, "* GM-Impossible * ");
			else if (cle->Admin() >= 200)
				strcpy(tmpgm, "* GM-Mgmt * ");
			else if (cle->Admin() >= 180)
				strcpy(tmpgm, "* GM-Coder * ");
			else if (cle->Admin() >= 170)
				strcpy(tmpgm, "* GM-Areas * ");
			else if (cle->Admin() >= 160)
				strcpy(tmpgm, "* QuestMaster * ");
			else if (cle->Admin() >= 150)
				strcpy(tmpgm, "* GM-Lead Admin * ");
			else if (cle->Admin() >= 100)
				strcpy(tmpgm, "* GM-Admin * ");
			else if (cle->Admin() >= 95)
				strcpy(tmpgm, "* GM-Staff * ");
			else if (cle->Admin() >= 90)
				strcpy(tmpgm, "* EQ Support * ");
			else if (cle->Admin() >= 85)
				strcpy(tmpgm, "* GM-Tester * ");
			else if (cle->Admin() >= 81)
				strcpy(tmpgm, "* Senior Guide * ");
			else if (cle->Admin() >= 80)
				strcpy(tmpgm, "* QuestTroupe * ");
			else if (cle->Admin() >= 50)
				strcpy(tmpgm, "* Guide * ");
			else if (cle->Admin() >= 20)
				strcpy(tmpgm, "* Apprentice Guide * ");
			else if (cle->Admin() >= 10)
				strcpy(tmpgm, "* Steward * ");
			else
				tmpgm[0] = 0;
// end Myra		

			if (guild_mgr.GuildExists(cle->GuildID())) {
				snprintf(tmpguild, 36, " <%s>", guild_mgr.GetGuildName(cle->GuildID()));
			} else
				tmpguild[0] = 0;

			if (cle->LFG())
				strcpy(LFG, " LFG");
			else
				LFG[0] = 0;

			if (admin >= 150 && admin >= cle->Admin()) {
				sprintf(accinfo, " AccID: %i AccName: %s LSID: %i Status: %i", cle->AccountID(), cle->AccountName(), cle->LSAccountID(), cle->Admin());
			}
			else
				accinfo[0] = 0;

			if (cle->Anon() == 2) { // Roleplay
				if (admin >= 100 && admin >= cle->Admin())
					sprintf(line, "  %s[RolePlay %i %s] %s (%s)%s zone: %s%s%s", tmpgm, cle->level(), GetEQClassName(cle->class_(),cle->level()), cle->name(), GetRaceName(cle->race()), tmpguild, tmpZone, LFG, accinfo);
				else if (cle->Admin() >= 80 && admin < 80 && cle->GetGM()) {
					iterator.Advance();
					continue;
				}
				else
					sprintf(line, "  %s[ANONYMOUS] %s%s%s%s", tmpgm, cle->name(), tmpguild, LFG, accinfo);
			}
			else if (cle->Anon() == 1) { // Anon
				if (admin >= 100 && admin >= cle->Admin())
					sprintf(line, "  %s[ANON %i %s] %s (%s)%s zone: %s%s%s", tmpgm, cle->level(), GetEQClassName(cle->class_(),cle->level()), cle->name(), GetRaceName(cle->race()), tmpguild, tmpZone, LFG, accinfo);
				else if (cle->Admin() >= 80 && cle->GetGM()) {
					iterator.Advance();
					continue;
				}
				else
					sprintf(line, "  %s[ANONYMOUS] %s%s%s", tmpgm, cle->name(), LFG, accinfo);
			}
			else
				sprintf(line, "  %s[%i %s] %s (%s)%s zone: %s%s%s", tmpgm, cle->level(), GetEQClassName(cle->class_(),cle->level()), cle->name(), GetRaceName(cle->race()), tmpguild, tmpZone, LFG, accinfo);

			AppendAnyLenString(&output, &outsize, &outlen, line);
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
					AppendAnyLenString(&output, &outsize, &outlen, "\n");
			}
			x++;
			if (x >= 20 && admin < 80)
				break;
		}
		iterator.Advance();
	}

	if (x >= 20 && admin < 80)
		AppendAnyLenString(&output, &outsize, &outlen, "too many results...20 players shown");
	else
		AppendAnyLenString(&output, &outsize, &outlen, "%i players online", x);
	if (admin >= 150 && (whom == 0 || whom->gmlookup != 0xFFFF)) {
		if (connection->IsConsole())
			AppendAnyLenString(&output, &outsize, &outlen, "\r\n");
		else
			AppendAnyLenString(&output, &outsize, &outlen, "\n");
		console_list.SendConsoleWho(connection, to, admin, &output, &outsize, &outlen);
	}
	if (output)
		connection->SendEmoteMessageRaw(to, 0, 0, 10, output);
	safe_delete(output);
}

void ClientList::Add(Client* client) {
	list.Insert(client);
}

Client* ClientList::FindByAccountID(int32 account_id) {
	LinkedListIterator<Client*> iterator(list);

	iterator.Reset();
	while(iterator.MoreElements()) {
		_log(WORLD__CLIENTLIST, "ClientList[0x%08x]::FindByAccountID(%p) iterator.GetData()[%p]", this, account_id, iterator.GetData());
		if (iterator.GetData()->GetAccountID() == account_id) {
			Client* tmp = iterator.GetData();
			return tmp;
		}
		iterator.Advance();
	}
	return 0;
}

Client* ClientList::FindByName(char* charname) {
	LinkedListIterator<Client*> iterator(list);

	iterator.Reset();
	while(iterator.MoreElements()) {
		_log(WORLD__CLIENTLIST, "ClientList[0x%08x]::FindByName(\"%s\") iterator.GetData()[%p]", this, charname, iterator.GetData());
		if (iterator.GetData()->GetCharName() == charname) {
			Client* tmp = iterator.GetData();
			return tmp;
		}
		iterator.Advance();
	}
	return 0;
}

Client* ClientList::FindByCharID(int32 charid) {
	LinkedListIterator<Client*> iterator(list);

	if ( charid == 0 )
		return NULL;

	iterator.Reset();
	while(iterator.MoreElements()) {
		_log(WORLD__CLIENTLIST, "ClientList[0x%08x]::FindByCharID(\"%d\") iterator.GetData()[%p]", this, charid, iterator.GetData());
		if (iterator.GetData()->GetCharID() == charid) {
			Client* tmp = iterator.GetData();
			return tmp;
		}
		iterator.Advance();
	}
	return 0;
}

bool ClientList::DetermineXPBonus(uint32 teamID, int pctValue)
{
	TeamSettings_Struct* settings = database.GetTeamSettings(teamID);
	if ( !settings )
		return false;

		if ( teamID < 5 && !settings->upkeepPaid ) // bandits dont pay upkeep so check all teams less than 5
		{
			settings->teamXPBonus = 0;
		}
		else if ( pctValue < 5 )
		{
			settings->teamXPBonus = 15;
		}
		else if ( pctValue < 10 )
		{
			settings->teamXPBonus = 10;
		}
		else if ( pctValue < 20 )
		{
			settings->teamXPBonus = 7;
		}
		else if ( pctValue < 30 )
		{
			settings->teamXPBonus = 5;
		}
		else
			settings->teamXPBonus = 3;

	return true;
}

void ClientList::CountPVPTotals(uint8 serverFlag, uint32* neutralPlayers, uint32* teamOne, uint32* teamTwo, uint32* teamThree, uint32* teamFour, uint32* teamFive) {

	mNeutralCount = 0;
	mTeamOneCount = 0;
	mTeamTwoCount = 0;
	mTeamThreeCount = 0;
	mTeamFourCount = 0;
	mTeamFiveCount = 0;

	LinkedListIterator<ClientListEntry*> iterator(clientlist);
	iterator.Reset();
	uint16 total = 0;
	while(iterator.MoreElements()) {
		_log(WORLD__CLIENTLIST, "ClientList[0x%08x]::CountPVPTotals(\"CharID:%i\") iterator.GetData()[%p]", this, iterator.GetData()->CharID(), iterator.GetData());
		// dont care about admins dont count them
		if ( iterator.GetData()->Admin() >= 80 )
		{
			iterator.Advance();
			continue;
		}

		switch(iterator.GetData()->GetPVPFlag())
		{
		case 0:
			if ( serverFlag == 1 )
			total++;
			if (neutralPlayers)
			*neutralPlayers += 1;

			mNeutralCount++;
			break;
		case 1:
			total++;
			if (teamOne)
			*teamOne += 1;

			mTeamOneCount++;
			break;
		case 2:
			total++;
			if (teamTwo)
			*teamTwo += 1;

			mTeamTwoCount++;
			break;
		case 3:
			total++;
			if (teamThree)
			*teamThree += 1;

			mTeamThreeCount++;
			break;
		case 4:
			total++;
			if (teamFour)
			*teamFour += 1;

			mTeamFourCount++;
			break;
		case 5:
			if ( serverFlag == 1 )
			total++;
			if (teamFive)
			*teamFive += 1;

			mTeamFiveCount++;
			break;
		}
		iterator.Advance();
	}
	
	// Determine XP Bonus for all teams
	/*if ( teamOne )
	{
		int totalPct = (int)(((float)(*teamOne)/(float)total) * 100);
		DetermineXPBonus(1,totalPct);
	}
	if ( teamTwo )
	{
		int totalPct = (int)(((float)(*teamTwo)/(float)total) * 100);
		DetermineXPBonus(2,totalPct);
	}
	if ( teamThree )
	{
		int totalPct = (int)(((float)(*teamThree)/(float)total) * 100);
		DetermineXPBonus(3,totalPct);
	}
	if ( teamFour )
	{
		int totalPct = (int)(((float)(*teamFour)/(float)total) * 100);
		DetermineXPBonus(4,totalPct);
	}
	if ( teamFive )
	{
		int totalPct = (int)(((float)(*teamFive)/(float)total) * 100);

		if ( serverFlag == 3 )
			totalPct = 0;

		DetermineXPBonus(5,totalPct);
	}*/
}

Client* ClientList::Get(int32 ip, int16 port) {
	LinkedListIterator<Client*> iterator(list);

	iterator.Reset();
	while(iterator.MoreElements())
	{
		if (iterator.GetData()->GetIP() == ip && iterator.GetData()->GetPort() == port)
		{
			Client* tmp = iterator.GetData();
			return tmp;
		}
		iterator.Advance();
	}
	return 0;
}

void ClientList::ZoneBootup(ZoneServer* zs) {
	LinkedListIterator<Client*> iterator(list);

	iterator.Reset();
	while(iterator.MoreElements())
	{
		if (iterator.GetData()->WaitingForBootup()) {
			if (iterator.GetData()->GetZoneID() == zs->GetZoneID() 
				&& iterator.GetData()->GetInstanceID() == zs->GetInstanceID()) {
				iterator.GetData()->EnterWorld(false);
			}
			else if (iterator.GetData()->WaitingForBootup() == zs->GetID()) {
				iterator.GetData()->ZoneUnavail();
			}
		}
		iterator.Advance();
	}
}

void ClientList::RemoveCLEReferances(ClientListEntry* cle) {
	LinkedListIterator<Client*> iterator(list);

	iterator.Reset();
	while(iterator.MoreElements()) {
		if (iterator.GetData()->GetCLE() == cle) {
			iterator.GetData()->SetCLE(0);
		}
		iterator.Advance();
	}
}


bool ClientList::SendPacket(const char* to, ServerPacket* pack) {
	if (to == 0 || to[0] == 0) {
		zoneserver_list.SendPacket(pack);
		return true;
	}
	else if (to[0] == '*') {
		// Cant send a packet to a console....
		return false;
	}
	else {
		ClientListEntry* cle = FindCharacter(to);
		if (cle != NULL) {
			if (cle->Server() != NULL) {
				cle->Server()->SendPacket(pack);
				return true;
			}
			return false;
		} else {
			ZoneServer* zs = zoneserver_list.FindByName(to);
			if (zs != NULL) {
				zs->SendPacket(pack);
				return true;
			}
			return false;
		}
	}
	return false;
}

void ClientList::SendGuildPacket(int32 guild_id, ServerPacket* pack) {
	set<int32> zone_ids;
	
	LinkedListIterator<ClientListEntry*> iterator(clientlist);

	iterator.Reset();
	while(iterator.MoreElements()) {
		if (iterator.GetData()->GuildID() == guild_id) {
			zone_ids.insert(iterator.GetData()->zone());
		}
		iterator.Advance();
	}
	
	//now we know all the zones, send it to each one... this is kinda a shitty way to do this
	//since its basically O(n^2)
	set<int32>::iterator cur, end;
	cur = zone_ids.begin();
	end = zone_ids.end();
	for(; cur != end; cur++) {
		zoneserver_list.SendPacket(*cur, pack);
	}
}

void ClientList::UpdateClientGuild(int32 char_id, int32 guild_id) {
	LinkedListIterator<ClientListEntry*> iterator(clientlist);

	iterator.Reset();
	while(iterator.MoreElements()) {
		ClientListEntry *cle = iterator.GetData();
		if (cle->CharID() == char_id) {
			cle->SetGuild(guild_id);
		}
		iterator.Advance();
	}
}



int ClientList::GetClientCount() {
	return((int)clientlist.Count());
}

void ClientList::GetClients(const char *zone_name, vector<ClientListEntry *> &res) {
	LinkedListIterator<ClientListEntry *> iterator(clientlist);
	iterator.Reset();
	
	if(zone_name[0] == '\0') {
		while(iterator.MoreElements()) {
			ClientListEntry* tmp = iterator.GetData();
			res.push_back(tmp);
			iterator.Advance();
		}
	} else {
		uint32 zoneid = database.GetZoneID(zone_name);
		while(iterator.MoreElements()) {
			ClientListEntry* tmp = iterator.GetData();
			if(tmp->zone() == zoneid)
				res.push_back(tmp);
			iterator.Advance();
		}
	}
}

void ClientList::SendClientVersionSummary(const char *Name)
{
	uint32 Client62Count = 0;
	uint32 ClientTitaniumCount = 0;
	uint32 ClientSoFCount = 0;
	uint32 ClientSoDCount = 0;
	uint32 ClientUnderfootCount = 0;
	uint32 ClientHoTCount = 0;

	LinkedListIterator<ClientListEntry*> Iterator(clientlist);

	Iterator.Reset();

	while(Iterator.MoreElements())
	{
		ClientListEntry* CLE = Iterator.GetData();

		if(CLE && CLE->zone())
		{
			switch(CLE->GetClientVersion())
			{
				case 1:
				{
					++Client62Count;
					break;
				}
				case 2:
				{
					++ClientTitaniumCount;
					break;
				}
				case 3:
				{
					++ClientSoFCount;
					break;
				}
				case 4:
				{
					++ClientSoDCount;
					break;
				}
				case 5:
				{
					++ClientUnderfootCount;
					break;
				}
				case 6:
				{
					++ClientHoTCount;
					break;
				}
				default:
					break;
			}
		}

		Iterator.Advance();

	}

	zoneserver_list.SendEmoteMessage(Name, 0, 0, 13, "There are %i 6.2, %i Titanium, %i SoF, %i SoD, %i UF and %i HoT clients currently connected.",
					  Client62Count, ClientTitaniumCount, ClientSoFCount, ClientSoDCount, ClientUnderfootCount, ClientHoTCount);
}
