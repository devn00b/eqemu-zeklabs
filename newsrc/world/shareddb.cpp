#include "shareddb.h"
#include "../common/Item.h"
#include "../common/EMuShareMem.h"
#include "../common/classes.h"
#include "../common/rulesys.h"
#include "MiscFunctions.h"
#include "eq_packet_structs.h"
#include "guilds.h"
#include "extprofile.h"
#include "CRC16.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;


extern LoadEMuShareMemDLL EMuShareMemDLL;

//hackish mechanism to support callbacks from sharedmem
SharedDatabase *SharedDatabase::s_usedb = NULL;

SharedDatabase::SharedDatabase()
	: Database()
{
	SDBInitVars();
	s_usedb = this;
}

SharedDatabase::SharedDatabase(const char* host, const char* user, const char* passwd, const char* database, int32 port)
	: Database(host, user, passwd, database, port)
{
	SDBInitVars();
	s_usedb = this;
}

void SharedDatabase::SDBInitVars() {
	max_item = 0;
	max_npc_type = 0;

	loottable_max = 0;
	lootdrop_max = 0;
	max_door_type = 0;
	npcfactionlist_max = 0;
}

SharedDatabase::~SharedDatabase() {
}

bool SharedDatabase::SetHideMe(int32 account_id, int8 hideme)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;

	if (!RunQuery(query, MakeAnyLenString(&query, "UPDATE account SET hideme = %i where id = %i", hideme, account_id), errbuf)) {
		cerr << "Error in SetGMSpeed query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}

	safe_delete_array(query);
	return true;

}

int8 SharedDatabase::GetGMSpeed(int32 account_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT gmspeed FROM account where id='%i'", account_id), errbuf, &result)) {
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1)
		{
			row = mysql_fetch_row(result);
			int8 gmspeed = atoi(row[0]);
			mysql_free_result(result);
			return gmspeed;
		}
		else
		{
			mysql_free_result(result);
			return 0;
		}
		mysql_free_result(result);
	}
	else
	{

		cerr << "Error in GetGMSpeed query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}

	return 0;


}

bool SharedDatabase::SetGMSpeed(int32 account_id, int8 gmspeed)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;

	if (!RunQuery(query, MakeAnyLenString(&query, "UPDATE account SET gmspeed = %i where id = %i", gmspeed, account_id), errbuf)) {
		cerr << "Error in SetGMSpeed query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}

	safe_delete_array(query);
	return true;

}

int32 SharedDatabase::GetTotalTimeEntitledOnAccount(uint32 AccountID) {

	uint32 EntitledTime = 0;

	const char *EntitledQuery = "select sum(ascii(substring(profile, 237, 1)) + (ascii(substring(profile, 238, 1)) * 256) +"
		"(ascii(substring(profile, 239, 1)) * 65536) + (ascii(substring(profile, 240, 1)) * 16777216))"
		"from character_ where account_id = %i";

	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	if (RunQuery(query, MakeAnyLenString(&query, EntitledQuery, AccountID), errbuf, &result)) {

		if (mysql_num_rows(result) == 1) {

			row = mysql_fetch_row(result);

			EntitledTime = atoi(row[0]);
		}

		mysql_free_result(result);
	}

	safe_delete_array(query);

	return EntitledTime;
}

bool SharedDatabase::SaveCursor(uint32 char_id, list<ItemInst*>::const_iterator &start, list<ItemInst*>::const_iterator &end)
{
	iter_queue it;
	int i;
	bool ret=true;
	char errbuf[MYSQL_ERRMSG_SIZE];
	char* query = 0;
	// Delete cursor items
	if ((ret = RunQuery(query, MakeAnyLenString(&query, "DELETE FROM inventory WHERE charid=%i AND ( (slotid >=8000 and slotid<=8999) or slotid=30 or (slotid>=331 and slotid<=340))", char_id), errbuf))) {
		for(it=start,i=8000;it!=end;it++,i++) {
			ItemInst *inst=*it;
			if (!(ret=SaveInventory(char_id,inst,(i==8000) ? 30 : i)))
				break;
		}
	} else {
		cout << "Clearing cursor failed: " << errbuf << endl;
	}
	safe_delete_array(query);

	return ret;
}

bool SharedDatabase::VerifyInventory(uint32 account_id, sint16 slot_id, const ItemInst* inst)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char* query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	// Delete cursor items
	if (!RunQuery(query, MakeAnyLenString(&query, 
		"SELECT itemid,charges FROM sharedbank "
		"WHERE acctid=%d AND slotid=%d",
		account_id, slot_id), errbuf, &result)) {
			LogFile->write(EQEMuLog::Error, "Error runing inventory verification query '%s': %s", query, errbuf);
			safe_delete_array(query);
			//returning true is less harmful in the face of a query error
			return(true);
	}
	safe_delete_array(query);

	row = mysql_fetch_row(result);
	bool found = false;
	if(row) {
		uint32 id = atoi(row[0]);
		int16 charges = atoi(row[1]);

		int16 expect_charges = 0;
		if(inst->GetCharges() >= 0)
			expect_charges = inst->GetCharges();
		else
			expect_charges = 255;

		if(id == inst->GetItem()->ID && charges == expect_charges)
			found = true;
	}
	mysql_free_result(result);
	return(found);
}

bool SharedDatabase::SaveInventory(uint32 char_id, const ItemInst* inst, sint16 slot_id) {
	_CP(Database_SaveInventory);
	char errbuf[MYSQL_ERRMSG_SIZE];
	char* query = 0;
	bool ret = false;
	uint32 augslot[5] = { 0, 0, 0, 0, 0 };

	//never save tribute slots:
	if(slot_id >= 400 && slot_id <= 404)
		return(true);

	if (inst && inst->IsType(ItemClassCommon)) {
		for(int i=0;i<5;i++) {
			ItemInst *auginst=inst->GetItem(i);
			augslot[i]=(auginst && auginst->GetItem()) ? auginst->GetItem()->ID : 0;
		}
	}

	if (slot_id>=2500 && slot_id<=2600) { // Shared bank inventory
		if (!inst) {
			// Delete item
			uint32 account_id = GetAccountIDByChar(char_id);
			uint32 len_query = MakeAnyLenString(&query, "DELETE FROM sharedbank WHERE acctid=%i AND slotid=%i",
				account_id, slot_id);

			ret = RunQuery(query, len_query, errbuf);

			// Delete bag slots, if need be
			if (ret && Inventory::SupportsContainers(slot_id)) {
				safe_delete_array(query);
				sint16 base_slot_id = Inventory::CalcSlotId(slot_id, 0);
				ret = RunQuery(query, MakeAnyLenString(&query, "DELETE FROM sharedbank WHERE acctid=%i AND slotid>=%i AND slotid<%i",
					account_id, base_slot_id, (base_slot_id+10)), errbuf);
			}

			// @merth: need to delete augments here
		}
		else {
			// Update/Insert item
			uint32 account_id = GetAccountIDByChar(char_id);
			int16 charges = 0;
			if(inst->GetCharges() >= 0)
				charges = inst->GetCharges();
			else
				charges = 255;

			uint32 len_query =  MakeAnyLenString(&query, 
				"REPLACE INTO sharedbank "
				"	(acctid,slotid,itemid,charges,"
				"	augslot1,augslot2,augslot3,augslot4,augslot5)"
				" VALUES(%lu,%lu,%lu,%lu,"
				"	%lu,%lu,%lu,%lu,%lu)",
				(unsigned long)account_id, (unsigned long)slot_id, (unsigned long)inst->GetItem()->ID, (unsigned long)charges ,
				(unsigned long)augslot[0],(unsigned long)augslot[1],(unsigned long)augslot[2],(unsigned long)augslot[3],(unsigned long)augslot[4]);


			ret = RunQuery(query, len_query, errbuf);
		}
	}
	else { // All other inventory
		if (!inst) {
			// Delete item
			ret = RunQuery(query, MakeAnyLenString(&query, "DELETE FROM inventory WHERE charid=%i AND slotid=%i",
				char_id, slot_id), errbuf);

			// Delete bag slots, if need be
			if (ret && Inventory::SupportsContainers(slot_id)) {
				safe_delete_array(query);
				sint16 base_slot_id = Inventory::CalcSlotId(slot_id, 0);
				ret = RunQuery(query, MakeAnyLenString(&query, "DELETE FROM inventory WHERE charid=%i AND slotid>=%i AND slotid<%i",
					char_id, base_slot_id, (base_slot_id+10)), errbuf);
			}

			// @merth: need to delete augments here
		}
		else {
			int16 charges = 0;
			if(inst->GetCharges() >= 0)
				charges = inst->GetCharges();
			else
				charges = 255;
			// Update/Insert item
			uint32 len_query = MakeAnyLenString(&query, 
				"REPLACE INTO inventory "
				"	(charid,slotid,itemid,charges,instnodrop,color,"
				"	augslot1,augslot2,augslot3,augslot4,augslot5)"
				" VALUES(%lu,%lu,%lu,%lu,%lu,%lu,"
				"	%lu,%lu,%lu,%lu,%lu)",
				(unsigned long)char_id, (unsigned long)slot_id, (unsigned long)inst->GetItem()->ID, (unsigned long)charges, (unsigned long)(inst->IsInstNoDrop() ? 1:0),(unsigned long)inst->GetColor(),
				(unsigned long)augslot[0],(unsigned long)augslot[1],(unsigned long)augslot[2],(unsigned long)augslot[3],(unsigned long)augslot[4] );

			ret = RunQuery(query, len_query, errbuf);
		}
	}

	if (!ret)
		LogFile->write(EQEMuLog::Error, "SaveInventory query '%s': %s", query, errbuf);
	safe_delete_array(query);

	// Save bag contents, if slot supports bag contents
	if (inst && inst->IsType(ItemClassContainer) && Inventory::SupportsContainers(slot_id)) {
		for (uint8 idx=0; idx<10; idx++) {
			const ItemInst* baginst = inst->GetItem(idx);
			SaveInventory(char_id, baginst, Inventory::CalcSlotId(slot_id, idx));
		}
	}

	// @merth: need to save augments here

	return ret;
}

sint32 SharedDatabase::GetSharedPlatinum(int32 account_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT sharedplat FROM account WHERE id='%i'", account_id), errbuf, &result)) {
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1)
		{
			row = mysql_fetch_row(result);
			uint32 shared_platinum = atoi(row[0]);
			mysql_free_result(result);
			return shared_platinum;
		}
		else
		{
			mysql_free_result(result);
			return 0;
		}
		mysql_free_result(result);
	}
	else
	{

		cerr << "Error in GetSharedPlatinum query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}

	return 0;
}

bool SharedDatabase::SetSharedPlatinum(uint32 account_id, sint32 amount_to_add)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;

	if (!RunQuery(query, MakeAnyLenString(&query, "UPDATE account SET sharedplat = sharedplat + %i WHERE id = %i", amount_to_add, account_id), errbuf)) {
		cerr << "Error in SetSharedPlatinum query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}

	safe_delete_array(query);
	return true;
}

bool SharedDatabase::SetStartingItems(PlayerProfile_Struct* pp, Inventory* inv, uint32 si_race, uint32 si_class, uint32 si_deity, uint32 si_current_zone, char* si_name, int admin_level)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char* query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	const Item_Struct* myitem;

	RunQuery
		(
		query,
		MakeAnyLenString
		(
		&query,
		"SELECT itemid, item_charges, slot FROM starting_items "
		"WHERE (race = %i or race = 0) AND (class = %i or class = 0) AND "
		"(deityid = %i or deityid=0) AND (zoneid = %i or zoneid = 0) AND "
		"gm <= %i ORDER BY id",
		si_race, si_class, si_deity, si_current_zone, admin_level
		),
		errbuf,
		&result
		);
	safe_delete_array(query);

	while((row = mysql_fetch_row(result))) {
		int itemid = atoi(row[0]);
		int charges = atoi(row[1]);
		int slot = atoi(row[2]);
		myitem = GetItem(itemid);
		if(!myitem)
			continue;
		ItemInst* myinst = CreateBaseItem(myitem, charges);	
		if(slot < 0)
			slot = inv->FindFreeSlot(0,0);
		inv->PutItem(slot, *myinst);
		safe_delete(myinst);
	}

	if(result) mysql_free_result(result);

	return true;
}


// Retrieve shared bank inventory based on either account or character
bool SharedDatabase::GetSharedBank(uint32 id, Inventory* inv, bool is_charid) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char* query = 0;
	uint32 len_query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	bool ret = false;

	if (is_charid) {
		len_query = MakeAnyLenString(&query,
			"SELECT sb.slotid,sb.itemid,sb.charges,sb.augslot1,sb.augslot2,sb.augslot3,sb.augslot4,sb.augslot5 from sharedbank sb "
			"INNER JOIN character_ ch ON ch.account_id=sb.acctid "
			"WHERE ch.id=%i", id);
	}
	else {
		len_query = MakeAnyLenString(&query,
			"SELECT slotid,itemid,charges,augslot1,augslot2,augslot3,augslot4,augslot5 from sharedbank WHERE acctid=%i", id);
	}

	if (RunQuery(query, len_query, errbuf, &result)) {
		while ((row = mysql_fetch_row(result))) {
			sint16 slot_id	= (sint16)atoi(row[0]);
			uint32 item_id	= (uint32)atoi(row[1]);
			sint8 charges	= (sint8)atoi(row[2]);
			uint32 aug[5];
			aug[0]	= (uint32)atoi(row[3]);
			aug[1]	= (uint32)atoi(row[4]);
			aug[2]	= (uint32)atoi(row[5]);
			aug[3]	= (uint32)atoi(row[6]);
			aug[4]	= (uint32)atoi(row[7]);
			const Item_Struct* item = GetItem(item_id);

			if (item) {
				sint16 put_slot_id = SLOT_INVALID;

				ItemInst* inst = CreateBaseItem(item, charges);
				if (item->ItemClass == ItemClassCommon) {
					for(int i=0;i<5;i++) {
						if (aug[i]) {
							inst->PutAugment(this, i, aug[i]);
						}
					}
				}
				put_slot_id = inv->PutItem(slot_id, *inst);
				safe_delete(inst);

				// Save ptr to item in inventory
				if (put_slot_id == SLOT_INVALID) {
					LogFile->write(EQEMuLog::Error,
						"Warning: Invalid slot_id for item in shared bank inventory: %s=%i, item_id=%i, slot_id=%i",
						((is_charid==true) ? "charid" : "acctid"), id, item_id, slot_id);

					if(is_charid)
						SaveInventory(id,NULL,slot_id);
				}
			}
			else {
				LogFile->write(EQEMuLog::Error,
					"Warning: %s %i has an invalid item_id %i in inventory slot %i",
					((is_charid==true) ? "charid" : "acctid"), id, item_id, slot_id);
			}
		}

		mysql_free_result(result);
		ret = true;
	}
	else {
		LogFile->write(EQEMuLog::Error, "Database::GetSharedBank(int32 account_id): %s", errbuf);
	}

	safe_delete_array(query);
	return ret;
}


// Overloaded: Retrieve character inventory based on character id
bool SharedDatabase::GetInventory(uint32 char_id, Inventory* inv) {
	_CP(Database_GetInventory);
	char errbuf[MYSQL_ERRMSG_SIZE];
	char* query = 0;
	MYSQL_RES* result;
	MYSQL_ROW row;
	bool ret = false;

	// Retrieve character inventory
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT slotid,itemid,charges,color,augslot1,augslot2,augslot3,augslot4,augslot5,instnodrop FROM inventory WHERE charid=%i ORDER BY slotid", char_id), errbuf, &result)) {

		while ((row = mysql_fetch_row(result))) {	
			sint16 slot_id	= atoi(row[0]);
			uint32 item_id	= atoi(row[1]);
			int16 charges	= atoi(row[2]);
			uint32 color		= atoul(row[3]);
			uint32 aug[5];
			aug[0]	= (uint32)atoul(row[4]);
			aug[1]	= (uint32)atoul(row[5]);
			aug[2]	= (uint32)atoul(row[6]);
			aug[3]	= (uint32)atoul(row[7]);
			aug[4]	= (uint32)atoul(row[8]);
			bool instnodrop	= (row[9] && (int16)atoi(row[9])) ? true : false;

			const Item_Struct* item = GetItem(item_id);

			if (item) {
				sint16 put_slot_id = SLOT_INVALID;

				ItemInst* inst = CreateBaseItem(item, charges);

				if (instnodrop || (slot_id >= 0 && slot_id <= 21 && inst->GetItem()->Attuneable))
					inst->SetInstNoDrop(true);
				if (color > 0)
					inst->SetColor(color);
				if(charges==255)
					inst->SetCharges(-1);
				else
					inst->SetCharges(charges);

				if (item->ItemClass == ItemClassCommon) {
					for(int i=0;i<5;i++) {
						if (aug[i]) {
							inst->PutAugment(this, i, aug[i]);
						}
					}
				}

				if (slot_id>=8000 && slot_id <= 8999)
					put_slot_id = inv->PushCursor(*inst);
				else 
					put_slot_id = inv->PutItem(slot_id, *inst);
				safe_delete(inst);

				// Save ptr to item in inventory
				if (put_slot_id == SLOT_INVALID) {
					LogFile->write(EQEMuLog::Error,
						"Warning: Invalid slot_id for item in inventory: charid=%i, item_id=%i, slot_id=%i",
						char_id, item_id, slot_id);
				}
			}
			else {
				LogFile->write(EQEMuLog::Error,
					"Warning: charid %i has an invalid item_id %i in inventory slot %i",
					char_id, item_id, slot_id);
			}
		}
		mysql_free_result(result);

		// Retrieve shared inventory
		ret = GetSharedBank(char_id, inv, true);
	}
	else {
		LogFile->write(EQEMuLog::Error, "GetInventory query '%s' %s", query, errbuf);
		LogFile->write(EQEMuLog::Error, "If you got an error related to the 'instnodrop' field, run the following SQL Queries:\nalter table inventory add instnodrop tinyint(1) unsigned default 0 not null;\n");
	}

	safe_delete_array(query);
	return ret;
}

// Overloaded: Retrieve character inventory based on account_id and character name
bool SharedDatabase::GetInventory(uint32 account_id, char* name, Inventory* inv) {
	_CP(Database_GetInventory_name);
	char errbuf[MYSQL_ERRMSG_SIZE];
	char* query = 0;
	MYSQL_RES* result;
	MYSQL_ROW row;
	bool ret = false;

	// Retrieve character inventory
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT slotid,itemid,charges,color,augslot1,augslot2,augslot3,augslot4,augslot5,instnodrop FROM inventory INNER JOIN character_ ch ON ch.id=charid WHERE ch.name='%s' AND ch.account_id=%i ORDER BY slotid", name, account_id), errbuf, &result))
	{
		while ((row = mysql_fetch_row(result))) {
			sint16 slot_id	= atoi(row[0]);
			uint32 item_id	= atoi(row[1]);
			sint8 charges	= atoi(row[2]);
			uint32 color		= atoul(row[3]);
			uint32 aug[5];
			aug[0]	= (uint32)atoi(row[4]);
			aug[1]	= (uint32)atoi(row[5]);
			aug[2]	= (uint32)atoi(row[6]);
			aug[3]	= (uint32)atoi(row[7]);
			aug[4]	= (uint32)atoi(row[8]);
			bool instnodrop	= (row[9] && (int16)atoi(row[9])) ? true : false;
			const Item_Struct* item = GetItem(item_id);
			sint16 put_slot_id = SLOT_INVALID;
			if(!item)
				continue;

			ItemInst* inst = CreateBaseItem(item, charges);			
			inst->SetInstNoDrop(instnodrop);

			if (color > 0)
				inst->SetColor(color);
			inst->SetCharges(charges);

			if (item->ItemClass == ItemClassCommon) {
				for(int i=0;i<5;i++) {
					if (aug[i]) {
						inst->PutAugment(this, i, aug[i]);
					}
				}
			}
			if (slot_id>=8000 && slot_id <= 8999)
				put_slot_id = inv->PushCursor(*inst);
			else 
				put_slot_id = inv->PutItem(slot_id, *inst);
			safe_delete(inst);

			// Save ptr to item in inventory
			if (put_slot_id == SLOT_INVALID) {
				LogFile->write(EQEMuLog::Error,
					"Warning: Invalid slot_id for item in inventory: name=%s, acctid=%i, item_id=%i, slot_id=%i",
					name, account_id, item_id, slot_id);
			}
		}
		mysql_free_result(result);

		// Retrieve shared inventory
		ret = GetSharedBank(account_id, inv, false);
	}
	else {
		LogFile->write(EQEMuLog::Error, "GetInventory query '%s' %s", query, errbuf);
		LogFile->write(EQEMuLog::Error, "If you got an error related to the 'instnodrop' field, run the following SQL Queries:\nalter table inventory add instnodrop tinyint(1) unsigned default 0 not null;\n");
	}

	safe_delete_array(query);
	return ret;
}


// Kings & Bandits - Modified GetItemsCount to support a new variable
// maxitemmemoryid (allows us to freely set allocated item slots which we can use to generate new items on the fly).
// when the variable is not described we simply use the max id and generate from there.
// A safe check exists if more items exist in the db than the variable, if a higher item id exists we will not use the variable.
sint32 SharedDatabase::GetItemsCount(int32* oMaxID) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 ret = -1;

	int maxIDMem = 0;

	char ndbuffer[32];
	if(GetVariable("maxitemmemoryid", ndbuffer, 32)) {
		maxIDMem = atoi(ndbuffer);
	}

	char query[] = "SELECT MAX(id),count(*) FROM items";
	if (RunQuery(query, strlen(query), errbuf, &result)) {
		row = mysql_fetch_row(result);
		if (row != NULL && row[1] != 0) {
			ret = atoi(row[1]);
			if (oMaxID) {
				if (row[0])
				{
					int32 maxID = atoi(row[0]);
					if ( maxIDMem > maxID )
						maxID = maxIDMem;
					*oMaxID = maxID;
				}
				else
				{
					if ( maxIDMem > 0 )
						*oMaxID = maxIDMem;
					else
						*oMaxID = 0;
				}
			}
		}
		mysql_free_result(result);
	}
	else {
		cerr << "Error in GetItemsCount query '" << query << "' " << errbuf << endl;
	}

	return ret;
}


sint32 SharedDatabase::GetNPCTypesCount(int32* oMaxID) {

	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	query = new char[256];
	strcpy(query, "SELECT MAX(id), count(*) FROM npc_types");
	if (RunQuery(query, strlen(query), errbuf, &result)) {
		safe_delete_array(query);
		row = mysql_fetch_row(result);
		if (row != NULL && row[1] != 0) {
			sint32 ret = atoi(row[1]);
			if (oMaxID) {
				if (row[0])
					*oMaxID = atoi(row[0]);
				else
					*oMaxID = 0;
			}
			mysql_free_result(result);
			return ret;
		}
		mysql_free_result(result);
	}
	else {
		cerr << "Error in GetNPCTypesCount query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);

		return -1;
	}

	return -1;
}


bool SharedDatabase::extDBLoadItems(sint32 iItemCount, int32 iMaxItemID) {
	return s_usedb->DBLoadItems(iItemCount, iMaxItemID);
}

bool SharedDatabase::LoadItems() {
	if (!EMuShareMemDLL.Load())
		return false;
	sint32 tmp = 0;
	tmp = GetItemsCount(&max_item);
	if (tmp == -1) {
		cout << "Error: SharedDatabase::LoadItems() (sharemem): GetItemsCount() returned -1" << endl;
		return false;
	}
	bool ret = EMuShareMemDLL.Items.DLLLoadItems(&extDBLoadItems, sizeof(Item_Struct), &tmp, &max_item);
	return ret;
}

// Load all database items into cache
bool SharedDatabase::DBLoadItems(sint32 iItemCount, uint32 iMaxItemID) {
	_CP(Database_DBLoadItems);
	char errbuf[MYSQL_ERRMSG_SIZE];
	MYSQL_RES *result;
	MYSQL_ROW row;
	bool ret = false;

	LogFile->write(EQEMuLog::Status, "Loading items from database: count=%i", iItemCount);

	// Make sure enough memory was alloc'd in cache
	sint32 item_count = GetItemsCount(&max_item);
	/*if (item_count > iItemCount) {
	LogFile->write(EQEMuLog::Error, "Insufficient shared memory to load items (actual=%i, allocated=%i)", item_count, iItemCount);
	return ret;
	}
	else if (max_item != iMaxItemID) {
	LogFile->write(EQEMuLog::Error, "Insufficient shared memory to load items (max item=%i, allocated=%i).  Increase MMF_EQMAX_ITEMS define", max_item, iMaxItemID);
	return ret;
	}*/

	bool disableNoRent = false;
	char ndbuffer[4];
	if(GetVariable("disablenorent", ndbuffer, 4)) {
		if(ndbuffer[0] == '1' && ndbuffer[1] == '\0') {
			disableNoRent = true;
		}
	}
	bool disableNoDrop = false;
	if(GetVariable("disablenodrop", ndbuffer, 4)) {
		if(ndbuffer[0] == '1' && ndbuffer[1] == '\0') {
			disableNoDrop = true;
		}
	}
	bool disableLoreGroup = false;
	if(GetVariable("disablelore", ndbuffer, 4)) {
		if(ndbuffer[0] == '1' && ndbuffer[1] == '\0') {
			disableLoreGroup = true;
		}
	}
	bool disableNoTransfer = false;
	if(GetVariable("disablenotransfer", ndbuffer, 4)) {
		if(ndbuffer[0] == '1' && ndbuffer[1] == '\0') {
			disableNoTransfer = true;
		}
	}

	// Retrieve all items from database
	char query[] = "select source,"
#define F(x) "`"#x"`,"
#include "item_fieldlist.h"
#undef F
		"updated"
		" from items order by id";

	Item_Struct item;
	if (RunQuery(query, sizeof(query), errbuf, &result)) {
		while((row = mysql_fetch_row(result))) {
#if EQDEBUG >= 6
			LogFile->write(EQEMuLog::Status, "Loading %s:%i", row[ItemField::name], row[ItemField::id]);
#endif				
			memset(&item, 0, sizeof(Item_Struct));

			item.ItemClass = (uint8)atoi(row[ItemField::itemclass]);
			strcpy(item.Name,row[ItemField::name]);
			strcpy(item.Lore,row[ItemField::lore]);
			strcpy(item.IDFile,row[ItemField::idfile]);
			item.ID = (uint32)atoul(row[ItemField::id]);
			item.Weight = (uint8)atoi(row[ItemField::weight]);
			item.NoRent = disableNoRent ? (uint8)atoi("255") : (uint8)atoi(row[ItemField::norent]);
			item.NoDrop = disableNoDrop ? (uint8)atoi("255") : (uint8)atoi(row[ItemField::nodrop]);
			item.Size = (uint8)atoi(row[ItemField::size]);
			item.Slots = (uint32)atoul(row[ItemField::slots]);
			item.Price = (uint32)atoul(row[ItemField::price]);
			item.Icon = (uint32)atoul(row[ItemField::icon]);
			//item.Unk012 = (sint32)atoul(row[ItemField::UNK012]);
			//item.Unk013 = (uint32)atoul(row[ItemField::UNK013]);
			item.BenefitFlag = (uint32)atoul(row[ItemField::benefitflag]);
			item.Tradeskills = (atoi(row[ItemField::tradeskills])==0) ? false : true;
			item.CR = (sint8)atoi(row[ItemField::cr]);
			item.DR = (sint8)atoi(row[ItemField::dr]);
			item.PR = (sint8)atoi(row[ItemField::pr]);
			item.MR = (sint8)atoi(row[ItemField::mr]);
			item.FR = (sint8)atoi(row[ItemField::fr]);
			item.AStr = (sint8)atoi(row[ItemField::astr]);
			item.ASta = (sint8)atoi(row[ItemField::asta]);
			item.AAgi = (sint8)atoi(row[ItemField::aagi]);
			item.ADex = (sint8)atoi(row[ItemField::adex]);
			item.ACha = (sint8)atoi(row[ItemField::acha]);
			item.AInt = (sint8)atoi(row[ItemField::aint]);
			item.AWis = (sint8)atoi(row[ItemField::awis]);
			item.HP = (sint32)atoul(row[ItemField::hp]);
			item.Mana = (sint32)atoul(row[ItemField::mana]);
			item.AC = (sint32)atoul(row[ItemField::ac]);
			item.Deity = (uint32)atoul(row[ItemField::deity]);
			item.SkillModValue = (sint32)atoul(row[ItemField::skillmodvalue]);
			//item.Unk033 = (sint32)atoul(row[ItemField::UNK033]);
			item.SkillModType = (uint32)atoul(row[ItemField::skillmodtype]);
			item.BaneDmgRace = (uint32)atoul(row[ItemField::banedmgrace]);
			item.BaneDmgAmt = (sint8)atoi(row[ItemField::banedmgamt]);
			item.BaneDmgBody = (uint32)atoul(row[ItemField::banedmgbody]);
			item.Magic = (atoi(row[ItemField::magic])==0) ? false : true;
			item.CastTime_ = (sint32)atoul(row[ItemField::casttime_]);
			item.ReqLevel = (uint8)atoi(row[ItemField::reqlevel]);
			item.BardType = (uint32)atoul(row[ItemField::bardtype]);
			item.BardValue = (sint32)atoul(row[ItemField::bardvalue]);
			item.Light = (sint8)atoi(row[ItemField::light]);
			item.Delay = (uint8)atoi(row[ItemField::delay]);
			item.RecLevel = (uint8)atoi(row[ItemField::reclevel]);
			item.RecSkill = (uint8)atoi(row[ItemField::recskill]);
			item.ElemDmgType = (uint8)atoi(row[ItemField::elemdmgtype]);
			item.ElemDmgAmt = (uint8)atoi(row[ItemField::elemdmgamt]);
			item.Range = (uint8)atoi(row[ItemField::range]);
			item.Damage = (uint8)atoi(row[ItemField::damage]);
			item.Color = (uint32)atoul(row[ItemField::color]);
			item.Classes = (uint32)atoul(row[ItemField::classes]);
			item.Races = (uint32)atoul(row[ItemField::races]);
			//item.Unk054 = (uint32)atoul(row[ItemField::UNK054]);
			item.MaxCharges = (sint16)atoi(row[ItemField::maxcharges]);
			item.ItemType = (uint8)atoi(row[ItemField::itemtype]);
			item.Material = (uint8)atoi(row[ItemField::material]);
			item.SellRate = (float)atof(row[ItemField::sellrate]);
			//item.Unk059 = (uint32)atoul(row[ItemField::UNK059]);
			item.CastTime = (uint32)atoul(row[ItemField::casttime]);
			item.EliteMaterial = (uint32)atoul(row[ItemField::elitematerial]);
			item.ProcRate = (sint32)atoi(row[ItemField::procrate]);
			item.CombatEffects = (sint8)atoi(row[ItemField::combateffects]);
			item.Shielding = (sint8)atoi(row[ItemField::shielding]);
			item.StunResist = (sint8)atoi(row[ItemField::stunresist]);
			item.StrikeThrough = (sint8)atoi(row[ItemField::strikethrough]);
			item.ExtraDmgSkill = (uint32)atoul(row[ItemField::extradmgskill]);
			item.ExtraDmgAmt = (uint32)atoul(row[ItemField::extradmgamt]);
			item.SpellShield = (sint8)atoi(row[ItemField::spellshield]);
			item.Avoidance = (sint8)atoi(row[ItemField::avoidance]);
			item.Accuracy = (sint8)atoi(row[ItemField::accuracy]);
			item.CharmFileID = (uint32)atoul(row[ItemField::charmfileid]);
			item.FactionMod1 = (sint32)atoul(row[ItemField::factionmod1]);
			item.FactionMod2 = (sint32)atoul(row[ItemField::factionmod2]);
			item.FactionMod3 = (sint32)atoul(row[ItemField::factionmod3]);
			item.FactionMod4 = (sint32)atoul(row[ItemField::factionmod4]);
			item.FactionAmt1 = (sint32)atoul(row[ItemField::factionamt1]);
			item.FactionAmt2 = (sint32)atoul(row[ItemField::factionamt2]);
			item.FactionAmt3 = (sint32)atoul(row[ItemField::factionamt3]);
			item.FactionAmt4 = (sint32)atoul(row[ItemField::factionamt4]);
			strcpy(item.CharmFile,row[ItemField::charmfile]);
			item.AugType = (uint32)atoul(row[ItemField::augtype]);
			item.AugSlotType[0] = (uint8)atoi(row[ItemField::augslot1type]);
			item.AugSlotVisible[0] = (uint8)atoi(row[ItemField::augslot1visible]);
			item.AugSlotUnk2[0] = 0;
			item.AugSlotType[1] = (uint8)atoi(row[ItemField::augslot2type]);
			item.AugSlotVisible[1] = (uint8)atoi(row[ItemField::augslot2visible]);
			item.AugSlotUnk2[1] = 0;
			item.AugSlotType[2] = (uint8)atoi(row[ItemField::augslot3type]);
			item.AugSlotVisible[2] = (uint8)atoi(row[ItemField::augslot3visible]);
			item.AugSlotUnk2[2] = 0;
			item.AugSlotType[3] = (uint8)atoi(row[ItemField::augslot4type]);
			item.AugSlotVisible[3] = (uint8)atoi(row[ItemField::augslot4visible]);
			item.AugSlotUnk2[3] = 0;
			item.AugSlotType[4] = (uint8)atoi(row[ItemField::augslot5type]);
			item.AugSlotVisible[4] = (uint8)atoi(row[ItemField::augslot5visible]);
			item.AugSlotUnk2[4] = 0;
			item.LDoNTheme = (uint32)atoul(row[ItemField::ldontheme]);
			item.LDoNPrice = (uint32)atoul(row[ItemField::ldonprice]);
			item.LDoNSold = (uint32)atoul(row[ItemField::ldonsold]);
			item.BagType = (uint8)atoi(row[ItemField::bagtype]);
			item.BagSlots = (uint8)atoi(row[ItemField::bagslots]);
			item.BagSize = (uint8)atoi(row[ItemField::bagsize]);
			item.BagWR = (uint8)atoi(row[ItemField::bagwr]);
			item.Book = (uint8)atoi(row[ItemField::book]);
			item.BookType = (uint32)atoul(row[ItemField::booktype]);
			strcpy(item.Filename,row[ItemField::filename]);
			item.BaneDmgRaceAmt = (uint32)atoul(row[ItemField::banedmgraceamt]);
			item.AugRestrict = (uint32)atoul(row[ItemField::augrestrict]);
			item.LoreGroup = disableLoreGroup ? (uint8)atoi("0") : atoi(row[ItemField::loregroup]);
			item.LoreFlag = item.LoreGroup!=0;
			item.PendingLoreFlag = (atoi(row[ItemField::pendingloreflag])==0) ? false : true;
			item.ArtifactFlag = (atoi(row[ItemField::artifactflag])==0) ? false : true;
			item.SummonedFlag = (atoi(row[ItemField::summonedflag])==0) ? false : true;
			item.Favor = (uint32)atoul(row[ItemField::favor]);
			item.FVNoDrop = (atoi(row[ItemField::fvnodrop])==0) ? false : true;
			item.Endur = (uint32)atoul(row[ItemField::endur]);
			item.DotShielding = (uint32)atoul(row[ItemField::dotshielding]);
			item.Attack = (uint32)atoul(row[ItemField::attack]);
			item.Regen = (uint32)atoul(row[ItemField::regen]);
			item.ManaRegen = (uint32)atoul(row[ItemField::manaregen]);
			item.EnduranceRegen = (uint32)atoul(row[ItemField::enduranceregen]);
			item.Haste = (uint32)atoul(row[ItemField::haste]);
			item.DamageShield = (uint32)atoul(row[ItemField::damageshield]);
			item.RecastDelay = (uint32)atoul(row[ItemField::recastdelay]);
			item.RecastType = (uint32)atoul(row[ItemField::recasttype]);
			item.GuildFavor = (uint32)atoul(row[ItemField::guildfavor]);
			item.AugDistiller = (uint32)atoul(row[ItemField::augdistiller]);
			//item.Unk123 = (uint32)atoul(row[ItemField::UNK123]);
			//item.Unk124 = (uint32)atoul(row[ItemField::UNK124]);
			item.Attuneable = (atoi(row[ItemField::attuneable])==0) ? false : true;
			item.NoPet = (atoi(row[ItemField::nopet])==0) ? false : true;
			//item.Unk127 = (uint32)atoul(row[ItemField::UNK127]);
			item.PointType = (uint32)atoul(row[ItemField::pointtype]);
			item.PotionBelt = (atoi(row[ItemField::potionbelt])==0) ? false : true;
			item.PotionBeltSlots = (atoi(row[ItemField::potionbeltslots])==0) ? false : true;
			item.StackSize = (uint8)atoi(row[ItemField::stacksize]);
			item.NoTransfer = disableNoTransfer ? false : (atoi(row[ItemField::notransfer])==0) ? false : true;
			item.Stackable = (atoi(row[ItemField::stackable])==0) ? false : true;
			//item.Unk134 = (uint32)atoul(row[ItemField::UNK134]);
			item.Click.Effect = (uint32)atoul(row[ItemField::clickeffect]);
			item.Click.Type = (uint8)atoul(row[ItemField::clicktype]);
			item.Click.Level = (uint8)atoul(row[ItemField::clicklevel]);
			item.Click.Level2 = (uint8)atoul(row[ItemField::clicklevel2]);
			strcpy(item.CharmFile,row[ItemField::charmfile]);
			item.Proc.Effect = (uint16)atoul(row[ItemField::proceffect]);
			item.Proc.Type = (uint8)atoul(row[ItemField::proctype]);
			item.Proc.Level = (uint8)atoul(row[ItemField::proclevel]);
			item.Proc.Level2 = (uint8)atoul(row[ItemField::proclevel2]);
			item.Worn.Effect = (uint16)atoul(row[ItemField::worneffect]);
			item.Worn.Type = (uint8)atoul(row[ItemField::worntype]);
			item.Worn.Level = (uint8)atoul(row[ItemField::wornlevel]);
			item.Worn.Level2 = (uint8)atoul(row[ItemField::wornlevel2]);
			item.Focus.Effect = (uint16)atoul(row[ItemField::focuseffect]);
			item.Focus.Type = (uint8)atoul(row[ItemField::focustype]);
			item.Focus.Level = (uint8)atoul(row[ItemField::focuslevel]);
			item.Focus.Level2 = (uint8)atoul(row[ItemField::focuslevel2]);
			item.Scroll.Effect = (uint16)atoul(row[ItemField::scrolleffect]);
			item.Scroll.Type = (uint8)atoul(row[ItemField::scrolltype]);
			item.Scroll.Level = (uint8)atoul(row[ItemField::scrolllevel]);
			item.Scroll.Level2 = (uint8)atoul(row[ItemField::scrolllevel2]);
			item.Bard.Effect = (uint16)atoul(row[ItemField::bardeffect]);
			item.Bard.Type = (uint8)atoul(row[ItemField::bardtype]);
			item.Bard.Level = (uint8)atoul(row[ItemField::bardlevel]);
			item.Bard.Level2 = (uint8)atoul(row[ItemField::bardlevel2]);
			item.QuestItemFlag = (atoi(row[ItemField::questitemflag])==0) ? false : true;
			item.SVCorruption = (sint32)atoi(row[ItemField::svcorruption]);
			item.Purity = (uint32)atoul(row[ItemField::purity]);
			item.BackstabDmg = (uint32)atoul(row[ItemField::backstabdmg]);
			item.DSMitigation = (uint32)atoul(row[ItemField::dsmitigation]);
			item.HeroicStr = (sint32)atoi(row[ItemField::heroic_str]);
			item.HeroicInt = (sint32)atoi(row[ItemField::heroic_int]);
			item.HeroicWis = (sint32)atoi(row[ItemField::heroic_wis]);
			item.HeroicAgi = (sint32)atoi(row[ItemField::heroic_agi]);
			item.HeroicDex = (sint32)atoi(row[ItemField::heroic_dex]);
			item.HeroicSta = (sint32)atoi(row[ItemField::heroic_sta]);
			item.HeroicCha = (sint32)atoi(row[ItemField::heroic_cha]);
			item.HeroicMR = (sint32)atoi(row[ItemField::heroic_mr]);
			item.HeroicFR = (sint32)atoi(row[ItemField::heroic_fr]);
			item.HeroicCR = (sint32)atoi(row[ItemField::heroic_cr]);
			item.HeroicDR = (sint32)atoi(row[ItemField::heroic_dr]);
			item.HeroicPR = (sint32)atoi(row[ItemField::heroic_pr]);
			item.HeroicSVCorrup = (sint32)atoi(row[ItemField::heroic_svcorrup]);
			item.HealAmt = (sint32)atoi(row[ItemField::healamt]);
			item.SpellDmg = (sint32)atoi(row[ItemField::spelldmg]);
			item.LDoNSellBackRate = (uint32)atoul(row[ItemField::ldonsellbackrate]);
			item.ScriptFileID = (uint32)atoul(row[ItemField::scriptfileid]);
			item.ExpendableArrow = (uint16)atoul(row[ItemField::expendablearrow]);
			item.Clairvoyance = (uint32)atoul(row[ItemField::clairvoyance]);
			strcpy(item.ClickName,row[ItemField::clickname]);
			strcpy(item.ProcName,row[ItemField::procname]);
			strcpy(item.WornName,row[ItemField::wornname]);
			strcpy(item.FocusName,row[ItemField::focusname]);
			strcpy(item.ScrollName,row[ItemField::scrollname]);

			if (!EMuShareMemDLL.Items.cbAddItem(item.ID, &item)) {
				LogFile->write(EQEMuLog::Error, "Database::DBLoadItems: Failure reported from EMuShareMemDLL.Items.cbAddItem(%i)", item.ID);
				break;
			}
		}

		mysql_free_result(result);
		ret = true;
	}
	else {
		LogFile->write(EQEMuLog::Error, "DBLoadItems query '%s', %s", query, errbuf);
		LogFile->write(EQEMuLog::Error, "If you got boat loads of errors, make sure you sourced all sql updates!\n");
	}
	return ret;
}

const Item_Struct* SharedDatabase::GetItem(uint32 id) {
	return EMuShareMemDLL.Items.GetItem(id);
}

const Item_Struct* SharedDatabase::IterateItems(uint32* NextIndex) {
	return EMuShareMemDLL.Items.IterateItems(NextIndex);
}


string SharedDatabase::GetBook(const char *txtfile)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	char txtfile2[20];
	string txtout;
	strcpy(txtfile2,txtfile);
	if (!RunQuery(query, MakeAnyLenString(&query, "SELECT txtfile FROM books where name='%s'", txtfile2), errbuf, &result)) {
		cerr << "Error in GetBook query '" << query << "' " << errbuf << endl;
		if (query != 0)
			safe_delete_array(query);
		txtout.assign(" ",1);
		return txtout;
	}
	else {
		safe_delete_array(query);
		if (mysql_num_rows(result) == 0) {
			mysql_free_result(result);
			LogFile->write(EQEMuLog::Error, "No book to send, (%s)", txtfile);
			txtout.assign(" ",1);
			return txtout;
		}
		else {
			row = mysql_fetch_row(result);
			txtout.assign(row[0],strlen(row[0]));
			mysql_free_result(result);
			return txtout;
		}
	}
}


bool SharedDatabase::extDBLoadNPCFactionLists(sint32 iNPCFactionListCount, int32 iMaxNPCFactionListID) {
	return s_usedb->DBLoadNPCFactionLists(iNPCFactionListCount, iMaxNPCFactionListID);
}

const NPCFactionList* SharedDatabase::GetNPCFactionEntry(uint32 id) {
	return EMuShareMemDLL.NPCFactionList.GetNPCFactionList(id);
}

bool SharedDatabase::LoadNPCFactionLists() {
	if (!EMuShareMemDLL.Load())
		return false;
	sint32 tmp = -1;
	int32 tmp_npcfactionlist_max;
	tmp = GetNPCFactionListsCount(&tmp_npcfactionlist_max);
	if (tmp < 0) {
		cout << "Error: SharedDatabase::LoadNPCFactionLists-ShareMem: GetNPCFactionListsCount() returned < 0" << endl;
		return false;
	}
	npcfactionlist_max = tmp_npcfactionlist_max;
	bool ret = EMuShareMemDLL.NPCFactionList.DLLLoadNPCFactionLists(&extDBLoadNPCFactionLists, sizeof(NPCFactionList), &tmp, &npcfactionlist_max, MAX_NPC_FACTIONS);
	return ret;
}

bool SharedDatabase::DBLoadNPCFactionLists(sint32 iNPCFactionListCount, int32 iMaxNPCFactionListID) {
	_CP(Database_DBLoadNPCFactionLists);
	LogFile->write(EQEMuLog::Status, "Loading NPC Faction Lists from database...");
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	query = new char[256];
	strcpy(query, "SELECT MAX(id), Count(*) FROM npc_faction");
	if (RunQuery(query, strlen(query), errbuf, &result)) {
		safe_delete_array(query);
		row = mysql_fetch_row(result);
		if (row && row[0]) {
			if ((int32)atoi(row[0]) > iMaxNPCFactionListID) {
				cout << "Error: Insufficient shared memory to load NPC Faction Lists." << endl;
				cout << "Max(id): " << atoi(row[0]) << ", iMaxNPCFactionListID: " << iMaxNPCFactionListID << endl;
				cout << "Fix this by increasing the MMF_MAX_NPCFactionList_ID define statement" << endl;
				mysql_free_result(result);
				return false;
			}
			if (atoi(row[1]) != iNPCFactionListCount) {
				cout << "Error: number of NPCFactionLists in memshare doesnt match database." << endl;
				cout << "Count(*): " << atoi(row[1]) << ", iNPCFactionListCount: " << iNPCFactionListCount << endl;
				mysql_free_result(result);
				return false;
			}
			npcfactionlist_max = atoi(row[0]);
			mysql_free_result(result);
			NPCFactionList tmpnfl;
			if (RunQuery(query, MakeAnyLenString(&query, "SELECT id, primaryfaction, ignore_primary_assist from npc_faction"), errbuf, &result)) {
				safe_delete_array(query);
				while((row = mysql_fetch_row(result))) {
					memset(&tmpnfl, 0, sizeof(NPCFactionList));
					tmpnfl.id = atoi(row[0]);
					tmpnfl.primaryfaction = atoi(row[1]);
					//if we have ignore_primary_assist set to non-zero then we will not assist our own faction
					//else we will assist (this is the default)
					tmpnfl.assistprimaryfaction = (atoi(row[2]) == 0) ? true : false;
					if (!EMuShareMemDLL.NPCFactionList.cbAddNPCFactionList(tmpnfl.id, &tmpnfl)) {
						mysql_free_result(result);
						cout << "Error: SharedDatabase::DBLoadNPCFactionLists: !EMuShareMemDLL.NPCFactionList.cbAddNPCFactionList" << endl;
						return false;
					}

					Sleep(0);
				}
				mysql_free_result(result);
			}
			else {
				cerr << "Error in DBLoadNPCFactionLists query2 '" << query << "' " << errbuf << endl;
				safe_delete_array(query);
				return false;
			}
			if (RunQuery(query, MakeAnyLenString(&query, "SELECT npc_faction_id, faction_id, value, npc_value FROM npc_faction_entries order by npc_faction_id"), errbuf, &result)) {
				safe_delete_array(query);
				sint8 i = 0;
				int32 curflid = 0;
				int32 tmpflid = 0;
				uint32 tmpfactionid[MAX_NPC_FACTIONS];
				sint32 tmpfactionvalue[MAX_NPC_FACTIONS];
				sint8 tmpfactionnpcvalue[MAX_NPC_FACTIONS];

				memset(tmpfactionid, 0, sizeof(tmpfactionid));
				memset(tmpfactionvalue, 0, sizeof(tmpfactionvalue));
				memset(tmpfactionnpcvalue, 0, sizeof(tmpfactionnpcvalue));

				while((row = mysql_fetch_row(result))) {
					tmpflid = atoi(row[0]);
					if (curflid != tmpflid && curflid != 0) {
						if (!EMuShareMemDLL.NPCFactionList.cbSetFaction(curflid, tmpfactionid, tmpfactionvalue, tmpfactionnpcvalue)) {
							mysql_free_result(result);
							cout << "Error: SharedDatabase::DBLoadNPCFactionLists: !EMuShareMemDLL.NPCFactionList.cbSetFaction" << endl;
							return false;
						}
						memset(tmpfactionid, 0, sizeof(tmpfactionid));
						memset(tmpfactionvalue, 0, sizeof(tmpfactionvalue));
						memset(tmpfactionnpcvalue, 0, sizeof(tmpfactionnpcvalue));
						i = 0;
					}
					curflid = tmpflid;
					tmpfactionid[i] = atoi(row[1]);
					tmpfactionvalue[i] = atoi(row[2]);
					tmpfactionnpcvalue[i] = atoi(row[3]);
					i++;
					if (i >= MAX_NPC_FACTIONS) {
						cerr << "Error in DBLoadNPCFactionLists: More than MAX_NPC_FACTIONS factions returned, flid=" << tmpflid << endl;
						break;
					}
					Sleep(0);
				}
				if (tmpflid) {
					EMuShareMemDLL.NPCFactionList.cbSetFaction(curflid, tmpfactionid, tmpfactionvalue, tmpfactionnpcvalue);
				}

				mysql_free_result(result);
			}
			else {
				cerr << "Error in DBLoadNPCFactionLists query3 '" << query << "' " << errbuf << endl;
				safe_delete_array(query);
				return false;
			}
		}
		else {
			mysql_free_result(result);
			//return false;
		}
	}
	else {
		cerr << "Error in DBLoadNPCFactionLists query1 '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}
	return true;
}

// Get the player profile and inventory for the given account "account_id" and
// character name "name".  Return true if the character was found, otherwise false.
// False will also be returned if there is a database error.
bool SharedDatabase::GetPlayerProfile(int32 account_id, char* name, PlayerProfile_Struct* pp, Inventory* inv, ExtendedProfile_Struct *ext, char* current_zone, uint32 *current_instance) {
	_CP(Database_GetPlayerProfile);
	char errbuf[MYSQL_ERRMSG_SIZE];
	char* query = 0;
	MYSQL_RES* result;
	MYSQL_ROW row;
	bool ret = false;

	unsigned long* lengths;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT profile,zonename,x,y,z,extprofile,instanceid FROM character_ WHERE account_id=%i AND name='%s'", account_id, name), errbuf, &result)) {
		if (mysql_num_rows(result) == 1) {	
			row = mysql_fetch_row(result);
			lengths = mysql_fetch_lengths(result);
			if (lengths[0] == sizeof(PlayerProfile_Struct)) {
				memcpy(pp, row[0], sizeof(PlayerProfile_Struct));

				if (current_zone)
					strcpy(current_zone, row[1]);
				pp->zone_id = GetZoneID(row[1]);
				pp->x = atof(row[2]);
				pp->y = atof(row[3]);
				pp->z = atof(row[4]);
				pp->zoneInstance = atoi(row[6]);
				if (pp->x == -1 && pp->y == -1 && pp->z == -1)
					GetSafePoints(pp->zone_id, GetInstanceVersion(pp->zoneInstance), &pp->x, &pp->y, &pp->z);

				if(current_instance)
					*current_instance = pp->zoneInstance;

				if(ext) {
					//SetExtendedProfile handles any conversion
					SetExtendedProfile(ext, row[5], lengths[5]);
				}

				// Retrieve character inventory
				ret = GetInventory(account_id, name, inv);
			}
			else {
				LogFile->write(EQEMuLog::Error, "Player profile length mismatch in GetPlayerProfile. Found: %i, Expected: %i",
					lengths[0], sizeof(PlayerProfile_Struct));
			}
		}

		mysql_free_result(result);
	}
	else {
		LogFile->write(EQEMuLog::Error, "GetPlayerProfile query '%s' %s", query, errbuf);
	}

	safe_delete_array(query);
	return ret;
}

bool SharedDatabase::SetPlayerProfile(uint32 account_id, uint32 charid, PlayerProfile_Struct* pp, Inventory* inv, ExtendedProfile_Struct *ext, uint32 current_zone, uint32 current_instance) {
	_CP(Database_SetPlayerProfile);
	char errbuf[MYSQL_ERRMSG_SIZE];
	char* query = 0;
	int32 affected_rows = 0;
	bool ret = false;

	if (RunQuery(query, SetPlayerProfile_MQ(&query, account_id, charid, pp, inv, ext, current_zone, current_instance), errbuf, 0, &affected_rows)) {
		ret = (affected_rows != 0);
	}

	UpdateCharacterPVPStats(charid, pp->PVPKills, pp->PVPDeaths, pp->PVPCurrentPoints, pp->PVPCareerPoints, pp->PVPBestKillStreak ,pp->PVPWorstDeathStreak,pp->PVPCurrentKillStreak,
		pp->PVPNumberOfKillsInLast24Hours, (char*)pp->PVPLastKill.Name, (char*)pp->PVPLastDeath.Name );

	UpdateCharacterStats(charid, pp->STR, pp->STA, pp->DEX, pp->AGI, pp->INT, pp->WIS, pp->CHA, pp->cur_hp,pp->cur_hp, pp->mana, pp->mana, pp->endurance, pp->endurance, pp->timePlayedMin);

	if (!ret) {
		LogFile->write(EQEMuLog::Error, "SetPlayerProfile query '%s' %s", query, errbuf);
	}

	safe_delete_array(query);
	return ret;
}

// Generate SQL for updating player profile
int32 SharedDatabase::SetPlayerProfile_MQ(char** query, uint32 account_id, uint32 charid, PlayerProfile_Struct* pp, Inventory* inv, ExtendedProfile_Struct *ext, uint32 current_zone, uint32 current_instance) {
	*query = new char[376 + sizeof(PlayerProfile_Struct)*2 + sizeof(ExtendedProfile_Struct)*2 + 4];
	char* end = *query;
	if (!current_zone)
		current_zone = pp->zone_id;

	if (!current_instance)
		current_instance = pp->zoneInstance;

	if(strlen(pp->name) == 0) // Sanity check in case pp never loaded
		return false;

	// Kings & Bandits updated to add pvpflag field.
	end += sprintf(end, "UPDATE character_ SET timelaston=unix_timestamp(now()),name=\'%s\', zonename=\'%s\', zoneid=%u, instanceid=%u, x = %f, y = %f, z = %f, pvpflag = %i, profile=\'", pp->name, GetZoneName(current_zone), current_zone, current_instance, pp->x, pp->y, pp->z, pp->pvp);
	end += DoEscapeString(end, (char*)pp, sizeof(PlayerProfile_Struct));
	end += sprintf(end,"\', extprofile=\'");
	end += DoEscapeString(end, (char*)ext, sizeof(ExtendedProfile_Struct));
	end += sprintf(end,"\',class=%d,level=%d WHERE id=%u", pp->class_, pp->level, charid);

	return (int32) (end - (*query));
}



// Create appropriate ItemInst class
ItemInst* SharedDatabase::CreateItem(uint32 item_id, sint16 charges, uint32 aug1, uint32 aug2, uint32 aug3, uint32 aug4, uint32 aug5)
{
	const Item_Struct* item = NULL;
	ItemInst* inst = NULL;
	item = GetItem(item_id);
	if (item) {
		inst = CreateBaseItem(item, charges);
		inst->PutAugment(this, 0, aug1);
		inst->PutAugment(this, 1, aug2);
		inst->PutAugment(this, 2, aug3);
		inst->PutAugment(this, 3, aug4);
		inst->PutAugment(this, 4, aug5);
	}

	return inst;
}


// Create appropriate ItemInst class
ItemInst* SharedDatabase::CreateItem(const Item_Struct* item, sint16 charges, uint32 aug1, uint32 aug2, uint32 aug3, uint32 aug4, uint32 aug5)
{
	ItemInst* inst = NULL;
	if (item) {
		if (charges == 0)
			charges = item->MaxCharges;
		inst = CreateBaseItem(item, charges);
		inst->PutAugment(this, 0, aug1);
		inst->PutAugment(this, 1, aug2);
		inst->PutAugment(this, 2, aug3);
		inst->PutAugment(this, 3, aug4);
		inst->PutAugment(this, 4, aug5);
		inst->SetCharges(charges);
	}

	return inst;
}

ItemInst* SharedDatabase::CreateBaseItem(const Item_Struct* item, sint16 charges) {
	ItemInst* inst = NULL;
	if (item) {
		if (charges == 0)
			charges = item->MaxCharges;

		if(item->CharmFileID != 0 || (item->LoreGroup >= 1000 && item->LoreGroup != -1)) {
			inst = new EvoItemInst(item, charges);
			((EvoItemInst*)inst)->Initialize(this);
		}
		else 
			inst = new ItemInst(item, charges);		
	}
	return inst;
}

sint32 SharedDatabase::DeleteStalePlayerCorpses() {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	int32 affected_rows = 0;

	if(RuleB(Zone, EnableShadowrest))
	{
		if (!RunQuery(query, MakeAnyLenString(&query, "UPDATE player_corpses SET IsBurried = 1 WHERE IsBurried=0 and "
			"(UNIX_TIMESTAMP() - UNIX_TIMESTAMP(timeofdeath)) > %d and not timeofdeath=0", 
			(RuleI(Character, CorpseDecayTimeMS) / 1000)), errbuf, 0, &affected_rows)) 
		{
			safe_delete_array(query);
			return -1;
		}
	}
	else
	{
		if (!RunQuery(query, MakeAnyLenString(&query, "Delete from player_corpses where (UNIX_TIMESTAMP() - "
			"UNIX_TIMESTAMP(timeofdeath)) > %d and not timeofdeath=0", (RuleI(Character, CorpseDecayTimeMS) / 1000)), 
			errbuf, 0, &affected_rows)) 
		{
			safe_delete_array(query);
			return -1;
		}
	}

	safe_delete_array(query);
	return affected_rows;
}

sint32 SharedDatabase::DeleteStalePlayerBackups() {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	int32 affected_rows = 0;

	// 1209600 seconds = 2 weeks
	if (!RunQuery(query, MakeAnyLenString(&query, "Delete from player_corpses_backup where (UNIX_TIMESTAMP() - UNIX_TIMESTAMP(timeofdeath)) > 1209600"), errbuf, 0, &affected_rows)) {
		safe_delete_array(query);
		return -1;
	}
	safe_delete_array(query);

	return affected_rows;
}

sint32 SharedDatabase::GetNPCFactionListsCount(int32* oMaxID) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	query = new char[256];
	strcpy(query, "SELECT MAX(id), count(*) FROM npc_faction");
	if (RunQuery(query, strlen(query), errbuf, &result)) {
		safe_delete_array(query);
		row = mysql_fetch_row(result);
		if (row != NULL && row[1] != 0) {
			sint32 ret = atoi(row[1]);
			if (oMaxID) {
				if (row[0])
					*oMaxID = atoi(row[0]);
				else
					*oMaxID = 0;
			}
			mysql_free_result(result);
			return ret;
		}
	}
	else {
		cerr << "Error in GetNPCFactionListsCount query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return -1;
	}

	return -1;
}

bool SharedDatabase::GetCommandSettings(map<string,uint8> &commands) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	query = new char[256];
	strcpy(query, "SELECT command,access from commands");
	commands.clear();
	if (RunQuery(query, strlen(query), errbuf, &result)) {
		safe_delete_array(query);
		while((row = mysql_fetch_row(result))) {
			commands[row[0]]=atoi(row[1]);
		}
		mysql_free_result(result);
		return true;
	} else {
		cerr << "Error in GetCommands query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}

	return false;
}












bool SharedDatabase::extDBLoadSkillCaps() {
	return s_usedb->DBLoadSkillCaps();
}

bool SharedDatabase::LoadSkillCaps() {
	if (!EMuShareMemDLL.Load())
		return false;

	int8 class_count = PLAYER_CLASS_COUNT;
	int8 skill_count = HIGHEST_SKILL+1;
	int8 level_count = HARD_LEVEL_CAP+1;

	return EMuShareMemDLL.SkillCaps.LoadSkillCaps(&extDBLoadSkillCaps,
		sizeof(uint16), class_count, skill_count, level_count);
}

bool SharedDatabase::DBLoadSkillCaps() {
	LogFile->write(EQEMuLog::Status, "Loading skill caps from database...");

	int8 class_count = PLAYER_CLASS_COUNT;
	int8 skill_count = HIGHEST_SKILL+1;
	int8 level_count = HARD_LEVEL_CAP+1;

	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	if (RunQuery(query, MakeAnyLenString(&query, 
		"SELECT skillID,class,level,cap FROM skill_caps ORDER BY skillID,class,level"), 
		errbuf, &result)) {
			safe_delete_array(query);

			while ((row = mysql_fetch_row(result))) {
				int8 skillID = atoi(row[0]);
				int8 class_ = atoi(row[1])-1;	//classes are base 1... 
				int8 level = atoi(row[2]);
				int16 cap = atoi(row[3]);
				if(skillID >= skill_count || class_ >= class_count || level >= level_count)
					continue;
				EMuShareMemDLL.SkillCaps.SetSkillCap(class_, skillID, level, cap);
			}
			mysql_free_result(result);
	}
	else {
		cerr << "Error in DBLoadSkillCaps (memshare) #2 query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}

	return true;
}

uint16 SharedDatabase::GetSkillCap(int8 Class_, SkillType Skill, int8 Level) {
	if(Class_ == 0)
		return(0);
	if(Level > SKILL_MAX_LEVEL){
		return EMuShareMemDLL.SkillCaps.GetSkillCap(Class_-1, Skill, SKILL_MAX_LEVEL);
	}
	else{
		return EMuShareMemDLL.SkillCaps.GetSkillCap(Class_-1, Skill, Level);
	}
}

uint8 SharedDatabase::GetTrainLevel(int8 Class_, SkillType Skill, int8 Level) {
	if(Class_ == 0)
		return(0);

	uint8 ret = 0;
	if(Level > SKILL_MAX_LEVEL){
		ret = EMuShareMemDLL.SkillCaps.GetTrainLevel(Class_-1, Skill, SKILL_MAX_LEVEL);
	}
	else
	{
		ret = EMuShareMemDLL.SkillCaps.GetTrainLevel(Class_-1, Skill, Level);
	}
	if(ret > GetSkillCap(Class_, Skill, Level))
		ret = GetSkillCap(Class_, Skill, Level);

	return ret;
}

void SharedDatabase::DBLoadDamageShieldTypes(SPDat_Spell_Struct* sp, sint32 iMaxSpellID) {

	const char *DSQuery = "SELECT `spellid`, `type` from `damageshieldtypes` WHERE `spellid` >0 "
		"AND `spellid` <= %i";

	const char *ERR_MYSQLERROR = "Error in DBLoadDamageShieldTypes: %s %s";

	char errbuf[MYSQL_ERRMSG_SIZE];
	char* query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;

	if(RunQuery(query,MakeAnyLenString(&query,DSQuery,iMaxSpellID),errbuf,&result)) {

		while((row = mysql_fetch_row(result))) {

			int SpellID = atoi(row[0]);
			if((SpellID > 0) && (SpellID <= iMaxSpellID))  {
				sp[SpellID].DamageShieldType = atoi(row[1]);
			}
		}
		mysql_free_result(result);
		safe_delete_array(query);
	}
	else {
		LogFile->write(EQEMuLog::Error, ERR_MYSQLERROR, query, errbuf);
		safe_delete_array(query);
	}
}

const EvolveInfo* SharedDatabase::GetEvolveInfo(uint32 loregroup) {
	return NULL;	// nothing here for now... database and/or sharemem pulls later
}



// Kings & Bandits
// AddNewItem - Support for creation of new items on the fly and transitioning it across the server.
bool SharedDatabase::AddNewItem(Item_Struct* item, uint32 templateID, char* hash, uint32* outItmID)
{
	if ( item == NULL )
	{
		LogFile->write(EQEMuLog::Error, "Database::AddNewItem: Bad item reference passed.");
		return false;
	}

	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MakeAnyLenString(&query,
		/*1*/"INSERT into items SET itemclass=%i,name=\"%s\",lore=\"%s\",idfile=\"%s\",weight=%i,norent=%i,nodrop=%i,size=%i,slots=%i,price=%i,icon=%i,"
		/*2*/"benefitflag=%i,tradeskills=%i,cr=%i,dr=%i,pr=%i,mr=%i,fr=%i,astr=%i,asta=%i,aagi=%i,adex=%i,acha=%i,aint=%i,awis=%i,hp=%i,mana=%i,ac=%i,deity=%i,skillmodvalue=%i,"
		/*3*/"skillmodtype=%i,banedmgrace=%i,banedmgamt=%i,banedmgbody=%i,magic=%i,casttime_=%i,reqlevel=%i,bardtype=%i,bardvalue=%i,light=%i,delay=%i,reclevel=%i,recskill=%i,"
		/*4*/"damage=%i,color=%u,classes=%i,races=%i,maxcharges=%i,itemtype=%i,material=%i,sellrate=%f,casttime=%i,elitematerial=%i,procrate=%i,"
		/*5*/"combateffects=%i,shielding=%i,stunresist=%i,strikethrough=%i,extradmgskill=%i,extradmgamt=%i,spellshield=%i,avoidance=%i,accuracy=%i,charmfileid=%i,factionmod1=%i,factionmod2=%i,"
		/*6*/"factionmod3=%i,factionmod4=%i,factionamt1=%i,factionamt2=%i,factionamt3=%i,factionamt4=%i,charmfile='%s',"
		/*7*/"ldontheme=%i,ldonprice=%i,ldonsold=%i,bagtype=%i,bagslots=%i,bagsize=%i,bagwr=%i,"
		/*8*/"book=%i,booktype=%i,filename='%s',banedmgraceamt=%i,augrestrict=%i,loregroup=%i,pendingloreflag=%i,artifactflag=%i,summonedflag=%i,favor=%i,fvnodrop=%i,endur=%i,dotshielding=%i,attack=%i,"
		/*9*/"regen=%i,manaregen=%i,enduranceregen=%i,haste=%i,damageshield=%i,recastdelay=%i,recasttype=%i,guildfavor=%i,augdistiller=%i,attuneable=%i,nopet=%i,"
		/*10*/"pointtype=%i,potionbelt=%i,potionbeltslots=%i,stacksize=%i,notransfer=%i,stackable=%i,clickeffect=%i,clicktype=%i,clicklevel=%i,clicklevel2=%i,proceffect=%i,proctype=%i,"
		/*11*/"proclevel=%i,proclevel2=%i,worneffect=%i,worntype=%i,wornlevel=%i,wornlevel2=%i,focuseffect=%i,focustype=%i,focuslevel=%i,focuslevel2=%i,scrolleffect=%i,scrolltype=%i,scrolllevel=%i,"
		/*12*/"scrolllevel2=%i,bardeffect=%i,bardeffecttype=%i,bardlevel2=%i,bardlevel=%i,questitemflag=%i,svcorruption=%i,purity=%i,backstabdmg=%i,dsmitigation=%i,heroic_str=%i,heroic_int=%i,"
		/*13*/"heroic_wis=%i,heroic_agi=%i,heroic_dex=%i,heroic_sta=%i,heroic_cha=%i,heroic_mr=%i,heroic_fr=%i,heroic_cr=%i,heroic_dr=%i,heroic_pr=%i,heroic_svcorrup=%i,healamt=%i,spelldmg=%i,"
		/*14*/"ldonsellbackrate=%i,scriptfileid=%i,expendablearrow=%i,clairvoyance=%i,clickname='%s',procname='%s',wornname='%s',focusname='%s',scrollname='%s',UNK132=0,templateid=%i,templatehash='%s'",
		/*1*/item->ItemClass,item->Name,item->Lore,item->IDFile,item->Weight,item->NoRent,item->NoDrop,item->Size,item->Slots,item->Price,item->Icon,
		/*2*/item->BenefitFlag,item->Tradeskills,item->CR,item->DR,item->PR,item->MR,item->FR,item->AStr,item->ASta,item->AAgi,item->ADex,item->ACha,item->AInt,item->AWis,item->HP,item->Mana,item->AC,item->Deity,item->SkillModValue,
		/*3*/item->SkillModType,item->BaneDmgRace,item->BaneDmgAmt,item->BaneDmgBody,item->Magic,item->CastTime_,item->ReqLevel,item->BardType,item->BardValue,item->Light,item->Delay,item->RecLevel,item->RecSkill,
		/*4*/item->Damage,item->Color,item->Classes,item->Races,item->MaxCharges,item->ItemType,item->Material,item->SellRate,item->CastTime,item->EliteMaterial,item->ProcRate,
		/*5*/item->CombatEffects,item->Shielding,item->StunResist,item->StrikeThrough,item->ExtraDmgSkill,item->ExtraDmgAmt,item->SpellShield,item->Avoidance,item->Accuracy,item->CharmFileID,item->FactionMod1,item->FactionMod2,
		/*6*/item->FactionMod3,item->FactionMod4,item->FactionAmt1,item->FactionAmt2,item->FactionAmt3,item->FactionAmt4,item->CharmFile,
		/*7*/item->LDoNTheme,item->LDoNPrice,item->LDoNSold,item->BagType,item->BagSlots,item->BagSize,item->BagWR,
		/*8*/item->Book,item->BookType,item->Filename,item->BaneDmgRaceAmt,item->AugRestrict,item->LoreGroup,item->PendingLoreFlag,item->ArtifactFlag,item->SummonedFlag,item->Favor,item->FVNoDrop,item->Endur,item->DotShielding,item->Attack,
		/*9*/item->Regen,item->ManaRegen,item->EnduranceRegen,item->Haste,item->DamageShield,item->RecastDelay,item->RecastType,item->GuildFavor,item->AugDistiller,item->Attuneable,item->NoPet,
		/*10*/item->PointType,item->PotionBelt,item->PotionBeltSlots,item->StackSize,item->NoTransfer,item->Stackable,item->Click.Effect,item->Click.Type,item->Click.Level,item->Click.Level2,item->Proc.Effect,item->Proc.Type,
		/*11*/item->Proc.Level,item->Proc.Level2,item->Worn.Effect,item->Worn.Type,item->Worn.Level,item->Worn.Level2,item->Focus.Effect,item->Focus.Type,item->Focus.Level,item->Focus.Level2,item->Scroll.Effect,item->Scroll.Type,item->Scroll.Level,
		/*12*/item->Scroll.Level2,item->Bard.Effect,item->Bard.Type,item->Bard.Level2,item->Bard.Level,item->QuestItemFlag,item->SVCorruption,item->Purity,item->BackstabDmg,item->DSMitigation,item->HeroicStr,item->HeroicInt,
		/*13*/item->HeroicWis,item->HeroicAgi,item->HeroicDex,item->HeroicSta,item->HeroicCha,item->HeroicMR,item->HeroicFR,item->HeroicCR,item->HeroicDR,item->HeroicPR,item->HeroicSVCorrup,item->HealAmt,item->SpellDmg,
		/*14*/item->LDoNSellBackRate,item->ScriptFileID,item->ExpendableArrow,item->Clairvoyance,item->ClickName,item->ProcName,item->WornName,item->FocusName,item->ScrollName, templateID, hash);

	int32 affected_rows,insert_id = 0;
	if (!RunQuery(query, strlen(query), errbuf, &result, &affected_rows, &insert_id)) {

		if ( outItmID )
			*outItmID = insert_id;

		item->ID = insert_id;

		safe_delete_array(query);
		//mysql_free_result(result);

		if ( insert_id < 1 )
		{
			LogFile->write(EQEMuLog::Error, "Database::AddNewItem: Failure, no item ID reported from EMuShareMemDLL.Items.cbAddItem(%s)", item->Name);
			return false;
		}
		else if (!EMuShareMemDLL.Items.cbAddItem(item->ID, item)) {
			LogFile->write(EQEMuLog::Error, "Database::AddNewItem: Failure reported from EMuShareMemDLL.Items.cbAddItem(%s)", item->Name);
			return false;
		}

		return true;
	}
	else
	{
		safe_delete_array(query);
		LogFile->write(EQEMuLog::Error, "Database::AddNewItem: Database item creation failed (%s).", item->Name);
		return false;
	}

	safe_delete_array(query);
	return false;
}

sint32 SharedDatabase::GetRandomLootDropCount(int32* oMaxID) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 ret = -1;


	char query[] = "SELECT MAX(id),count(*) FROM randomlootdrop";
	if (RunQuery(query, strlen(query), errbuf, &result)) {
		row = mysql_fetch_row(result);
		if (row != NULL && row[1] != 0) {
			ret = atoi(row[1]);
			if (oMaxID) {
				if (row[0])
				{
					int32 maxID = atoi(row[0]);
					*oMaxID = maxID;
				}
				else
				{
					*oMaxID = 0;
				}
			}
		}
		mysql_free_result(result);
	}
	else {
		cerr << "Error in GetRandomLootDropCount query '" << query << "' " << errbuf << endl;
	}

	return ret;
}

sint32 SharedDatabase::GetRandomDropRateCount(int32* oMaxID) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 ret = -1;


	char query[] = "SELECT MAX(id),count(*) FROM randomdroprate";
	if (RunQuery(query, strlen(query), errbuf, &result)) {
		row = mysql_fetch_row(result);
		if (row != NULL && row[1] != 0) {
			ret = atoi(row[1]);
			if (oMaxID) {
				if (row[0])
				{
					int32 maxID = atoi(row[0]);
					*oMaxID = maxID;
				}
				else
				{
					*oMaxID = 0;
				}
			}
		}
		mysql_free_result(result);
	}
	else {
		cerr << "Error in GetRandomLootDropCount query '" << query << "' " << errbuf << endl;
	}

	return ret;
}

sint32 SharedDatabase::GetRandomNameGroupCount(int32* oMaxID) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 ret = -1;


	char query[] = "SELECT MAX(templateid),count(*) FROM randomnamegroups";
	if (RunQuery(query, strlen(query), errbuf, &result)) {
		row = mysql_fetch_row(result);
		if (row != NULL && row[1] != 0) {
			ret = atoi(row[1]);
			if (oMaxID) {
				if (row[0])
				{
					int32 maxID = atoi(row[0]);
					*oMaxID = maxID;
				}
				else
				{
					*oMaxID = 0;
				}
			}
		}
		mysql_free_result(result);
	}
	else {
		cerr << "Error in GetRandomNameGroupCount query '" << query << "' " << errbuf << endl;
	}

	return ret;
}


void SharedDatabase::UpdateTeamSettings(TeamSettings_Struct* tss)
{
	TeamSettings_Struct data;
	memcpy(&data,tss,sizeof(TeamSettings_Struct));
			if (!EMuShareMemDLL.TeamSettings.cbUpdateTeamSettings(data.teamID, &data)) {
				LogFile->write(EQEMuLog::Error, "Database::UpdateTeamSettings: Failure reported from EMuShareMemDLL.TeamSettings.cbUpdateTeamSettings(%i)", tss->teamID);
			}
}


void SharedDatabase::UpdatePVPLeaderBoard(PVPLeaderBoardEntry_Struct* lbe)
{
	PVPLeaderBoardEntry_Struct cnstEntry;
	memcpy(&cnstEntry,lbe,sizeof(PVPLeaderBoardEntry_Struct));
			if (!EMuShareMemDLL.TeamSettings.cbUpdatePVPLeaderBoard(&cnstEntry)) {
				LogFile->write(EQEMuLog::Error, "Database::UpdatePVPLeaderBoard: Failure reported from EMuShareMemDLL.TeamSettings.cbUpdatePVPLeaderBoard()");
			}
}

// kings & bandits - generic update query
bool SharedDatabase::UpdateDatabaseQuery(const char* query) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	int32	affected_rows = 0;
	if (!RunQuery(query, strlen(query), errbuf, 0,&affected_rows)) {
		cerr << "Error in UpdateDatabaseQuery(id) query '" << query << "' " << errbuf << endl;
		return false;
	}

	if (affected_rows == 0)
		return false;

	return true;
}

// Kings & Bandits - load settings
bool SharedDatabase::LoadTeamSettings() {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	query = new char[256];
	strcpy(query, "SELECT count(teamid) FROM team_settings");


	bool ret = false;
	if (RunQuery(query, strlen(query), errbuf, &result)) {
		safe_delete_array(query);
		row = mysql_fetch_row(result);
		if (row && row[0])
		{
			sint32 count = atoi(row[0]);

			if (EMuShareMemDLL.Load())
			{
				// Kings & Bandits - hardcoded max teams to 10 here
				uint32 maxID = 10;
				bool ret = EMuShareMemDLL.TeamSettings.DLLLoadTeamSettings(&extDBLoadTeamSettings, sizeof(TeamSettings_Struct), &count, &maxID);
			}

			mysql_free_result(result);
		}
	}
	else {
		cerr << "Error in LoadTeamSettings query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return ret;
	}
	return ret;
}

// Kings & Bandits - load team settings
bool SharedDatabase::extDBLoadTeamSettings(sint32 iItemCount, int32 iMaxItemID) {
	return s_usedb->DBLoadTeamSettings(iItemCount, iMaxItemID);
}

// Kings & Bandits - load team settings
bool SharedDatabase::DBLoadTeamSettings(sint32 iItemCount, uint32 iMaxItemID) {
	_CP(Database_DBLoadItems);
	char errbuf[MYSQL_ERRMSG_SIZE];
	MYSQL_RES *result;
	MYSQL_ROW row;
	bool ret = false;

	char query[] = "SELECT teamid,bankreserves,newbietaxrate,citizentaxrate,kingpcname,kingnpcid,upkeeprate from team_settings";
	if (RunQuery(query, sizeof(query), errbuf, &result)) {
		while((row = mysql_fetch_row(result))) {
#if EQDEBUG >= 6
			LogFile->write(EQEMuLog::Status, "Loading %s:%i", row[ItemField::name], row[ItemField::id]);
#endif				
			TeamSettings_Struct team;
			memset(&team, 0, sizeof(TeamSettings_Struct));
			team.teamID = atoul(row[0]);
			team.bankReserve = atoul(row[1]);
			team.newbieTaxRate = atoul(row[2]);
			team.citizenTaxRate = atoul(row[3]);
			strcpy(team.kingPCName,row[4]);
			team.kingNPCTypeID = atoul(row[5]);
			team.upkeepRate = atoul(row[6]);

			if (!EMuShareMemDLL.TeamSettings.cbAddTeamSettings(team.teamID, &team)) {
				LogFile->write(EQEMuLog::Error, "Database::DBLoadTeamSettings: Failure reported from EMuShareMemDLL.TeamSettings.cbAddTeamSettings(%i)", team.teamID);
				break;
			}
		}

		mysql_free_result(result);
		ret = true;
	}
	else {
		LogFile->write(EQEMuLog::Error, "DBLoadTeamSettings query '%s', %s", query, errbuf);
	}
	
	char query2[] = "SELECT t1.charid,t1.pvpkills,t1.pvpdeaths,t1.pvpcareerpts,t2.name from characterdb_pvpstats as t1, character_ as t2 where t2.id=t1.charid order by t1.pvpkills desc limit 100";
	if (RunQuery(query2, sizeof(query2), errbuf, &result)) {
		while((row = mysql_fetch_row(result))) {
#if EQDEBUG >= 6
			LogFile->write(EQEMuLog::Status, "Loading %s:%i", row[ItemField::name], row[ItemField::id]);
#endif				
			PVPLeaderBoardEntry_Struct lbe;
			lbe.Kills = atoul(row[1]);
			lbe.Deaths = atoul(row[2]);
			lbe.TotalPoints = atoul(row[3]);
			strcpy(lbe.Name,row[4]);

			if (!EMuShareMemDLL.TeamSettings.cbUpdatePVPLeaderBoard(&lbe)) {
				LogFile->write(EQEMuLog::Error, "Database::DBLoadTeamSettings: Failure reported from EMuShareMemDLL.TeamSettings.cbUpdatePVPLeaderBoard()");
				break;
			}
		}

		mysql_free_result(result);
		ret = true;
	}
	else {
		LogFile->write(EQEMuLog::Error, "DBLoadTeamSettings query '%s', %s", query, errbuf);
	}
	return ret;
}

TeamSettings_Struct* SharedDatabase::GetTeamSettings(uint8 id) {
	return EMuShareMemDLL.TeamSettings.GetTeamSettings(id);
}

PVPLeaderBoardEntry_Struct* SharedDatabase::GetPVPLeaderBoardEntries() {
	return EMuShareMemDLL.TeamSettings.GetPVPLeadershipBoard();
}

bool SharedDatabase::LoadRandomLootDrops() {
	if (!EMuShareMemDLL.Load())
		return false;
	sint32 tmp = 0;
	tmp = GetRandomLootDropCount(&max_randomlootdropid);
	if (tmp == -1) {
		cout << "Error: SharedDatabase::LoadRandomLootDrops() (sharemem): GetRandomLootDropCount() returned -1" << endl;
		return false;
	}
	bool ret = EMuShareMemDLL.RandomLootDrops.DLLLoadRandomLootDrops(&extDBLoadRandomLootDrops, sizeof(RandomLootDrop_Struct), &tmp, &max_randomlootdropid);
	return ret;
}

bool SharedDatabase::LoadRandomDropRates() {
	if (!EMuShareMemDLL.Load())
		return false;
	sint32 tmp = 0;
	tmp = GetRandomDropRateCount(&max_randomdroprateid);
	if (tmp == -1) {
		cout << "Error: SharedDatabase::LoadRandomDropRates() (sharemem): GetRandomLootDropCount() returned -1" << endl;
		return false;
	}
	bool ret = EMuShareMemDLL.RandomDropRates.DLLLoadRandomDropRates(&extDBLoadRandomDropRates, sizeof(RandomDropRate_Struct), &tmp, &max_randomdroprateid);
	return ret;
}

bool SharedDatabase::LoadRandomNameGroups() {
	if (!EMuShareMemDLL.Load())
		return false;
	sint32 tmp = 0;
	tmp = GetRandomNameGroupCount(&max_randomnamegroupid);
	if (tmp == -1) {
		cout << "Error: SharedDatabase::LoadRandomDropRates() (sharemem): GetRandomLootDropCount() returned -1" << endl;
		return false;
	}
	bool ret = EMuShareMemDLL.RandomNameGroups.DLLLoadRandomNameGroups(&extDBLoadRandomNameGroups, sizeof(RandomNameGroup_Struct), &tmp, &max_randomnamegroupid);
	return ret;
}

bool SharedDatabase::extDBLoadRandomLootDrops(sint32 dropCount, int32 maxDropID) {
	return s_usedb->LoadRandomLootDropsTable();
}

bool SharedDatabase::extDBLoadRandomDropRates(sint32 dropCount, int32 maxDropID) {
	return s_usedb->LoadRandomDropRatesTable();
}

bool SharedDatabase::extDBLoadRandomNameGroups(sint32 dropCount, int32 maxDropID) {
	return s_usedb->LoadRandomNameGroupsTable();
}

// Kings & Bandits
// LoadRandomLootDrops - load the LootDrop table to handle all items in memshare
bool SharedDatabase::LoadRandomLootDropsTable()
{
	// Retrieve all items from database
	char query[] = "select id,itemgroupid,droprateid,minitemdrop,maxitemdrop,mincoindrop,maxcoindrop,minlevel,maxlevel,zoneid,npcid,npcspawngroupid from randomlootdrop order by id";
	char errbuf[MYSQL_ERRMSG_SIZE];
	MYSQL_RES *result;
	MYSQL_ROW row;
	if (RunQuery(query, sizeof(query), errbuf, &result)) {
		while((row = mysql_fetch_row(result))) {
			RandomLootDrop_Struct rld;
			memset(&rld,0,sizeof(RandomLootDrop_Struct));
			uint32 dropID = (uint32)atoi(row[0]);
			uint32 itemGroupID = (uint32)atoi(row[1]);
			uint32 dropRateID = (uint32)atoi(row[2]);
			uint8 minItemDrop = (uint8)atoi(row[3]);
			uint8 maxItemDrop = (uint8)atoi(row[4]);
			uint32 minCoinDrop = (uint32)atoi(row[5]);
			uint32 maxCoinDrop = (uint32)atoi(row[6]);
			uint8 minLevel = (uint8)atoi(row[7]);
			uint8 maxLevel = (uint8)atoi(row[8]);
			uint32 zoneid = (uint32)atoi(row[9]);
			uint32 npcid = (uint32)atoi(row[10]);
			uint32 npcspawngroupid = (uint32)atoi(row[11]);

			rld.lootDropID = dropID;
			rld.itemGroupID = itemGroupID;
			rld.dropRateID = dropRateID;
			rld.minItemDrop = minItemDrop;
			rld.maxItemDrop = maxItemDrop;
			rld.minCoinDrop = minCoinDrop;
			rld.maxCoinDrop = maxCoinDrop;
			rld.minLevel = minLevel;
			rld.maxLevel = maxLevel;
			rld.zoneID = zoneid;
			rld.npcID = npcid;
			rld.npcGroupID = npcspawngroupid;

			if ( itemGroupID > 0 )
				PopulateLootDropItemList(&rld);

			if (!EMuShareMemDLL.RandomLootDrops.cbAddRandomLootDrop(rld.lootDropID, &rld)) {
				LogFile->write(EQEMuLog::Error, "Database::LoadRandomLootDrops: Failure reported from EMuShareMemDLL.RandomLootDrops.cbAddRandomLootDrop(%i)", rld.lootDropID);
				break;
			}
		}

		mysql_free_result(result);

		return true;
	}
	return false;
}

bool SharedDatabase::PopulateLootDropItemList(RandomLootDrop_Struct* rld)
{

	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;

	char queryBase[] = "select "
#define F(x) ""#x","
#include "../KingsBandits/KBItemStructs.h"
#undef F
		"NOW() from randomitemtemplates";

	int itemCount = 0;
	if (RunQuery(query, MakeAnyLenString(&query, "%s where groupid=%i limit %i", queryBase,rld->itemGroupID,MAXRANDOMITEMTEMPLATE), errbuf, &result)) {
		while((row = mysql_fetch_row(result))) {
			RandomItemTemplate_Struct rit;
			memset(&rit,0,sizeof(RandomItemTemplate_Struct));
			rit.itemclass = (uint8)atoi(row[RandomItemField::itemclass]);
			strncpy(rit.idfile,row[RandomItemField::idfile],sizeof(rit.idfile));
			rit.itemid = (uint32)atoul(row[RandomItemField::id]);
			rit.weight = (uint8)atoi(row[RandomItemField::weight]);
			rit.norent = (uint8)atoi(row[RandomItemField::norent]);
			rit.nodrop = (uint8)atoi(row[RandomItemField::nodrop]);
			rit.size = (uint8)atoi(row[RandomItemField::size]);
			rit.slots = (uint32)atoul(row[RandomItemField::slots]);

			rit.minstatus = (uint32)atoul(row[RandomItemField::minstatus]);

			rit.minprice = (uint32)atoul(row[RandomItemField::minprice]);
			rit.maxprice = (uint32)atoul(row[RandomItemField::maxprice]);

			rit.icon = (uint32)atoul(row[RandomItemField::icon]);
			rit.loregroup = (uint32)atoul(row[RandomItemField::loregroup]);
			rit.loreflag = (rit.loregroup != 0);

			rit.pendingloreflag = (atoi(row[RandomItemField::pendingloreflag])==0) ? false : true;
			rit.artifactflag = (atoi(row[RandomItemField::artifactflag])==0) ? false : true;

			rit.minfavor = (uint32)atoul(row[RandomItemField::minfavor]);
			rit.maxfavor = (uint32)atoul(row[RandomItemField::maxfavor]);

			rit.minguildfavor = (uint32)atoul(row[RandomItemField::minguildfavor]);
			rit.maxguildfavor = (uint32)atoul(row[RandomItemField::maxguildfavor]);

			rit.pointtype = (uint32)atoul(row[RandomItemField::pointtype]);

			rit.bagtype = (uint8)atoi(row[RandomItemField::bagtype]);

			rit.bagslots = (uint8)atoi(row[RandomItemField::bagslots]);
			rit.bagsize = (uint8)atoi(row[RandomItemField::bagsize]);

			rit.minbagwr = (uint8)atoi(row[RandomItemField::minbagwr]);
			rit.maxbagwr = (uint8)atoi(row[RandomItemField::maxbagwr]);

			rit.benefitflag = (atoi(row[RandomItemField::benefitflag])==0) ? false : true;
			rit.tradeskills = (atoi(row[RandomItemField::tradeskills])==0) ? false : true;

			rit.mincr = (sint8)atoi(row[RandomItemField::mincr]);
			rit.maxcr = (sint8)atoi(row[RandomItemField::maxcr]);
			rit.mindr = (sint8)atoi(row[RandomItemField::mindr]);
			rit.maxdr = (sint8)atoi(row[RandomItemField::maxdr]);
			rit.minpr = (sint8)atoi(row[RandomItemField::minpr]);
			rit.maxpr = (sint8)atoi(row[RandomItemField::maxpr]);
			rit.minmr = (sint8)atoi(row[RandomItemField::minmr]);
			rit.maxmr = (sint8)atoi(row[RandomItemField::maxmr]);
			rit.minfr = (sint8)atoi(row[RandomItemField::minfr]);
			rit.maxfr = (sint8)atoi(row[RandomItemField::maxfr]);

			rit.minstr = (sint8)atoi(row[RandomItemField::minstr]);
			rit.maxstr = (sint8)atoi(row[RandomItemField::maxstr]);
			rit.minsta = (sint8)atoi(row[RandomItemField::minsta]);
			rit.maxsta = (sint8)atoi(row[RandomItemField::maxsta]);
			rit.minagi = (sint8)atoi(row[RandomItemField::minagi]);
			rit.maxagi = (sint8)atoi(row[RandomItemField::maxagi]);
			rit.mindex = (sint8)atoi(row[RandomItemField::mindex]);
			rit.maxdex = (sint8)atoi(row[RandomItemField::maxdex]);
			rit.mincha = (sint8)atoi(row[RandomItemField::mincha]);
			rit.maxcha = (sint8)atoi(row[RandomItemField::maxcha]);
			rit.minint = (sint8)atoi(row[RandomItemField::minint]);
			rit.maxint = (sint8)atoi(row[RandomItemField::maxint]);
			rit.minwis = (sint8)atoi(row[RandomItemField::minwis]);
			rit.maxwis = (sint8)atoi(row[RandomItemField::maxwis]);

			rit.minhp = (sint32)atoul(row[RandomItemField::minhp]);
			rit.maxhp = (sint32)atoul(row[RandomItemField::maxhp]);

			rit.minac = (sint32)atoul(row[RandomItemField::minac]);
			rit.maxac = (sint32)atoul(row[RandomItemField::maxac]);

			rit.deity = (uint32)atoul(row[RandomItemField::deity]);

			rit.minskillmodvalue = (uint32)atoul(row[RandomItemField::minskillmodvalue]);
			rit.maxskillmodvalue = (uint32)atoul(row[RandomItemField::maxskillmodvalue]);

			rit.skillmodtype = (uint32)atoul(row[RandomItemField::skillmodtype]);

			rit.minbanedmgamt = (uint32)atoul(row[RandomItemField::minbanedmgamt]);
			rit.maxbanedmgamt = (uint32)atoul(row[RandomItemField::maxbanedmgamt]);

			rit.magic = (atoi(row[RandomItemField::magic])==0) ? false : true;

			rit.mincasttime_ = (sint32)atoul(row[RandomItemField::mincasttime_]);
			rit.maxcasttime_ = (sint32)atoul(row[RandomItemField::maxcasttime_]);

			rit.minreqlevel = (uint8)atoi(row[RandomItemField::minreqlevel]);
			rit.maxreqlevel = (uint8)atoi(row[RandomItemField::maxreqlevel]);

			rit.minreclevel = (uint8)atoi(row[RandomItemField::minreclevel]);
			rit.maxreclevel = (uint8)atoi(row[RandomItemField::maxreclevel]);

			rit.recskill = (uint8)atoi(row[RandomItemField::recskill]);

			rit.minrange = (uint8)atoi(row[RandomItemField::minrange]);
			rit.maxrange = (uint8)atoi(row[RandomItemField::maxrange]);

			rit.mindamage = (uint8)atoi(row[RandomItemField::mindamage]);
			rit.maxdamage = (uint8)atoi(row[RandomItemField::maxdamage]);

			rit.mincolor = (uint32)atoul(row[RandomItemField::mincolor]);
			rit.maxcolor = (uint32)atoul(row[RandomItemField::maxcolor]);

			rit.classes = (uint32)atoul(row[RandomItemField::classes]);
			rit.races = (uint32)atoul(row[RandomItemField::races]);

			rit.minmaxcharges = (sint16)atoi(row[RandomItemField::minmaxcharges]);
			rit.maxmaxcharges = (sint16)atoi(row[RandomItemField::maxmaxcharges]);

			rit.itemtype = (uint8)atoi(row[RandomItemField::itemtype]);
			rit.material = (uint8)atoi(row[RandomItemField::material]);

			rit.minsellrate = (float)atof(row[RandomItemField::minsellrate]);
			rit.maxsellrate = (float)atof(row[RandomItemField::maxsellrate]);

			rit.mincasttime = (uint32)atoul(row[RandomItemField::mincasttime]);
			rit.maxcasttime = (uint32)atoul(row[RandomItemField::maxcasttime]);

			rit.elitematerial = (uint32)atoul(row[RandomItemField::elitematerial]);
			rit.procrate = (sint32)atoul(row[RandomItemField::procrate]);
			rit.charmfileid = (uint32)atoul(row[RandomItemField::charmfileid]);

			strcpy(rit.charmfile,row[RandomItemField::charmfile]);

			rit.augtype = (uint32)atoul(row[RandomItemField::augtype]);

			/*			rit.augslot1type = (uint8)atoi(row[RandomItemField::augslot1type]);
			rit.augslot2type = (uint8)atoi(row[RandomItemField::augslot2type]);
			rit.augslot3type = (uint8)atoi(row[RandomItemField::augslot3type]);
			rit.augslot4type = (uint8)atoi(row[RandomItemField::augslot4type]);
			rit.augslot5type = (uint8)atoi(row[RandomItemField::augslot5type]);

			rit.augslots1visible = (uint8)atoi(row[RandomItemField::augslot1visible]);
			rit.augslots2visible = (uint8)atoi(row[RandomItemField::augslot2visible]);
			rit.augslots3visible = (uint8)atoi(row[RandomItemField::augslot3visible]);
			rit.augslots4visible = (uint8)atoi(row[RandomItemField::augslot4visible]);
			rit.augslots5visible = (uint8)atoi(row[RandomItemField::augslot5visible]);
			*/			
			rit.minbanedmgraceamt = (uint32)atoul(row[RandomItemField::minbanedmgraceamt]);
			rit.maxbanedmgraceamt = (uint32)atoul(row[RandomItemField::maxbanedmgraceamt]);

			rit.banedmgrace = (uint32)atoul(row[RandomItemField::banedmgrace]);

			rit.augrestrict = (uint32)atoul(row[RandomItemField::augrestrict]);

			rit.minendurance = (uint32)atoul(row[RandomItemField::minendurance]);
			rit.maxendurance = (uint32)atoul(row[RandomItemField::maxendurance]);

			rit.dotshielding = (uint32)atoul(row[RandomItemField::dotshielding]);

			rit.minattack = (uint32)atoul(row[RandomItemField::minattack]);
			rit.maxattack = (uint32)atoul(row[RandomItemField::maxattack]);

			rit.mindelay = (uint8)atoul(row[RandomItemField::mindelay]);
			rit.maxdelay = (uint8)atoul(row[RandomItemField::maxdelay]);
			rit.minregen = (uint32)atoul(row[RandomItemField::minregen]);
			rit.maxregen = (uint32)atoul(row[RandomItemField::maxregen]);

			rit.minmana = (sint32)atoul(row[RandomItemField::minmana]);
			rit.maxmana = (sint32)atoul(row[RandomItemField::maxmana]);

			rit.minmanaregen = (uint32)atoul(row[RandomItemField::minmanaregen]);
			rit.maxmanaregen = (uint32)atoul(row[RandomItemField::maxmanaregen]);

			rit.minenduranceregen = (uint32)atoul(row[RandomItemField::minenduranceregen]);
			rit.maxenduranceregen = (uint32)atoul(row[RandomItemField::maxenduranceregen]);

			rit.minhaste = (uint32)atoul(row[RandomItemField::minhaste]);
			rit.maxhaste = (uint32)atoul(row[RandomItemField::maxhaste]);

			rit.damageshield = (uint32)atoul(row[RandomItemField::damageshield]);
			rit.recastdelay = (uint32)atoul(row[RandomItemField::recastdelay]);
			rit.recasttype = (uint32)atoul(row[RandomItemField::recasttype]);
			rit.augdistiller = (uint32)atoul(row[RandomItemField::augdistiller]);

			rit.attuneable = (atoi(row[RandomItemField::attuneable])==0) ? false : true;
			rit.nopet = (atoi(row[RandomItemField::nopet])==0) ? false : true;
			rit.potionbelt = (atoi(row[RandomItemField::potionbelt])==0) ? false : true;
			rit.stackable = (atoi(row[RandomItemField::stackable])==0) ? false : true;
			rit.notransfer = (atoi(row[RandomItemField::notransfer])==0) ? false : true;

			rit.bardtype = (sint32)atoul(row[RandomItemField::bardtype]);
			rit.bardvalue = (sint32)atoul(row[RandomItemField::bardvalue]);

			rit.stacksize = (uint8)atoi(row[RandomItemField::stacksize]);
			rit.potionbeltslots = (uint8)atoi(row[RandomItemField::potionbeltslots]);


			rit.Click.Effect = (uint32)atoul(row[RandomItemField::clickeffect]);
			rit.Click.Type = (uint8)atoul(row[RandomItemField::clicktype]);
			rit.Click.Level = (uint8)atoul(row[RandomItemField::clicklevel]);
			rit.Click.Level2 = (uint8)atoul(row[RandomItemField::clicklevel2]);

			rit.Proc.Effect = (uint16)atoul(row[RandomItemField::proceffect]);
			rit.Proc.Type = (uint8)atoul(row[RandomItemField::proctype]);
			rit.Proc.Level = (uint8)atoul(row[RandomItemField::proclevel]);
			rit.Proc.Level2 = (uint8)atoul(row[RandomItemField::proclevel2]);

			rit.Worn.Effect = (uint16)atoul(row[RandomItemField::worneffect]);
			rit.Worn.Type = (uint8)atoul(row[RandomItemField::worntype]);
			rit.Worn.Level = (uint8)atoul(row[RandomItemField::wornlevel]);
			rit.Worn.Level2 = (uint8)atoul(row[RandomItemField::wornlevel2]);

			rit.Focus.Effect = (uint16)atoul(row[RandomItemField::focuseffect]);
			rit.Focus.Type = (uint8)atoul(row[RandomItemField::focustype]);
			rit.Focus.Level = (uint8)atoul(row[RandomItemField::focuslevel]);
			rit.Focus.Level2 = (uint8)atoul(row[RandomItemField::focuslevel2]);

			rit.Scroll.Effect = (uint16)atoul(row[RandomItemField::scrolleffect]);
			rit.Scroll.Type = (uint8)atoul(row[RandomItemField::scrolltype]);
			rit.Scroll.Level = (uint8)atoul(row[RandomItemField::scrolllevel]);
			rit.Scroll.Level2 = (uint8)atoul(row[RandomItemField::scrolllevel2]);

			rit.Bard.Effect = (uint16)atoul(row[RandomItemField::bardeffect]);
			rit.Bard.Type = (uint8)atoul(row[RandomItemField::bardtype]);
			rit.Bard.Level = (uint8)atoul(row[RandomItemField::bardlevel]);
			rit.Bard.Level2 = (uint8)atoul(row[RandomItemField::bardlevel2]);

			rit.book = (uint8)atoul(row[RandomItemField::book]);
			rit.booktype = (uint32)atoul(row[RandomItemField::booktype]);

			rit.minhealamt = (sint32)atoul(row[RandomItemField::minhealamt]);
			rit.maxhealamt = (sint32)atoul(row[RandomItemField::maxhealamt]);

			rit.minspelldmg = (sint32)atoul(row[RandomItemField::minspelldmg]);
			rit.maxspelldmg = (sint32)atoul(row[RandomItemField::maxspelldmg]);

			rit.scriptfileid = (uint32)atoul(row[RandomItemField::scriptfileid]);
			rit.expendablearrow = (uint16)atoul(row[RandomItemField::expendablearrow]);
			rit.clairvoyance = (uint32)atoul(row[RandomItemField::clairvoyance]);

			strcpy(rit.clickname,row[RandomItemField::clickname]);
			strcpy(rit.procname,row[RandomItemField::procname]);
			strcpy(rit.wornname,row[RandomItemField::wornname]);
			strcpy(rit.focusname,row[RandomItemField::focusname]);
			strcpy(rit.scrollname,row[RandomItemField::scrollname]);

			rit.itemNameGroup = (uint32)atoul(row[RandomItemField::itemnamegroup]);

			rit.itemRandCount = (uint32)atoul(row[RandomItemField::itemcount]);

			memset(&rld->itemTemplates[itemCount],0,sizeof(RandomItemTemplate_Struct));
			memcpy(&rld->itemTemplates[itemCount],&rit,sizeof(RandomItemTemplate_Struct));
			itemCount++;
			rld->itemGroupCount = itemCount;
		}

		mysql_free_result(result);

		return true;
	}
	return false;
}


bool SharedDatabase::LoadRandomDropRatesTable()
{
	// Retrieve all items from database
	char query[] = "select id,basedropchance,extremedropchance,raredropchance,specialdropchance,commondropchance,newextremeraredropchance,newraredropchance"
		",newspecialdropchance,newcommondropchance from randomdroprate order by id";

	char errbuf[MYSQL_ERRMSG_SIZE];
	MYSQL_RES *result;
	MYSQL_ROW row;
	if (RunQuery(query, sizeof(query), errbuf, &result)) {
		while((row = mysql_fetch_row(result))) {
			RandomDropRate_Struct rdr;
			memset(&rdr,0,sizeof(RandomDropRate_Struct));
			uint32 dropID = (uint32)atoi(row[0]);
			uint32 baseDropChance = (uint32)atoi(row[1]);
			uint32 extremeDropChance = (uint32)atoi(row[2]);
			uint8 rareDropChance = (uint8)atoi(row[3]);
			uint8 specialDropChance = (uint8)atoi(row[4]);
			uint32 commonDropChance = (uint32)atoi(row[5]);
			uint32 newExtremeDropChance = (uint32)atoi(row[6]);
			uint8 newRareDropChance = (uint8)atoi(row[7]);
			uint8 newSpecialDropChance = (uint8)atoi(row[8]);
			uint32 newCommonDropChance = (uint32)atoi(row[9]);

			rdr.dropRateID = dropID;

			rdr.baseDropChance = baseDropChance;

			rdr.extremeRareDropChance = extremeDropChance;
			rdr.rareDropChance = rareDropChance;
			rdr.specialDropChance = specialDropChance;
			rdr.commonDropChance = commonDropChance;

			rdr.newExtremeRareDropChance = newExtremeDropChance;
			rdr.newRareDropChance = newRareDropChance;
			rdr.newSpecialDropChance = newSpecialDropChance;
			rdr.newCommonDropChance = newCommonDropChance;

			if (!EMuShareMemDLL.RandomDropRates.cbAddRandomDropRate(rdr.dropRateID, &rdr)) {
				LogFile->write(EQEMuLog::Error, "Database::LoadRandomDropRatesTable: Failure reported from EMuShareMemDLL.RandomDropRates.cbAddRandomDropRate(%i)", rdr.dropRateID);
				break;
			}
		}

		mysql_free_result(result);

		return true;
	}
	return false;
}


bool SharedDatabase::GetRandomNameMaxWeight(uint32 templateID, uint32* oMaxID) {
	if ( templateID < 1 )
		return false;

	char errbuf[MYSQL_ERRMSG_SIZE];
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 ret = -1;


	char* query = 0;
	MakeAnyLenString(&query, "SELECT MAX(weight) from randomnames where templateid=%i", templateID);
	if (RunQuery(query, strlen(query), errbuf, &result)) {
		safe_delete_array(query);
		row = mysql_fetch_row(result);
		uint8 maxID = 0;
		if (row != NULL && row[0] != 0) {
			if (oMaxID) {
				if (row[0])
				{
					maxID = (uint8)atoi(row[0]);
					*oMaxID = maxID;
				}
			}
		}

		mysql_free_result(result);
		return true;
	}
	else {
		safe_delete_array(query);
		cerr << "Error in GetRandomNameMaxWeight query '" << query << "' " << errbuf << endl;
	}

	return false;
}

bool SharedDatabase::GetRandomName(uint32 templateID, uint32 weight,string* data) {
	string returnData("");
	if ( templateID < 1 )
		return false;

	uint32 maxID = 0;

	// Kings & Bandits - temporary hack for max weight and avoiding bad names
	GetRandomNameMaxWeight(templateID,&maxID);
	if ( weight > maxID )
		weight = maxID;

	char errbuf[MYSQL_ERRMSG_SIZE];
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 ret = -1;


	char* query = 0;
	MakeAnyLenString(&query, "SELECT name from randomnames WHERE templateid=%i AND weight=%i order by rand() limit 1", templateID,weight);
	if (RunQuery(query, strlen(query), errbuf, &result)) {
		safe_delete_array(query);
		row = mysql_fetch_row(result);
		uint8 maxID = 0;
		if (row != NULL && row[0] != 0) {
			char outStr[32];
			snprintf(outStr,32,"%s",row[0]);
			data->append(outStr);
		}

		mysql_free_result(result);
		return true;
	}
	else {
		safe_delete_array(query);
		cerr << "Error in GetRandomNameMaxWeight query '" << query << "' " << errbuf << endl;
	}

	return false;
}


bool SharedDatabase::LoadRandomNameGroupsTable()
{
	// Retrieve all items from database
	char query[] = "select templateid,startgroupid,middlegroupid,connectgroupid,endgroupid from randomnamegroups";

	char errbuf[MYSQL_ERRMSG_SIZE];
	MYSQL_RES *result;
	MYSQL_ROW row;
	if (RunQuery(query, sizeof(query), errbuf, &result)) {
		while((row = mysql_fetch_row(result))) {
			RandomNameGroup_Struct rng;
			memset(&rng,0,sizeof(RandomNameGroup_Struct));
			uint32 templateID = (uint32)atoi(row[0]);
			uint32 startID = (uint32)atoi(row[1]);
			uint32 middleID = (uint32)atoi(row[2]);
			uint32 connectID = (uint32)atoi(row[3]);
			uint32 endID = (uint32)atoi(row[4]);

			rng.templateID = templateID;
			rng.startGroupID = startID;
			rng.middleGroupID = middleID;
			rng.connectGroupID = connectID;
			rng.endGroupID = endID;

			GetRandomNameMaxWeight(rng.startGroupID,&rng.startGroupWeight);
			GetRandomNameMaxWeight(rng.middleGroupID,&rng.middleGroupWeight);
			GetRandomNameMaxWeight(rng.connectGroupID,&rng.connectGroupWeight);
			GetRandomNameMaxWeight(rng.endGroupID,&rng.endGroupWeight);

			if (!EMuShareMemDLL.RandomNameGroups.cbAddRandomNameGroup(rng.templateID, &rng)) {
				LogFile->write(EQEMuLog::Error, "Database::LoadRandomNameGroupsTable: Failure reported from EMuShareMemDLL.RandomNameGroups.cbAddRandomNameGroup(%i)", rng.templateID);
				break;
			}
		}

		mysql_free_result(result);

		return true;
	}
	return false;
}

const RandomLootDrop_Struct* SharedDatabase::GetRandomLootDrop(uint32 id) {
	return EMuShareMemDLL.RandomLootDrops.GetItem(id);
}

const RandomDropRate_Struct* SharedDatabase::GetRandomDropRate(uint32 id) {
	return EMuShareMemDLL.RandomDropRates.GetItem(id);
}

const RandomNameGroup_Struct* SharedDatabase::GetRandomNameGroup(uint32 id) {
	return EMuShareMemDLL.RandomNameGroups.GetItem(id);
}

// TODO: Add character name array and target npc name array for passing random name generation.
bool SharedDatabase::GenerateRandomLootGroupResult(RandomLootDropResult_Struct* rldr, uint32 groupid, char* npcName, char* pcName)
{
	const RandomLootDrop_Struct* rld = GetRandomLootDrop(groupid);
	if ( rld != NULL )
	{
		float randomFormula = MakeRandomFloat(.01f,.9999f);
		uint32 randomFormulaInt = (uint32) ( randomFormula * 100.0f );
		float dbValue = 0.5f;

		uint32 newExtremeRareChance = 0;
		uint32 newRareChance = 0;
		uint32 newSpecialChance = 0;
		uint32 newCommonChance = 0;

		uint32 extremeRareChance = 0;
		uint32 rareChance = 0;
		uint32 specialChance = 0;
		uint32 commonChance = 0;

		const RandomDropRate_Struct* rdr = GetRandomDropRate(rld->dropRateID);

		if ( rdr != NULL )
		{
			// dbValue is the baseDropchance out of 100
			dbValue = (float)rdr->baseDropChance/100.0f;

			extremeRareChance = rdr->extremeRareDropChance;
			rareChance = rdr->rareDropChance;
			specialChance = rdr->specialDropChance;
			commonChance = rdr->commonDropChance;

			newExtremeRareChance = rdr->newExtremeRareDropChance;
			newRareChance = rdr->newRareDropChance;
			newSpecialChance = rdr->newSpecialDropChance;
			newCommonChance = rdr->newCommonDropChance;
		}
		
		uint8 itemCountResult = 0;
		switch(rld->minItemDrop)
		{
		case 0:
			{
				if ( rld->maxItemDrop > 0 )
					itemCountResult = MakeRandomInt(0,rld->maxItemDrop);

				break;
			}
		default:
			{
				itemCountResult = CalculateBaseFormula(randomFormula,dbValue,rld->minItemDrop,rld->maxItemDrop);
				break;
			}
		}

		switch(rld->minCoinDrop)
		{
		case 0:
			{
				if ( rld->maxCoinDrop > 0 )
					rldr->coinDropValue = MakeRandomInt(0,rld->maxCoinDrop);

				break;
			}
		default:
			{
				rldr->coinDropValue = CalculateBaseFormula(randomFormula,dbValue,rld->minCoinDrop,rld->maxCoinDrop);
				break;
			}
		}

		for(int i=0;i<itemCountResult;i++)
		{
			float newRndFormula = 0.0f;
			uint32 newRndInt = 0;

			// generate a new percentage to base the item on
			GetRandomPercentage(&randomFormula,&randomFormulaInt);

			// determine what output item we will get
			RNDITEM_DROP_TYPE dropType = NONE;

			// check if the random chance is higher than the db fields
			if ( extremeRareChance > 0 && extremeRareChance < randomFormulaInt )
			{
				dropType = EXTREMERARE;

				if ( newExtremeRareChance > 0 )
				{
					GetRandomPercentage(&newRndFormula,&newRndInt);
					if ( newExtremeRareChance < newRndInt )
						dropType = NEWEXTREMERARE;
				}
			}
			else if ( rareChance > 0 && rareChance < randomFormulaInt )
			{
				dropType = RARE;

				if ( newRareChance > 0 )
				{
					GetRandomPercentage(&newRndFormula,&newRndInt);
					if ( newRareChance < newRndInt )
						dropType = NEWRARE;
				}
			}
			else if ( specialChance > 0 && specialChance < randomFormulaInt )
			{
				dropType = SPECIAL;

				if ( newSpecialChance > 0 )
				{
					GetRandomPercentage(&newRndFormula,&newRndInt);
					if ( newSpecialChance < newRndInt )
						dropType = NEWSPECIAL;
				}
			}
			else if ( commonChance > 0 && commonChance < randomFormulaInt )
			{
				dropType = COMMON;

				if ( newCommonChance > 0 )
				{
					GetRandomPercentage(&newRndFormula,&newRndInt);
					if ( newCommonChance < newRndInt )
						dropType = NEWCOMMON;
				}
			}

			if ( dropType == NONE || rld->itemGroupCount <  1 )
			{
				_log(ZONE__INIT, "Random Drop Type Returned 'None', no item will be generated. Item count in list is %i.",rld->itemGroupCount);
				continue;
			}

			// determine the random item slot
			uint8 rndInt = (uint8)MakeRandomInt(0,rld->itemGroupCount-1);

			enum RNDITEM_DROP_TYPE actualDropType = NONE;
			switch(dropType)
			{
			case NEWEXTREMERARE:
				actualDropType = EXTREMERARE;
				break;
			case NEWRARE:
				actualDropType = RARE;
				break;
			case NEWSPECIAL:
				actualDropType = SPECIAL;
				break;
			case NEWCOMMON:
				actualDropType = COMMON;
				break;
			default:
				actualDropType = dropType;
				break;
			}

			bool gotExistingItem = false;
			uint32 outID = 0;

			const RandomItemTemplate_Struct* itmTemplate = &rld->itemTemplates[rndInt];

			uint16 resultCRC = GetItemHash(randomFormula, itmTemplate);

			// Kings & Bandits - added code for if the random count is 0 we will always try and create a new item.
			// here we will loop and try to get a unique id, after 3 attempts we give up.
			if ( itmTemplate->itemRandCount < 1 )
			{
				int attempts = 3;
				for ( int i=0;i<attempts;i++)
				{
					// i sure hope this is random enough! :-/
					resultCRC = (MakeRandomFloat(0.00000001,0.99999999) * 10000000) + attempts * MakeRandomInt(0,10000);
					gotExistingItem = GetExistingRandomDropID(actualDropType,resultCRC,itmTemplate->itemid,&outID);

					if ( !gotExistingItem )
						break;
				}
			}
			else
			{
				gotExistingItem = GetExistingRandomDropID(actualDropType,resultCRC,itmTemplate->itemid,&outID);
				if ( !gotExistingItem )
					_log(ZONE__INIT, "No item existed, generating new.");
				else
					_log(ZONE__INIT, "Found existing item %i.",outID);
			}

			bool success = false;

			Item_Struct itm;
			memset(&itm,0,sizeof(Item_Struct));

			if ( !gotExistingItem )
			{
				success = GenerateRandItemStruct(randomFormula, dbValue, &itm, itmTemplate, actualDropType, npcName, pcName);

				if ( success )
				{
					uint32 resultID = 0;

					char hash[80];
					uint16 resultCRC = GetItemHash(randomFormula, itmTemplate);
					snprintf(hash,80,"Random%i %i",actualDropType,resultCRC);
					if ( AddNewItem(&itm,itmTemplate->itemid,hash,&resultID) )
					{
						ItemDropResponse_Struct dropStruct;
						dropStruct.itemID = resultID;
						dropStruct.isNewItem = true;
						rldr->items[rldr->itemDropCount] = dropStruct;
						rldr->itemDropCount++;
					}
				}
			}
			else
			{
				ItemDropResponse_Struct dropStruct;
				dropStruct.itemID = outID;
				dropStruct.isNewItem = false;
				rldr->items[rldr->itemDropCount] = dropStruct;
				rldr->itemDropCount++;
			}
		}

		return true;
	}

	return false;
}

void SharedDatabase::GetRandomPercentage(float* floatPercent, uint32* intPercent)
{
	float value = MakeRandomFloat(.01f,1.0f);
	if ( floatPercent != NULL )
		*floatPercent = value;
	if ( intPercent != NULL )
		*intPercent = (uint32) ( value * 100.0f );
}

uint32 SharedDatabase::GetItemHash(float dbValue,const RandomItemTemplate_Struct* rndItem)
{
	if ( rndItem == NULL )
		return 0;

	char value[40];
	snprintf(value,40,"%f",dbValue);
	uint16 resultCRC = (uint16)CRC16((unsigned char*)value,strlen(value),rndItem->itemid);

	if ( rndItem->itemRandCount < 2 )
		resultCRC = 0;
	else
		resultCRC %= rndItem->itemRandCount;

	return resultCRC;
}

string SharedDatabase::GenerateRandomName(uint32 nameGroupID)
{
	float baseValue = 0.1f;
	float dbValue = MakeRandomFloat(.01f,.9999f);
	const RandomNameGroup_Struct* rngStruct = GetRandomNameGroup(nameGroupID);

	string constructedName("");
	if ( rngStruct != NULL )
	{
		if ( rngStruct->startGroupID > 0 )
		{
			string data("");

			uint32 weight = (uint32)CalculateBaseFormula(baseValue,dbValue,(uint32)0,rngStruct->startGroupWeight);
			GetRandomName(rngStruct->startGroupID,weight,&data);
			if ( data.length() > 0 )
			{
				if ( constructedName.length() > 0 )
					constructedName.append(" ");
				constructedName.append(data);
			}
		}
		if ( rngStruct->middleGroupID > 0 )
		{
			string data("");

			uint32 weight = (uint32)CalculateBaseFormula(baseValue,dbValue,(uint32)0,rngStruct->middleGroupWeight);
			GetRandomName(rngStruct->middleGroupID,weight,&data);
			if ( data.length() > 0 )
			{
				if ( constructedName.length() > 0 )
					constructedName.append(" ");
				constructedName.append(data);
			}
		}
		if ( rngStruct->connectGroupID > 0 )
		{
			string data("");

			uint32 weight = (uint32)CalculateBaseFormula(baseValue,dbValue,(uint32)0,rngStruct->connectGroupWeight);
			GetRandomName(rngStruct->connectGroupID,weight,&data);
			if ( data.length() > 0 )
			{
				if ( constructedName.length() > 0 )
					constructedName.append(" ");
				constructedName.append(data);
			}
		}
		if ( rngStruct->endGroupID > 0 )
		{
			string data("");
			uint32 newWeight = 0;

			uint32 weight = (uint32)CalculateBaseFormula(baseValue,dbValue,(uint32)0,(uint32)rngStruct->endGroupWeight);
			GetRandomName(rngStruct->endGroupID,weight,&data);
			if ( data.length() > 0 )
			{
				if ( constructedName.length() > 0 )
					constructedName.append(" ");

				constructedName.append(data);
			}
		}
	}
	return constructedName;
}

void trim1(string& str)
{
	string::size_type pos1 = str.find_first_not_of(' ');
	string::size_type pos2 = str.find_last_not_of(' ');
	str = str.substr(pos1 == string::npos ? 0 : pos1, 
		pos2 == string::npos ? str.length() - 1 : pos2 - pos1 + 1);
}

void trim2(string& str)
{
	string::size_type pos = str.find_last_not_of(' ');
	if(pos != string::npos) {
		str.erase(pos + 1);
		pos = str.find_first_not_of(' ');
		if(pos != string::npos) str.erase(0, pos);
	}
	else str.erase(str.begin(), str.end());
}

bool SharedDatabase::GenerateRandItemStruct(float baseValue, float dbValue, Item_Struct* itmStruct, const RandomItemTemplate_Struct* rndItem, RNDITEM_DROP_TYPE dropType, char* npcNameStr, char* pcNameStr)
{
	string npcName("");
	npcName.append(npcNameStr);
	string pcName("");
	pcName.append(pcNameStr);

	// TODO: Randomize name

	bool nameSet = false;

	uint32 weightModifier = 0;
	switch ( dropType )
	{
	case COMMON:
		{
			weightModifier = 3;
			break;
		}
	case SPECIAL:
		{
			weightModifier = 2;
			break;
		}
	case RARE:
		{
			weightModifier = 1;
			break;
		}
	}

	if ( rndItem->itemNameGroup > 0 )
	{
		string constructedName;
		const RandomNameGroup_Struct* rngStruct = GetRandomNameGroup(rndItem->itemNameGroup);
		if ( rngStruct != NULL )
		{
			if ( rngStruct->startGroupID > 0 )
			{
				string data("");
				uint32 newWeight = 0;
				if (  rngStruct->startGroupWeight > 0 && ( (sint32)rngStruct->startGroupWeight - (sint32)weightModifier ) > 0 )
					newWeight = rngStruct->startGroupWeight - weightModifier;

				uint32 minWeight = 0;
				switch( dropType )
				{
				case RARE:
				case EXTREMERARE:
					{
						minWeight = (uint32)CalculateBaseFormula(baseValue,dbValue,newWeight,rngStruct->startGroupWeight);
						break;
					}
				}

				uint32 weight = (uint32)CalculateBaseFormula(baseValue,dbValue,minWeight,newWeight);
				GetRandomName(rngStruct->startGroupID,weight,&data);
				if ( data.length() > 0 )
				{
					if ( data.c_str()[0] != '%' && constructedName.length() > 0 )
						constructedName.append(" ");

					if ( !strcasecmp(data.c_str(),"%npcname%") )
					{
						constructedName.append(npcName);
					}
					else if ( !strcasecmp(data.c_str(),"%pcname%") )
					{
						constructedName.append(pcName);
					}
					else if ( data.c_str()[0] == '%')
						constructedName.append(data.substr(1));
					else
						constructedName.append(data);
				}
			}
			if ( rngStruct->middleGroupID > 0 )
			{
				string data("");
				uint32 newWeight = 0;
				if (  rngStruct->middleGroupWeight > 0 && ( (sint32)rngStruct->middleGroupWeight - (sint32)weightModifier ) > 0 )
					newWeight = rngStruct->middleGroupWeight - weightModifier;

				uint32 minWeight = 0;
				switch( dropType )
				{
				case RARE:
				case EXTREMERARE:
					{
						minWeight = (uint32)CalculateBaseFormula(baseValue,dbValue,newWeight,rngStruct->middleGroupID);
						break;
					}
				}

				uint32 weight = (uint32)CalculateBaseFormula(baseValue,dbValue,minWeight,newWeight);
				GetRandomName(rngStruct->middleGroupID,weight,&data);
				if ( data.length() > 0 )
				{
					if ( data.c_str()[0] != '%' && constructedName.length() > 0 )
						constructedName.append(" ");

					if ( !strcasecmp(data.c_str(),"%npcname%") )
					{
						if ( constructedName.length() > 0 )
							constructedName.append(" ");
						constructedName.append(npcName);
					}
					else if ( !strcasecmp(data.c_str(),"%pcname%") )
					{
						if ( constructedName.length() > 0 )
							constructedName.append(" ");
						constructedName.append(pcName);
					}
					else if ( data.c_str()[0] == '%')
						constructedName.append(data.substr(1));
					else
						constructedName.append(data);
				}
			}
			if ( rngStruct->connectGroupID > 0 )
			{
				string data("");
				uint32 newWeight = 0;
				if ( rngStruct->connectGroupWeight > 0 &&  ( (sint32)rngStruct->connectGroupWeight - (sint32)weightModifier ) > 0 )
					newWeight = rngStruct->connectGroupWeight - weightModifier;

				uint32 minWeight = 0;
				switch( dropType )
				{
				case RARE:
				case EXTREMERARE:
					{
						minWeight = (uint32)CalculateBaseFormula(baseValue,dbValue,newWeight,rngStruct->connectGroupID);
						break;
					}
				}

				uint32 weight = (uint32)CalculateBaseFormula(baseValue,dbValue,minWeight,newWeight);
				GetRandomName(rngStruct->connectGroupID,weight,&data);
				if ( data.length() > 0 )
				{
					if ( data.c_str()[0] != '%' && constructedName.length() > 0 )
						constructedName.append(" ");

					if ( !strcasecmp(data.c_str(),"%npcname%") )
					{
						if ( constructedName.length() > 0 )
							constructedName.append(" ");
						constructedName.append(npcName);
					}
					else if ( !strcasecmp(data.c_str(),"%pcname%") )
					{
						if ( constructedName.length() > 0 )
							constructedName.append(" ");
						constructedName.append(pcName);
					}
					else if ( data.c_str()[0] == '%')
						constructedName.append(data.substr(1));
					else
						constructedName.append(data);
				}
			}
			if ( rngStruct->endGroupID > 0 )
			{
				string data("");
				uint32 newWeight = 0;
				if ( rngStruct->endGroupWeight > 0 && ( (sint32)rngStruct->endGroupWeight - (sint32)weightModifier ) > 0 )
					newWeight = rngStruct->endGroupWeight - weightModifier;

				uint32 minWeight = 0;
				switch( dropType )
				{
				case RARE:
				case EXTREMERARE:
					{
						minWeight = (uint32)CalculateBaseFormula(baseValue,dbValue,newWeight,rngStruct->endGroupID);
						break;
					}
				}

				uint32 weight = (uint32)CalculateBaseFormula(baseValue,dbValue,minWeight,newWeight);
				GetRandomName(rngStruct->endGroupID,weight,&data);
				if ( data.length() > 0 )
				{
					if ( data.c_str()[0] != '%' && constructedName.length() > 0 )
						constructedName.append(" ");

					if ( !strcasecmp(data.c_str(),"%npcname%") )
					{
						if ( constructedName.length() > 0 )
							constructedName.append(" ");
						constructedName.append(npcName);
					}
					else if ( !strcasecmp(data.c_str(),"%pcname%") )
					{
						if ( constructedName.length() > 0 )
							constructedName.append(" ");
						constructedName.append(pcName);
					}
					else if ( data.c_str()[0] == '%')
						constructedName.append(data.substr(1));
					else
						constructedName.append(data);
				}
			}

			if ( constructedName.length() > 0 )
			{
				//trim1(constructedName);
				strncpy(itmStruct->Name,constructedName.c_str(),63);
				LogFile->write(EQEMuLog::Debug, "Item is being generated with name %s.", itmStruct->Name);
				nameSet = true;
			}
		}
		else
		{
			_log(ZONE__INIT, "No item name group found for item template %i.",rndItem->itemid);
		}
	}

	if ( !nameSet )
		strcpy(itmStruct->Name,"Random Item");

	strcpy(itmStruct->Lore,"");

	itmStruct->ItemClass = rndItem->itemclass;
	itmStruct->MinStatus = rndItem->minstatus;
	strcpy(itmStruct->IDFile,rndItem->idfile);
	itmStruct->Weight = rndItem->weight;

	itmStruct->NoDrop = rndItem->nodrop;
	itmStruct->NoRent = rndItem->norent;

	itmStruct->Size = rndItem->size;
	itmStruct->Slots = rndItem->slots;

	itmStruct->Price = (uint32)CalculateBaseFormula(baseValue,dbValue,rndItem->minprice,rndItem->maxprice);

	itmStruct->Icon = rndItem->icon;
	itmStruct->BenefitFlag = rndItem->benefitflag;
	itmStruct->Tradeskills = rndItem->tradeskills;

	itmStruct->CR = (sint8)CalculateBaseFormula(baseValue,dbValue,rndItem->mincr,rndItem->maxcr);
	itmStruct->FR = (sint8)CalculateBaseFormula(baseValue,dbValue,rndItem->minfr,rndItem->maxfr);
	itmStruct->MR = (sint8)CalculateBaseFormula(baseValue,dbValue,rndItem->minmr,rndItem->maxmr);
	itmStruct->DR = (sint8)CalculateBaseFormula(baseValue,dbValue,rndItem->mindr,rndItem->maxdr);
	itmStruct->PR = (sint8)CalculateBaseFormula(baseValue,dbValue,rndItem->minpr,rndItem->maxpr);

	itmStruct->AAgi = (sint8)CalculateBaseFormula(baseValue,dbValue,rndItem->minagi,rndItem->maxagi);
	itmStruct->AStr = (sint8)CalculateBaseFormula(baseValue,dbValue,rndItem->minstr,rndItem->maxstr);
	itmStruct->ASta = (sint8)CalculateBaseFormula(baseValue,dbValue,rndItem->minsta,rndItem->maxsta);
	itmStruct->ADex = (sint8)CalculateBaseFormula(baseValue,dbValue,rndItem->mindex,rndItem->maxdex);
	itmStruct->AInt = (sint8)CalculateBaseFormula(baseValue,dbValue,rndItem->minint,rndItem->maxint);
	itmStruct->ACha = (sint8)CalculateBaseFormula(baseValue,dbValue,rndItem->mincha,rndItem->maxcha);
	itmStruct->AWis = (sint8)CalculateBaseFormula(baseValue,dbValue,rndItem->minwis,rndItem->maxwis);

	itmStruct->AC = (sint32)CalculateBaseFormula(baseValue,dbValue,rndItem->minac,rndItem->maxac);
	itmStruct->HP = (sint32)CalculateBaseFormula(baseValue,dbValue,(sint32)rndItem->minhp,(sint32)rndItem->maxhp);
	itmStruct->Mana = (sint32)CalculateBaseFormula(baseValue,dbValue,rndItem->minmana,rndItem->maxmana);

	itmStruct->Attack = (uint32)CalculateBaseFormula(baseValue,dbValue,rndItem->minattack,rndItem->maxattack);
	itmStruct->Damage = (uint8)CalculateBaseFormula(baseValue,dbValue,rndItem->mindamage,rndItem->maxdamage);

	//float delayBaseValue = 1.0f - baseValue;
	itmStruct->Delay = (uint8)CalculateOppositeBaseFormula(baseValue,dbValue,(uint32)rndItem->mindelay,(uint32)rndItem->maxdelay);

	itmStruct->Deity = rndItem->deity;
	itmStruct->SkillModValue = (sint32)CalculateBaseFormula(baseValue,dbValue,rndItem->minskillmodvalue,rndItem->maxskillmodvalue);
	itmStruct->SkillModType = rndItem->skillmodtype;

	itmStruct->BaneDmgRace = rndItem->banedmgrace;
	itmStruct->BaneDmgAmt = (sint8)CalculateBaseFormula(baseValue,dbValue,rndItem->minbanedmgamt,rndItem->maxbanedmgamt);
	//itmStruct->BaneDmgBody

	itmStruct->Magic = rndItem->magic;

	itmStruct->CastTime_ = (uint32)CalculateBaseFormula(baseValue,dbValue,rndItem->mincasttime_, rndItem->maxcasttime_);

	itmStruct->ReqLevel = (uint8)CalculateBaseFormula(baseValue,dbValue,rndItem->minreqlevel,rndItem->maxreqlevel);

	itmStruct->BardType = rndItem->bardtype;
	itmStruct->BardValue = rndItem->bardvalue;

	itmStruct->RecSkill = rndItem->recskill;
	itmStruct->RecLevel = CalculateBaseFormula(baseValue,dbValue,rndItem->minreclevel,rndItem->maxreclevel);

	itmStruct->Range = (uint8)CalculateBaseFormula(baseValue,dbValue,rndItem->minrange,rndItem->maxrange);
	itmStruct->Damage = (uint8)CalculateBaseFormula(baseValue,dbValue,rndItem->mindamage,rndItem->maxdamage);

	itmStruct->Color = (uint32)CalculateBaseFormula(baseValue,dbValue,(uint32)rndItem->mincolor,(uint32)rndItem->maxcolor);

	itmStruct->Classes = rndItem->classes;
	itmStruct->Races = rndItem->races;

	itmStruct->MaxCharges = (sint16)CalculateBaseFormula(baseValue,dbValue,rndItem->minmaxcharges,rndItem->maxmaxcharges);

	itmStruct->ItemType = rndItem->itemtype;
	itmStruct->Material = rndItem->material;

	itmStruct->SellRate = (float)CalculateBaseFormula(baseValue,dbValue,rndItem->minsellrate,rndItem->maxsellrate);

	itmStruct->CastTime = (sint16)CalculateBaseFormula(baseValue,dbValue,rndItem->mincasttime,rndItem->maxcasttime);

	itmStruct->EliteMaterial = (uint32)rndItem->elitematerial;

	itmStruct->ProcRate = rndItem->procrate;
	//itmStruct->CombatEffects
	//itmStruct->Shielding
	//itmStruct->StunResist
	//itmStruct->Strikethrough
	//itmStruct->SpellShield

	// CharmFileID is used in random items to reference the itemid in the randomitemtemplate
	//itmStruct->CharmFileID = rndItem->itemid;
	itmStruct->CharmFileID = rndItem->charmfileid;
	strcpy(itmStruct->CharmFile,rndItem->charmfile);

	itmStruct->AugType = rndItem->augtype;

	itmStruct->AugSlotType[0] = rndItem->augslot1type;
	itmStruct->AugSlotType[1] = rndItem->augslot2type;
	itmStruct->AugSlotType[2] = rndItem->augslot3type;
	itmStruct->AugSlotType[3] = rndItem->augslot4type;
	itmStruct->AugSlotType[4] = rndItem->augslot5type;
	itmStruct->AugSlotVisible[0] = rndItem->augslots1visible;
	itmStruct->AugSlotVisible[1] = rndItem->augslots2visible;
	itmStruct->AugSlotVisible[2] = rndItem->augslots3visible;
	itmStruct->AugSlotVisible[3] = rndItem->augslots4visible;
	itmStruct->AugSlotVisible[4] = rndItem->augslots5visible;

	itmStruct->BagSize = rndItem->bagsize;
	itmStruct->BagSlots = rndItem->bagslots;
	itmStruct->BagType = rndItem->bagtype;
	itmStruct->BagWR = (uint8)CalculateBaseFormula(baseValue,dbValue,rndItem->minbagwr,rndItem->maxbagwr);

	itmStruct->Book = rndItem->book;
	itmStruct->BookType = rndItem->booktype;
	strcpy(itmStruct->Filename,rndItem->filename);

	itmStruct->BaneDmgRaceAmt = (uint32)CalculateBaseFormula(baseValue,dbValue,rndItem->minbanedmgraceamt,rndItem->maxbanedmgraceamt);

	itmStruct->AugRestrict = rndItem->augrestrict;

	itmStruct->LoreGroup = rndItem->loregroup;
	itmStruct->LoreFlag = itmStruct->LoreGroup!=0;

	itmStruct->PendingLoreFlag = rndItem->pendingloreflag;
	itmStruct->ArtifactFlag = rndItem->artifactflag;
	//itmStruct->SummonedFlag

	itmStruct->Favor = CalculateBaseFormula(baseValue,dbValue,rndItem->minfavor,rndItem->maxfavor);

	itmStruct->Endur = (uint32)CalculateBaseFormula(baseValue,dbValue,rndItem->minendurance,rndItem->maxendurance);
	itmStruct->DotShielding = rndItem->dotshielding;
	itmStruct->Attack = (uint32)CalculateBaseFormula(baseValue,dbValue,rndItem->minattack,rndItem->maxattack);
	itmStruct->Regen = (uint32)CalculateBaseFormula(baseValue,dbValue,rndItem->minregen,rndItem->maxregen);
	itmStruct->ManaRegen = (uint32)CalculateBaseFormula(baseValue,dbValue,rndItem->minmanaregen,rndItem->maxmanaregen);
	itmStruct->EnduranceRegen = (uint32)CalculateBaseFormula(baseValue,dbValue,rndItem->minenduranceregen,rndItem->maxenduranceregen);
	itmStruct->Haste = (uint32)CalculateBaseFormula(baseValue,dbValue,rndItem->minhaste,rndItem->maxhaste);

	itmStruct->DamageShield = rndItem->damageshield;
	itmStruct->RecastDelay = rndItem->recastdelay;
	itmStruct->RecastType = rndItem->recasttype;

	itmStruct->GuildFavor = CalculateBaseFormula(baseValue,dbValue,rndItem->minguildfavor,rndItem->maxguildfavor);

	itmStruct->AugDistiller = rndItem->augdistiller;
	itmStruct->Attuneable = rndItem->attuneable;
	itmStruct->NoPet = rndItem->nopet;
	itmStruct->PointType = rndItem->pointtype;
	itmStruct->PotionBelt = rndItem->potionbelt;
	itmStruct->PotionBeltSlots = rndItem->potionbeltslots;
	itmStruct->StackSize = rndItem->stacksize;
	itmStruct->NoTransfer = rndItem->notransfer;
	itmStruct->Stackable = rndItem->stackable;

	itmStruct->Click.Effect = rndItem->Click.Effect;
	itmStruct->Click.Type = rndItem->Click.Type;
	itmStruct->Click.Level = rndItem->Click.Level;
	itmStruct->Click.Level2 = rndItem->Click.Level2;

	itmStruct->Proc.Effect = rndItem->Proc.Effect;
	itmStruct->Proc.Type = rndItem->Proc.Type;
	itmStruct->Proc.Level = rndItem->Proc.Level;
	itmStruct->Proc.Level2 = rndItem->Proc.Level2;

	itmStruct->Worn.Effect = rndItem->Worn.Effect;
	itmStruct->Worn.Type = rndItem->Worn.Type;
	itmStruct->Worn.Level = rndItem->Worn.Level;
	itmStruct->Worn.Level2 = rndItem->Worn.Level2;

	itmStruct->Focus.Effect = rndItem->Focus.Effect;
	itmStruct->Focus.Type = rndItem->Focus.Type;
	itmStruct->Focus.Level = rndItem->Focus.Level;
	itmStruct->Focus.Level2 = rndItem->Focus.Level2;

	itmStruct->Scroll.Effect = rndItem->Scroll.Effect;
	itmStruct->Scroll.Type = rndItem->Scroll.Type;
	itmStruct->Scroll.Level = rndItem->Scroll.Level;
	itmStruct->Scroll.Level2 = rndItem->Scroll.Level2;

	itmStruct->Bard.Type = rndItem->Bard.Type;
	itmStruct->Bard.Effect = rndItem->Bard.Effect;
	itmStruct->Bard.Level = rndItem->Bard.Level;
	itmStruct->Bard.Level2 = rndItem->Bard.Level2;

	//itmStruct->BackstabDmg

	itmStruct->SpellDmg =  (sint32)CalculateBaseFormula(baseValue,dbValue,rndItem->minspelldmg,rndItem->maxspelldmg);
	itmStruct->ScriptFileID = rndItem->scriptfileid;
	itmStruct->ExpendableArrow = rndItem->expendablearrow;
	itmStruct->Clairvoyance = rndItem->clairvoyance;

	strcpy(itmStruct->ClickName,rndItem->clickname);
	strcpy(itmStruct->ProcName,rndItem->procname);
	strcpy(itmStruct->WornName,rndItem->wornname);
	strcpy(itmStruct->FocusName,rndItem->focusname);
	strcpy(itmStruct->ScrollName,rndItem->scrollname);

	itmStruct->Weight = rndItem->weight;

	return true;
}

uint32 SharedDatabase::CalculateOppositeBaseFormula(float baseValue,float dbValue, uint32 min, uint32 max)
{
	uint32 resultValue = 0;
	uint32 newMinValue = 0;
	float inverseBase = 1.0f - baseValue;
	bool rndReq = true;

	uint32 newMaxValue = max;
	if ( max < 1 )
	{
		if ( min < 1 )
			return 0;

		resultValue = min;
		rndReq = false;
	}
	else
	{
		if ( baseValue > dbValue )
		{
			newMaxValue = max - ( ( ( max - min ) * inverseBase ) );
		}
		if ( baseValue > dbValue )
			newMinValue = MakeRandomInt(min, (int) min + ( (dbValue*inverseBase) * ( max - min ) ));
		else
			newMinValue = min + ( inverseBase * ( max - min ) );
	}

	if ( rndReq )
	{
		resultValue = MakeRandomInt(newMinValue,newMaxValue);
	}

	return resultValue;
}

uint32 SharedDatabase::CalculateBaseFormula(float baseValue,float dbValue, uint32 min, uint32 max)
{
	uint32 resultValue = 0;
	uint32 newMinValue = 0;
	float inverseBase = 1.0f - baseValue;
	bool rndReq = true;

	uint32 newMaxValue = max;
	if ( max < 1 )
	{
		if ( min < 1 )
			return 0;

		resultValue = min;
		rndReq = false;
	}
	else
	{
		if ( dbValue > baseValue )
		{
			newMaxValue = max - ( ( max - min ) * inverseBase);
		}
		newMinValue = min + ( baseValue * ( max - min ) );
	}

	if ( rndReq )
	{
		newMinValue = MakeRandomInt(min,newMinValue);
		resultValue = MakeRandomInt(newMinValue,newMaxValue);
	}

	return resultValue;
}

sint16 SharedDatabase::CalculateBaseFormula(float baseValue,float dbValue, sint16 min, sint16 max)
{
	return (sint16)CalculateBaseFormula(baseValue,dbValue,(sint32)min,(sint32)max);
}
sint32 SharedDatabase::CalculateBaseFormula(float baseValue,float dbValue, sint32 min, sint32 max)
{
	sint32 resultValue = 0;
	sint32 newMinValue = 0;
	float inverseBase = 1.0f - baseValue;
	bool rndReq = true;

	sint32 newMaxValue = max;
	if ( max < min )
	{
		if ( min == 0 )
			return 0;

		resultValue = min;
		rndReq = false;
	}
	else
	{
		if ( dbValue > baseValue )
		{
			newMaxValue = max - ( ( max - min ) * inverseBase);
		}
		newMinValue = min + ( baseValue * ( max - min ) );
	}

	if ( rndReq )
	{
		newMinValue = MakeRandomInt(min,newMinValue);
		resultValue = MakeRandomInt(newMinValue,newMaxValue);
	}

	return resultValue;
}

float SharedDatabase::CalculateBaseFormula(float baseValue,float dbValue, float min, float max)
{
	float resultValue = 0;
	float newMinValue = 0;
	float newMaxValue = 0;
	bool rndReq = true;

	float inverseBase = 1.0f - baseValue;

	if ( max < min )
	{
		if ( min == 0 )
			return 0;

		resultValue = min;
		rndReq = false;
	}
	else
	{
		if ( dbValue > baseValue )
		{
			newMaxValue = max - ( ( max - min ) * inverseBase);
		}
		newMinValue = min + ( baseValue * ( max - min ) );
	}

	if ( rndReq )
	{
		newMinValue = MakeRandomInt(min,newMinValue);
		resultValue = MakeRandomInt(newMinValue,newMaxValue);
	}

	return resultValue;
}

uint8 SharedDatabase::CalculateBaseFormula(float baseValue,float dbValue,uint8 min, uint8 max)
{
	return (uint8)CalculateBaseFormula(baseValue, dbValue, (uint32)min, (uint32)max);
}

bool SharedDatabase::GetExistingRandomDropID(uint8 typeID, uint32 checksum, uint32 itemTemplateID, uint32* itemID) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	MYSQL_RES *result;
	MYSQL_ROW row;
	uint32 itemDBID = 0;
	if ( itemID == NULL )
		return false;

	char* query = 0;
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT id FROM items where templatehash='Random%i %i' and templateid=%i order by rand() limit 1"
		, typeID, checksum,itemTemplateID, typeID, itemTemplateID), errbuf, &result)) {
			safe_delete_array(query);
			if (mysql_num_rows(result) == 1)
			{
				row = mysql_fetch_row(result);
				if (row != NULL) {
					itemDBID = atoi(row[0]);
					*itemID = itemDBID;
				}

				mysql_free_result(result);

				if ( itemDBID>0 )
					return true;
			}
	}
	else {
		cerr << "Error in GetExistingRandomDropID query '" << query << "' " << errbuf << endl;
	}

	safe_delete_array(query);

	return false;
}


bool SharedDatabase::extDBZoneNames(sint32 iZoneCount, int32 iMaxZoneID) {
	return s_usedb->DBLoadZoneNames(iZoneCount, iMaxZoneID);
}

bool SharedDatabase::QueryZoneNames(sint32 count, uint32* maxID) {
	if (!EMuShareMemDLL.Load())
		return false;

	if (maxID == NULL || *maxID < 0) {
		cout << "Error: SharedDatabase::LoadItems() (sharemem): GetItemsCount() returned -1" << endl;
		return false;
	}
	bool ret = EMuShareMemDLL.ZoneNames.DLLLoadZoneNames(&extDBZoneNames, sizeof(ZoneShortName_Struct), &count, maxID);
	return ret;
}

bool SharedDatabase::LoadZoneNames() {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	query = new char[256];
	strcpy(query, "SELECT count(id),MAX(zoneidnumber) FROM zone");

	if (RunQuery(query, strlen(query), errbuf, &result)) {
		safe_delete_array(query);
		row = mysql_fetch_row(result);
		if (row && row[0])
		{
			sint32 count = atoi(row[0]);
			max_zonename = atoi(row[1]);

			// load list
			QueryZoneNames(count, &max_zonename);

			mysql_free_result(result);
		}
	}
	else {
		cerr << "Error in LoadZoneNames query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}
	return true;
}


bool SharedDatabase::DBLoadZoneNames(sint32 iZoneCount, uint32 iMaxZoneID) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	query = new char[256];
	MakeAnyLenString(&query, "SELECT id, zoneidnumber, short_name FROM zone order by zoneidnumber,id asc");
	if (RunQuery(query, strlen(query), errbuf, &result)) {
		safe_delete_array(query);
		uint32 lastZoneID = 0;
		while((row = mysql_fetch_row(result))) {
			//strcpy(zonename_array[atoi(row[0])], row[1]);
			ZoneShortName_Struct zsn;
			memset(&zsn,0,sizeof(ZoneShortName_Struct));

			zsn.dbID = (int32)atoul(row[0]);
			zsn.zoneID = (int32)atoul(row[1]);
			strncpy(zsn.zoneShortName,row[2],32); // db is set for 32 chars

			if ( zsn.zoneID <= lastZoneID ) // got a response
			{
				// Chances are we already loaded a zone with this zone id, so we need to make it an instance so its unique, copy the db id.
				zsn.instanceID = zsn.dbID;
			}

			lastZoneID = zsn.zoneID;

			if (!EMuShareMemDLL.ZoneNames.cbAddZoneName(zsn.dbID, &zsn)) {
				LogFile->write(EQEMuLog::Error, "Database::DBLoadZoneNames: Failure reported from EMuShareMemDLL.ZoneNames.cbAddZoneName(%i)", zsn.dbID);
				break;
			}
		}
		mysql_free_result(result);
		return true;
	}
	else {
		cerr << "Error in LoadZoneNames query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}

	return false;
}

const ZoneShortName_Struct* SharedDatabase::IterateZoneNames(uint32* NextIndex) {
	return EMuShareMemDLL.ZoneNames.IterateZoneNames(NextIndex);
}

const RandomLootDrop_Struct* SharedDatabase::IterateRandomLootDrops(uint32* NextIndex) {
	return EMuShareMemDLL.RandomLootDrops.IterateItems(NextIndex);
}

int32 SharedDatabase::GetZoneID(const char* zonename) {

	const ZoneShortName_Struct* zns = 0;
	uint32 it = 0;

	while ((zns = IterateZoneNames(&it))) {
		if ( strcasecmp(zns->zoneShortName, zonename) == 0 )
			return zns->zoneID;
	}

	return 0;
}



const ZoneShortName_Struct* SharedDatabase::GetZoneInfo(const char* zonename) {

	const ZoneShortName_Struct* zns = 0;
	uint32 it = 0;

	while ((zns = IterateZoneNames(&it))) {
		if ( strcasecmp(zns->zoneShortName, zonename) == 0 )
			return zns;
	}

	return 0;
}

const char* SharedDatabase::GetZoneName(int32 zoneID, int32 instanceID, bool ErrorUnknown, bool ReturnStaticZone) {
	// return "UNKNOWN"; if ErrorUnknown is true

	const ZoneShortName_Struct* zns = 0;
	uint32 it = 0;

	const char* tmpZoneName = NULL;
	while ((zns = IterateZoneNames(&it))) {

		// Kings & Bandits got a zone id without instance
		if ( zns->zoneID == zoneID )
			tmpZoneName = zns->zoneShortName;

		if ( instanceID > 0 && zns->instanceID != instanceID )
			continue;

		if ( zns->zoneID == zoneID )
			return zns->zoneShortName;
	}

	// Kings & Bandits got a zone id without instance
	if ( ReturnStaticZone && tmpZoneName != NULL )
		return tmpZoneName;

	if ( ErrorUnknown )
		return "UNKNOWN";
	else
		return 0;
}

int32 SharedDatabase::GetZoneIDByDBID(int32 dbID) {
	// return "UNKNOWN"; if ErrorUnknown is true

	const ZoneShortName_Struct* zns = EMuShareMemDLL.ZoneNames.GetItem(dbID);

	if ( zns != NULL )
		return zns->zoneID;
	else
		return 0;
}





// Store new character information into the character_ and inventory tables
bool SharedDatabase::StoreCharacter(uint32 account_id, PlayerProfile_Struct* pp, Inventory* inv, ExtendedProfile_Struct *ext, uint32* characterID)
{
	_CP(Database_StoreCharacter);
	char errbuf[MYSQL_ERRMSG_SIZE];
	char query[256+sizeof(PlayerProfile_Struct)*2+sizeof(ExtendedProfile_Struct)*2+5];
	char* end = query;
	int32 affected_rows = 0;
	int i;
	int32 charid = 0;
	char* charidquery = 0;
	char* invquery = 0;
	MYSQL_RES *result;
	MYSQL_ROW row = 0;
	char zone[50];
	float x, y, z;

	//	memset(&playeraa, 0, sizeof(playeraa));

	// get the char id (used in inventory inserts below)
	if(!RunQuery
		(
		charidquery,
		MakeAnyLenString
		(
		&charidquery,
		"SELECT id FROM character_ where name='%s'",
		pp->name
		),
		errbuf,
		&result
		)) {
			LogFile->write(EQEMuLog::Error, "Error in char store id query: %s: %s", charidquery, errbuf);
			return(false);
	}
	safe_delete_array(charidquery);

	if(mysql_num_rows(result) == 1)
	{
		row = mysql_fetch_row(result);
		if(row[0])
		{
			charid = atoi(row[0]);

			// Kings & Bandits - return character id if ptr is set
			if ( characterID )
				*characterID = charid;
		}
	}

	if(!charid)
	{
		LogFile->write(EQEMuLog::Error, "StoreCharacter: no character id");
		return false;
	}

	const char *zname = GetZoneName(pp->zone_id);
	if(zname == NULL) {
		//zone not in the DB, something to prevent crash...
		strn0cpy(zone, "qeynos", 49);
		pp->zone_id = 1;
	} else
		strn0cpy(zone, zname, 49);
	x=pp->x;
	y=pp->y;
	z=pp->z;

	// construct the character_ query
	end += sprintf(end,
		"UPDATE character_ SET timelaston=0, "
		"zonename=\'%s\', x=%f, y=%f, z=%f, instanceid=%i, profile=\'",
		zone, x, y, z, pp->zoneInstance
		);
	end += DoEscapeString(end, (char*)pp, sizeof(PlayerProfile_Struct));
	end += sprintf(end, "\', extprofile=\'");
	end += DoEscapeString(end, (char*)ext, sizeof(ExtendedProfile_Struct));
	end += sprintf(end, "\' WHERE account_id=%d AND name='%s'",account_id, pp->name);

	RunQuery(query, (int32) (end - query), errbuf, 0, &affected_rows);

	if(!affected_rows)
	{
		LogFile->write(EQEMuLog::Error, "StoreCharacter query '%s' %s", query, errbuf);
		return false;
	}

	affected_rows = 0;


	// Doodman: Is this even used?
	// now the inventory

	for (i=0; i<=2270;)
	{
		const ItemInst* newinv = inv->GetItem((sint16)i);
		if (newinv)
		{
			MakeAnyLenString
				(
				&invquery,
				"INSERT INTO inventory SET "
				"charid=%0u, slotid=%0d, itemid=%0u, charges=%0d, color=%0u",
				charid, i, newinv->GetItem()->ID,
				newinv->GetCharges(), newinv->GetColor()
				);

			RunQuery(invquery, strlen(invquery), errbuf, 0, &affected_rows);
			if(!affected_rows)
			{
				LogFile->write(EQEMuLog::Error, "StoreCharacter inventory failed.  Query '%s' %s", invquery, errbuf);
			}
			safe_delete_array(invquery);
#if EQDEBUG >= 9
			else
			{
				LogFile->write(EQEMuLog::Debug, "StoreCharacter inventory succeeded.  Query '%s' %s", invquery, errbuf);
			}
#endif
		}

		if(i==30){ //end of standard inventory/cursor, jump to internals of bags/cursor
			i = 251;
			continue;
		} else if(i==340){ //end of internals of bags/cursor, jump to bank slots
			i = 2000;
			continue;
		} else if(i==2023){ //end of bank slots, jump to internals of bank bags
			i = 2031;
			continue;
		}

		i++;
	}

	return true;
}

bool SharedDatabase::MoveCharacterToZone(const char* charname, const char* zonename) {
	return MoveCharacterToZone(charname, zonename, GetZoneID(zonename));
}

bool SharedDatabase::MoveCharacterToZone(int32 iCharID, const char* iZonename) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	int32	affected_rows = 0;
	if (!RunQuery(query, MakeAnyLenString(&query, "UPDATE character_ SET zonename = '%s', zoneid=%i, x=-1, y=-1, z=-1 WHERE id=%i", iZonename, GetZoneID(iZonename), iCharID), errbuf, 0,&affected_rows)) {
		cerr << "Error in MoveCharacterToZone(id) query '" << query << "' " << errbuf << endl;
		return false;
	}
	safe_delete_array(query);

	if (affected_rows == 0)
		return false;

	return true;
}

int8 SharedDatabase::CopyCharacter(const char* oldname, const char* newname, int32 acctid) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	PlayerProfile_Struct* pp;
	ExtendedProfile_Struct* ext;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT profile, guild, guildrank, extprofile FROM character_ WHERE name='%s'", oldname), errbuf, &result)) {
		safe_delete_array(query);

		row = mysql_fetch_row(result);

		pp = (PlayerProfile_Struct*)row[0];
		strcpy(pp->name, newname);

		ext = (ExtendedProfile_Struct*)row[3];

		mysql_free_result(result);
	}

	else {
		cerr << "Error in CopyCharacter read query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return 0;
	}

	int32 affected_rows = 0;
	char query2[276 + sizeof(PlayerProfile_Struct)*2 + sizeof(ExtendedProfile_Struct)*2 + 1];
	char* end=query2;

	end += sprintf(end, "INSERT INTO character_ SET zonename=\'%s\', x = %f, y = %f, z = %f, profile=\'", GetZoneName(pp->zone_id), pp->x, pp->y, pp->z);
	end += DoEscapeString(end, (char*) pp, sizeof(PlayerProfile_Struct));
	end += sprintf(end,"\', extprofile=\'");
	end += DoEscapeString(end, (char*) ext, sizeof(ExtendedProfile_Struct));
	end += sprintf(end, "\', account_id=%d, name='%s'", acctid, newname);

	if (!RunQuery(query2, (int32) (end - query2), errbuf, 0, &affected_rows)) {
		cerr << "Error in CopyCharacter query '" << query << "' " << errbuf << endl;
		return 0;
	}

	// @merth: Need to copy inventory as well (and shared bank?)
	if (affected_rows == 0) {
		return 0;
	}

	return 1;
}


int32 SharedDatabase::GetCharacterInfo(const char* iName, int32* oAccID, int32* oZoneID, int32* oInstanceID, float* oX, float* oY, float* oZ, uint8* pvpFlag) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	
	int len = strlen(iName)
	char* char_name = new char[2*len+1];
	DoEscapeString(char_name, iName, len);

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT id, account_id, zonename, instanceid, x, y, z, pvpflag FROM character_ WHERE name='%s'", char_name), errbuf, &result)) {
		safe_delete_array(query);
		safe_delete_array(char_name);
		if (mysql_num_rows(result) == 1) {
			row = mysql_fetch_row(result);
			int32 charid = atoi(row[0]);
			if (oAccID)
				*oAccID = atoi(row[1]);
			if (oZoneID)
				*oZoneID = GetZoneID(row[2]);
			if(oInstanceID)
				*oInstanceID = atoi(row[3]);
			if (oX)
				*oX = atof(row[4]);
			if (oY)
				*oY = atof(row[5]);
			if (oZ)
				*oZ = atof(row[6]);
			if (pvpFlag)
				*pvpFlag = (uint8)atoul(row[7]);

			mysql_free_result(result);
			return charid;
		}
		mysql_free_result(result);
	}
	else {
		cerr << "Error in GetCharacterInfo query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		safe_delete_array(char_name);
	}
	return 0;
}


int16 SharedDatabase::GetInstanceID(const char* zone, int32 charid, int16 version)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	int16 ret;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT instance_lockout.id FROM instance_lockout, instance_lockout_player "
		"WHERE instance_lockout.zone=%u AND instance_lockout.version=%u AND instance_lockout.id=instance_lockout_player.id AND "
		"instance_lockout_player.charid=%u LIMIT 1;", GetZoneID(zone), version, charid, charid), errbuf, &result))
	{
		safe_delete_array(query);
		if (mysql_num_rows(result) != 0) 
		{
			row = mysql_fetch_row(result);
			ret = atoi(row[0]);
			mysql_free_result(result);
			return ret;		
		}
		else
		{
			mysql_free_result(result);
			return 0;
		}
	}
	else 
	{
		safe_delete_array(query);
		return 0;
	}
	return 0;
}


int16 SharedDatabase::GetInstanceID(int32 zone, int32 charid, int16 version)
{
	if(!zone)
		return 0;

	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	int16 ret;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT instance_lockout.id FROM instance_lockout, instance_lockout_player "
		"WHERE instance_lockout.zone=%u AND instance_lockout.version=%u AND instance_lockout.id=instance_lockout_player.id AND "
		"instance_lockout_player.charid=%u LIMIT 1;", zone, version, charid), errbuf, &result))
	{
		safe_delete_array(query);
		if (mysql_num_rows(result) != 0) 
		{
			row = mysql_fetch_row(result);
			ret = atoi(row[0]);
			mysql_free_result(result);
			return ret;		
		}
		else
		{
			mysql_free_result(result);
			return 0;
		}
	}
	else 
	{
		safe_delete_array(query);
		return 0;
	}
	return 0;
}


// Pyro: Get zone starting points from DB
bool SharedDatabase::GetSafePoints(const char* short_name, int32 version, float* safe_x, float* safe_y, float* safe_z, sint16* minstatus, int8* minlevel, char *flag_needed) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	//	int buf_len = 256;
	//    int chars = -1;
	MYSQL_RES *result;
	MYSQL_ROW row;

	if (RunQuery(query, MakeAnyLenString(&query,
		"SELECT safe_x, safe_y, safe_z, min_status, min_level, "
		" flag_needed FROM zone "
		" WHERE short_name='%s' AND (version=%i OR version=0) ORDER BY version DESC", short_name, version), errbuf, &result)) {
			safe_delete_array(query);
			if (mysql_num_rows(result) > 0) {
				row = mysql_fetch_row(result);
				if (safe_x != 0)
					*safe_x = atof(row[0]);
				if (safe_y != 0)
					*safe_y = atof(row[1]);
				if (safe_z != 0)
					*safe_z = atof(row[2]);
				if (minstatus != 0)
					*minstatus = atoi(row[3]);
				if (minlevel != 0)
					*minlevel = atoi(row[4]);
				if (flag_needed != NULL)
					strcpy(flag_needed, row[5]);
				mysql_free_result(result);
				return true;
			}

			mysql_free_result(result);
	}
	else
	{
		cerr << "Error in GetSafePoint query '" << query << "' " << errbuf << endl;
		cerr << "If it errors, run the following querys:\n";
		cerr << "ALTER TABLE `zone` CHANGE `minium_level` `min_level` TINYINT(3)  UNSIGNED DEFAULT \"0\" NOT NULL;\n";
		cerr << "ALTER TABLE `zone` CHANGE `minium_status` `min_status` TINYINT(3)  UNSIGNED DEFAULT \"0\" NOT NULL;\n";
		cerr << "ALTER TABLE `zone` ADD flag_needed VARCHAR(128) NOT NULL DEFAULT '';\n";

		safe_delete_array(query);
	}
	return false;
}


void SharedDatabase::AssignGroupToInstance(int32 gid, int32 instance_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	int32 zone_id = ZoneIDFromInstanceID(instance_id);
	int16 version = VersionFromInstanceID(instance_id);

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT charid FROM group_id WHERE groupid=%u", gid), 
		errbuf, &result))
	{
		safe_delete_array(query);
		while((row = mysql_fetch_row(result)) != NULL)
		{
			int32 charid = atoi(row[0]);
			if(GetInstanceID(zone_id, charid, version) == 0)
			{
				AddClientToInstance(instance_id, charid);
			}
		}
		mysql_free_result(result);
	}
	else 
	{
		safe_delete_array(query);
	}
}

void SharedDatabase::AssignRaidToInstance(int32 rid, int32 instance_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	int32 zone_id = ZoneIDFromInstanceID(instance_id);
	int16 version = VersionFromInstanceID(instance_id);

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT charid FROM raid_members WHERE raidid=%u", rid), 
		errbuf, &result))
	{
		safe_delete_array(query);
		while((row = mysql_fetch_row(result)) != NULL)
		{
			int32 charid = atoi(row[0]);
			if(GetInstanceID(zone_id, charid, version) == 0)
			{
				AddClientToInstance(instance_id, charid);
			}
		}
		mysql_free_result(result);
	}
	else 
	{
		safe_delete_array(query);
	}
}

void SharedDatabase::FlagInstanceByGroupLeader(int32 zone, int16 version, int32 charid, int32 gid)
{
	int16 id = GetInstanceID(zone, charid, version);
	if(id != 0)
		return;

	char ln[128];
	memset(ln, 0, 128);
	strcpy(ln, GetGroupLeadershipInfo(gid, ln));
	int32 l_charid = GetCharacterID((const char*)ln);
	int16 l_id = GetInstanceID(zone, l_charid, version);

	if(l_id == 0)
		return;

	AddClientToInstance(l_id, charid);
}

void SharedDatabase::FlagInstanceByRaidLeader(int32 zone, int16 version, int32 charid, int32 rid)
{
	int16 id = GetInstanceID(zone, charid, version);
	if(id != 0)
		return;

	int32 l_charid = GetCharacterID(GetRaidLeaderName(rid));
	int16 l_id = GetInstanceID(zone, l_charid, version);

	if(l_id == 0)
		return;

	AddClientToInstance(l_id, charid);
}


bool SharedDatabase::MoveCharacterToZone(const char* charname, const char* zonename,int32 zoneid) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	int32	affected_rows = 0;

	if(zonename == NULL || strlen(zonename) == 0)
		return(false);

	if (!RunQuery(query, MakeAnyLenString(&query, "UPDATE character_ SET zonename = '%s',zoneid=%i,x=-1, y=-1, z=-1 WHERE name='%s'", zonename,zoneid, charname), errbuf, 0,&affected_rows)) {
		cerr << "Error in MoveCharacterToZone(name) query '" << query << "' " << errbuf << endl;
		return false;
	}
	safe_delete_array(query);

	if (affected_rows == 0)
		return false;

	return true;
}

bool SharedDatabase::logevents(const char* accountname,int32 accountid,int8 status,const char* charname, const char* target,const char* descriptiontype, const char* description,int event_nid){
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	uint32 len = strlen(description);
	uint32 len2 = strlen(target);
	char* descriptiontext = new char[2*len+1];
	char* targetarr = new char[2*len2+1];
	memset(descriptiontext, 0, 2*len+1);
	memset(targetarr, 0, 2*len2+1);
	DoEscapeString(descriptiontext, description, len);
	DoEscapeString(targetarr, target, len2);
	if (!RunQuery(query, MakeAnyLenString(&query, "Insert into eventlog (accountname,accountid,status,charname,target,descriptiontype,description,event_nid) values('%s',%i,%i,'%s','%s','%s','%s','%i')", accountname,accountid,status,charname,targetarr,descriptiontype,descriptiontext,event_nid), errbuf))	{
		cerr << "Error in logevents" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}
	safe_delete_array(query);
	safe_delete_array(descriptiontext);
	safe_delete_array(targetarr);
	return true;
}



void SharedDatabase::InsertCharacterStats(uint32 charid)
{	
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	if (!RunQuery(query, MakeAnyLenString(&query, "insert into characterdb_stats set charid=%i", charid), errbuf))	{
		//cout << "Error in SharedDatabase::InsertClearPVPStats characterdb_stats " << query << "' " << errbuf << endl;
		safe_delete_array(query);
		//return false;
	}

	if (!RunQuery(query, MakeAnyLenString(&query, "insert into characterdb_pvpstats set charid=%i", charid), errbuf))	{
		//	cout << "Error in SharedDatabase::InsertClearPVPStats characterdb_pvpstats " << query << "' " << errbuf << endl;
		safe_delete_array(query);
		//	return false;
	}
	safe_delete_array(query);
	//return true;
}

void SharedDatabase::UpdateCharacterPVPStats(uint32 charid, uint32 pvpkills, uint32 pvpdeaths, uint32 pvpcurpts, uint32 pvpcareerpts, uint32 bestkillstreak, 
	uint32 worstdeathstreak, uint32 curkillstreak, uint32 killsinday, char* lastkillname, char* lastdeathname)
{	
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	if (!RunQuery(query, MakeAnyLenString(&query, "update characterdb_pvpstats set pvpkills=%i,pvpdeaths=%i,pvpcurpts=%i,pvpcareerpts=%i,pvpbestkillstreak=%i,"
		"pvpworstdeathstreak=%i,pvpcurrentkillstreak=%i,killsinlastday=%i,lastkillname='%s',lastdeathname='%s' where charid=%i", 
		pvpkills, pvpdeaths, pvpcurpts, pvpcareerpts, bestkillstreak, worstdeathstreak, curkillstreak, killsinday, lastkillname, lastdeathname, charid), errbuf))	{
			//cout << "Error in SharedDatabase::UpdateCharacterPVPStats" << query << "' " << errbuf << endl;
			safe_delete_array(query);
			//	return false;
	}
	safe_delete_array(query);
	//return true;
}

void SharedDatabase::AddCharacterPVPKill(uint32 charidattacker, uint32 charidkilled, uint32 ptsearned, char* attackerName, char* killedName, char* zoneShortName)
{	
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	if (!RunQuery(query, MakeAnyLenString(&query, "insert into characterdb_pvpkills set charidattacker=%i,charidkilled=%i,ptsearned=%i,attackername='%s',killedname='%s',zoneshortname='%s'", 
		charidattacker, charidkilled, ptsearned, attackerName, killedName, zoneShortName ), errbuf))	{
			//cerr << "Error in SharedDatabase::UpdateCharacterPVPStats" << query << "' " << errbuf << endl;
			safe_delete_array(query);
			//return false;
	}
	safe_delete_array(query);
	//return true;
}

void SharedDatabase::UpdateCharacterStats(uint32 charid, uint32 astr, uint32 asta, uint32 adex, uint32 aagi, uint32 aint, uint32 awis, uint32 acha, uint32 hp, uint32 maxhp, 
	uint32 mana, uint32 maxmana, uint32 endurance, uint32 maxendurance, uint32 timeplayed, uint8 ischaronline)
{	
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	if (!RunQuery(query, MakeAnyLenString(&query, "update characterdb_stats set astr=%i,asta=%i,adex=%i,aagi=%i,aint=%i,awis=%i,acha=%i,hp=%i,maxhp=%i,mana=%i,maxmana=%i,endurance=%i,maxendurance=%i,timeplayedminutes=%i,ischaronline=%i,lastzonetime=CURRENT_TIMESTAMP where charid=%i", 
		astr, asta, adex, aagi, aint, awis, acha, hp, maxhp, mana, maxmana, endurance, maxendurance, timeplayed, ischaronline, charid), errbuf))	{
			//cout << "Error in SharedDatabase::UpdateCharacterPVPStats" << query << "' " << errbuf << endl;
			safe_delete_array(query);
			//	return false;
	}
	safe_delete_array(query);
	//return true;
}