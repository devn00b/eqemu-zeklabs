/*  EQEMu:  Everquest Server Emulator
	Copyright (C) 2001-2003  EQEMu Development Team (http://eqemulator.net)

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
#include "../common/rulesys.h"
#include <iostream>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errmsg.h>
#include <mysqld_error.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>
#include <map>

// Disgrace: for windows compile
#ifdef WIN32
#include <windows.h>
#define snprintf	_snprintf
#define strncasecmp	_strnicmp
#define strcasecmp	_stricmp
#else
#include "unix.h"
#include <netinet/in.h>
#include <sys/time.h>
#endif

#include "database.h"
#include "eq_packet_structs.h"
#include "guilds.h"
#include "MiscFunctions.h"
#include "extprofile.h"
/*#include "../common/classes.h"
#include "../common/races.h"
#include "../common/files.h"
#include "../common/EQEMuError.h"
#include "../common/packet_dump.h"
*/
extern Client client;

/*
This is the amount of time in seconds the client has to enter the zone
server after the world server, or inbetween zones when that is finished
*/

/*
Establish a connection to a mysql database with the supplied parameters

  Added a very simple .ini file parser - Bounce

	Modify to use for win32 & linux - misanthropicfiend
*/
Database::Database ()
{
	DBInitVars();
}

/*
Establish a connection to a mysql database with the supplied parameters
*/

Database::Database(const char* host, const char* user, const char* passwd, const char* database, int32 port)
{
	DBInitVars();
	Connect(host, user, passwd, database, port);
}

bool Database::Connect(const char* host, const char* user, const char* passwd, const char* database, int32 port)
{
	int32 errnum= 0;
	char errbuf[MYSQL_ERRMSG_SIZE];
	if (!Open(host, user, passwd, database, port, &errnum, errbuf))
	{
		LogFile->write(EQEMuLog::Error, "Failed to connect to database: Error: %s", errbuf);
		HandleMysqlError(errnum);

		return false;
	}
	else
	{
		LogFile->write(EQEMuLog::Status, "Using database '%s' at %s:%d",database,host,port);
		return true;
	}
}

void Database::DBInitVars() {

	max_zonename = 0;
	zonename_array = 0;
	varcache_array = 0;
	varcache_max = 0;
	varcache_lastupdate = 0;
}



void Database::HandleMysqlError(int32 errnum) {
/*	switch(errnum) {
		case 0:
			break;
		case 1045: // Access Denied
		case 2001: {
			AddEQEMuError(EQEMuError_Mysql_1405, true);
			break;
		}
		case 2003: { // Unable to connect
			AddEQEMuError(EQEMuError_Mysql_2003, true);
			break;
		}
		case 2005: { // Unable to connect
			AddEQEMuError(EQEMuError_Mysql_2005, true);
			break;
		}
		case 2007: { // Unable to connect
			AddEQEMuError(EQEMuError_Mysql_2007, true);
			break;
		}
	}*/
}

/*

Close the connection to the database
*/
Database::~Database()
{
	unsigned int x;
	if (zonename_array) {
		for (x=0; x<=max_zonename; x++) {
			if (zonename_array[x])
				safe_delete_array(zonename_array[x]);
		}
		safe_delete_array(zonename_array);
	}
	if (varcache_array) {
		for (x=0; x<varcache_max; x++) {
			safe_delete_array(varcache_array[x]);
		}
		safe_delete_array(varcache_array);
	}
}


/*
Check if there is an account with name "name" and password "password"
Return the account id or zero if no account matches.
Zero will also be returned if there is a database error.
*/
int32 Database::CheckLogin(const char* name, const char* password, sint16* oStatus, bool* eqemuLogin) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;

	if(strlen(name) >= 50 || strlen(password) >= 50)
		return(0);

	char tmpUN[100];
	char tmpPW[100];
	DoEscapeString(tmpUN, name, strlen(name));
	DoEscapeString(tmpPW, password, strlen(password));

	if (RunQuery(query, MakeAnyLenString(&query,
		"SELECT id, status, eqemutable FROM account WHERE name='%s' AND password is not null "
		"and length(password) > 0 and (password='%s' or password=MD5('%s'))",
		tmpUN, tmpPW, tmpPW), errbuf, &result)) {
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1)
		{
			row = mysql_fetch_row(result);
			int32 id = atoi(row[0]);
			if (oStatus)
				*oStatus = atoi(row[1]);
			if ( eqemuLogin )
				*eqemuLogin = atoi(row[2]);

			mysql_free_result(result);
			return id;
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
		cerr << "Error in CheckLogin query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}

	return 0;
}


//Lieka:  Get Banned IP Address List - Only return false if the incoming connection's IP address is not present in the banned_ips table.
bool Database::CheckBannedIPs(const char* loginIP)
{
 	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
 	//cout << "Checking against Banned IPs table."<< endl; //Lieka:  Debugging
 	if (RunQuery(query, MakeAnyLenString(&query, "SELECT ip_address FROM Banned_IPs WHERE ip_address='%s'", loginIP), errbuf, &result)) {
 		safe_delete_array(query);
 		if (mysql_num_rows(result) != 0)
 		{
 			//cout << loginIP << " was present in the banned IPs table" << endl; //Lieka:  Debugging
 			mysql_free_result(result);
 			return true;
 		}
 		else
 		{
 			//cout << loginIP << " was not present in the banned IPs table." << endl; //Lieka:  Debugging
 			mysql_free_result(result);
 			return false;
 		}
 		mysql_free_result(result);
 	}
 	else
 	{
 		cerr << "Error in CheckBannedIPs query '" << query << "' " << errbuf << endl;
 		safe_delete_array(query);
 		return true;
 	}
 	return true;
}

bool Database::AddBannedIP(char* bannedIP, const char* notes)
{
 	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;

 	if (!RunQuery(query, MakeAnyLenString(&query, "INSERT into Banned_IPs SET ip_address='%s', notes='%s'", bannedIP, notes), errbuf)) {
 		cerr << "Error in ReserveName query '" << query << "' " << errbuf << endl;
 		safe_delete_array(query);
 		return false;
 	}
 	safe_delete_array(query);
 	return true;
}
 //End Lieka Edit
 
 bool Database::CheckGMIPs(const char* ip_address, int32 account_id) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT * FROM `gm_ips` WHERE `ip_address` = '%s' AND `account_id` = %i", ip_address, account_id), errbuf, &result)) {
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1) {
			mysql_free_result(result);
			return true;
		} else {
			mysql_free_result(result);
			return false;
		}
		mysql_free_result(result);

	} else {
		safe_delete_array(query);
		return false;
	}
	
	return false;
}

bool Database::AddGMIP(char* ip_address, char* name) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;

	if (!RunQuery(query, MakeAnyLenString(&query, "INSERT into `gm_ips` SET `ip_address` = '%s', `name` = '%s'", ip_address, name), errbuf)) {
		safe_delete_array(query);
		return false;
	}
	safe_delete_array(query);
	return true;
}

void Database::LoginIP(int32 AccountID, const char* LoginIP)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;

	if (!RunQuery(query, MakeAnyLenString(&query, "INSERT INTO account_ip SET accid=%i, ip='%s' ON DUPLICATE KEY UPDATE count=count+1, lastused=now()", AccountID, LoginIP), errbuf)) {
 		cerr << "Error in Log IP query '" << query << "' " << errbuf << endl;
 	}
 	safe_delete_array(query);
}

sint16 Database::CheckStatus(int32 account_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT `status`, UNIX_TIMESTAMP(`suspendeduntil`) as `suspendeduntil`, UNIX_TIMESTAMP() as `current`"
						     " FROM `account` WHERE `id` = %i", account_id), errbuf, &result))
	{
		safe_delete_array(query);

		if (mysql_num_rows(result) == 1)
		{
			row = mysql_fetch_row(result);

			sint16 status = atoi(row[0]);
			

			sint32 suspendeduntil = 0;
			
			if ( row[1] != NULL )
				suspendeduntil = atoi(row[1]);

			sint32 current = atoi(row[2]);

			mysql_free_result(result);

			if(suspendeduntil > current)
				return -1;

			return status;
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
		cerr << "Error in CheckStatus query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}

	return 0;
}

int32 Database::CreateAccount(bool eqemuTable,const char* name, const char* password, sint16 status, int32 lsaccount_id) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
	int32 querylen;
	int32 last_insert_id;

	if ( eqemuTable )
	{
	if (password)
		querylen = MakeAnyLenString(&query, "INSERT INTO account SET name='%s', password='%s', status=%i, lsaccount_id=%i, eqemutable=1, time_creation=UNIX_TIMESTAMP();",name,password,status, lsaccount_id);
	else
		querylen = MakeAnyLenString(&query, "INSERT INTO account SET name='%s', status=%i, lsaccount_id=%i, eqemutable=1, time_creation=UNIX_TIMESTAMP();",name, status, lsaccount_id);

	cerr << "Account Attempting to be created:" << name << " " << (sint16) status << endl;
	if (!RunQuery(query, querylen, errbuf, 0, 0, &last_insert_id)) {
		cerr << "Error in (EQEMU)CreateAccount query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return 0;
	}
	safe_delete_array(query);

	if (last_insert_id == 0) {
		cerr << "Error in (EQEMU)CreateAccount query '" << query << "' " << errbuf << endl;
		return 0;
	}
	}
	else
	{
	if (password)
		querylen = MakeAnyLenString(&query, "INSERT INTO account SET name='%s', password='%s', status=%i, lsaccount_id=%i, time_creation=UNIX_TIMESTAMP();",name,password,status, lsaccount_id);
	else
		querylen = MakeAnyLenString(&query, "INSERT INTO account SET name='%s', status=%i, lsaccount_id=%i, time_creation=UNIX_TIMESTAMP();",name, status, lsaccount_id);

	cerr << "Account Attempting to be created:" << name << " " << (sint16) status << endl;
	if (!RunQuery(query, querylen, errbuf, 0, 0, &last_insert_id)) {
		cerr << "Error in CreateAccount query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return 0;
	}
	safe_delete_array(query);

	if (last_insert_id == 0) {
		cerr << "Error in CreateAccount query '" << query << "' " << errbuf << endl;
		return 0;
	}
	}

	return last_insert_id;
}

bool Database::DeleteAccount(const char* name) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
	int32 affected_rows = 0;

	cerr << "Account Attempting to be deleted:" << name << endl;
	if (RunQuery(query, MakeAnyLenString(&query, "DELETE FROM account WHERE name='%s';",name), errbuf, 0, &affected_rows)) {
		safe_delete_array(query);
		if (affected_rows == 1) {
			return true;
		}
	}
	else {

		cerr << "Error in DeleteAccount query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
	}

	return false;
}

bool Database::SetLocalPassword(int32 accid, const char* password) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;

	if (!RunQuery(query, MakeAnyLenString(&query, "UPDATE account SET password=MD5('%s') where id=%i;", password, accid), errbuf)) {
		cerr << "Error in SetLocalPassword query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}

	safe_delete_array(query);
	return true;
}

bool Database::SetAccountStatus(const char* name, sint16 status) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	int32	affected_rows = 0;

	cout << "Account being GM Flagged:" << name << ", Level: " << (sint16) status << endl;
	if (!RunQuery(query, MakeAnyLenString(&query, "UPDATE account SET status=%i WHERE name='%s';", status, name), errbuf, 0, &affected_rows)) {
		safe_delete_array(query);
		return false;
	}
	safe_delete_array(query);

	if (affected_rows == 0) {
		cout << "Account: " << name << " does not exist, therefore it cannot be flagged\n";
		return false;
	}

	return true;
}

bool Database::ReserveName(int32 account_id, char* name)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
	
	// Kings & Bandits fix escape string of name in db for world
	char *escName = (char *)malloc(strlen(name)*2+1);
	DoEscapeString(escName,name,strlen(name));

	if (!RunQuery(query, MakeAnyLenString(&query, "INSERT into character_ SET account_id=%i, name='%s', profile=NULL", account_id, escName), errbuf)) {
		cerr << "Error in ReserveName query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		free(escName);
		return false;
	}
	free(escName);
	safe_delete_array(query);
	return true;
}

/*
Delete the character with the name "name"
returns false on failure, true otherwise
*/
bool Database::DeleteCharacter(char *name)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query=0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	int charid, matches;
	int32 affected_rows;

	if(!name ||	!strlen(name))
	{
		printf("DeleteCharacter: request to delete without a name (empty char slot)\n");
		return false;
	}

// SCORPIOUS2K - get id from character_ before deleting record so we can clean up inventory and qglobal

	// Kings & Bandits fix escape string of name in db for world
	char *escName = (char *)malloc(strlen(name)*2+1);
	DoEscapeString(escName,name,strlen(name));

#if DEBUG >= 4
	printf("DeleteCharacter: Attempting to delete '%s'\n", escName);
#endif
	RunQuery(query, MakeAnyLenString(&query, "SELECT id from character_ WHERE name='%s'", escName), errbuf, &result);
	free(escName);
	if (query)
	{
		safe_delete_array(query);
		query = NULL;
	}
	matches = mysql_num_rows(result);
	if(matches == 1)
	{
		row = mysql_fetch_row(result);
		charid = atoi(row[0]);
#if DEBUG >= 5
		printf("DeleteCharacter: found '%s' with char id: %d\n", name, charid);
#endif
	}
	else
	{
		printf("DeleteCharacter: error: got %d rows matching '%s'\n", matches, name);
		if(result)
		{
			mysql_free_result(result);
			result = NULL;
		}
		return false;
	}

	if(result)
	{
		mysql_free_result(result);
		result = NULL;
	}



#if DEBUG >= 5
	printf("DeleteCharacter: deleting '%s' (id %d): ", name, charid);
	printf(" quest_globals");
#endif
	RunQuery(query, MakeAnyLenString(&query, "DELETE from quest_globals WHERE charid='%d'", charid), errbuf, NULL, &affected_rows);
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}

#if DEBUG >= 5
	printf(" character_tasks");
#endif
	RunQuery(query, MakeAnyLenString(&query, "DELETE from character_tasks WHERE charid='%d'", charid), errbuf, NULL, &affected_rows);
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}

#if DEBUG >= 5
	printf(" character_activities");
#endif
	RunQuery(query, MakeAnyLenString(&query, "DELETE from character_activities WHERE charid='%d'", charid), errbuf, NULL, &affected_rows);
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}

#if DEBUG >= 5
	printf(" character_enabledtasks");
#endif
	RunQuery(query, MakeAnyLenString(&query, "DELETE from character_enabledtasks WHERE charid='%d'", charid), errbuf, NULL, &affected_rows);
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}

#if DEBUG >= 5
	printf(" completed_tasks");
#endif
	RunQuery(query, MakeAnyLenString(&query, "DELETE from completed_tasks WHERE charid='%d'", charid), errbuf, NULL, &affected_rows);
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}

#if DEBUG >= 5
	printf(" friends");
#endif
	RunQuery(query, MakeAnyLenString(&query, "DELETE from friends WHERE charid='%d'", charid), errbuf, NULL, &affected_rows);
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}

#if DEBUG >= 5
	printf(" ptimers");
#endif
	RunQuery(query, MakeAnyLenString(&query, "DELETE from timers WHERE char_id='%d'", charid), errbuf, NULL, &affected_rows);
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}

#if DEBUG >= 5
	printf(" inventory");
#endif
	// Disabled because we want to be able to recover characters
	/*RunQuery(query, MakeAnyLenString(&query, "DELETE from inventory WHERE charid='%d'", charid), errbuf, NULL, &affected_rows);
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}*/

#if DEBUG >= 5
	printf(" guild_members");
#endif
#ifdef BOTS
	RunQuery(query, MakeAnyLenString(&query, "DELETE FROM guild_members WHERE char_id='%d' AND GetMobTypeById(%i) = 'C'", charid), errbuf, NULL, &affected_rows);
#else
	RunQuery(query, MakeAnyLenString(&query, "DELETE FROM guild_members WHERE char_id='%d'", charid), errbuf, NULL, &affected_rows);
#endif
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}

#if DEBUG >= 5
	printf(" recipes");
#endif
	RunQuery(query, MakeAnyLenString(&query, "DELETE FROM char_recipe_list WHERE char_id='%d'", charid), errbuf, NULL, &affected_rows);
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}

#if DEBUG >= 5
	printf(" adventure_stats");
#endif
	RunQuery(query, MakeAnyLenString(&query, "DELETE FROM adventure_stats WHERE player_id='%d'", charid), errbuf, NULL, &affected_rows);
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}

#if DEBUG >= 5
	printf(" zone_flags");
#endif
	RunQuery(query, MakeAnyLenString(&query, "DELETE FROM zone_flags WHERE charID='%d'", charid), errbuf, NULL, &affected_rows);
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}
	
#if DEBUG >= 5
	printf(" titles");
#endif
	RunQuery(query, MakeAnyLenString(&query, "DELETE FROM titles WHERE char_id='%d'", charid), errbuf, NULL, &affected_rows);
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}
	
	// Kings & Bandits additions
	
#if DEBUG >= 5
	printf(" kings stats");
#endif
	RunQuery(query, MakeAnyLenString(&query, "DELETE FROM characterdb_stats WHERE charid='%d'", charid), errbuf, NULL, &affected_rows);
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}
	
#if DEBUG >= 5
	printf(" kings pvpstats");
#endif
	RunQuery(query, MakeAnyLenString(&query, "DELETE FROM characterdb_pvpstats WHERE charid='%d'", charid), errbuf, NULL, &affected_rows);
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}

#if DEBUG >= 5
	printf(" kings achievements");
#endif
	RunQuery(query, MakeAnyLenString(&query, "DELETE FROM character_achievements WHERE charid='%d'", charid), errbuf, NULL, &affected_rows);
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}

#if DEBUG >= 5
	printf(" _character");
#endif
	RunQuery(query, MakeAnyLenString(&query, "DELETE from character_ WHERE id='%d'", charid), errbuf, NULL, &affected_rows);
	if(query)
	{
		safe_delete_array(query);
		query = NULL;
	}
	if(affected_rows != 1)	// here we have to have a match or it's an error
	{
		LogFile->write(EQEMuLog::Error, "DeleteCharacter: error: delete operation affected %d rows\n", affected_rows);
		return false;
	}
#if DEBUG >= 5
    printf(" keyring");
#endif
    RunQuery(query, MakeAnyLenString(&query, "DELETE FROM keyring WHERE char_id='%d'", charid), errbuf, NULL, &affected_rows);
    if(query)
    {
        safe_delete_array(query);
        query = NULL;
    }
#if DEBUG >= 5
	printf("\n");
#endif
	printf("DeleteCharacter: successfully deleted '%s' (id %d)\n", name, charid);

	return true;
}
//0=failure, otherwise returns the char ID for the given char name.
int32 Database::GetCharacterID(const char *name) {
	int32 cid = 0;
	if(GetAccountIDByChar(name, &cid) == 0)
		return(0);
	return(cid);
}

/*
This function returns the account_id that owns the character with
the name "name" or zero if no character with that name was found
Zero will also be returned if there is a database error.
*/
int32 Database::GetAccountIDByChar(const char* charname, int32* oCharID) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT account_id, id FROM character_ WHERE name='%s'", charname), errbuf, &result)) {
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1)
		{
			row = mysql_fetch_row(result);
			int32 tmp = atoi(row[0]); // copy to temp var because gotta free the result before exitting this function
			if (oCharID)
				*oCharID = atoi(row[1]);
			mysql_free_result(result);
			return tmp;
		}
		mysql_free_result(result);
	}
	else {
		cerr << "Error in GetAccountIDByChar query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
	}

	return 0;
}

// Retrieve account_id for a given char_id
uint32 Database::GetAccountIDByChar(uint32 char_id) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char* query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;
	uint32 ret = 0;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT account_id FROM character_ WHERE id=%i", char_id), errbuf, &result)) {
		if (mysql_num_rows(result) == 1) {
			row = mysql_fetch_row(result);
			ret = atoi(row[0]); // copy to temp var because gotta free the result before exitting this function
		}
		mysql_free_result(result);
	}
	else {
		LogFile->write(EQEMuLog::Error, "Error in GetAccountIDByChar query '%s': %s", query, errbuf);
	}

	safe_delete_array(query);
	return ret;
}

int32 Database::GetAccountIDByName(bool eqemuTable,const char* accname, sint16* status, int32* lsid) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;


	for (unsigned int i=0; i<strlen(accname); i++) {
		if ((accname[i] < 'a' || accname[i] > 'z') &&
			(accname[i] < 'A' || accname[i] > 'Z') &&
			(accname[i] < '0' || accname[i] > '9'))
			return 0;
	}

	if ( eqemuTable )
	{

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT id, status, lsaccount_id FROM account WHERE name='%s' and eqemutable=1", accname), errbuf, &result)) {
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1) {
			row = mysql_fetch_row(result);
			int32 tmp = atoi(row[0]); // copy to temp var because gotta free the result before exitting this function
			if (status)
				*status = atoi(row[1]);
			if (lsid) {
				if (row[2])
					*lsid = atoi(row[2]);
				else
					*lsid = 0;
			}
			mysql_free_result(result);
			return tmp;
		}
		mysql_free_result(result);
	}
	else {
		cerr << "Error in (EQEMU)GetAccountIDByAcc query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
	}
	}
	else
	{
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT id, status, lsaccount_id FROM account WHERE name='%s' and eqemutable=0", accname), errbuf, &result)) {
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1) {
			row = mysql_fetch_row(result);
			int32 tmp = atoi(row[0]); // copy to temp var because gotta free the result before exitting this function
			if (status)
				*status = atoi(row[1]);
			if (lsid) {
				if (row[2])
					*lsid = atoi(row[2]);
				else
					*lsid = 0;
			}
			mysql_free_result(result);
			return tmp;
		}
		mysql_free_result(result);
	}
	else {
		cerr << "Error in GetAccountIDByAcc query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
	}
	}

	return 0;
}

void Database::GetAccountName(int32 accountid, char* name, int32* oLSAccountID) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT name, lsaccount_id FROM account WHERE id='%i'", accountid), errbuf, &result)) {
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1) {
			row = mysql_fetch_row(result);

			strcpy(name, row[0]);
			if (row[1] && oLSAccountID) {
				*oLSAccountID = atoi(row[1]);
			}
		}

		mysql_free_result(result);
	}
	else {
		safe_delete_array(query);
		cerr << "Error in GetAccountName query '" << query << "' " << errbuf << endl;
	}
}

void Database::GetCharName(int32 char_id, char* name) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT name FROM character_ WHERE id='%i'", char_id), errbuf, &result)) {
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1) {
			row = mysql_fetch_row(result);

			strcpy(name, row[0]);
		}

		mysql_free_result(result);
	}
	else {
		safe_delete_array(query);
		cerr << "Error in GetCharName query '" << query << "' " << errbuf << endl;
	}

}

uint8 Database::GetCharLevel(int32 char_id) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;

	uint8 level = 0;
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT level FROM character_ WHERE id='%i'", char_id), errbuf, &result)) {
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1) {
			row = mysql_fetch_row(result);
			level = atoi(row[0]);
		}

		mysql_free_result(result);
	}
	else {
		safe_delete_array(query);
		cerr << "Error in GetCharLevel query '" << query << "' " << errbuf << endl;
	}

	return level;
}

bool Database::LoadVariables() {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;

	if (RunQuery(query, LoadVariables_MQ(&query), errbuf, &result)) {
		safe_delete_array(query);
		bool ret = LoadVariables_result(result);
		mysql_free_result(result);
		return ret;
	}
	else {
		cerr << "Error in LoadVariables query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
	}
	return false;
}

int32 Database::LoadVariables_MQ(char** query) {
// the read of this single variable should be atomic... this was causing strange problems
//	LockMutex lock(&Mvarcache);
	return MakeAnyLenString(query, "SELECT varname, value, unix_timestamp() FROM variables where unix_timestamp(ts) >= %d", varcache_lastupdate);
}

bool Database::LoadVariables_result(MYSQL_RES* result) {
	int32 i;
    MYSQL_ROW row;
	LockMutex lock(&Mvarcache);
	if (mysql_num_rows(result) > 0) {
		if (!varcache_array) {
			varcache_max = mysql_num_rows(result);
			varcache_array = new VarCache_Struct*[varcache_max];
			for (i=0; i<varcache_max; i++)
				varcache_array[i] = 0;
		}
		else {
			int32 tmpnewmax = varcache_max + mysql_num_rows(result);
			VarCache_Struct** tmp = new VarCache_Struct*[tmpnewmax];
			for (i=0; i<tmpnewmax; i++)
				tmp[i] = 0;
			for (i=0; i<varcache_max; i++)
				tmp[i] = varcache_array[i];
			VarCache_Struct** tmpdel = varcache_array;
			varcache_array = tmp;
			varcache_max = tmpnewmax;
			delete [] tmpdel;
		}
		while ((row = mysql_fetch_row(result))) {
			varcache_lastupdate = atoi(row[2]);
			for (i=0; i<varcache_max; i++) {
				if (varcache_array[i]) {
					if (strcasecmp(varcache_array[i]->varname, row[0]) == 0) {
						delete varcache_array[i];
						varcache_array[i] = (VarCache_Struct*) new int8[sizeof(VarCache_Struct) + strlen(row[1]) + 1];
						strn0cpy(varcache_array[i]->varname, row[0], sizeof(varcache_array[i]->varname));
						strcpy(varcache_array[i]->value, row[1]);
						break;
					}
				}
				else {
					varcache_array[i] = (VarCache_Struct*) new int8[sizeof(VarCache_Struct) + strlen(row[1]) + 1];
					strcpy(varcache_array[i]->varname, row[0]);
					strcpy(varcache_array[i]->value, row[1]);
					break;
				}
			}
		}
		int32 max_used = 0;
		for (i=0; i<varcache_max; i++) {
			if (varcache_array[i]) {
				if (i > max_used)
					max_used = i;
			}
		}
		max_used++;
		varcache_max = max_used;
	}
	return true;
}

// Gets variable from 'variables' table
bool Database::GetVariable(const char* varname, char* varvalue, int16 varvalue_len) {
	varvalue[0] = '\0';

	LockMutex lock(&Mvarcache);
	if (strlen(varname) <= 1)
		return false;
	for (int32 i=0; i<varcache_max; i++) {

		if (varcache_array[i]) {
			if (strcasecmp(varcache_array[i]->varname, varname) == 0) {
				snprintf(varvalue, varvalue_len, "%s", varcache_array[i]->value);
				varvalue[varvalue_len-1] = 0;
				return true;
			}
		}
		else
			return false;
	}
	return false;
}

bool Database::SetVariable(const char* varname_in, const char* varvalue_in) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
	int32 affected_rows = 0;

	char *varname,*varvalue;

	varname=(char *)malloc(strlen(varname_in)*2+1);
	varvalue=(char *)malloc(strlen(varvalue_in)*2+1);
	DoEscapeString(varname, varname_in, strlen(varname_in));
	DoEscapeString(varvalue, varvalue_in, strlen(varvalue_in));

	if (RunQuery(query, MakeAnyLenString(&query, "Update variables set value='%s' WHERE varname like '%s'", varvalue, varname), errbuf, 0, &affected_rows)) {
		safe_delete_array(query);
		if (affected_rows == 1) {
			LoadVariables(); // refresh cache
			free(varname);
			free(varvalue);
			return true;
		}
		else {
			if (RunQuery(query, MakeAnyLenString(&query, "Insert Into variables (varname, value) values ('%s', '%s')", varname, varvalue), errbuf, 0, &affected_rows)) {
				safe_delete_array(query);
				if (affected_rows == 1) {
					LoadVariables(); // refresh cache
					free(varname);
					free(varvalue);
					return true;
				}
			}
		}
	}
	else {
		cerr << "Error in SetVariable query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
	}
	free(varname);
	free(varvalue);
	return false;
}

int32 Database::GetMiniLoginAccount(char* ip){
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
	MYSQL_ROW row;
	int32 retid = 0;
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT id FROM account WHERE minilogin_ip='%s'", ip), errbuf, &result)) {
		safe_delete_array(query);
		if ((row = mysql_fetch_row(result)))
			retid = atoi(row[0]);
		mysql_free_result(result);
	}
	else
	{
		cerr << "Error in GetMiniLoginAccount query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
	}
	return retid;
}

bool Database::GetZoneLongName(const char* short_name, char** long_name, char* file_name, float* safe_x, float* safe_y, float* safe_z, int32* graveyard_id, int32* maxclients, bool* disableGuildInviteRemove) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT long_name, file_name, safe_x, safe_y, safe_z, graveyard_id, maxclients, guildmanagedisabled FROM zone WHERE short_name='%s' AND version=0", short_name), errbuf, &result))
	{
		safe_delete_array(query);
		if (mysql_num_rows(result) > 0) {
			row = mysql_fetch_row(result);
			if (long_name != 0) {
				*long_name = strcpy(new char[strlen(row[0])+1], row[0]);
			}
			if (file_name != 0) {
				if (row[1] == 0)
					strcpy(file_name, short_name);
				else
					strcpy(file_name, row[1]);
			}
			if (safe_x != 0)
				*safe_x = atof(row[2]);
			if (safe_y != 0)
				*safe_y = atof(row[3]);
			if (safe_z != 0)
				*safe_z = atof(row[4]);
			if (graveyard_id != 0)
				*graveyard_id = atoi(row[5]);
			if (maxclients)
				*maxclients = atoi(row[6]);
			if ( disableGuildInviteRemove )
				*disableGuildInviteRemove = atoi(row[7]);
			mysql_free_result(result);
			return true;
		}
		mysql_free_result(result);
	}
	else
	{
		cerr << "Error in GetZoneLongName query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}

	return false;
}
int32 Database::GetZoneGraveyardID(int32 zone_id, int32 version) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;
    int32 GraveyardID = 0;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT graveyard_id FROM zone WHERE zoneidnumber='%u' AND (version=%i OR version=0) ORDER BY version DESC", zone_id, version), errbuf, &result))
	{
		if (mysql_num_rows(result) > 0) {
			row = mysql_fetch_row(result);
			GraveyardID = atoi(row[0]);
		}
		mysql_free_result(result);
		safe_delete_array(query);
		return GraveyardID;
	}
	else
	{
		cerr << "Error in GetZoneGraveyardID query '" << query << "' " << errbuf << endl;
	}
	safe_delete_array(query);
	return GraveyardID;
}

bool Database::GetZoneGraveyard(const int32 graveyard_id, int32* graveyard_zoneid, float* graveyard_x, float* graveyard_y, float* graveyard_z, float* graveyard_heading) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT zone_id, x, y, z, heading FROM graveyard WHERE id=%i", graveyard_id), errbuf, &result))
	{
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1) {
			row = mysql_fetch_row(result);
			if(graveyard_zoneid != 0)
				*graveyard_zoneid = atoi(row[0]);
			if(graveyard_x != 0)
				*graveyard_x = atof(row[1]);
			if(graveyard_y != 0)
				*graveyard_y = atof(row[2]);
			if(graveyard_z != 0)
				*graveyard_z = atof(row[3]);
			if(graveyard_heading != 0)
				*graveyard_heading = atof(row[4]);
			mysql_free_result(result);
			return true;
		}
		mysql_free_result(result);
	}
	else
	{
		cerr << "Error in GetZoneGraveyard query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}

	return false;
}

/*
bool Database::LoadZoneNames() {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;
	query = new char[256];
	strcpy(query, "SELECT MAX(zoneidnumber) FROM zone");

	if (RunQuery(query, strlen(query), errbuf, &result)) {
		safe_delete_array(query);
		row = mysql_fetch_row(result);
		if (row && row[0])
		{
			max_zonename = atoi(row[0]);
			zonename_array = new char*[max_zonename+1];
			for(unsigned int i=0; i<max_zonename; i++) {
				zonename_array[i] = 0;
			}
			mysql_free_result(result);

			MakeAnyLenString(&query, "SELECT zoneidnumber, short_name FROM zone");
			if (RunQuery(query, strlen(query), errbuf, &result)) {
				safe_delete_array(query);
				while((row = mysql_fetch_row(result))) {
					zonename_array[atoi(row[0])] = new char[strlen(row[1]) + 1];
					strcpy(zonename_array[atoi(row[0])], row[1]);
					Sleep(0);
				}
				mysql_free_result(result);
			}
			else {
				cerr << "Error in LoadZoneNames query '" << query << "' " << errbuf << endl;
				safe_delete_array(query);
				return false;
			}
		}
		else {
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
*/

/*
int32 Database::GetZoneID(const char* zonename) {
	if (zonename_array == 0)
		return 0;
	if (zonename == 0)
		return 0;
	for (unsigned int i=0; i<=max_zonename; i++) {
		if (zonename_array[i] != 0 && strcasecmp(zonename_array[i], zonename) == 0) {
			return i;
		}
	}
	return 0;
}

const char* Database::GetZoneName(int32 zoneID, bool ErrorUnknown) {
	if (zonename_array == 0) {
		if (ErrorUnknown)
			return "UNKNOWN";
		else
			return 0;
	}
	
	if (zoneID <= max_zonename) {
  		if (zonename_array[zoneID])
  			return zonename_array[zoneID];
  		else {
  			if (ErrorUnknown)
  				return "UNKNOWN";
  			else
  				return 0;
  		}
  	}
	else {
		if (ErrorUnknown)
			return "UNKNOWN";
		else
			return 0;
	}
	return 0;
}
*/

int8 Database::GetPEQZone(int32 zoneID, int32 version){
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
	MYSQL_ROW row;
	int peqzone = 0;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT peqzone from zone where zoneidnumber='%i' AND (version=%i OR version=0) ORDER BY version DESC", zoneID, version), errbuf, &result)) 
	{
		if (mysql_num_rows(result) > 0) 
		{
			row = mysql_fetch_row(result);
			peqzone = atoi(row[0]);
		}
		safe_delete_array(query);
		mysql_free_result(result);
		return peqzone;
	}
	else
	{
			cerr << "Error in GetPEQZone query '" << query << "' " << errbuf << endl;
	}
	safe_delete_array(query);
	return peqzone;
}

bool Database::CheckNameFilter(const char* name, bool surname) 
{
	std::string str_name = name;
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;

	if(surname)
	{
		// the minimum 4 is enforced by the client too
		if(!name || strlen(name) < 2)
		{
			return false;
		}
	}
	else
	{
		// the minimum 4 is enforced by the client too
		if(!name || strlen(name) < 4 || strlen(name) > 64)
		{
			return false;
		}
	}

	bool maxSpecialChars = false;
	for (int i = 0; i < str_name.size(); i++)
	{
		// 0x27 = '
		// 0x60 = `
		if ( surname && ( str_name[i] == 0x60 || str_name[i] == 0x27 ) )
		{
			if ( maxSpecialChars )
				return false;
			else
				maxSpecialChars = true;
		}
		else if(!isalpha(str_name[i]))
		{
			return false;
		}
	}

	for(int x = 0; x < str_name.size(); ++x)
	{
		str_name[x] = tolower(str_name[x]);
	}

	char c = '\0';
	int8 num_c = 0;
	for(int x = 0; x < str_name.size(); ++x)
	{
		if(str_name[x] == c)
		{
			num_c++;
		}
		else
		{
			num_c = 1;
			c = str_name[x];
		}
		if(num_c > 2)
		{
			return false;
		}
	}

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT name FROM name_filter"), errbuf, &result)) {
		safe_delete_array(query);
		while(row = mysql_fetch_row(result))
		{
			std::string current_row = row[0];
			for(int x = 0; x < current_row.size(); ++x)
			{
				current_row[x] = tolower(current_row[x]);
			}

			if(str_name.find(current_row) != std::string::npos)
			{
				return false;
			}
		}

		mysql_free_result(result);
		return true;
	}
	else
	{
		cerr << "Error in CheckNameFilter query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
	}

	return true;
}

bool Database::AddToNameFilter(const char* name) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
	int32 affected_rows = 0;

	if (!RunQuery(query, MakeAnyLenString(&query, "INSERT INTO name_filter (name) values ('%s')", name), errbuf, 0, &affected_rows)) {
		cerr << "Error in AddToNameFilter query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}

	safe_delete_array(query);

	if (affected_rows == 0) {
		return false;
	}

	return true;
}

int32 Database::GetAccountIDFromLSID(bool eqemuTable, int32 iLSID, char* oAccountName, sint16* oStatus) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;
	if ( eqemuTable )
	{
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT id, name, status FROM account WHERE lsaccount_id=%i and eqemutable=1", iLSID), errbuf, &result))
	{
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1) {
			row = mysql_fetch_row(result);
			int32 account_id = atoi(row[0]);
			if (oAccountName)
				strcpy(oAccountName, row[1]);
			if (oStatus)
				*oStatus = atoi(row[2]);
			mysql_free_result(result);
			return account_id;
		}
		else
		{
			mysql_free_result(result);
			return 0;
		}
		mysql_free_result(result);
	}
	else {
		cerr << "Error in (EQEMU)GetAccountIDFromLSID query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return 0;
	}
	}
	else
	{
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT id, name, status FROM account WHERE lsaccount_id=%i and eqemutable=0", iLSID), errbuf, &result))
	{
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1) {
			row = mysql_fetch_row(result);
			int32 account_id = atoi(row[0]);
			if (oAccountName)
				strcpy(oAccountName, row[1]);
			if (oStatus)
				*oStatus = atoi(row[2]);
			mysql_free_result(result);
			return account_id;
		}
		else
		{
			mysql_free_result(result);
			return 0;
		}
		mysql_free_result(result);
	}
	else {
		cerr << "Error in GetAccountIDFromLSID query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return 0;
	}
	}

	return 0;
}

void Database::GetAccountFromID(int32 id, char* oAccountName, sint16* oStatus) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT name, status FROM account WHERE id=%i", id), errbuf, &result))
	{
		if (mysql_num_rows(result) == 1) {
			row = mysql_fetch_row(result);
			if (oAccountName)
				strcpy(oAccountName, row[0]);
			if (oStatus)
				*oStatus = atoi(row[1]);
		}
		mysql_free_result(result);
	}
	else
		cerr << "Error in GetAccountFromID query '" << query << "' " << errbuf << endl;
	safe_delete_array(query);
}

void Database::ClearMerchantTemp(){
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;

	if (!RunQuery(query, MakeAnyLenString(&query, "delete from merchantlist_temp"), errbuf)) {
		cerr << "Error in ClearMerchantTemp query '" << query << "' " << errbuf << endl;
	}
	safe_delete_array(query);
}

bool Database::UpdateName(const char* oldname, const char* newname) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
	int32	affected_rows = 0;

	cout << "Renaming " << oldname << " to " << newname << "..." << endl;
	if (!RunQuery(query, MakeAnyLenString(&query, "UPDATE character_ SET name='%s' WHERE name='%s';", newname, oldname), errbuf, 0, &affected_rows)) {
		safe_delete_array(query);
		return false;
	}
	safe_delete_array(query);
	
	if (affected_rows == 0)
	{
		return false;
	}

	if (!RunQuery(query, MakeAnyLenString(&query, "UPDATE characterdb_pvpkills SET killedname='%s' WHERE killedname='%s'", newname, oldname), errbuf, 0, &affected_rows)) {
		safe_delete_array(query);
	}
	safe_delete_array(query);

	if (!RunQuery(query, MakeAnyLenString(&query, "UPDATE characterdb_pvpkills SET attackername='%s' WHERE attackername='%s'", newname, oldname), errbuf, 0, &affected_rows)) {
		safe_delete_array(query);
	}
	safe_delete_array(query);

	return true;
}

// If the name is used or an error occurs, it returns false, otherwise it returns true
bool Database::CheckUsedName(const char* name)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
    MYSQL_RES *result;
	//if (strlen(name) > 15)
	//	return false;
	if (!RunQuery(query, MakeAnyLenString(&query, "SELECT id FROM character_ where name='%s'", name), errbuf, &result)) {
		cerr << "Error in CheckUsedName query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}
	else { // It was a valid Query, so lets do our counts!
		safe_delete_array(query);
		int32 tmp = mysql_num_rows(result);
		mysql_free_result(result);
		if (tmp > 0) // There is a Name!  No change (Return False)
			return false;
		else // Everything is okay, so we go and do this.
			return true;
	}
}

int8 Database::GetServerType()
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT value FROM variables WHERE varname='ServerType'"), errbuf, &result)) {
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1)
		{
			row = mysql_fetch_row(result);
			int8 ServerType = atoi(row[0]);
			mysql_free_result(result);
			return ServerType;
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


		cerr << "Error in GetServerType query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}

	return 0;

}

bool Database::SetHackerFlag(const char* accountname, const char* charactername, const char* hacked) {
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	int32	affected_rows = 0;

	char *escape_str = new char[strlen(hacked)*2+1];
	DoEscapeString(escape_str, hacked, strlen(hacked));

	if (!RunQuery(query, MakeAnyLenString(&query, "INSERT INTO hackers(account,name,hacked) values('%s','%s','%s')", accountname, charactername, escape_str), errbuf, 0,&affected_rows)) {
		cerr << "Error in SetHackerFlag query '" << query << "' " << errbuf << endl;
		safe_delete_array(escape_str);
		return false;
	}
	safe_delete_array(query);
	safe_delete_array(escape_str);

	if (affected_rows == 0)
	{
		return false;
	}

	return true;
}

bool Database::SetMQDetectionFlag(const char* accountname, const char* charactername, const char* hacked, const char* zone) { //Lieka:  Utilize the "hacker" table, but also give zone information.

	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	int32	affected_rows = 0;
	
	char *escape_str = new char[strlen(hacked)*2+1];
	DoEscapeString(escape_str, hacked, strlen(hacked));

	if (!RunQuery(query, MakeAnyLenString(&query, "INSERT INTO hackers(account,name,hacked,zone) values('%s','%s','%s','%s')", accountname, charactername, escape_str, zone), errbuf, 0,&affected_rows)) {
		cerr << "Error in SetMQDetectionFlag query '" << query << "' " << errbuf << endl;
		safe_delete_array(escape_str);
		return false;
	}

	safe_delete_array(query);
	safe_delete_array(escape_str);

	if (affected_rows == 0)
	{
		return false;
	}

	return true;
}

int8 Database::GetRaceSkill(int8 skillid, int8 in_race)
{
	int16 race_cap = 0;
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	int32	affected_rows = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;

	//Check for a racial cap!
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT skillcap from race_skillcaps where skill = %i && race = %i", skillid, in_race), errbuf, &result, &affected_rows))
	{
		if (affected_rows != 0)
		{
			row = mysql_fetch_row(result);
			race_cap = atoi(row[0]);
		}
		delete[] query;
		mysql_free_result(result);
	}

	return race_cap;
}

int8 Database::GetSkillCap(int8 skillid, int8 in_race, int8 in_class, int16 in_level)
{
	int8 skill_level = 0, skill_formula = 0;
	int16 base_cap = 0, skill_cap = 0, skill_cap2 = 0, skill_cap3 = 0;
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	int32	affected_rows = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	//Fetch the data from DB.
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT level, formula, pre50cap, post50cap, post60cap from skillcaps where skill = %i && class = %i", skillid, in_class), errbuf, &result, &affected_rows))
	{
		if (affected_rows != 0)
		{
			row = mysql_fetch_row(result);
			skill_level = atoi(row[0]);
			skill_formula = atoi(row[1]);
			skill_cap = atoi(row[2]);
			if (atoi(row[3]) > skill_cap)
				skill_cap2 = (atoi(row[3])-skill_cap)/10; //Split the post-50 skill cap into difference between pre-50 cap and post-50 cap / 10 to determine amount of points per level.
			skill_cap3 = atoi(row[4]);
		}
		delete[] query;
		mysql_free_result(result);
	}

	int race_skill = GetRaceSkill(skillid,in_race);

	if (race_skill > 0 && (race_skill > skill_cap || skill_cap == 0 || in_level < skill_level))
		return race_skill;

	if (skill_cap == 0) //Can't train this skill at all.
		return 255; //Untrainable

	if (in_level < skill_level)
		return 254; //Untrained

	//Determine pre-51 level-based cap
	if (skill_formula > 0)
		base_cap = in_level*skill_formula+skill_formula;
	if (base_cap > skill_cap || skill_formula == 0)
		base_cap = skill_cap;
	//If post 50, add post 50 cap to base cap.
	if (in_level > 50 && skill_cap2 > 0)
		base_cap += skill_cap2*(in_level-50);
	//No cap should ever go above its post50cap
	if (skill_cap3 > 0 && base_cap > skill_cap3)
		base_cap = skill_cap3;
	//Base cap is now the max value at the person's level, return it!
	return base_cap;
}

bool Database::UpdateLiveChar(char* charname,int32 lsaccount_id) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
	if (!RunQuery(query, MakeAnyLenString(&query, "UPDATE account SET charname='%s' WHERE id=%i;",charname, lsaccount_id), errbuf)) {
		cerr << "Error in UpdateLiveChar query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}

	safe_delete_array(query);
	return true;
}

bool Database::GetLiveChar(int32 account_id, char* cname) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT charname FROM account WHERE id=%i", account_id), errbuf, &result)) {
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1) {
			row = mysql_fetch_row(result);
			strcpy(cname,row[0]);
			mysql_free_result(result);
			return true;
		}
		mysql_free_result(result);
	}
	else {
		cerr << "Error in GetLiveChar query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
	}

	return false;
}

void Database::SetLFP(int32 CharID, bool LFP) {

	char ErrBuf[MYSQL_ERRMSG_SIZE];
	char *Query = 0;

	if (!RunQuery(Query, MakeAnyLenString(&Query, "update character_ set lfp=%i where id=%i",LFP, CharID), ErrBuf))
		LogFile->write(EQEMuLog::Error, "Error updating LFP for character %i : %s", CharID, ErrBuf);

	safe_delete_array(Query);

}

void Database::SetLFG(int32 CharID, bool LFG) {

	char ErrBuf[MYSQL_ERRMSG_SIZE];
	char *Query = 0;

	if (!RunQuery(Query, MakeAnyLenString(&Query, "update character_ set lfg=%i where id=%i",LFG, CharID), ErrBuf))
		LogFile->write(EQEMuLog::Error, "Error updating LFP for character %i : %s", CharID, ErrBuf);

	safe_delete_array(Query);

}

void Database::AddReport(std::string who, std::string against, std::string lines)
{
	char ErrBuf[MYSQL_ERRMSG_SIZE];
	char *Query = 0;
	char *escape_str = new char[lines.size()*2+1];
	DoEscapeString(escape_str, lines.c_str(), lines.size());

	if (!RunQuery(Query, MakeAnyLenString(&Query, "INSERT INTO reports (name, reported, reported_text) VALUES('%s', '%s', '%s')", who.c_str(), against.c_str(), escape_str), ErrBuf))
		LogFile->write(EQEMuLog::Error, "Error adding a report for %s: %s", who.c_str(), ErrBuf);

	safe_delete_array(Query);
	safe_delete_array(escape_str);
}

void  Database::SetGroupID(const char* name,int32 id, int32 charid){
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
	if(id == 0){ //removing you from table
	if (!RunQuery(query, MakeAnyLenString(&query, "delete from group_id where charid=%i and name='%s'",charid, name), errbuf))
		printf("Unable to get group id: %s\n",errbuf);
	}
	else{
	if (!RunQuery(query, MakeAnyLenString(&query, "replace into group_id set charid=%i, groupid=%i, name='%s'",charid, id, name), errbuf))
		printf("Unable to get group id: %s\n",errbuf);
	}
#ifdef _EQDEBUG
	printf("Set group id on '%s' to %d\n", name, id);
#endif
	safe_delete_array(query);
}

void Database::ClearGroup(int32 gid) {
	ClearGroupLeader(gid);
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
	if(gid == 0) {  //clear all groups
		//if (!RunQuery(query, MakeAnyLenString(&query, "update group_id set groupid=0 where groupid!=0"), errbuf))
		if (!RunQuery(query, MakeAnyLenString(&query, "delete from group_id"), errbuf))
			printf("Unable to clear groups: %s\n",errbuf);
	} else {	//clear a specific group
		//if (!RunQuery(query, MakeAnyLenString(&query, "update group_id set groupid=0 where groupid = %lu", gid), errbuf))
		if (!RunQuery(query, MakeAnyLenString(&query, "delete from group_id where groupid = %lu", (unsigned long)gid), errbuf))
			printf("Unable to clear groups: %s\n",errbuf);
	}
	safe_delete_array(query);
}

int32 Database::GetGroupID(const char* name){
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
	MYSQL_ROW row;
	int32 groupid=0;
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT groupid from group_id where name='%s'", name), errbuf, &result)) {
		if((row = mysql_fetch_row(result)))
		{
			if(row[0])
				groupid=atoi(row[0]);
		}
		else
			printf("Unable to get group id, char not found!\n");
		mysql_free_result(result);
	}
	else
			printf("Unable to get group id: %s\n",errbuf);
	safe_delete_array(query);
	return groupid;
}

char* Database::GetGroupLeaderForLogin(const char* name,char* leaderbuf){
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
	MYSQL_ROW row;
	PlayerProfile_Struct pp;
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT profile from character_ where name='%s'", name), errbuf, &result)) {
		row = mysql_fetch_row(result);
		unsigned long* lengths = mysql_fetch_lengths(result);
		if (lengths[0] == sizeof(PlayerProfile_Struct)) {
			memcpy(&pp, row[0], sizeof(PlayerProfile_Struct));
			strcpy(leaderbuf,pp.groupMembers[0]);
		}
		mysql_free_result(result);
	}
	else{
			printf("Unable to get leader name: %s\n",errbuf);
	}
	safe_delete_array(query);
	return leaderbuf;
}

void Database::SetGroupLeaderName(int32 gid, const char* name) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;

	if (!RunQuery(query, MakeAnyLenString(&query, "Replace into group_leaders set gid=%lu, leadername='%s'",(unsigned long)gid,name), errbuf))
		printf("Unable to set group leader: %s\n",errbuf);

	safe_delete_array(query);
}

char *Database::GetGroupLeadershipInfo(int32 gid, char* leaderbuf, char* assist, char *marknpc, GroupLeadershipAA_Struct* GLAA){
	char errbuf[MYSQL_ERRMSG_SIZE];
	char* query = 0;
	MYSQL_RES* result;
	MYSQL_ROW row;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT leadername, assist, marknpc, leadershipaa FROM group_leaders WHERE gid=%lu",(unsigned long)gid),
		     errbuf, &result)) {

		safe_delete_array(query);

		row = mysql_fetch_row(result);
		unsigned long* Lengths = mysql_fetch_lengths(result);

		if(row != NULL){

			if(leaderbuf)
				strcpy(leaderbuf, row[0]);

			if(assist)
				strcpy(assist, row[1]);

			if(marknpc)
				strcpy(marknpc, row[2]);

			if(GLAA && (Lengths[3] == sizeof(GroupLeadershipAA_Struct)))
				memcpy(GLAA, row[3], sizeof(GroupLeadershipAA_Struct));

			mysql_free_result(result);
			return leaderbuf;
		}
	}
	else
		safe_delete_array(query);

	if(leaderbuf)
		strcpy(leaderbuf, "UNKNOWN");

	if(assist)
		assist[0] = 0;

	if(marknpc)
		marknpc[0] = 0;

	return leaderbuf;
}

void Database::ClearGroupLeader(int32 gid){
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
	if(gid == 0) {  //clear all group leaders
		if (!RunQuery(query, MakeAnyLenString(&query, "DELETE from group_leaders"), errbuf))
			printf("Unable to clear group leaders: %s\n",errbuf);
	} else {	//clear a specific group leader
		if (!RunQuery(query, MakeAnyLenString(&query, "DELETE from group_leaders where gid = %lu", (unsigned long)gid), errbuf))
			printf("Unable to clear group leader: %s\n",errbuf);
	}
	safe_delete_array(query);
}

bool FetchRowMap(MYSQL_RES *result, map<string,string> &rowmap)
{
MYSQL_FIELD *fields;
MYSQL_ROW row;
unsigned long num_fields,i;
bool  retval=false;
	rowmap.clear();
	if (result && (num_fields=mysql_num_fields(result)) && (row = mysql_fetch_row(result))!=NULL && (fields = mysql_fetch_fields(result))!=NULL) {
		retval=true;
		for(i=0;i<num_fields;i++) {
			rowmap[fields[i].name]=(row[i] ? row[i] : "");
		}
	}

	return retval;
}

bool Database::GetAgreementFlag(int32 acctid, int8* agreementFlag, int32* syncAccID)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char* query = 0;
	MYSQL_RES* result;
	MYSQL_ROW row;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT rulesflag,sync_accid FROM account WHERE id=%i",acctid), errbuf, &result)) {
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1)
		{
			row = mysql_fetch_row(result);
			int8 flag = atoi(row[0]);
			if ( agreementFlag )
				*agreementFlag = flag;
			
			int32 accid = (int32)atoul(row[1]);
			if ( syncAccID )
				*syncAccID = accid;

			mysql_free_result(result);
			return true;
		}
			mysql_free_result(result);
	}
	else
	{
		safe_delete_array(query);
	}
	return false;
}

void Database::SetAgreementFlag(int32 acctid)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	int32	affected_rows = 0;

	if (!RunQuery(query, MakeAnyLenString(&query, "UPDATE account SET rulesflag=1 where id=%i",acctid), errbuf, 0, &affected_rows)) {
		safe_delete_array(query);
	}
	else
	safe_delete_array(query);
}

void Database::ClearRaid(int32 rid) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
	if(rid == 0) {  //clear all raids
		if (!RunQuery(query, MakeAnyLenString(&query, "delete from raid_members"), errbuf))
			printf("Unable to clear raids: %s\n",errbuf);
	} else {	//clear a specific group
		if (!RunQuery(query, MakeAnyLenString(&query, "delete from raid_members where raidid = %lu", (unsigned long)rid), errbuf))
			printf("Unable to clear raids: %s\n",errbuf);
	}
	safe_delete_array(query);
}

void Database::ClearRaidDetails(int32 rid) {
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
	if(rid == 0) {  //clear all raids
		if (!RunQuery(query, MakeAnyLenString(&query, "delete from raid_details"), errbuf))
			printf("Unable to clear raid details: %s\n",errbuf);
	} else {	//clear a specific group
		if (!RunQuery(query, MakeAnyLenString(&query, "delete from raid_details where raidid = %lu", (unsigned long)rid), errbuf))
			printf("Unable to clear raid details: %s\n",errbuf);
	}
	safe_delete_array(query);
}

int32 Database::GetRaidID(const char* name){
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
	MYSQL_ROW row;
	int32 raidid=0;
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT raidid from raid_members where name='%s'", name), 
		errbuf, &result)) {
		if((row = mysql_fetch_row(result)))
		{
			if(row[0])
				raidid=atoi(row[0]);
		}
		else
			printf("Unable to get raid id, char not found!\n");
		mysql_free_result(result);
	}
	else
			printf("Unable to get raid id: %s\n",errbuf);
	safe_delete_array(query);
	return raidid;
}

const char *Database::GetRaidLeaderName(int32 rid)
{
	static char name[128];

	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;
    MYSQL_RES *result;
	MYSQL_ROW row;
	
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT name FROM raid_members WHERE raidid=%u AND israidleader=1", 
		rid), errbuf, &result)) {
		if((row = mysql_fetch_row(result)) != NULL)
		{
			memset(name, 0, 128);
			strcpy(name, row[0]);
			mysql_free_result(result);
			safe_delete_array(query);
			return name;
		}
		else
			printf("Unable to get raid id, char not found!\n");
		mysql_free_result(result);
	}
	else
		printf("Unable to get raid id: %s\n",errbuf);
	safe_delete_array(query);
	return "UNKNOWN";
}

bool Database::VerifyInstanceAlive(int16 instance_id, int32 char_id)
{

	//we are not saved to this instance so set our instance to 0
	if(!GlobalInstance(instance_id) && !CharacterInInstanceGroup(instance_id, char_id))
	{
		return false;
	}

	if(CheckInstanceExpired(instance_id))
	{
		DeleteInstance(instance_id);
		return false;
	}
	return true;
}

bool Database::VerifyZoneInstance(int32 zone_id, int16 instance_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT id FROM instance_lockout where id=%u AND zone=%u", 
		instance_id, zone_id), errbuf, &result))
	{
		safe_delete_array(query);
		if (mysql_num_rows(result) != 0) 
		{
			mysql_free_result(result);
			return true;
		}
		else
		{
			mysql_free_result(result);
			return false;
		}
	}
	else 
	{
		safe_delete_array(query);
		return false;
	}
	return false;
}

bool Database::CharacterInInstanceGroup(int16 instance_id, int32 char_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	bool lockout_instance_player = false;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT charid FROM instance_lockout_player where id=%u AND charid=%u", 
		instance_id, char_id), errbuf, &result))
	{
		safe_delete_array(query);
		if (mysql_num_rows(result) == 1) 
		{
			lockout_instance_player = true;
		}
		mysql_free_result(result);
	}
	else 
	{
		safe_delete_array(query);
	}
	return lockout_instance_player;
}

void Database::DeleteInstance(uint16 instance_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;

	RunQuery(query, MakeAnyLenString(&query, "DELETE FROM instance_lockout WHERE id=%u", instance_id), errbuf);
	safe_delete_array(query);

	RunQuery(query, MakeAnyLenString(&query, "DELETE FROM instance_lockout_player WHERE id=%u", instance_id), errbuf);
	safe_delete_array(query);

	RunQuery(query, MakeAnyLenString(&query, "DELETE FROM respawn_times WHERE instance_id=%u", instance_id), errbuf);
	safe_delete_array(query);

	RunQuery(query, MakeAnyLenString(&query, "DELETE FROM spawn_condition_values WHERE instance_id=%u", instance_id), errbuf);
	safe_delete_array(query);
	BuryCorpsesInInstance(instance_id);
}

bool Database::CheckInstanceExpired(uint16 instance_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;

	int32 start_time = 0;
	int32 duration = 0;
	int32 never_expires = 0;
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT start_time, duration, never_expires FROM instance_lockout WHERE id=%u", 
		instance_id), errbuf, &result))
	{
		safe_delete_array(query);
		if (mysql_num_rows(result) != 0) 
		{
			row = mysql_fetch_row(result);
			start_time = atoi(row[0]);
			duration = atoi(row[1]);
			never_expires = atoi(row[2]);
		}
		else
		{
			mysql_free_result(result);
			return true;
		}
		mysql_free_result(result);
	}
	else 
	{
		safe_delete_array(query);
		return true;
	}

	if(never_expires == 1)
	{
		return false;
	}

	timeval tv;
	gettimeofday(&tv, NULL);
	if((start_time + duration) <= tv.tv_sec)
	{
		return true;
	}
	return false;
}

int32 Database::ZoneIDFromInstanceID(uint16 instance_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	int32 ret;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT zone FROM instance_lockout where id=%u", instance_id), 
		errbuf, &result))
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

int32 Database::VersionFromInstanceID(uint16 instance_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	int32 ret;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT version FROM instance_lockout where id=%u", instance_id), 
		errbuf, &result))
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

int32 Database::GetTimeRemainingInstance(uint16 instance_id, bool &is_perma)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	int32 start_time = 0;
	int32 duration = 0;
	int32 never_expires = 0;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT start_time, duration, never_expires FROM instance_lockout WHERE id=%u", 
		instance_id), errbuf, &result))
	{
		safe_delete_array(query);
		if (mysql_num_rows(result) != 0) 
		{
			row = mysql_fetch_row(result);
			start_time = atoi(row[0]);
			duration = atoi(row[1]);
			never_expires = atoi(row[2]);
		}
		else
		{
			mysql_free_result(result);
			is_perma = false;
			return 0;
		}
		mysql_free_result(result);
	}
	else 
	{
		safe_delete_array(query);
		is_perma = false;
		return 0;
	}

	if(never_expires == 1)
	{
		is_perma = true;
		return 0;
	}
	else
	{
		is_perma = false;
	}

	timeval tv;
	gettimeofday(&tv, NULL);
	return ((start_time + duration) - tv.tv_sec);
}

bool Database::GetUnusedInstanceID(uint16 &instance_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;

	// hardcoded a 10000+ count because we want the instance ids below 10000 to be available for zones
	// same as MAX_ZONES in Zones.h memshare
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT 10000+count(id) FROM instance_lockout"), errbuf, &result))
	{
		safe_delete_array(query);
		if (mysql_num_rows(result) != 0) 
		{
			row = mysql_fetch_row(result);
			int count = atoi(row[0]);
			if(count == 0)
			{
				mysql_free_result(result);
				instance_id = RuleI(Zone, ReservedInstances) + 1;
				return true;
			}
		}
		else
		{
			mysql_free_result(result);
		}
		mysql_free_result(result);
	}
	else 
	{
		safe_delete_array(query);
		instance_id = 0;
		return false;
	}

	int32 count = 10000 + RuleI(Zone, ReservedInstances) + 1;
	int32 max = 65535;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT id FROM instance_lockout where id >= %i ORDER BY id", count), errbuf, &result))
	{
		safe_delete_array(query);
		if (mysql_num_rows(result) != 0) 
		{
			while(row = mysql_fetch_row(result))
			{
				if(count < atoi(row[0]))
				{
					instance_id = count;
					mysql_free_result(result);
					return true;
				}
				else if(count > max)
				{
					instance_id = 0;
					mysql_free_result(result);
					return false;
				}
				else
				{
					count++;
				}
			}
		}
		else
		{
			mysql_free_result(result);
		}
	}
	else 
	{
		safe_delete_array(query);
	}
	instance_id = count;
	return true;
}

//perhaps purge any expireds too
bool Database::CreateInstance(uint16 instance_id, uint32 zone_id, uint32 version, uint32 duration)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;

	if(RunQuery(query, MakeAnyLenString(&query, "INSERT INTO instance_lockout (id, zone, version, start_time, duration)" 
		" values(%lu, %lu, %lu, UNIX_TIMESTAMP(), %lu)", (unsigned long)instance_id, (unsigned long)zone_id, (unsigned long)version, (unsigned long)duration), errbuf))
	{
		safe_delete_array(query);
		return true;
	}
	else 
	{
		safe_delete_array(query);
		return false;
	}
}

void Database::PurgeExpiredInstances()
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;

	int16 id = 0;
	if (RunQuery(query, MakeAnyLenString(&query, "SELECT id FROM instance_lockout where "
			"(start_time+duration) <= UNIX_TIMESTAMP() and never_expires = 0"), errbuf, &result))
	{
		safe_delete_array(query);
		if (mysql_num_rows(result) > 0) 
		{
			row = mysql_fetch_row(result);
			while(row != NULL)
			{
				id = atoi(row[0]);
				DeleteInstance(id);
				row = mysql_fetch_row(result);
			}
		}
		mysql_free_result(result);
	}
	else 
	{
		safe_delete_array(query);
	}
}

bool Database::AddClientToInstance(uint16 instance_id, uint32 char_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;

	if(RunQuery(query, MakeAnyLenString(&query, "INSERT INTO instance_lockout_player(id, charid) "
			"values(%lu, %lu)", (unsigned long)instance_id, (unsigned long)char_id), errbuf))
	{
		safe_delete_array(query);
		return true;
	}
	else 
	{
		safe_delete_array(query);
		return false;
	}
}

bool Database::RemoveClientFromInstance(uint16 instance_id, uint32 char_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;

	if(RunQuery(query, MakeAnyLenString(&query, "DELETE FROM instance_lockout_player WHERE id=%lu AND charid=%lu", 
		(unsigned long)instance_id, (unsigned long)char_id), errbuf))
	{
		safe_delete_array(query);
		return true;
	}
	else 
	{
		safe_delete_array(query);
		return false;
	}
}

bool Database::RemoveClientsFromInstance(uint16 instance_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;

	if(RunQuery(query, MakeAnyLenString(&query, "DELETE FROM instance_lockout_player WHERE id=%lu", 
		(unsigned long)instance_id), errbuf))
	{
		safe_delete_array(query);
		return true;
	}
	else 
	{
		safe_delete_array(query);
		return false;
	}
}

bool Database::CheckInstanceExists(uint16 instance_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT * FROM instance_lockout where id=%u", instance_id), 
		errbuf, &result))
	{
		safe_delete_array(query);
		if (mysql_num_rows(result) != 0) 
		{
			mysql_free_result(result);
			return true;
		}
		mysql_free_result(result);
		return false;
	}
	else 
	{
		safe_delete_array(query);
		return false;
	}
	return false;
}

void Database::BuryCorpsesInInstance(uint16 instance_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;

	if(RunQuery(query, MakeAnyLenString(&query, "UPDATE player_corpses SET IsBurried=1, instanceid=0 WHERE instanceid=%u", 
		instance_id), errbuf, &result))
	{
		mysql_free_result(result);
	}
	safe_delete_array(query);
}

int16 Database::GetInstanceVersion(uint16 instance_id)
{
	if(instance_id < 1)
		return 0;

	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	int32 ret;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT version FROM instance_lockout where id=%u", instance_id), 
		errbuf, &result))
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

void Database::SetInstanceDuration(int16 instance_id, int32 new_duration)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;

	if(RunQuery(query, MakeAnyLenString(&query, "UPDATE `instance_lockout` SET start_time=UNIX_TIMESTAMP(), "
		"duration=%u WHERE id=%u", new_duration, instance_id), errbuf))
	{
		safe_delete_array(query);
	}
	else
	{
		//error
		safe_delete_array(query);
	}
}

bool Database::GlobalInstance(uint16 instance_id)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;
	bool ret;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT is_global from instance_lockout where id=%u LIMIT 1", instance_id), errbuf, &result))
	{
		safe_delete_array(query);
		row = mysql_fetch_row(result);
		if(row)
		{
			ret = (atoi(row[0]) == 1) ? true : false;
		}
		else
		{
			mysql_free_result(result);
			return false;
		}
	}
	else 
	{
		safe_delete_array(query);
		return false;
	}
	return ret;
}

void Database::UpdateAdventureStatsEntry(int32 char_id, int8 theme, bool win)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	int32 affected = 0;

	std::string field;

	if(win)
	{
		switch(theme)
		{
			case 1:
			{
				field = "guk_wins";
				break;
			}
			case 2:
			{
				field = "mir_wins";
				break;
			}
			case 3:
			{
				field = "mmc_wins";
				break;
			}
			case 4:
			{
				field = "ruj_wins";
				break;
			}
			case 5:
			{
				field = "tak_wins";
				break;
			}
			default:
			{
				return;
			}
		}
	}
	else
	{
		switch(theme)
		{
			case 1:
			{
				field = "guk_losses";
				break;
			}
			case 2:
			{
				field = "mir_losses";
				break;
			}
			case 3:
			{
				field = "mmc_losses";
				break;
			}
			case 4:
			{
				field = "ruj_losses";
				break;
			}
			case 5:
			{
				field = "tak_losses";
				break;
			}
			default:
			{
				return;
			}
		}
	}

	if(RunQuery(query, MakeAnyLenString(&query, "UPDATE `adventure_stats` SET %s=%s+1 WHERE player_id=%u",
		field.c_str(), field.c_str(), char_id), errbuf, NULL, &affected))
	{
		safe_delete_array(query);
	}
	else
	{
		//error
		safe_delete_array(query);
	}

	if(affected == 0)
	{
		if(RunQuery(query, MakeAnyLenString(&query, "INSERT INTO `adventure_stats` SET %s=1, player_id=%u",
			field.c_str(), char_id), errbuf))
		{
			safe_delete_array(query);
		}
		else
		{
			//error
			safe_delete_array(query);
		}
	}
}

bool Database::GetAdventureStats(int32 char_id, int32 &guk_w, int32 &mir_w, int32 &mmc_w, int32 &ruj_w, 
								 int32 &tak_w, int32 &guk_l, int32 &mir_l, int32 &mmc_l, int32 &ruj_l, int32 &tak_l)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;

	if (RunQuery(query, MakeAnyLenString(&query, "SELECT `guk_wins`, `mir_wins`, `mmc_wins`, `ruj_wins`, `tak_wins`, "
		"`guk_losses`, `mir_losses`, `mmc_losses`, `ruj_losses`, `tak_losses` FROM `adventure_stats` WHERE player_id=%u", 
		char_id), errbuf, &result))
	{
		safe_delete_array(query);
		while((row = mysql_fetch_row(result)) != NULL)
		{
			guk_w = atoi(row[0]);
			mir_w = atoi(row[1]);
			mmc_w = atoi(row[2]);
			ruj_w = atoi(row[3]);
			tak_w = atoi(row[4]);
			guk_l = atoi(row[5]);
			mir_l = atoi(row[6]);
			mmc_l = atoi(row[7]);
			ruj_l = atoi(row[8]);
			tak_l = atoi(row[9]);
		}
		mysql_free_result(result);
		return true;
	}
	else 
	{
		safe_delete_array(query);
		return false;
	}
}


bool Database::GetStartZone(PlayerProfile_Struct* in_pp, CharCreate_Struct* in_cc)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row = 0;
	int rows;

	if(!in_pp || !in_cc)
		return false;

	in_pp->x = in_pp->y = in_pp->z = in_pp->zone_id = 0;
	in_pp->binds[0].x = in_pp->binds[0].y = in_pp->binds[0].z = in_pp->binds[0].zoneId = 0;

	if ( in_cc->race == 7 ) // half elf hard coded to not spawn in kelethin
		in_cc->start_zone = -1;

	if ( in_cc->start_zone == -1 )
	{
	RunQuery
	(
		query,
		MakeAnyLenString
		(
			&query,
			"SELECT x,y,z,zone_id,bind_id FROM start_zones "
			"WHERE player_class=%i "
			"AND player_deity=%i AND player_race=%i",
			in_cc->class_,
			in_cc->deity,
			in_cc->race
		),
		errbuf,
		&result
	);
	}
	else
	{
	RunQuery
	(
		query,
		MakeAnyLenString
		(
			&query,
			"SELECT x,y,z,zone_id,bind_id FROM start_zones "
			"WHERE player_choice=%i AND player_class=%i "
			"AND player_deity=%i AND player_race=%i",
			in_cc->start_zone,
			in_cc->class_,
			in_cc->deity,
			in_cc->race
		),
		errbuf,
		&result
	);
	}
	LogFile->write(EQEMuLog::Status, "Start zone query: %s\n", query);
	safe_delete_array(query); 
	
	if((rows = mysql_num_rows(result)) > 0)
		row = mysql_fetch_row(result);
	
	if(row)
	{
		LogFile->write(EQEMuLog::Status, "Found starting location in start_zones");
		in_pp->x = atof(row[0]); 
		in_pp->y = atof(row[1]); 
		in_pp->z = atof(row[2]); 
		in_pp->zone_id = atoi(row[3]); 
		in_pp->binds[0].zoneId = atoi(row[4]); 
	} 
	else
	{
		printf("No start_zones entry in database, using defaults\n");
		switch(in_cc->start_zone)
		{
			case 0:
			{
				in_pp->zone_id = 24;	// erudnext
				in_pp->binds[0].zoneId = 38;	// tox
				break;
			}
			case 1:
			{
				in_pp->zone_id =2;	// qeynos2
				in_pp->binds[0].zoneId = 2;	// qeynos2
				break;
			}
			case 2:
			{
				in_pp->zone_id =29;	// halas
				in_pp->binds[0].zoneId = 30;	// everfrost
				break;
			}
			case 3:
			{
				in_pp->zone_id =19;	// rivervale
				in_pp->binds[0].zoneId = 20;	// kithicor
				break;
			}
			case 4:
			{
				in_pp->zone_id =9;	// freportw
				in_pp->binds[0].zoneId = 9;	// freportw
				break;
			}
			case 5:
			{
				in_pp->zone_id =40;	// neriaka
				in_pp->binds[0].zoneId = 25;	// nektulos
				break;
			}
			case 6:
			{
				in_pp->zone_id =52;	// gukta
				in_pp->binds[0].zoneId = 46;	// innothule
				break;
			}
			case 7:
			{
				in_pp->zone_id =49;	// oggok
				in_pp->binds[0].zoneId = 47;	// feerrott
				break;
			}
			case 8:
			{
				in_pp->zone_id =60;	// kaladima
				in_pp->binds[0].zoneId = 68;	// butcher
				break;
			}
			case 9:
			{
				in_pp->zone_id =54;	// gfaydark
				in_pp->binds[0].zoneId = 54;	// gfaydark
				break;
			}
			case 10:
			{	
				in_pp->zone_id =61;	// felwithea
				in_pp->binds[0].zoneId = 54;	// gfaydark
				break;
			}
			case 11:
			{	
				in_pp->zone_id =55;	// akanon
				in_pp->binds[0].zoneId = 56;	// steamfont
				break;
			}
			case 12:
			{	
				in_pp->zone_id =82;	// cabwest
				in_pp->binds[0].zoneId = 78;	// fieldofbone
				break;
			}
			case 13:
			{
				in_pp->zone_id =155;	// sharvahl
				in_pp->binds[0].zoneId = 155;	// sharvahl
				break;
			}
		}
	}
	if(result) 
		mysql_free_result(result);	
	return true;
}

bool Database::GetStartZoneSoF(PlayerProfile_Struct* in_pp, CharCreate_Struct* in_cc)
{

	// SoF doesn't send the player_choice field in character creation, it now sends the real zoneID instead.
	//
	// For SoF, search for an entry in start_zones with a matching zone_id, class, race and deity.
	//
	// For now, if no row matching row is found, send them to Crescent Reach, as that is probably the most likely
	// reason for no match being found.
	//
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row = 0;
	int rows;

	if(!in_pp || !in_cc)
		return false;

	in_pp->x = in_pp->y = in_pp->z = in_pp->zone_id = 0;
	in_pp->binds[0].x = in_pp->binds[0].y = in_pp->binds[0].z = in_pp->binds[0].zoneId = 0;

	RunQuery
	(
		query,
		MakeAnyLenString
		(
			&query,
			"SELECT x,y,z,bind_id FROM start_zones "
			"WHERE zone_id=%i AND player_class=%i "
			"AND player_deity=%i AND player_race=%i",
			in_cc->start_zone,
			in_cc->class_,
			in_cc->deity,
			in_cc->race
		),
		errbuf,
		&result
	);
	LogFile->write(EQEMuLog::Status, "SoF Start zone query: %s\n", query);
	_log(WORLD__CLIENT_TRACE, "SoF Start zone query: %s\n", query);
	safe_delete_array(query); 
	
	if((rows = mysql_num_rows(result)) > 0)
		row = mysql_fetch_row(result);
	
	if(row)
	{         
		LogFile->write(EQEMuLog::Status, "Found starting location in start_zones");
		in_pp->x = atof(row[0]); 
		in_pp->y = atof(row[1]); 
		in_pp->z = atof(row[2]); 
		in_pp->zone_id = in_cc->start_zone;
		in_pp->binds[0].zoneId = atoi(row[4]); 
	} 
	else
	{
		printf("No start_zones entry in database, using defaults\n");

		if(in_cc->start_zone == RuleI(World, TutorialZoneID))
			in_pp->zone_id = in_cc->start_zone;
		else {
			in_pp->x = in_pp->binds[0].x = -51;
			in_pp->y = in_pp->binds[0].y = -20;
			in_pp->z = in_pp->binds[0].z = 0.79;
			in_pp->zone_id = in_pp->binds[0].zoneId = 394; // Crescent Reach.
		}

	}

	if(result) 
		mysql_free_result(result);	
	return true;
}

void Database::DeleteFactionTable(uint32 charID)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;

	if (RunQuery(query, MakeAnyLenString(&query, "DELETE from faction_values where char_id = %i;",charID), errbuf, 0)) {
	}
	
	safe_delete_array(query);
}


bool Database::AddVeteranReward(int32 account_id, int32 reward_id, int32 amount)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
    char *query = 0;

	if (!RunQuery(query, MakeAnyLenString(&query, "INSERT into account_rewards SET account_id=%i, reward_id=%i, amount=%i", account_id, reward_id, amount), errbuf)) {
		cerr << "Error in ReserveName query '" << query << "' " << errbuf << endl;
		safe_delete_array(query);
		return false;
	}
	safe_delete_array(query);
	return true;
}
