#include "../common/debug.h"
#include "../common/MiscFunctions.h"
#include "QGlobals.h"
#include "zonedb.h"

void QGlobalCache::AddGlobal(uint32 id, QGlobal global)
{
	global.id = id;
	qGlobalBucket.push_back(global);
}

void QGlobalCache::RemoveGlobal(std::string name, uint32 npcID, uint32 charID, uint32 zoneID)
{
	std::list<QGlobal>::iterator iter = qGlobalBucket.begin();
	while(iter != qGlobalBucket.end())
	{
		if(name.compare((*iter).name) == 0)
		{
			if((npcID == (*iter).npc_id || (*iter).npc_id == 0) && (charID == (*iter).char_id || (*iter).char_id == 0) && (zoneID == (*iter).zone_id || (*iter).zone_id == 0))
			{
				qGlobalBucket.erase(iter);
				return;
			}
		}
		++iter;
	}
}

void QGlobalCache::Combine(std::list<QGlobal> &cacheA, std::list<QGlobal> cacheB, uint32 npcID, uint32 charID, uint32 zoneID)
{
	std::list<QGlobal>::iterator iter = cacheB.begin();
	while(iter != cacheB.end())
	{
		QGlobal cur = (*iter);

		if((cur.npc_id == npcID || cur.npc_id == 0) && (cur.char_id == charID || cur.char_id == 0) && (cur.zone_id == zoneID || cur.zone_id == 0))
		{
			if(Timer::GetTimeSeconds() < cur.expdate)
			{
				cacheA.push_back(cur);
			}
		}
		++iter;
	}
}

bool QGlobalCache::GetQGlobal(QGlobal &result, std::list<QGlobal> cacheB, char* name, uint32 npcID, uint32 charID, uint32 zoneID)
{
	std::list<QGlobal>::iterator iter = cacheB.begin();
	while(iter != cacheB.end())
	{
		QGlobal cur = (*iter);
		
		//printf("Entry: %s, %i %i %i, Value: %s (%i < %i)\n",cur.name.c_str(),cur.npc_id,cur.char_id,cur.zone_id,cur.value.c_str(),Timer::GetTimeSeconds(),cur.expdate);
		if(!strcasecmp(name, (char*)cur.name.c_str()) && (cur.npc_id == npcID || cur.npc_id == 0) && (cur.char_id == charID || cur.char_id == 0) && (cur.zone_id == zoneID || cur.zone_id == 0))
		{
			if(Timer::GetTimeSeconds() < cur.expdate)
			{
				result = cur;
				return true;
			}
		}
		++iter;
	}
	return false;
}

void QGlobalCache::PurgeExpiredGlobals()
{
	if(!qGlobalBucket.size())
		return;

	std::list<QGlobal>::iterator iter = qGlobalBucket.begin();
	while(iter != qGlobalBucket.end())
	{
		QGlobal cur = (*iter);
		if(Timer::GetTimeSeconds() > cur.expdate)
		{
			iter = qGlobalBucket.erase(iter);
			continue;
		}
		++iter;
	}
}

void QGlobalCache::LoadByNPCID(uint32 npcID)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;

	if (database.RunQuery(query, MakeAnyLenString(&query, "select id, name, charid, npcid, zoneid, value, expdate"
		" from quest_globals where npcid = %d", npcID), errbuf, &result))
	{
		while((row = mysql_fetch_row(result)))
		{
			AddGlobal(atoi(row[0]), QGlobal(std::string(row[1]), atoi(row[2]), atoi(row[3]), atoi(row[4]), row[5], row[6]?atoi(row[6]):0xFFFFFFFF));
		}
		mysql_free_result(result);
	}
	safe_delete_array(query);
}

void QGlobalCache::LoadByCharID(uint32 charID)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;

	if (database.RunQuery(query, MakeAnyLenString(&query, "select id, name, charid, npcid, zoneid, value, expdate from"
		" quest_globals where charid = %d && npcid = 0", charID), errbuf, &result))
	{
		while((row = mysql_fetch_row(result)))
		{
			AddGlobal(atoi(row[0]), QGlobal(std::string(row[1]), atoi(row[2]), atoi(row[3]), atoi(row[4]), row[5], row[6]?atoi(row[6]):0xFFFFFFFF));
		}
		mysql_free_result(result);
	}
	safe_delete_array(query);
}

void QGlobalCache::LoadByZoneID(uint32 zoneID)
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;

	if (database.RunQuery(query, MakeAnyLenString(&query, "select id, name, charid, npcid, zoneid, value, expdate from quest_globals"
		" where zoneid = %d && npcid = 0 && charid = 0", zoneID), errbuf, &result))
	{
		while((row = mysql_fetch_row(result)))
		{
			AddGlobal(atoi(row[0]), QGlobal(std::string(row[1]), atoi(row[2]), atoi(row[3]), atoi(row[4]), row[5], row[6]?atoi(row[6]):0xFFFFFFFF));
		}
		mysql_free_result(result);
	}
	safe_delete_array(query);
}
void QGlobalCache::LoadByGlobalContext()
{
	char errbuf[MYSQL_ERRMSG_SIZE];
	char *query = 0;
	MYSQL_RES *result;
	MYSQL_ROW row;

	if (database.RunQuery(query, MakeAnyLenString(&query, "select id, name, charid, npcid, zoneid, value, expdate from quest_globals"
		" where zoneid = 0 && npcid = 0 && charid = 0"), errbuf, &result))
	{
		while((row = mysql_fetch_row(result)))
		{
			AddGlobal(atoi(row[0]), QGlobal(std::string(row[1]), atoi(row[2]), atoi(row[3]), atoi(row[4]), row[5], row[6]?atoi(row[6]):0xFFFFFFFF));
		}
		mysql_free_result(result);
	}
	safe_delete_array(query);
}
