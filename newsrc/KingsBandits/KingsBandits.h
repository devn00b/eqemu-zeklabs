#ifdef ZONE
#include "../zone/zonedb.h"
#include "../zone/client.h"
#include "../zone/guild_mgr.h"
#endif
#include "../common/types.h"

class KingsBandits;

// Server Types
enum SERVER_TYPES
{
	SERVERTYPE_NOPVP=0,
	SERVERTYPE_RED69=1,
	SERVERTYPE_KAB=2,
	SERVERTYPE_ZEK=3
};

enum LOG_TYPES
{
	LOG_KINGDOMSTATUS=30,
	LOG_GUARDSTATUS=31

};
#define MAXKINGDOMS		5000 // this is MAX_TEAMS from TeamSettings.h in memshare

#define NEUTRALPLAYER 0
// Kings & Bandits Team ID's
#define QEYNOSTEAMID 1
#define HALASTEAMID 2
#define FREEPORTTEAMID 3
#define RIVERVALETEAMID 4
#define AKANONTEAMID 5
#define BANDITTEAMID 5 // FFA team id
#define THURGADINTEAMID 6
#define FELWITHETEAMID 7
#define ERUDINTEAMID 8
#define GREATDIVIDEENCAMPMENT 9
#define TOXENCAMPMENT 10
#define GFAYENCAMPMENT 11
#define LAVASTORMENCAMPMENT 12
#define PLACEHOLDER13 13 // akanon
#define PLACEHOLDER14 14 // neriak
#define PLACEHOLDER15 15 // kaladim
#define PLACEHOLDER16 16 // paineel
#define PLACEHOLDER17 17 // kelethin
#define PLACEHOLDER18 18 // cabilis
#define PLACEHOLDER19 19 // oggok
#define PLACEHOLDER20 20 // grobb
#define PLACEHOLDER21 21 // shar vahl
#define PLACEHOLDER22 22
#define PLACEHOLDER23 23
#define PLACEHOLDER24 24
#define PLACEHOLDER25 25
#define PLACEHOLDER26 26
#define PLACEHOLDER27 27
#define PLACEHOLDER28 28
#define PLACEHOLDER29 29
#define PLACEHOLDER30 30


#define TAKELEADERSHIPPVPPOINTS 5

// ZEK Team ID's - matches to KaB ID's
#define GOODTEAM 3 // cyan colored team
#define EVILTEAM 2 // orange colored team
#define NEUTRALTEAM 4 // yellow colored team 


#define NOPVPVALUE				0 // no pvp allowed, no cast restrictions
#define PVPTEAMVALUE			1 // team with level restrictions
#define PVPFFAVALUE				2 // ffa with level restrictions
#define DISABLEACTIONVALUE		3 // disable spells/attack
#define NEUTRALPVPVALUE			4 // pvp everyone but group
#define PVPFFANOLEVELVALUE		5 // pvp ffa all (no group check)
#define PVPALLVALUE				6 // pvp FFA all (group check)
#define PVPTEAMNOLEVELVALUE		7 // pvp for teams (any level) against other teams, has group check

// hardcoded item template for the player ears
#define EARTEMPLATEID			41
#define PVPCOINITEMID			31140

// Time before we allow the player to earn points again from the same individual they killed
#define EARNPOINTS_KILLDELAY 3600 // 1 hours
#define EARNKILLS_DELAY		600 // 10 min

// Base PVP points chance (decrease for higher chance of points earned depending on level)
#define EARNPOINTS_LVLRATE 200.0f // Level/value = min % chance versus .9999 chance of most pts.

// Guard limits
#define GUARDHPLIMIT		20000
#define GUARDACLIMIT		1000
#define GUARDMANALIMIT		5000
#define GUARDMAXHPREGEN		100
#define GUARDMAXMANAREGEN	100

#define QEYNOSCITIZENHEAD		20470
#define FREEPORTCITIZENHEAD		20471
#define RIVERVALECITIZENHEAD	20480
#define HALASCITIZENHEAD		20489

class KingsBandits
{
public:
#ifdef ZONE
	static bool IsCitySacked(uint32 kingdomID, int32& charIDOwner, int32& guildIDOwner, int32& teamIDOwner, int32& charLevel);
	static void ResetKingdomGuards(uint32 kingdomID);

	/* DB Queries Hah Not! :-/ */
	static char* GetKingdomName(int id, bool isClient=false, bool skipGuildNames=false, bool kingdomNames=false)
	{
		bool skipTeams = false;
		if ( RuleB(Kingdoms, NPCGuildNames) )
		{
			skipTeams = true;
			if ( !skipGuildNames )
			{
				TeamSettings_Struct* team = database.GetTeamSettings(id);
				if ( team && team->guildid > 0 && team->guildid != GUILD_NONE )
				{
					const char* guildName = guild_mgr.GetGuildName(team->guildid);
					if ( guildName != NULL && strlen(guildName) > 4 )
						return (char*)guildName;
				}
			}
		}

		bool red = RuleB(PVPCombat, PureRed);
		if ( red && isClient )
			return "<No Guild>";

		char tmp[20];
		database.GetVariable("ServerType", tmp, 9);
		int pvpFlag = atoi(tmp);

		if ( !kingdomNames && pvpFlag == SERVERTYPE_ZEK )
		{
			if ( RuleB(PVPCombat,TeamGuildWars) && !isClient )
			{
				TeamSettings_Struct* team = database.GetTeamSettings(id);
				if ( team )
					id = team->ownerteamid;
			}

			switch(id){
			case GOODTEAM:
				return "Good Team";
				break;
			case EVILTEAM:
				return "Evil Team";
				break;
			case NEUTRALTEAM:
				return "Neutral Team";
				break;
			case BANDITTEAMID:
				return "Bandits";
				break;
			default:
				return "Unknown";
				break;
			}
		}else
		{
			switch(id){
			case NEUTRALPLAYER:
				return "Neutral";
				break;
			case QEYNOSTEAMID:
				return "Qeynos Kingdom";
				break;
			case HALASTEAMID:
				return "Halas Kingdom";
				break;
			case FREEPORTTEAMID:
				return "Freeport Kingdom";
				break;
			case RIVERVALETEAMID:
				return "Rivervale Kingdom";
				break;
			case AKANONTEAMID:
				if ( pvpFlag == SERVERTYPE_KAB )
					return "Bandits";
				else
					return "Akanon Kingdom";
				break;
			case THURGADINTEAMID:
				return "Thurgadin Kingdom";
				break;
			case FELWITHETEAMID:
				return "Felwithe Kingdom";
				break;
			case ERUDINTEAMID:
				return "Erudin Kingdom";
				break;
			case GREATDIVIDEENCAMPMENT:
				return "Great Divide Encampment";
				break;
			case TOXENCAMPMENT:
				return "Toxxulia Forest Encampment";
				break;
			case GFAYENCAMPMENT:
				return "Greater Faydark Encampment";
				break;
			case LAVASTORMENCAMPMENT:
				return "Lavastorm Encampment";
				break;
			case PLACEHOLDER13:
				return "Akanon Kingdom";
				break;
			case PLACEHOLDER14:
				return "Neriak Kingdom";
				break;
			case PLACEHOLDER15:
				return "Kaladim Kingdom";
				break;
			case PLACEHOLDER16:
				return "Paineel Kingdom";
				break;
			case PLACEHOLDER17:
				return "Kelethin Kingdom";
				break;
			case PLACEHOLDER18:
				return "Cabilis Kingdom";
				break;
			case PLACEHOLDER19:
				return "Oggok Kingdom";
				break;
			case PLACEHOLDER20:
				return "Grobb Kingdom";
				break;
			case PLACEHOLDER21:
				return "Shar Vahl Kingdom";
				break;
			default:
				return "Unknown";
				break;
			}
		}

		return "";
	}
#endif

	static int8 LeaderInCityBank(int32 charid, int32 zoneid)
	{
#ifdef ZONE
		if ( zoneid == 0 )
			return 0;

		for(int i=1;i<MAXKINGDOMS;i++)
		{
			TeamSettings_Struct* team = database.GetTeamSettings(i);
			if ( team != NULL && team->charid == charid && team->bankzoneid == zoneid )
			{
				return team->teamID;
			}
			if ( team == NULL )
				break;
		}
#endif
		return 0;
	}

	static bool UpdateTeamBank(uint32 teamID)
	{
#ifdef ZONE
		TeamSettings_Struct* team = database.GetTeamSettings(teamID);
		if ( team != NULL )
		{
			char query[256];
			snprintf(query,256,"UPDATE team_settings set bankcopper=%i,banksilver=%i,bankgold=%i,bankplatinum=%i where teamid=%i",team->bankcopper,team->banksilver,team->bankgold,team->bankplatinum,team->teamID);
			database.UpdateDatabaseQuery(query);
			return true;
		}
#endif
		return false;
	}

#ifdef ZONE
	static void UpdatePlayerCityBank(Client* client)
	{
		uint32 teamID = KingsBandits::LeaderInCityBank(client->CharacterID(),client->GetPP().zone_id);
		if ( teamID > 0 )
		{
			TeamSettings_Struct* team = database.GetTeamSettings(teamID);
			if ( team != NULL && client->GetInv().cityBankCoinLoaded )
			{
				EQApplicationPacket *outapp=new EQApplicationPacket(OP_BankerChange,NULL,sizeof(BankerChange_Struct));
				BankerChange_Struct *bc=(BankerChange_Struct *)outapp->pBuffer;
				bc->copper=client->GetPP().copper;
				bc->silver=client->GetPP().silver;
				bc->gold=client->GetPP().gold;
				bc->platinum=client->GetPP().platinum;

				bc->copper_bank = team->bankcopper;
				bc->silver_bank = team->banksilver;
				bc->gold_bank = team->bankgold;
				bc->platinum_bank = team->bankplatinum;

				client->GetPP().copper_bank = team->bankcopper;
				client->GetPP().silver_bank = team->banksilver;
				client->GetPP().gold_bank = team->bankgold;
				client->GetPP().platinum_bank = team->bankplatinum;

				client->FastQueuePacket(&outapp);
			}
		}
	}

	// this is used to reload the players coin if they are in the zone but lose the city, then they can re-use their personal bank
	// and not interfere with the active city bank
	static void ReloadPlayerBank(Client* client)
	{
		EQApplicationPacket *outapp=new EQApplicationPacket(OP_BankerChange,NULL,sizeof(BankerChange_Struct));
		BankerChange_Struct *bc=(BankerChange_Struct *)outapp->pBuffer;
		bc->copper=client->GetPP().copper;
		bc->silver=client->GetPP().silver;
		bc->gold=client->GetPP().gold;
		bc->platinum=client->GetPP().platinum;

		bc->copper_bank = client->GetPP().copper_bank;
		bc->silver_bank = client->GetPP().silver_bank;
		bc->gold_bank = client->GetPP().gold_bank;
		bc->platinum_bank = client->GetPP().platinum_bank;

		client->GetInv().cityBankCoinLoaded = false;

		client->FastQueuePacket(&outapp);
	}
#endif

	static bool LoadCityBankCoin(PlayerProfile_Struct* pp, Inventory* inv, int32 charID, int32 zoneID, bool reload=false)
	{
#ifdef ZONE
		uint32 teamID = KingsBandits::LeaderInCityBank(charID,zoneID);

		if ( teamID > 0 && inv->cityBankLoaded )
		{
			TeamSettings_Struct* team = database.GetTeamSettings(teamID);
			if ( team != NULL )
			{

				if ( !reload || (reload && inv->cityBankCoinLoaded ) )
				{
					team->placeholdercopper = pp->copper_bank;
					team->placeholdersilver = pp->silver_bank;
					team->placeholdergold = pp->gold_bank;
					team->placeholderplatinum = pp->platinum_bank;

					pp->copper_bank = team->bankcopper;
					pp->silver_bank = team->banksilver;
					pp->gold_bank = team->bankgold;
					pp->platinum_bank = team->bankplatinum;
					inv->cityBankCoinLoaded = true;
				}
				else
					return false;

				return true;
			}
		}
#endif
		return false;
	}

	static bool PlayerOwnsCity(int32 charid)
	{
#ifdef ZONE	
		for(int i=1;i<MAXKINGDOMS;i++)
		{
			TeamSettings_Struct* team = database.GetTeamSettings(i);
			if ( team && team->charid == charid && team->citymode != CAMPDESTROYED )
			{
				return true;
			}
			if ( team == NULL )
				break;
		}
#endif
		return false;
	}
	
	static bool GuildOwnsCity(int32 guildid)
	{

#ifdef ZONE	
		for(int i=1;i<MAXKINGDOMS;i++)
		{
			TeamSettings_Struct* team = database.GetTeamSettings(i);
			if ( team && team->guildid == guildid && team->citymode != CAMPDESTROYED )
			{
				return true;
			}
			if ( team == NULL )
				break;
		}
#endif
		return false;
	}
	
	static bool LocationNearby(float x, float y, float z, int32 zoneid, float maxdist)
	{
#ifdef ZONE	
		for(int i=1;i<MAXKINGDOMS;i++)
		{
			TeamSettings_Struct* team = database.GetTeamSettings(i);
			if ( team && team->zoneid == zoneid && team->citymode != CAMPDESTROYED )
			{
				float xDiff = team->x - x;
				float yDiff = team->y - y;

				float dist = sqrtf( (xDiff * xDiff) 
					+ (yDiff * yDiff) );
				if ( dist <= maxdist )
					return true;
			}
			if ( team == NULL )
				break;
		}
#endif
		return false;
	}

	static uint8 GuildCityXPBonus(uint32 guildid)
	{
		if ( guildid == 0 )
			return 0;

		uint8 val = 0;		
#ifdef ZONE	
		for(int i=1;i<MAXKINGDOMS;i++)
		{
			TeamSettings_Struct* team = database.GetTeamSettings(i);
			if ( team && team->guildid == guildid )
			{
				val += team->teamXPBonus;
			}
			if ( team == NULL )
				break;
		}
#endif
		return val;
	}

	static uint32 CountGuardsAlive(uint32 kingdomid)
	{
		uint32 val = 0;		
#ifdef ZONE	
		if(kingdomid >= MAXKINGDOMS)
			return 0;
		TeamSettings_Struct* team = database.GetTeamSettings(kingdomid);
		if ( team )
		{
			for(int i=0;i<team->totalGuardsCount;i++)
			{
				if ( team->TeamGuards[i].didMobSpawn )
					val++;
			}
		}
#endif
		return val;
	}

	static bool IsCityGuardByNPCID(TeamSettings_Struct* team, int32 npcid)
	{
		if ( team == NULL )
			return false;

		for(int i=0;i<team->totalGuardsCount;i++)
		{
			if ( team->TeamGuards[i].guardNPCTypeID == npcid )
				return true;
		}

		return false;
	}


#ifdef ZONE
	static bool IsGuardAttackAllowed(Client* compare, Mob* npc)
	{	
		if ( npc->CastToNPC()->teamID > 0 )
		{
			if ( compare->GetPVP() == 0 && !compare->GetGM() )
				return false;

			TeamSettings_Struct* team = database.GetTeamSettings(npc->CastToNPC()->teamID);
			if ( team )
			{
				if ( !RuleB(Kingdoms,CanAttackOwnGuards) )
				{
					bool teamGuildWars = RuleB(PVPCombat,TeamGuildWars);
					bool pureRed = RuleB(PVPCombat,PureRed);
					if ( teamGuildWars && ( (team->ownerteamid != BANDITTEAMID && team->ownerteamid > 0 && team->ownerteamid == compare->CastToClient()->GetPVP()) || 
						( team->ownerteamid == BANDITTEAMID && team->guildid == compare->CastToClient()->GuildID()) ) )
						return false;
					else if ( !teamGuildWars && !pureRed && ( ( team->teamID == compare->CastToClient()->GetPVP() ) || ( team->guildid > 0 && team->guildid == compare->CastToClient()->GuildID() ) ) )
						return false;
					else if ( !teamGuildWars && pureRed && ( team->guildid > 0 && team->guildid == compare->CastToClient()->GuildID() ) )
						return false;
				}
			}
		}
		return true;
	}
#endif

	static bool isCityLocation(int kingdomid) {
		if ( kingdomid >= QEYNOSTEAMID && kingdomid <= ERUDINTEAMID )
			return true;
		if ( kingdomid >= PLACEHOLDER13 && kingdomid <= PLACEHOLDER21 )
			return true;
		
		return false;
	}
	
	static void ReloadCityGuards(uint32 kingdomID);
private:
};
