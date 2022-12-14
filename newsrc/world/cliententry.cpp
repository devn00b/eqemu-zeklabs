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
#include "cliententry.h"
#include "clientlist.h"
#include "LoginServer.h"
#include "LoginServerList.h"
#include "worlddb.h"
#include "zoneserver.h"
#include "WorldConfig.h"
#include "../common/guilds.h"

extern LoginServerList loginserverlist;
extern ClientList		client_list;
extern volatile bool RunLoops;

ClientListEntry::ClientListEntry(int32 in_id, int32 iLSID, const char* iLoginName, const char* iLoginKey, sint16 iWorldAdmin, int32 ip, uint8 local, bool isLocalLogin)
: id(in_id)
{
	ClearVars(true);
	
	pIP = ip;
	pLSID = iLSID;
	pvpFlag = 0;

	if ( isLocalLogin )
		IsRemoteLoginServer = false;
	else
		IsRemoteLoginServer = true;

	if(iLSID > 0)
	{
		paccountid = database.GetAccountIDFromLSID(IsRemoteLoginServer,iLSID, paccountname, &padmin);
		int8 flag = 0;
		int32 tmpSyncAccID = 0;
		database.GetAgreementFlag(paccountid,&flag,&tmpSyncAccID);
		if ( flag > 0 )SetRulesFlag(flag);
		if ( tmpSyncAccID > 0 ) SetSyncAccID(tmpSyncAccID); else SetSyncAccID(0);
	}
	strn0cpy(plsname, iLoginName, sizeof(plsname));
	strn0cpy(plskey, iLoginKey, sizeof(plskey));
	pworldadmin = iWorldAdmin;
	plocal=(local==1);

	pinstance = 0;
}

ClientListEntry::ClientListEntry(int32 in_id, int32 iAccID, const char* iAccName, MD5& iMD5Pass, sint16 iAdmin, bool isLocalLogin)
: id(in_id)
{
	ClearVars(true);
	
	pIP = 0;
	pLSID = 0;
	pworldadmin = 0;
	pvpFlag = 0;
	
	if ( isLocalLogin )
		IsRemoteLoginServer = false;
	else
		IsRemoteLoginServer = true;

	paccountid = iAccID;
	strn0cpy(paccountname, iAccName, sizeof(paccountname));
	pMD5Pass = iMD5Pass;
	padmin = iAdmin;
	if ( paccountid )
	{
		int8 flag = 0;
		int32 tmpSyncAccID = 0;
		database.GetAgreementFlag(paccountid,&flag,&tmpSyncAccID);
		if ( flag > 0 )SetRulesFlag(flag);
		if ( tmpSyncAccID > 0 ) SetSyncAccID(tmpSyncAccID); else SetSyncAccID(0);
	}

	pinstance = 0;
}

ClientListEntry::ClientListEntry(int32 in_id, ZoneServer* iZS, ServerClientList_Struct* scl, sint8 iOnline)
: id(in_id)
{
	ClearVars(true);
	
	pIP = 0;
	pLSID = scl->LSAccountID;
	strn0cpy(plsname, scl->name, sizeof(plsname));
	strn0cpy(plskey, scl->lskey, sizeof(plskey));
	pworldadmin = 0;
	
	IsRemoteLoginServer = scl->RemoteLoginServer;

	paccountid = scl->AccountID;
	strn0cpy(paccountname, scl->AccountName, sizeof(paccountname));
	padmin = scl->Admin;

	if ( paccountid )
	{
		int8 flag = 0;
		int32 tmpSyncAccID = 0;
		database.GetAgreementFlag(paccountid,&flag,&tmpSyncAccID);
		if ( flag > 0 )SetRulesFlag(flag);
		if ( tmpSyncAccID > 0 ) SetSyncAccID(tmpSyncAccID); else SetSyncAccID(0);
	}

	pinstance = 0;
	pvpFlag = 0;

	if (iOnline >= CLE_Status_Zoning)
		Update(iZS, scl, iOnline);
	else
		SetOnline(iOnline);
}

ClientListEntry::~ClientListEntry() {
	if (RunLoops) {
		Camp(); // updates zoneserver's numplayers
		client_list.RemoveCLEReferances(this);
	}
}

void ClientListEntry::SetChar(int32 iCharID, const char* iCharName) {
	pcharid = iCharID;
	strn0cpy(pname, iCharName, sizeof(pname));
}

void ClientListEntry::SetOnline(ZoneServer* iZS, sint8 iOnline) {
	if (iZS == this->Server())
		SetOnline(iOnline);
}

void ClientListEntry::SetOnline(sint8 iOnline) {
	if (iOnline != CLE_Status_Online || pOnline < CLE_Status_Online)
		pOnline = iOnline;
	if (iOnline < CLE_Status_Zoning)
		Camp();
	if (pOnline > CLE_Status_Offline)
		stale = 0;
}
void ClientListEntry::LSUpdate(ZoneServer* iZS){
	if(WorldConfig::get()->UpdateStats){
		ServerPacket* pack = new ServerPacket;
		pack->opcode = ServerOP_LSZoneInfo;
		pack->size = sizeof(ZoneInfo_Struct);
		pack->pBuffer = new uchar[pack->size];
		ZoneInfo_Struct* zone =(ZoneInfo_Struct*)pack->pBuffer;
		zone->count=iZS->NumPlayers();
		zone->zone = iZS->GetZoneID();
		zone->zone_wid = iZS->GetID();
		loginserverlist.SendPacket(pack);
		safe_delete(pack);
	}
}
void ClientListEntry::LSZoneChange(ZoneToZone_Struct* ztz){
	if(WorldConfig::get()->UpdateStats){
		ServerPacket* pack = new ServerPacket;
		pack->opcode = ServerOP_LSPlayerZoneChange;
		pack->size = sizeof(ServerLSPlayerZoneChange_Struct);
		pack->pBuffer = new uchar[pack->size];
		ServerLSPlayerZoneChange_Struct* zonechange =(ServerLSPlayerZoneChange_Struct*)pack->pBuffer;
		zonechange->lsaccount_id = LSID();
		zonechange->from = ztz->current_zone_id;
		zonechange->to = ztz->requested_zone_id;
		loginserverlist.SendPacket(pack);
		safe_delete(pack);
	}
}
void ClientListEntry::Update(ZoneServer* iZS, ServerClientList_Struct* scl, sint8 iOnline) {
	if (pzoneserver != iZS) {
		if (pzoneserver){
			pzoneserver->RemovePlayer();
			LSUpdate(pzoneserver);
		}
		if (iZS){
			iZS->AddPlayer();
			LSUpdate(iZS);
		}
	}
	pzoneserver = iZS;
	pzone = scl->zone;
	pinstance = scl->instance_id;
	pcharid = scl->charid;
	stale = 0;

	strcpy(pname, scl->name);
	if (paccountid == 0) {
		paccountid = scl->AccountID;
		strcpy(paccountname, scl->AccountName);
		strcpy(plsname, scl->AccountName);
		pIP = scl->IP;
		pLSID = scl->LSAccountID;
		strn0cpy(plskey, scl->lskey, sizeof(plskey));
	}
	padmin = scl->Admin;
	plevel = scl->level;
	pclass_ = scl->class_;
	prace = scl->race;
	panon = scl->anon;
	ptellsoff = scl->tellsoff;
	pguild_id = scl->guild_id;
	pLFG = scl->LFG;
	gm = scl->gm;
	pClientVersion = scl->ClientVersion;
	IsRemoteLoginServer = scl->RemoteLoginServer;

	// Fields from the LFG Window
	if((scl->LFGFromLevel != 0) && (scl->LFGToLevel != 0)) {
		pLFGFromLevel = scl->LFGFromLevel;
		pLFGToLevel = scl->LFGToLevel;
		pLFGMatchFilter = scl->LFGMatchFilter;
		memcpy(pLFGComments, scl->LFGComments, sizeof(pLFGComments));
	}

	SetOnline(iOnline);
}

void ClientListEntry::LeavingZone(ZoneServer* iZS, sint8 iOnline) {
	if (iZS != 0 && iZS != pzoneserver)
		return;
	SetOnline(iOnline);
	
	stale = 0;
	
	if (pzoneserver){
		pzoneserver->RemovePlayer();
		LSUpdate(pzoneserver);
	}
	pzoneserver = 0;
	pzone = 0;
}

void ClientListEntry::ClearVars(bool iAll) {
	if (iAll) {
		pOnline = CLE_Status_Never;
		stale = 0;

		pLSID = 0;
		memset(plsname, 0, sizeof(plsname));
		memset(plskey, 0, sizeof(plskey));
		pworldadmin = 0;

		paccountid = 0;
		memset(paccountname, 0, sizeof(paccountname));
		padmin = 0;
	}
	IsSyncCharacterActive = false;
	pzoneserver = 0;
	pzone = 0;
	pcharid = 0;
	memset(pname, 0, sizeof(pname));
	plevel = 0;
	pclass_ = 0;
	prace = 0;
	panon = 0;
	ptellsoff = 0;
	pguild_id = GUILD_NONE;
	pLFG = 0;
	gm = 0;
	pClientVersion = 0;
}

void ClientListEntry::Camp(ZoneServer* iZS) {
	if (iZS != 0 && iZS != pzoneserver)
		return;
	if (pzoneserver){
		pzoneserver->RemovePlayer();
		LSUpdate(pzoneserver);
	}

	ClearVars();

	stale = 0;
}

bool ClientListEntry::CheckStale() {
	stale++;
	if (stale > 3) {
		if (pOnline > CLE_Status_Offline)
			SetOnline(CLE_Status_Offline);
		else
			return true;
	}
	return false;
}

void ClientListEntry::SetRulesFlag(int8 value)
{
	rulesAccepted = value;
}

void ClientListEntry::SetSyncAccID(int32 accid)
{
	syncAccID = accid;
}

bool ClientListEntry::CheckAuth(int32 iLSID, const char* iKey) {
//	if (LSID() == iLSID && strncmp(plskey, iKey,10) == 0) {
	if (strncmp(plskey, iKey,10) == 0) {
		if (paccountid == 0 && LSID()>0) {
			sint16 tmpStatus = WorldConfig::get()->DefaultStatus;
			paccountid = database.CreateAccount(IsRemoteLoginServer, plsname, 0, tmpStatus, LSID());
			if (!paccountid) {
				_log(WORLD__CLIENTLIST_ERR,"Error adding local account for LS login: '%s', duplicate name?" ,plsname);
				return false;
			}
			strn0cpy(paccountname, plsname, sizeof(paccountname));
			padmin = tmpStatus;

			if ( RuleB(World,GiveReward) )
				database.AddVeteranReward(paccountid,RuleI(World,VeteranRewardID),RuleI(World,VeteranRewardCount));
		}
		char lsworldadmin[15] = "0";
		database.GetVariable("honorlsworldadmin", lsworldadmin, sizeof(lsworldadmin));
		if (atoi(lsworldadmin) == 1 && pworldadmin != 0 && (padmin < pworldadmin || padmin == 0))
			padmin = pworldadmin;
		return true;
	}
	return false;
}

bool ClientListEntry::CheckAuth(const char* iName, MD5& iMD5Password) {
	if (LSAccountID() == 0 && strcmp(paccountname, iName) == 0 && pMD5Pass == iMD5Password)
		return true;
	return false;
}

bool ClientListEntry::CheckAuth(int32 id, const char* iKey, int32 ip) {
	if (pIP==ip && strncmp(plskey, iKey,10) == 0){
		paccountid = id;
		database.GetAccountFromID(id,paccountname,&padmin);
		return true;
	}
	return false;
}






























