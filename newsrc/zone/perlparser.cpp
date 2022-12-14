/*  EQEMu:  Everquest Server Emulator
    Copyright (C) 2001-2006  EQEMu Development Team (http://eqemulator.net)

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

#include "features.h"

#ifdef EMBPERL
#ifdef EMBPERL_XS

#include "../common/debug.h"
#include "perlparser.h"
#include "questmgr.h"
#include "embxs.h"
#include "entity.h"
#include "../common/MiscFunctions.h"
#include "zone.h"
#include "../KingsBandits/KingsBandits.h"

extern Zone* zone;
extern uint32 staticZones;
extern uint32 bootedZones;
extern uint32 availZones;

/*

Some useful perl API info:

SvUV == string to unsigned value (char->ulong)
SvIV == string to signed value (char->long)
SvNV == string to real value (float,double)
SvPV_nolen == string with no length restriction


*/

PerlXSParser::PerlXSParser() : PerlembParser() {
	//we cannot rely on PerlembParser to call the right map_funs because
	//our virtual table is not set up until after we call them, so we need to move
	//the call to ReloadQuests out of the constructor.
}

void PerlXSParser::map_funs() {
	_empty_sv = newSV(0);

	perl->eval(
	"{"
	"package quest;"
	"&boot_quest;"			//load our quest XS
#ifdef EMBPERL_XS_CLASSES
	"package Mob;"
	"&boot_Mob;"			//load our Mob XS

	"package Client;"
	"our @ISA = qw(Mob);"	//client inherits mob.
	"&boot_Mob;"			//load our Mob XS
	"&boot_Client;"			//load our Client XS

	"package NPC;"
	"our @ISA = qw(Mob);"	//NPC inherits mob.
	"&boot_Mob;"			//load our Mob XS
	"&boot_NPC;"			//load our NPC XS

	"package Corpse;"
	"our @ISA = qw(Mob);"	//Corpse inherits mob.
	"&boot_Mob;"			//load our Mob XS
	"&boot_Corpse;"			//load our Mob XS

	"package EntityList;"
	"&boot_EntityList;"		//load our EntityList XS

	"package PerlPacket;"
	"&boot_PerlPacket;"		//load our PerlPacket XS

	"package Group;"
	"&boot_Group;"		//load our Group XS

	"package Raid;"
	"&boot_Raid;"		//load our Raid XS

	"package QuestItem;"
	"&boot_QuestItem;"	// load quest Item XS

	"package HateEntry;"
	"&boot_HateEntry;"	// load quest Hate XS

	"package Object;"
	"&boot_Object;"	// load quest Object XS

#endif
	"package main;"
	"}"
	);//eval
}

void PerlXSParser::SendCommands(const char * pkgprefix, const char *event, int32 npcid, Mob* other, Mob* mob, ItemInst* iteminst)
{
	if(!perl)
		return;
	_ZP(PerlXSParser_SendCommands);

	if(mob && mob->IsClient())
		quest_manager.StartQuest(other, mob->CastToClient(), iteminst);
	else
		quest_manager.StartQuest(other, NULL, NULL);

	try {

		std::string cmd = "package " + (std::string)(pkgprefix) + (std::string)(";");
		perl->eval(cmd.c_str());

#ifdef EMBPERL_XS_CLASSES
		char namebuf[64];

		//init a couple special vars: client, npc, entity_list
		Client *curc = quest_manager.GetInitiator();
		snprintf(namebuf, 64, "%s::client", pkgprefix);
		SV *client = get_sv(namebuf, true);
		if(curc != NULL) {
			sv_setref_pv(client, "Client", curc);
		} else {
			//clear out the value, mainly to get rid of blessedness
			sv_setsv(client, _empty_sv);
		}
		
		//only export NPC if it's a npc quest
		if(!other->IsClient()){
			NPC *curn = quest_manager.GetNPC();
			snprintf(namebuf, 64, "%s::npc", pkgprefix);
			SV *npc = get_sv(namebuf, true);
			sv_setref_pv(npc, "NPC", curn);
			
			snprintf(namebuf, 64, "%s::target", pkgprefix);
			SV *target = get_sv(namebuf, true);
			sv_setref_pv(target, "Mob", mob);
		}

		//only export QuestItem if it's an item quest
		if(iteminst) {
			ItemInst* curi = quest_manager.GetQuestItem();
			snprintf(namebuf, 64, "%s::questitem", pkgprefix);
			SV *questitem = get_sv(namebuf, true);
			sv_setref_pv(questitem, "QuestItem", curi);
		}

		snprintf(namebuf, 64, "%s::entity_list", pkgprefix);
		SV *el = get_sv(namebuf, true);
		sv_setref_pv(el, "EntityList", &entity_list);
#endif

		//now call the requested sub
		perl->dosub(std::string(pkgprefix).append("::").append(event).c_str());

	} catch(const char * err) {

		//try to reduce some of the console spam...
		//todo: tweak this to be more accurate at deciding what to filter (we don't want to gag legit errors)
		if(!strstr(err,"Undefined subroutine"))
			LogFile->write(EQEMuLog::Status, "Script error: %s::%s - %s", pkgprefix, event, err);
	}

	quest_manager.EndQuest();
}


#ifdef EMBPERL_XS_CLASSES

//Any creation of new Client objects gets the current quest Client
XS(XS_Client_new);
XS(XS_Client_new)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: Client::new()");
	{
		Client *		RETVAL;

		RETVAL = quest_manager.GetInitiator();
		ST(0) = sv_newmortal();
		if(RETVAL)
			sv_setref_pv(ST(0), "Client", (void*)RETVAL);
	}
	XSRETURN(1);
}

//Any creation of new NPC objects gets the current quest NPC
XS(XS_NPC_new);
XS(XS_NPC_new)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: NPC::new()");
	{
		NPC *		RETVAL;

		RETVAL = quest_manager.GetNPC();
		ST(0) = sv_newmortal();
		if(RETVAL)
			sv_setref_pv(ST(0), "NPC", (void*)RETVAL);
	}
	XSRETURN(1);
}

//Any creation of new NPC objects gets the current quest NPC
XS(XS_EntityList_new);
XS(XS_EntityList_new)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: EntityList::new()");
	{
		EntityList *		RETVAL;

		RETVAL = &entity_list;
		ST(0) = sv_newmortal();
		if(RETVAL)
			sv_setref_pv(ST(0), "EntityList", (void*)RETVAL);
	}
	XSRETURN(1);
}

//Any creation of new quest items gets the current quest item
XS(XS_QuestItem_new);
XS(XS_QuestItem_new)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: QuestItem::new()");

	ItemInst* RETVAL;

	RETVAL = quest_manager.GetQuestItem();
	ST(0) = sv_newmortal();
	if(RETVAL)
		sv_setref_pv(ST(0), "QuestItem", (void*)RETVAL);

	XSRETURN(1);
}

//Any creation of new quest items gets the current quest item
XS(XS_MobList_new);
XS(XS_MobList_new)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: MobList::new()");

	ListElement<Mob*>* RETVAL;

	RETVAL = NULL;
	ST(0) = sv_newmortal();
	if(RETVAL)
		sv_setref_pv(ST(0), "MobList", (void*)RETVAL);

	XSRETURN(1);
}

#endif //EMBPERL_XS_CLASSES


XS(XS__echo); // prototype to pass -Wmissing-prototypes
XS(XS__echo) {
	dXSARGS;

   if (items != 2)
      Perl_croak(aTHX_ "Usage: echo(id#, str)");

	quest_manager.echo(SvUV(ST(0)), SvPV_nolen(ST(1)));

	XSRETURN_EMPTY;
}

XS(XS__say); // prototype to pass -Wmissing-prototypes
XS(XS__say) {
	dXSARGS;

	if (items == 1)
		quest_manager.say(SvPV_nolen(ST(0)));
	else if (items == 2)
		quest_manager.say(SvPV_nolen(ST(0)), (int)SvIV(ST(1)));
	else
		Perl_croak(aTHX_ "Usage: say(str [, language])");

	XSRETURN_EMPTY;
}

XS(XS__me); // prototype to pass -Wmissing-prototypes
XS(XS__me) {
	dXSARGS;

	if (items != 1)
		Perl_croak(aTHX_ "Usage: %s(str)", "me");

	quest_manager.me(SvPV_nolen(ST(0)));

	XSRETURN_EMPTY;
}

XS(XS__summonitem); // prototype to pass -Wmissing-prototypes
XS(XS__summonitem)
{
	dXSARGS;
	if (items == 1)
		quest_manager.summonitem(SvUV(ST(0)));
	else if(items == 2)
		quest_manager.summonitem(SvUV(ST(0)), SvUV(ST(1)));
	else
		Perl_croak(aTHX_ "Usage: summonitem(itemid, [charges])");
	XSRETURN_EMPTY;
}

XS(XS__write);
XS(XS__write)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: write(file, str)");

	char *		file = (char *)SvPV_nolen(ST(0));
	char *		str = (char *)SvPV_nolen(ST(1));

	quest_manager.write(file, str);

	XSRETURN_EMPTY;
}

XS(XS__spawn);
XS(XS__spawn)
{
	dXSARGS;
	if (items != 6)
		Perl_croak(aTHX_ "Usage: spawn(npc_type, grid, unused, x, y, z)");

	int16		RETVAL;
	dXSTARG;

	int	npc_type = (int)SvIV(ST(0));
	int	grid = (int)SvIV(ST(1));
	int	unused = (int)SvIV(ST(2));
	float	x = (float)SvNV(ST(3));
	float	y = (float)SvNV(ST(4));
	float	z = (float)SvNV(ST(5));

	RETVAL = quest_manager.spawn2(npc_type, grid, unused, x, y, z, 0);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__spawn2);
XS(XS__spawn2)
{
	dXSARGS;
	if (items < 6)
		Perl_croak(aTHX_ "Usage: spawn2(npc_type, grid, unused, x, y, z, heading, nomapz)");

	int16		RETVAL;
	dXSTARG;

	int	npc_type = (int)SvIV(ST(0));
	int	grid = (int)SvIV(ST(1));
	int	unused = (int)SvIV(ST(2));
	float	x = (float)SvNV(ST(3));
	float	y = (float)SvNV(ST(4));
	float	z = (float)SvNV(ST(5));
	float	heading = (float)SvNV(ST(6));

	int nomapz = 0;

	if ( items > 7 )
	{
		// value for nomapz
		int value = (int)SvIV(ST(7));
		if ( value )
			nomapz = 1;
	}

	// KaB / Zek / Red69 - Allows Z to be properly set on spawn and not be under world 9/19/2012
	if ( zone->HasMap() && !nomapz )
	{
			VERTEX dest(x, y, z);

			float newz = zone->zonemap->FindClosestZ(dest);

			z = newz + 1;
	}

	RETVAL = quest_manager.spawn2(npc_type, grid, unused, x, y, z, heading);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__unique_spawn);
XS(XS__unique_spawn)
{
	dXSARGS;
	if (items != 6 && items != 7)
		Perl_croak(aTHX_ "Usage: unique_spawn(npc_type, grid, unused, x, y, z, [heading])");

	int16		RETVAL;
	dXSTARG;

	int	npc_type = (int)SvIV(ST(0));
	int	grid = (int)SvIV(ST(1));
	int	unused = (int)SvIV(ST(2));
	float	x = (float)SvNV(ST(3));
	float	y = (float)SvNV(ST(4));
	float	z = (float)SvNV(ST(5));
	float	heading = 0;
	if(items == 7)
		heading = (float)SvNV(ST(6));

	RETVAL = quest_manager.unique_spawn(npc_type, grid, unused, x, y, z, heading);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__spawn_from_spawn2);
XS(XS__spawn_from_spawn2)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: spawn_from_spawn2(spawn2_id)");

	int16		RETVAL;
	dXSTARG;

	int	spawn2_id = (int)SvIV(ST(0));

	RETVAL = quest_manager.spawn_from_spawn2(spawn2_id);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__enable_spawn2);
XS(XS__enable_spawn2)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: enable_spawn2(spawn2_id)");

	int	spawn2_id = (int)SvIV(ST(0));

	quest_manager.enable_spawn2(spawn2_id);
	XSRETURN_EMPTY;
}

XS(XS__disable_spawn2);
XS(XS__disable_spawn2)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: disable_spawn2(spawn2_id)");

	int	spawn2_id = (int)SvIV(ST(0));

	quest_manager.disable_spawn2(spawn2_id);
	XSRETURN_EMPTY;
}

XS(XS__setstat);
XS(XS__setstat)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: setstat(stat, value)");

	int	stat = (int)SvIV(ST(0));
	int	value = (int)SvIV(ST(1));

	quest_manager.setstat(stat, value);

	XSRETURN_EMPTY;
}

XS(XS__incstat);  //old setstat command aza
XS(XS__incstat)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: incstat(stat, value)");

	int	stat = (int)SvIV(ST(0));
	int	value = (int)SvIV(ST(1));

	quest_manager.incstat(stat, value);

	XSRETURN_EMPTY;
}

XS(XS__castspell);
XS(XS__castspell)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: castspell(spell_id, target_id)");

	int	spell_id = (int)SvIV(ST(0));
	int	target_id = (int)SvIV(ST(1));

	quest_manager.castspell(spell_id, target_id);

	XSRETURN_EMPTY;
}

XS(XS__selfcast);
XS(XS__selfcast)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: selfcast(spell_id)");

	int	spell_id = (int)SvIV(ST(0));

	quest_manager.selfcast(spell_id);

	XSRETURN_EMPTY;
}

XS(XS__addloot);
XS(XS__addloot)
{
	dXSARGS;
	if(items < 1 || items > 3)
		Perl_croak(aTHX_ "Usage: addloot(item_id, charges = 0, equipitem = true)");

	int32	itemid = (int32)SvUV(ST(0));
	int8	charges = 0;
	bool	equipitem = true;

	if (items > 1)
		charges = (int8)SvUV(ST(1));
	if (items > 2)
		equipitem = (bool)SvTRUE(ST(2));

	quest_manager.addloot(itemid, charges, equipitem);

	XSRETURN_EMPTY;
}

XS(XS__zone);
XS(XS__zone)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: zone(zone_name)");

	char *		zone_name = (char *)SvPV_nolen(ST(0));

	quest_manager.Zone(zone_name);

	XSRETURN_EMPTY;
}

XS(XS__settimer);
XS(XS__settimer)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: settimer(timer_name, seconds)");

	char *		timer_name = (char *)SvPV_nolen(ST(0));
	int	seconds = (int)SvIV(ST(1));

	quest_manager.settimer(timer_name, seconds);

	XSRETURN_EMPTY;
}

XS(XS__stoptimer);
XS(XS__stoptimer)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: stoptimer(timer_name)");

	char *		timer_name = (char *)SvPV_nolen(ST(0));

	quest_manager.stoptimer(timer_name);

	XSRETURN_EMPTY;
}

XS(XS__stopalltimers);
XS(XS__stopalltimers)
{
	dXSARGS;
	if (items != 0)
		Perl_croak(aTHX_ "Usage: stopalltimers()");

	quest_manager.stopalltimers();

	XSRETURN_EMPTY;
}

XS(XS__emote);
XS(XS__emote)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: emote(str)");

	char *		str = (char *)SvPV_nolen(ST(0));

	quest_manager.emote(str);

	XSRETURN_EMPTY;
}

XS(XS__shout);
XS(XS__shout)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: shout(str)");

	char *		str = (char *)SvPV_nolen(ST(0));

	quest_manager.shout(str);

	XSRETURN_EMPTY;
}

XS(XS__shout2);
XS(XS__shout2)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: shout2(str)");

	char *		str = (char *)SvPV_nolen(ST(0));

	quest_manager.shout2(str);

	XSRETURN_EMPTY;
}

XS(XS__gmsay);
XS(XS__gmsay)
{
	dXSARGS;
	if (items != 1 && items != 2 && items != 3)
		Perl_croak(aTHX_ "Usage: gmsay(str, color, send_to_world?)");

	char * str = (char *)SvPV_nolen(ST(0));
	int	color = 7;
	bool send_to_world = 0;

	if (items > 1) {
		color = (int)SvIV(ST(1));
	}

	if (items > 2) {
		send_to_world = ((int)SvIV(ST(2))) == 0?false:true;
	}

	quest_manager.gmsay(str, color, send_to_world);

	XSRETURN_EMPTY;
}

XS(XS__depop);
XS(XS__depop)
{
	dXSARGS;
	if (items < 0 || items > 1)
		Perl_croak(aTHX_ "Usage: depop(npc_type= 0)");

	int	npc_type;

	if (items < 1)
		npc_type = 0;
	else
		npc_type = (int)SvIV(ST(0));


	quest_manager.depop(npc_type);

	XSRETURN_EMPTY;
}

XS(XS__depopall);
XS(XS__depopall)
{
	dXSARGS;
	if (items < 0 || items > 1)
		Perl_croak(aTHX_ "Usage: depopall(npc_type= 0)");

	int	npc_type;

	if (items < 1)
		npc_type = 0;
	else
		npc_type = (int)SvIV(ST(0));


	quest_manager.depopall(npc_type);

	XSRETURN_EMPTY;
}

XS(XS__settarget);
XS(XS__settarget)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: settarget(type, target_id)");

	char *		type = (char *)SvPV_nolen(ST(0));
	int	target_id = (int)SvIV(ST(1));

	quest_manager.settarget(type, target_id);

	XSRETURN_EMPTY;
}

XS(XS__follow);
XS(XS__follow)
{
	dXSARGS;
	if (items != 1 && items != 2)
		Perl_croak(aTHX_ "Usage: follow(entity_id, [distance])");

	int	entity_id = (int)SvIV(ST(0));
	int	distance;

	if (items == 2)
		distance = (int)SvIV(ST(1));
	else
		distance = 10;

	quest_manager.follow(entity_id, distance);

	XSRETURN_EMPTY;
}

XS(XS__sfollow);
XS(XS__sfollow)
{
	dXSARGS;
	if (items != 0)
		Perl_croak(aTHX_ "Usage: sfollow()");


	quest_manager.sfollow();

	XSRETURN_EMPTY;
}

XS(XS__changedeity);
XS(XS__changedeity)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: changedeity(diety_id)");

	int	diety_id = (int)SvIV(ST(0));

	quest_manager.changedeity(diety_id);

	XSRETURN_EMPTY;
}

XS(XS__exp);
XS(XS__exp)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: exp(amt)");

	int	amt = (int)SvIV(ST(0));

	quest_manager.exp(amt);

	XSRETURN_EMPTY;
}

XS(XS__level);
XS(XS__level)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: level(newlevel)");

	int	newlevel = (int)SvIV(ST(0));

	quest_manager.level(newlevel);

	XSRETURN_EMPTY;
}

XS(XS__traindisc);
XS(XS__traindisc)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: traindisc(discipline_tome_item_id)");

	int	discipline_tome_item_id = (int)SvIV(ST(0));

	quest_manager.traindisc(discipline_tome_item_id);

	XSRETURN_EMPTY;
}

XS(XS__isdisctome);
XS(XS__isdisctome)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: isdisctome(item_id)");

	bool RETVAL;
	int	item_id = (int)SvIV(ST(0));

	RETVAL = quest_manager.isdisctome(item_id);

	ST(0) = boolSV(RETVAL);
	sv_2mortal(ST(0));
	XSRETURN(1);
}

XS(XS__safemove);
XS(XS__safemove)
{
	dXSARGS;
	if (items != 0)
		Perl_croak(aTHX_ "Usage: safemove()");


	quest_manager.safemove();

	XSRETURN_EMPTY;
}

XS(XS__rain);
XS(XS__rain)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: rain(weather)");

	int	weather = (int)SvIV(ST(0));

	quest_manager.rain(weather);

	XSRETURN_EMPTY;
}

XS(XS__snow);
XS(XS__snow)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: snow(weather)");

	int	weather = (int)SvIV(ST(0));

	quest_manager.snow(weather);

	XSRETURN_EMPTY;
}

XS(XS__surname);
XS(XS__surname)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: surname(name)");

	char *		name = (char *)SvPV_nolen(ST(0));

	quest_manager.surname(name);

	XSRETURN_EMPTY;
}

XS(XS__permaclass);
XS(XS__permaclass)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: permaclass(class_id)");

	int	class_id = (int)SvIV(ST(0));

	quest_manager.permaclass(class_id);

	XSRETURN_EMPTY;
}

XS(XS__permarace);
XS(XS__permarace)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: permarace(race_id)");

	int	race_id = (int)SvIV(ST(0));

	quest_manager.permarace(race_id);

	XSRETURN_EMPTY;
}

XS(XS__permagender);
XS(XS__permagender)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: permagender(gender_id)");

	int	gender_id = (int)SvIV(ST(0));

	quest_manager.permagender(gender_id);

	XSRETURN_EMPTY;
}

XS(XS__scribespells);
XS(XS__scribespells)
{
	dXSARGS;
	if (items < 1)
		Perl_croak(aTHX_ "Usage: scribespells(max_level, min_level = 1)");

	uint16	RETVAL;
	dXSTARG;

	uint8 max_level = (uint8)SvIV(ST(0));
	uint8 min_level = (uint8)SvIV(ST(1));

	if (min_level)
		RETVAL = quest_manager.scribespells(max_level, min_level);
	else
		RETVAL = quest_manager.scribespells(max_level);

	XSprePUSH; PUSHu((IV)RETVAL);
	XSRETURN(1);
}

XS(XS__traindiscs);
XS(XS__traindiscs)
{
	dXSARGS;
	if (items < 1)
		Perl_croak(aTHX_ "Usage: traindiscs(max_level, min_level = 1)");

	uint16	RETVAL;
	dXSTARG;

	uint8 max_level = (uint8)SvIV(ST(0));
	uint8 min_level = (uint8)SvIV(ST(1));

	if (min_level)
		RETVAL = quest_manager.traindiscs(max_level, min_level);
	else
		RETVAL = quest_manager.traindiscs(max_level);

	XSprePUSH; PUSHu((IV)RETVAL);
	XSRETURN(1);
}

XS(XS__unscribespells);
XS(XS__unscribespells)
{
	dXSARGS;
	if (items != 0)
		Perl_croak(aTHX_ "Usage: unscribespells()");


	quest_manager.unscribespells();

	XSRETURN_EMPTY;
}

XS(XS__untraindiscs);
XS(XS__untraindiscs)
{
	dXSARGS;
	if (items != 0)
		Perl_croak(aTHX_ "Usage: untraindiscs()");


	quest_manager.untraindiscs();

	XSRETURN_EMPTY;
}

XS(XS__givecash);
XS(XS__givecash)
{
	dXSARGS;
	if (items < 4)
		Perl_croak(aTHX_ "Usage: givecash(copper, silver, gold, platinum, convertcopper=0/1)");

	int	copper = (int)SvIV(ST(0));
	int	silver = (int)SvIV(ST(1));
	int	gold = (int)SvIV(ST(2));
	int	platinum = (int)SvIV(ST(3));
	bool convertCopper = false;
	
	if ( items > 4 ) convertCopper = (bool)SvIV(ST(4));

	quest_manager.givecash(copper, silver, gold, platinum, convertCopper);

	XSRETURN_EMPTY;
}

XS(XS__pvp);
XS(XS__pvp)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: pvp(mode)");

	char *		mode = (char *)SvPV_nolen(ST(0));

	quest_manager.pvp(mode);

	XSRETURN_EMPTY;
}

// Kings & Bandits - perlparser addons for quest

XS(XS__GetClientByEntityID);
XS(XS__GetClientByEntityID)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: getclientbyentityid(id)");
	{
		Client *		RETVAL;
		
		int id = (int)SvIV(ST(0));

		RETVAL = entity_list.GetClientByID((int16)id);
		ST(0) = sv_newmortal();
		if(RETVAL)
			sv_setref_pv(ST(0), "Client", (void*)RETVAL);
	}
	XSRETURN(1);
}

XS(XS__pvpvalue);
XS(XS__pvpvalue)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: pvpvalue(mode)");

	char *		mode = (char *)SvPV_nolen(ST(0));

	quest_manager.pvpvalue(mode);

	XSRETURN_EMPTY;
}

XS(XS__getstaticzonecount);
XS(XS__getstaticzonecount)
{
	dXSARGS;

	int32	RETVAL;
	dXSTARG;

	uint32 count = staticZones;

	RETVAL = count;

	XSprePUSH; PUSHu((UV)RETVAL);
	XSRETURN(1);
}

XS(XS__getbootedzonecount);
XS(XS__getbootedzonecount)
{
	dXSARGS;
	int32	RETVAL;
	dXSTARG;

	uint32 count = bootedZones;

	RETVAL = count;

	XSprePUSH; PUSHu((UV)RETVAL);
	XSRETURN(1);
}

XS(XS__getavailzonecount);
XS(XS__getavailzonecount)
{
	dXSARGS;

	int32	RETVAL;
	dXSTARG;

	uint32 count = availZones;

	RETVAL = count;

	XSprePUSH; PUSHu((UV)RETVAL);
	XSRETURN(1);
}

XS(XS__getzonepvp);
XS(XS__getzonepvp)
{
	dXSARGS;

	int32	RETVAL;
	dXSTARG;

	uint8 zoneid = zone->CanPVPValue();

	RETVAL = zoneid;

	XSprePUSH; PUSHu((UV)RETVAL);
	XSRETURN(1);
}

XS(XS__setzonepvp);
XS(XS__setzonepvp)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: setzonepvp(newvalue)");

	uint8 newvalue = (uint8)SvUV(ST(0));

	zone->SetCanPVPValue(newvalue);

	XSRETURN_EMPTY;
}

XS(XS__summonlootgroup);
XS(XS__summonlootgroup)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: summonlootgroup(group)");

	int lootgroupid = (int)SvIV(ST(0));

	quest_manager.summonlootgroup(lootgroupid);

	XSRETURN_EMPTY;
}

XS(XS__getzoneid);
XS(XS__getzoneid)
{
	dXSARGS;

	int32	RETVAL;
	dXSTARG;

	int32 zoneid = zone->GetZoneID();

	RETVAL = zoneid;

	XSprePUSH; PUSHu((IV)RETVAL);
	XSRETURN(1);
}

XS(XS__movepc);
XS(XS__movepc)
{
	dXSARGS;
	if (items != 4 && items != 5)
		Perl_croak(aTHX_ "Usage: movepc(zone_id, x, y, z [,heading])");

	int	zoneid = (int)SvIV(ST(0));
	float	x = (float)SvNV(ST(1));
	float	y = (float)SvNV(ST(2));
	float	z = (float)SvNV(ST(3));

	if (items == 4)

	quest_manager.movepc(zoneid, x, y, z, 0.0f);

	else {
	float	heading = (float)SvNV(ST(4));
	quest_manager.movepc(zoneid, x, y, z, heading);
	}

	XSRETURN_EMPTY;
}

XS(XS__gmmove);
XS(XS__gmmove)
{
	dXSARGS;
	if (items != 3)
		Perl_croak(aTHX_ "Usage: gmmove(x, y, z)");

	float	x = (float)SvNV(ST(0));
	float	y = (float)SvNV(ST(1));
	float	z = (float)SvNV(ST(2));

	quest_manager.gmmove(x, y, z);

	XSRETURN_EMPTY;
}

XS(XS__movegrp);
XS(XS__movegrp)
{
	dXSARGS;
	if (items != 4)
		Perl_croak(aTHX_ "Usage: movegrp(zoneid, x, y, z)");

	int	zoneid = (int)SvIV(ST(0));
	float	x = (float)SvNV(ST(1));
	float	y = (float)SvNV(ST(2));
	float	z = (float)SvNV(ST(3));

	quest_manager.movegrp(zoneid, x, y, z);

	XSRETURN_EMPTY;
}

XS(XS__doanim);
XS(XS__doanim)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: doanim(anim_id)");

	int	anim_id = (int)SvIV(ST(0));

	quest_manager.doanim(anim_id);

	XSRETURN_EMPTY;
}

XS(XS__addskill);
XS(XS__addskill)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: addskill(skill_id, value)");

	int	skill_id = (int)SvIV(ST(0));
	int	value = (int)SvIV(ST(1));

	quest_manager.addskill(skill_id, value);

	XSRETURN_EMPTY;
}

XS(XS__setlanguage);
XS(XS__setlanguage)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: setlanguage(skill_id, value)");

	int	skill_id = (int)SvIV(ST(0));
	int	value = (int)SvIV(ST(1));

	quest_manager.setlanguage(skill_id, value);

	XSRETURN_EMPTY;
}

XS(XS__setskill);
XS(XS__setskill)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: setskill(skill_id, value)");

	int	skill_id = (int)SvIV(ST(0));
	int	value = (int)SvIV(ST(1));

	quest_manager.setskill(skill_id, value);

	XSRETURN_EMPTY;
}

XS(XS__setallskill);
XS(XS__setallskill)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: setallskill(value)");

	int	value = (int)SvIV(ST(0));

	quest_manager.setallskill(value);

	XSRETURN_EMPTY;
}

XS(XS__attack);
XS(XS__attack)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: attack(client_name)");

	char *		client_name = (char *)SvPV_nolen(ST(0));

	quest_manager.attack(client_name);

	XSRETURN_EMPTY;
}

XS(XS__attacknpc);
XS(XS__attacknpc)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: attacknpc(npc_entity_id)");

	int	npc_entity_id = (int)SvIV(ST(0));

	quest_manager.attacknpc(npc_entity_id);

	XSRETURN_EMPTY;
}

XS(XS__attacknpctype);
XS(XS__attacknpctype)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: attacknpctype(npc_type_id)");

	int	npc_type_id = (int)SvIV(ST(0));

	quest_manager.attacknpctype(npc_type_id);

	XSRETURN_EMPTY;
}

// Kings & Bandits kingdom additions
XS(XS__setcitycitizentaxrate);
XS(XS__setcitycitizentaxrate)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: setcitycitizentaxrate(int:kingdomid,int:taxratepercent)");

	uint32	kingdomid = (uint32)SvIV(ST(0));
	uint8 taxratepct = (uint8)SvIV(ST(1));

	quest_manager.setcitycitizentaxrate(kingdomid,taxratepct);

	XSRETURN_EMPTY;
}

XS(XS__setcitynewbietaxrate);
XS(XS__setcitynewbietaxrate)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: setcitynewbietaxrate(int:kingdomid,int:taxratepercent)");

	uint32	kingdomid = (uint32)SvIV(ST(0));
	uint8 taxratepct = (uint8)SvIV(ST(1));

	quest_manager.setcitynewbietaxrate(kingdomid,taxratepct);

	XSRETURN_EMPTY;
}

XS(XS__getcitycitizentaxrate);
XS(XS__getcitycitizentaxrate)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: getcitycitizentaxrate(int:kingdomid)");

	uint32		RETVAL;
	dXSTARG;

	uint32	kingdomid = (uint32)SvIV(ST(0));

	RETVAL = quest_manager.getcitycitizentaxrate(kingdomid);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__getcitynewbietaxrate);
XS(XS__getcitynewbietaxrate)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: getcitynewbietaxrate(int:kingdomid)");

	uint32		RETVAL;
	dXSTARG;

	uint32	kingdomid = (uint32)SvIV(ST(0));

	RETVAL = quest_manager.getcitynewbietaxrate(kingdomid);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__setcitybankreserve);
XS(XS__setcitybankreserve)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: setcitybankreserve(int:kingdomid,uint32:reservevalue)");

	uint32	kingdomid = (uint32)SvIV(ST(0));
	uint32	value = (uint32)SvIV(ST(1));

	quest_manager.setcitybankreserve(kingdomid,value);

	XSRETURN_EMPTY;
}

XS(XS__getcitybankreserve);
XS(XS__getcitybankreserve)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: getcitybankreserve(int:kingdomid)");

	uint32		RETVAL;
	dXSTARG;

	uint32	kingdomid = (uint32)SvIV(ST(0));

	RETVAL = quest_manager.getcitybankreserve(kingdomid);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__getcityupkeepcost);
XS(XS__getcityupkeepcost)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: getcityupkeepcost(int:kingdomid)");

	uint32		RETVAL;
	dXSTARG;

	uint32	kingdomid = (uint32)SvIV(ST(0));

	RETVAL = quest_manager.getcityupkeepcost(kingdomid);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}



XS(XS__setcitymode);
XS(XS__setcitymode)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: setcitymode(int:kingdomid,int:citymode)");

	uint32	kingdomid = (uint32)SvIV(ST(0));
	uint8 mode = (uint8)SvIV(ST(1));

	quest_manager.setcitymode(kingdomid,mode,true);

	XSRETURN_EMPTY;
}

XS(XS__setcitylocation);
XS(XS__setcitylocation)
{
	dXSARGS;
	if (items != 6)
		Perl_croak(aTHX_ "Usage: setcitymode(int:kingdomid,float:x,float:y,float:z,float:radius,int:zoneid)");
	
	uint32	kingdomid = (uint32)SvIV(ST(0));
	float x = (float)SvNV(ST(1));
	float y = (float)SvNV(ST(2));
	float z = (float)SvNV(ST(3));
	float radius = (float)SvNV(ST(4));
	int32 zoneid = (int32)SvIV(ST(5));

	quest_manager.setcitylocation(kingdomid,x,y,z,radius,zoneid);

	XSRETURN_EMPTY;
}

XS(XS__getcitymode);
XS(XS__getcitymode)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: getcitymode(int:kingdomid)");

	uint32		RETVAL;
	dXSTARG;

	uint32	kingdomid = (uint32)SvIV(ST(0));

	RETVAL = quest_manager.getcitymode(kingdomid);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__getteamsettings);
XS(XS__getteamsettings)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: getteamsettings(int:kingdomid,char* fieldname)");

	uint32		RETVAL = 0;
	dXSTARG;

	uint32	kingdomid = (uint32)SvIV(ST(0));
	char *	field_name = (char *)SvPV_nolen(ST(1));
	TeamSettings_Struct* team = quest_manager.getteamsettings(kingdomid);
	if (team != NULL )
	{
		if ( !strcasecmp(field_name,"zoneid") )
		{
			RETVAL = team->zoneid;
		}
	}

	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__getteamlocation);
XS(XS__getteamlocation)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: getteamlocation(int:kingdomid,char* fieldname)");

	float		RETVAL = 0;
	dXSTARG;

	uint32	kingdomid = (uint32)SvIV(ST(0));
	char *	field_name = (char *)SvPV_nolen(ST(1));
	TeamSettings_Struct* team = quest_manager.getteamsettings(kingdomid);
	if (team != NULL )
	{
		if ( !strcasecmp(field_name,"x") )
		{
			RETVAL = team->x;
		}
		else if ( !strcasecmp(field_name,"y") )
		{
			RETVAL = team->y;
		}
		else if ( !strcasecmp(field_name,"z") )
		{
			RETVAL = team->z;
		}
		else if ( !strcasecmp(field_name,"zoneid") )
		{
			RETVAL = team->zoneid;
		}
	}
	
	XSprePUSH; PUSHn((double)RETVAL);

	XSRETURN(1);
}

// Kings & Bandits facenpctype addition
XS(XS__facenpctype);
XS(XS__facenpctype)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: facenpctype(npc_type_id)");

	int	npc_type_id = (int)SvIV(ST(0));

	quest_manager.facenpctype(npc_type_id);

	XSRETURN_EMPTY;
}

XS(XS__getteamexpbonus);
XS(XS__getteamexpbonus)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: getteamexpbonus(int:kingdomid)");

	uint32		RETVAL;
	dXSTARG;

	uint32	kingdomid = (uint32)SvIV(ST(0));

	RETVAL = quest_manager.getteamexpbonus(kingdomid);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__setteamexpbonus);
XS(XS__setteamexpbonus)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: setteamexpbonus(int:kingdomid,uint8:value)");

	uint32		RETVAL;
	dXSTARG;
	
	uint32	kingdomid = (uint32)SvIV(ST(0));
	uint8 xp = (uint8)SvIV(ST(1));

	RETVAL = quest_manager.setteamexpbonus(kingdomid,xp);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

// Kings & Bandits Guard Additions

XS(XS__addguardtype);
XS(XS__addguardtype)
{
	dXSARGS;
	if (items != 4)
		Perl_croak(aTHX_ "Usage: addguardtype(int:kingdomid,uint32:npctypeid,uint16:classid,uint32:spawngroupid)");

	uint32	kingdomid = (uint32)SvIV(ST(0));
	uint32	npcid = (uint32)SvIV(ST(1));
	uint16	class_ = (uint32)SvIV(ST(2));
	uint16	spawngroupid = (uint32)SvIV(ST(3));
	
	uint32		RETVAL;
	dXSTARG;

	RETVAL = quest_manager.addguardtype(kingdomid,npcid,class_,spawngroupid);
	XSprePUSH; PUSHu((UV)RETVAL);
	
	XSRETURN(1);
}


XS(XS__loadguardtype);
XS(XS__loadguardtype)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: loadguardtype(int:kingdomid,uint32:npctypeid)");

	uint32	kingdomid = (uint32)SvIV(ST(0));
	uint32	npcid = (uint32)SvIV(ST(1));
	
	uint32		RETVAL;
	dXSTARG;

	RETVAL = quest_manager.loadguardtype(kingdomid,npcid);
	XSprePUSH; PUSHu((UV)RETVAL);
	
	XSRETURN(1);
}

XS(XS__findcampteamid);
XS(XS__findcampteamid)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: findcampteamid(uint32:guildid)");

	uint32	guildid = (uint32)SvIV(ST(0));
	
	uint32		RETVAL;
	dXSTARG;

	RETVAL = quest_manager.findcampteamid(guildid);
	XSprePUSH; PUSHu((UV)RETVAL);
	
	XSRETURN(1);
}

XS(XS__findguardteamid);
XS(XS__findguardteamid)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: findguardteamid(uint32:npctypeid,uint32:spawngroupid)");

	uint32	npctypeid = (uint32)SvIV(ST(0));
	uint32	spawngroupid = (uint32)SvIV(ST(1));
	
	uint32		RETVAL;
	dXSTARG;

	RETVAL = quest_manager.findguardteamid(npctypeid,spawngroupid);
	XSprePUSH; PUSHu((UV)RETVAL);
	
	XSRETURN(1);
}


XS(XS__addteamid);
XS(XS__addteamid)
{
	dXSARGS;
	if (items != 8)
		Perl_croak(aTHX_ "Usage: addteamid(uint32 guildid, uint32 ownerteamid, uint32 charid, float:x, float:y, float:z, float:radius, uint32 zoneid)");

	uint32	guildid = (uint32)SvIV(ST(0));
	uint32	ownerteamid = (uint32)SvIV(ST(1));
	uint32	charid = (uint32)SvIV(ST(2));
	float	x = (float)SvNV(ST(3));
	float	y = (float)SvNV(ST(4));
	float	z = (float)SvNV(ST(5));
	float	radius = (float)SvNV(ST(6));
	uint32	zoneid = (uint32)SvIV(ST(7));
	
	uint32		RETVAL;
	dXSTARG;

	RETVAL = quest_manager.addteamid(guildid,ownerteamid,charid,x,y,z, radius, zoneid);
	XSprePUSH; PUSHu((UV)RETVAL);
	
	XSRETURN(1);
}


XS(XS__updateteamid);
XS(XS__updateteamid)
{
	dXSARGS;
	if (items != 8)
		Perl_croak(aTHX_ "Usage: updateteamid(uint32 guildid, uint32 ownerteamid, uint32 charid, float:x, float:y, float:z, float:radius, uint32 zoneid)");

	uint32	guildid = (uint32)SvIV(ST(0));
	uint32	ownerteamid = (uint32)SvIV(ST(1));
	uint32	charid = (uint32)SvIV(ST(2));
	float	x = (float)SvNV(ST(3));
	float	y = (float)SvNV(ST(4));
	float	z = (float)SvNV(ST(5));
	float	radius = (float)SvNV(ST(6));
	uint32	zoneid = (uint32)SvIV(ST(7));
	
	uint32		RETVAL;
	dXSTARG;

	RETVAL = quest_manager.updateteamid(guildid,ownerteamid,charid,x,y,z, radius, zoneid);
	XSprePUSH; PUSHu((UV)RETVAL);
	
	XSRETURN(1);
}


XS(XS__spawninnpc);
XS(XS__spawninnpc)
{
	dXSARGS;
	if (items != 8)
		Perl_croak(aTHX_ "Usage: spawninnpc(uint32 kingdomid, uint32 guildid, uint32 ownerteamid, uint32 charid, float:x, float:y, float:z, float:radius, uint32 zoneid)");
	
	uint32	kingdomid = (uint32)SvIV(ST(0));
	char *	spawnName = (char *)SvPV_nolen(ST(1));
	uint32	npcid = (uint32)SvIV(ST(2));
	float	x = (float)SvNV(ST(3));
	float	y = (float)SvNV(ST(4));
	float	z = (float)SvNV(ST(5));
	float	heading = (float)SvNV(ST(6));
	uint32	respawntime = (uint32)SvIV(ST(7));
	
	uint32		RETVAL;
	dXSTARG;

	RETVAL = quest_manager.spawninnpc(kingdomid,spawnName,npcid,x,y,z,heading,respawntime);
	XSprePUSH; PUSHu((UV)RETVAL);
	
	XSRETURN(1);
}

XS(XS__getguardfield);
XS(XS__getguardfield)
{
	dXSARGS;
	if (items != 3)
		Perl_croak(aTHX_ "Usage: getguardfield(int:kingdomid,uint32:npctypeid,char*:fieldname)");

	uint32	kingdomid = (uint32)SvIV(ST(0));
	uint32	npcid = (uint32)SvIV(ST(1));
	char *		field_name = (char *)SvPV_nolen(ST(2));
	
	uint32		RETVAL;
	dXSTARG;

	RETVAL = quest_manager.getguardfield(kingdomid,npcid,field_name);
	XSprePUSH; PUSHu((UV)RETVAL);
	
	XSRETURN(1);
}

XS(XS__resetguardfields);
XS(XS__resetguardfields)
{
	dXSARGS;
	if (items < 2)
		Perl_croak(aTHX_ "Usage: resetguardfields(int:kingdomid,uint32:npctypeid,int16:killerentid=0)");

	uint32	kingdomid = (uint32)SvIV(ST(0));
	uint32	npcid = (uint32)SvIV(ST(1));
	int16	killerid = 0;
	
	if ( items > 2 )
		killerid = (int16)SvIV(ST(2));

	uint32		RETVAL;
	dXSTARG;

	RETVAL = quest_manager.resetguardfields(kingdomid,npcid,killerid);
	XSprePUSH; PUSHu((UV)RETVAL);
	
	XSRETURN(1);
}

XS(XS__getisupkeeppaid);
XS(XS__getisupkeeppaid)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: getisupkeeppaid(int:kingdomid)");

	uint32		RETVAL;
	dXSTARG;

	uint32	kingdomid = (uint32)SvIV(ST(0));

	RETVAL = quest_manager.getisupkeeppaid(kingdomid);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__zonepointnearby);
XS(XS__zonepointnearby)
{
	dXSARGS;
	if (items != 4)
		Perl_croak(aTHX_ "Usage: zonepointnearby(float:x,float:y,float:z,float:radius)");

	uint32		RETVAL;
	dXSTARG;
	
	float	x = (float)SvNV(ST(0));
	float	y = (float)SvNV(ST(1));
	float	z = (float)SvNV(ST(2));
	float	radius = (float)SvNV(ST(3));

	RETVAL = quest_manager.zonepointnearby(x,y,z,radius);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__findnpcnear);
XS(XS__findnpcnear)
{
	dXSARGS;
	if (items != 4)
		Perl_croak(aTHX_ "Usage: findnpcnear(float:x,float:y,float:z,float:radius)");

	uint32		RETVAL;
	dXSTARG;
	
	float	x = (float)SvNV(ST(0));
	float	y = (float)SvNV(ST(1));
	float	z = (float)SvNV(ST(2));
	float	radius = (float)SvNV(ST(3));

	RETVAL = quest_manager.findnpcnear(x,y,z,radius);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}


XS(XS__locationnearby);
XS(XS__locationnearby)
{
	dXSARGS;
	if (items != 5)
		Perl_croak(aTHX_ "Usage: locationnearby(float:x,float:y,float:z,int:zoneid,float:maxdist)");

	uint32		RETVAL;
	dXSTARG;
	
	float	x = (float)SvNV(ST(0));
	float	y = (float)SvNV(ST(1));
	float	z = (float)SvNV(ST(2));
	int	zoneid = (int)SvIV(ST(3));
	float	maxdist = (float)SvNV(ST(4));

	RETVAL = KingsBandits::LocationNearby(x,y,z,zoneid,maxdist);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

// KaB / Red69 / LoZ additions on 4/29/2013

XS(XS__setkingdomowner);
XS(XS__setkingdomowner)
{
	dXSARGS;
	if (items != 4)
		Perl_croak(aTHX_ "Usage: setkingdomowner(int:kingdomid,int32:guildid,int32:charid,int32:teamid)");

	uint32		RETVAL;
	dXSTARG;
	
	uint32	kingdomid = (uint32)SvIV(ST(0));
	int32 guildid = (int32)SvIV(ST(1));
	int32 charid = (int32)SvIV(ST(2));
	int32 teamid = (int32)SvIV(ST(3));

	RETVAL = quest_manager.setkingdomowner(kingdomid,guildid,charid,teamid);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__getkingdomownercharid);
XS(XS__getkingdomownercharid)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: getkingdomownercharid(int:kingdomid)");

	int32		RETVAL;
	dXSTARG;

	uint32	kingdomid = (uint32)SvIV(ST(0));

	RETVAL = quest_manager.getkingdomownercharid(kingdomid);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}
XS(XS__getkingdomownerguildid);
XS(XS__getkingdomownerguildid)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: getkingdomownerguildid(int:kingdomid)");

	int32		RETVAL;
	dXSTARG;

	uint32	kingdomid = (uint32)SvIV(ST(0));

	RETVAL = quest_manager.getkingdomownerguildid(kingdomid);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__setkingdomownerlastvisit);
XS(XS__setkingdomownerlastvisit)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: setkingdomownerlastvisit(int:kingdomid,int32:timestamp)");

	uint32		RETVAL;
	dXSTARG;
	
	uint32	kingdomid = (uint32)SvIV(ST(0));
	int32 timestamp = (int32)SvIV(ST(1));

	RETVAL = quest_manager.setkingdomownerlastvisit(kingdomid,timestamp);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__getkingdomownerlastvisit);
XS(XS__getkingdomownerlastvisit)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: getkingdomownerlastvisit(int:kingdomid)");

	int32		RETVAL;
	dXSTARG;

	uint32	kingdomid = (uint32)SvIV(ST(0));

	RETVAL = quest_manager.getkingdomownerlastvisit(kingdomid);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__getmaxlevel);
XS(XS__getmaxlevel)
{
	dXSARGS;
	if (items != 0)
		Perl_croak(aTHX_ "Usage: getmaxlevel()");

	int32		RETVAL;
	dXSTARG;

	RETVAL = RuleI(Character, MaxLevel);
	XSprePUSH; PUSHu((UV)RETVAL);

	XSRETURN(1);
}

XS(XS__save);
XS(XS__save)
{
	dXSARGS;
	if (items != 0)
		Perl_croak(aTHX_ "Usage: save()");


	quest_manager.save();

	XSRETURN_EMPTY;
}

XS(XS__faction);
XS(XS__faction)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: faction(faction_id, faction_value)");

	int	faction_id = (int)SvIV(ST(0));
	int	faction_value = (int)SvIV(ST(1));

	quest_manager.faction(faction_id, faction_value);

	XSRETURN_EMPTY;
}

XS(XS__setsky);
XS(XS__setsky)
{
	dXSARGS;
	if (items < 1)
		Perl_croak(aTHX_ "Usage: setsky(new_sky,red=0,green=0,blue=0)");

		unsigned char		new_sky = (unsigned char)SvUV(ST(0));
		uint8 red = 0;
		uint8 green = 0;
		uint8 blue = 0;
		bool override_ = false;
		if ( items > 0 )
		{
			red = (uint8)SvIV(ST(1));
			override_ = true;
		}
		if ( items > 1 )	
		{
			green = (uint8)SvIV(ST(2));
			override_ = true;
		}
		if ( items > 2 )	
		{
			blue = (uint8)SvIV(ST(3));
			override_ = true;
		}


	quest_manager.setsky(new_sky,red,green,blue,override_);

	XSRETURN_EMPTY;
}

XS(XS__setguild);
XS(XS__setguild)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: setguild(new_guild_id, new_rank)");

		unsigned long		new_guild_id = (unsigned long)SvUV(ST(0));
	int	new_rank = (int)SvIV(ST(1));

	quest_manager.setguild(new_guild_id, new_rank);

	XSRETURN_EMPTY;
}

XS(XS__settime);
XS(XS__settime)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: settime(new_hour, new_min)");

	int	new_hour = (int)SvIV(ST(0));
	int	new_min = (int)SvIV(ST(1));

	quest_manager.settime(new_hour, new_min);

	XSRETURN_EMPTY;
}

XS(XS__itemlink);
XS(XS__itemlink)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: itemlink(item_id)");

	int	item_id = (int)SvIV(ST(0));

	quest_manager.itemlink(item_id);

	XSRETURN_EMPTY;
}

XS(XS__signalwith);
XS(XS__signalwith)
{
	dXSARGS;

	if (items == 2) {
		int	npc_id = (int)SvIV(ST(0));
		int	signal_id = (int)SvIV(ST(1));
		quest_manager.signalwith(npc_id, signal_id);
	} else if(items == 3) {
		int	npc_id = (int)SvIV(ST(0));
		int	signal_id = (int)SvIV(ST(1));
		int	wait = (int)SvIV(ST(2));
		quest_manager.signalwith(npc_id, signal_id, wait);
	} else {
		Perl_croak(aTHX_ "Usage: signalwith(npc_id,signal_id[,wait_ms])");
	}

	XSRETURN_EMPTY;
}

XS(XS__signal);
XS(XS__signal)
{
	dXSARGS;

	if (items == 1) {
		int	npc_id = (int)SvIV(ST(0));
		quest_manager.signal(npc_id);
	} else if(items == 2) {
		int	npc_id = (int)SvIV(ST(0));
		int	wait = (int)SvIV(ST(1));
		quest_manager.signal(npc_id, wait);
	} else {
		Perl_croak(aTHX_ "Usage: signal(npc_id[,wait_ms])");
	}

	XSRETURN_EMPTY;
}

XS(XS__setglobal);
XS(XS__setglobal)
{
	dXSARGS;
	if (items != 4)
		Perl_croak(aTHX_ "Usage: setglobal(varname, newvalue, options, duration)");

	char *		varname = (char *)SvPV_nolen(ST(0));
	char *		newvalue = (char *)SvPV_nolen(ST(1));
	int	options = (int)SvIV(ST(2));
	char *		duration = (char *)SvPV_nolen(ST(3));

	quest_manager.setglobal(varname, newvalue, options, duration);

	XSRETURN_EMPTY;
}

XS(XS__targlobal);
XS(XS__targlobal)
{
	dXSARGS;
	if (items != 6)
		Perl_croak(aTHX_ "Usage: targlobal(varname, value, duration, npcid, charid, zoneid)");

	char *		varname = (char *)SvPV_nolen(ST(0));
	char *		value = (char *)SvPV_nolen(ST(1));
	char *		duration = (char *)SvPV_nolen(ST(2));
	int	npcid = (int)SvIV(ST(3));
	int	charid = (int)SvIV(ST(4));
	int	zoneid = (int)SvIV(ST(5));

	quest_manager.targlobal(varname, value, duration, npcid, charid, zoneid);

	XSRETURN_EMPTY;
}

XS(XS__delglobal);
XS(XS__delglobal)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: delglobal(varname)");

	char *		varname = (char *)SvPV_nolen(ST(0));

	quest_manager.delglobal(varname);

	XSRETURN_EMPTY;
}

XS(XS__ding);
XS(XS__ding)
{
	dXSARGS;
	if (items != 0)
		Perl_croak(aTHX_ "Usage: ding()");


	quest_manager.ding();

	XSRETURN_EMPTY;
}

XS(XS__rebind);
XS(XS__rebind)
{
	dXSARGS;
	if (items != 4)
		Perl_croak(aTHX_ "Usage: rebind(zoneid, x, y, z)");

	int	zoneid = (int)SvIV(ST(0));
	float	x = (float)SvNV(ST(1));
	float	y = (float)SvNV(ST(2));
	float	z = (float)SvNV(ST(3));

	quest_manager.rebind(zoneid, x, y, z);

	XSRETURN_EMPTY;
}

XS(XS__start);
XS(XS__start)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: start(wp)");

	int	wp = (int)SvIV(ST(0));

	quest_manager.start(wp);

	XSRETURN_EMPTY;
}

XS(XS__stop);
XS(XS__stop)
{
	dXSARGS;
	if (items != 0)
		Perl_croak(aTHX_ "Usage: stop()");


	quest_manager.stop();

	XSRETURN_EMPTY;
}

XS(XS__pause);
XS(XS__pause)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: pause(duration)");

	int	duration = (int)SvIV(ST(0));

	quest_manager.pause(duration);

	XSRETURN_EMPTY;
}

XS(XS__moveto);
XS(XS__moveto)
{
	dXSARGS;
	if (items != 3 && items != 4 && items != 5)
		Perl_croak(aTHX_ "Usage: moveto(x, y, z, [mth, saveguard?])");

	float	x = (float)SvNV(ST(0));
	float	y = (float)SvNV(ST(1));
	float	z = (float)SvNV(ST(2));
	float	h;
	bool	saveguard;

	if(items > 3)
		h = (float)SvNV(ST(3));
	else
		h = 0;

	if(items > 4)
		saveguard = (bool)SvTRUE(ST(4));
	else
		saveguard = false;

	quest_manager.moveto(x, y, z, h, saveguard);

	XSRETURN_EMPTY;
}

XS(XS__resume);
XS(XS__resume)
{
	dXSARGS;
	if (items != 0)
		Perl_croak(aTHX_ "Usage: resume()");


	quest_manager.resume();

	XSRETURN_EMPTY;
}

XS(XS__addldonpoints);
XS(XS__addldonpoints)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: addldonpoints(points, theme)");

	long	points = (long)SvIV(ST(0));
	unsigned long		theme = (unsigned long)SvUV(ST(1));

	quest_manager.addldonpoints(points, theme);

	XSRETURN_EMPTY;
}

XS(XS__setnexthpevent);
XS(XS__setnexthpevent)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: setnexthpevent(at)");

	int	at = (int)SvIV(ST(0));

	quest_manager.setnexthpevent(at);

	XSRETURN_EMPTY;
}

XS(XS__setnextinchpevent);
XS(XS__setnextinchpevent)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: setnextinchpevent(at)");

	int	at = (int)SvIV(ST(0));

	quest_manager.setnextinchpevent(at);

	XSRETURN_EMPTY;
}

XS(XS__sethp);
XS(XS__sethp)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: sethp(percentage)");

	int	hpperc = (int)SvIV(ST(0));

	quest_manager.sethp(hpperc);

	XSRETURN_EMPTY;
}

XS(XS__respawn);
XS(XS__respawn)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: respawn(npc_type, grid)");

	int	npc_type = (int)SvIV(ST(0));
	int	grid = (int)SvIV(ST(1));

	quest_manager.respawn(npc_type, grid);

	XSRETURN_EMPTY;
}

XS(XS__ChooseRandom);
XS(XS__ChooseRandom)
{
	dXSARGS;
	if (items < 1)
		Perl_croak(aTHX_ "Usage: ChooseRandom(... list ...)");

	int index = MakeRandomInt(0, items-1);

	SV *tmp = ST(0);
	ST(0) = ST(index);
	ST(index) = tmp;

	XSRETURN(1);	//return 1 element from the stack (ST(0))
}

XS(XS__set_proximity);
XS(XS__set_proximity)
{
	dXSARGS;
	if (items != 4 && items != 6)
		Perl_croak(aTHX_ "Usage: set_proximity(minx, maxx, miny, maxy [, minz, maxz])");

	float minx = (float)SvNV(ST(0));
	float maxx = (float)SvNV(ST(1));
	float miny = (float)SvNV(ST(2));
	float maxy = (float)SvNV(ST(3));

	if(items == 4)
		quest_manager.set_proximity(minx, maxx, miny, maxy);
	else {
		float minz = (float)SvNV(ST(4));
		float maxz = (float)SvNV(ST(5));
		quest_manager.set_proximity(minx, maxx, miny, maxy, minz, maxz);
	}

	XSRETURN_EMPTY;
}

XS(XS__clear_proximity);
XS(XS__clear_proximity)
{
	dXSARGS;
	if (items != 0)
		Perl_croak(aTHX_ "Usage: clear_proximity()");

	quest_manager.clear_proximity();

	XSRETURN_EMPTY;
}

XS(XS__setanim);
XS(XS__setanim) //Cisyouc: mob->setappearance() addition
{
	dXSARGS;
	if(items != 2)
		Perl_croak(aTHX_ "Usage: quest::setanim(npc_type, animnum);");

	quest_manager.setanim(SvUV(ST(0)), SvUV(ST(1)));

	XSRETURN_EMPTY;
}

XS(XS__showgrid);
XS(XS__showgrid)
{
	dXSARGS;
	if(items != 1)
		Perl_croak(aTHX_ "Usage: quest::showgrid(grid_id);");

	quest_manager.showgrid(SvUV(ST(0)));

	XSRETURN_EMPTY;
}

XS(XS__showpath);
XS(XS__showpath)
{
	dXSARGS;
	if (items != 3)
		Perl_croak(aTHX_ "Usage: showpath(x, y, z)");

	float	x = (float)SvNV(ST(0));
	float	y = (float)SvNV(ST(1));
	float	z = (float)SvNV(ST(2));

	quest_manager.showpath(x, y, z);

	XSRETURN_EMPTY;
}

XS(XS__pathto);
XS(XS__pathto)
{
	dXSARGS;
	if (items != 3)
		Perl_croak(aTHX_ "Usage: pathto(x, y, z)");

	float	x = (float)SvNV(ST(0));
	float	y = (float)SvNV(ST(1));
	float	z = (float)SvNV(ST(2));

	quest_manager.pathto(x, y, z);

	XSRETURN_EMPTY;
}

XS(XS__spawn_condition);
XS(XS__spawn_condition)
{
	dXSARGS;
	if (items < 3 || items > 4)
		Perl_croak(aTHX_ "Usage: spawn_condition(zone_short, [instance_id], condition_id, value)");

	if(items == 3)
	{
		char *	zone_short = (char *)SvPV_nolen(ST(0));
		uint16	cond_id = (int)SvUV(ST(1));
		sint16	value = (int)SvIV(ST(2));

		quest_manager.spawn_condition(zone_short, zone->GetInstanceID(), cond_id, value);
	}
	else
	{
		char *	zone_short = (char *)SvPV_nolen(ST(0));
		uint32	instance_id = (int)SvUV(ST(1));
		uint16	cond_id = (int)SvUV(ST(2));
		sint16	value = (int)SvIV(ST(3));

		quest_manager.spawn_condition(zone_short, instance_id, cond_id, value);
	}
	XSRETURN_EMPTY;
}

XS(XS__get_spawn_condition);
XS(XS__get_spawn_condition)
{
	dXSARGS;
	if (items < 2 || items > 3)
		Perl_croak(aTHX_ "Usage: get_spawn_condition(zone_short, [instance_id], condition_id)");

	if(items == 2)
	{
		sint16		RETVAL;
		dXSTARG;

		char *	zone_short = (char *)SvPV_nolen(ST(0));
		uint16	cond_id = (int)SvIV(ST(1));

		RETVAL = quest_manager.get_spawn_condition(zone_short, zone->GetInstanceID(), cond_id);
		XSprePUSH; PUSHu((IV)RETVAL);

		XSRETURN(1);
	}
	else
	{
		sint16		RETVAL;
		dXSTARG;

		char *	zone_short = (char *)SvPV_nolen(ST(0));
		uint16	instance_id = (int)SvIV(ST(1));
		uint16	cond_id = (int)SvIV(ST(2));

		RETVAL = quest_manager.get_spawn_condition(zone_short, instance_id, cond_id);
		XSprePUSH; PUSHu((IV)RETVAL);

		XSRETURN(1);
	}
}

XS(XS__toggle_spawn_event);
XS(XS__toggle_spawn_event)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: toggle_spawn_event(event_id, enabled?, reset_base)");

	int32	event_id = (int)SvIV(ST(0));
	bool	enabled = ((int)SvIV(ST(1))) == 0?false:true;
	bool	reset_base = ((int)SvIV(ST(1))) == 0?false:true;

	quest_manager.toggle_spawn_event(event_id, enabled, reset_base);

	XSRETURN_EMPTY;
}

XS(XS__has_zone_flag);
XS(XS__has_zone_flag)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: has_zone_flag(zone_id)");

	sint16		RETVAL;
	dXSTARG;

	int32	zone_id = (int)SvIV(ST(0));

	RETVAL = quest_manager.has_zone_flag(zone_id);
	XSprePUSH; PUSHu((IV)RETVAL);

	XSRETURN(1);

}

XS(XS__set_zone_flag);
XS(XS__set_zone_flag)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: set_zone_flag(zone_id)");

	int32	zone_id = (int)SvIV(ST(0));

	quest_manager.set_zone_flag(zone_id);

	XSRETURN_EMPTY;
}

XS(XS__clear_zone_flag);
XS(XS__clear_zone_flag)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: clear_zone_flag(zone_id)");

	int32	zone_id = (int)SvIV(ST(0));

	quest_manager.clear_zone_flag(zone_id);

	XSRETURN_EMPTY;
}

XS(XS__summonburriedplayercorpse);
XS(XS__summonburriedplayercorpse)
{
    dXSARGS;
    if (items != 5)
        Perl_croak(aTHX_ "Usage: summonburriedplayercorpse(char_id,dest_x,dest_y,dest_z,dest_heading)");

    bool RETVAL;
    int32    char_id = (int)SvIV(ST(0));
    float    dest_x = (float)SvIV(ST(1));
    float    dest_y = (float)SvIV(ST(2));
    float    dest_z = (float)SvIV(ST(3));
    float    dest_heading = (float)SvIV(ST(4));

    RETVAL = quest_manager.summonburriedplayercorpse(char_id, dest_x, dest_y, dest_z, dest_heading);

    ST(0) = boolSV(RETVAL);
    sv_2mortal(ST(0));
    XSRETURN(1);
}

XS(XS__getnextcorpsedecaytime);
XS(XS__getnextcorpsedecaytime)
{
    dXSARGS;
    if (items != 1)
        Perl_croak(aTHX_ "Usage: getnextcorpsedecaytime(char_id)");

    int32        RETVAL;
    dXSTARG;

    int32    char_id = (int)SvIV(ST(0));

    RETVAL = quest_manager.getnextcorpsedecaytime(char_id);
    XSprePUSH; PUSHu((IV)RETVAL);

    XSRETURN(1);
}

XS(XS__getplayerburriedcorpsecount);
XS(XS__getplayerburriedcorpsecount)
{
    dXSARGS;
    if (items != 1)
        Perl_croak(aTHX_ "Usage: getplayerburriedcorpsecount(char_id)");

    int32        RETVAL;
    dXSTARG;

    int32    char_id = (int)SvIV(ST(0));

    RETVAL = quest_manager.getplayerburriedcorpsecount(char_id);
    XSprePUSH; PUSHu((IV)RETVAL);

    XSRETURN(1);
}

XS(XS__buryplayercorpse);
XS(XS__buryplayercorpse)
{
    dXSARGS;
    if (items != 1)
        Perl_croak(aTHX_ "Usage: buryplayercorpse(char_id)");

    int32        RETVAL;
    dXSTARG;

    int32    char_id = (int)SvIV(ST(0));

    RETVAL = quest_manager.buryplayercorpse(char_id);
    XSprePUSH; PUSHu((IV)RETVAL);

    XSRETURN(1);
}

XS(XS__forcedooropen);
XS(XS__forcedooropen)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: forcedooropen(doorid)");

	int32	did = (int)SvIV(ST(0));

	quest_manager.forcedooropen(did);

	XSRETURN_EMPTY;
}

XS(XS__forcedoorclose);
XS(XS__forcedoorclose)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: forcedoorclose(doorid)");

	int32	did = (int)SvIV(ST(0));

	quest_manager.forcedoorclose(did);

	XSRETURN_EMPTY;
}

XS(XS__isdooropen);
XS(XS__isdooropen)
{
    dXSARGS;
    if (items != 1)
        Perl_croak(aTHX_ "Usage: isdooropen(doorid)");

    bool        RETVAL;
    dXSTARG;

    int32    doorid = (int)SvIV(ST(0));

	RETVAL = quest_manager.isdooropen(doorid);
	XSprePUSH; PUSHu((IV)RETVAL);

	XSRETURN(1);
}

XS(XS__depopzone);
XS(XS__depopzone)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: depopzone(StartSpawnStatus)");

	bool StartSpawnStatus = ((int)SvIV(ST(0))) == 0?false:true;

	quest_manager.depopzone(StartSpawnStatus);

	XSRETURN_EMPTY;
}

XS(XS__repopzone);
XS(XS__repopzone)
{
	dXSARGS;
	if (items != 0)
		Perl_croak(aTHX_ "Usage: repopzone()");

	quest_manager.repopzone();

	XSRETURN_EMPTY;
}

XS(XS__npcrace);
XS(XS__npcrace)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: npcrace(race_id)");

	int	race_id = (int)SvIV(ST(0));

	quest_manager.npcrace(race_id);

	XSRETURN_EMPTY;
}

XS(XS__npcgender);
XS(XS__npcgender)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: npcgender(gender_id)");

	int	gender_id= (int)SvIV(ST(0));

	quest_manager.npcgender(gender_id);

	XSRETURN_EMPTY;
}

XS(XS__npcsize);
XS(XS__npcsize)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: npcsize(newsize)");

	int	newsize = (int)SvIV(ST(0));

	quest_manager.npcsize(newsize);

	XSRETURN_EMPTY;
}

XS(XS__npctexture);
XS(XS__npctexture)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: npctexture(newtexture)");

	int	newtexture = (int)SvIV(ST(0));

	quest_manager.npctexture(newtexture);

	XSRETURN_EMPTY;
}

XS(XS__playerrace);
XS(XS__playerrace)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: playerrace(race_id)");

	int	race_id = (int)SvIV(ST(0));

	quest_manager.playerrace(race_id);

	XSRETURN_EMPTY;
}

XS(XS__playergender);
XS(XS__playergender)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: playergender(gender_id)");

	int	gender_id= (int)SvIV(ST(0));

	quest_manager.playergender(gender_id);

	XSRETURN_EMPTY;
}

XS(XS__playersize);
XS(XS__playersize)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: playersize(newsize)");

	int	newsize = (int)SvIV(ST(0));

	quest_manager.playersize(newsize);

	XSRETURN_EMPTY;
}

XS(XS__playertexture);
XS(XS__playertexture)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: playertexture(newtexture)");

	int	newtexture = (int)SvIV(ST(0));

	quest_manager.playertexture(newtexture);

	XSRETURN_EMPTY;
}

XS(XS__playerfeature);
XS(XS__playerfeature)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: playerfeature(feature, setting)");

	char *	feature	= (char *)SvPV_nolen(ST(0));
	int		setting	= (int)SvIV(ST(1));

	quest_manager.playerfeature(feature, setting);

	XSRETURN_EMPTY;
}

XS(XS__npcfeature);
XS(XS__npcfeature)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: npcfeature(feature, setting)");

	char *	feature	= (char *)SvPV_nolen(ST(0));
	int		setting	= (int)SvIV(ST(1));

	quest_manager.npcfeature(feature, setting);

	XSRETURN_EMPTY;
}

#ifdef BOTS

XS(XS__createbotcount);
XS(XS__createbotcount)
{
    dXSARGS;
	int        RETVAL;
    dXSTARG;

	RETVAL = quest_manager.createbotcount();
	XSprePUSH; PUSHi((IV)RETVAL);

	XSRETURN(1);
}

XS(XS__spawnbotcount);
XS(XS__spawnbotcount)
{
    dXSARGS;
	int        RETVAL;
    dXSTARG;

	RETVAL = quest_manager.spawnbotcount();
	XSprePUSH; PUSHi((IV)RETVAL);

	XSRETURN(1);
}

XS(XS__botquest);
XS(XS__botquest)
{
    dXSARGS;
    bool        RETVAL;
    dXSTARG;

	RETVAL = quest_manager.botquest();
	XSprePUSH; PUSHu((IV)RETVAL);

	XSRETURN(1);
}

XS(XS__createBot);
XS(XS__createBot)
{
	dXSARGS;
	bool		RETVAL;
	dXSTARG;

	if(items != 6)
	{
		Perl_croak(aTHX_ "Usage: createBot(firstname, lastname, level, race, class, gender)");
	}

	char *firstname = (char *)SvPV_nolen(ST(0));
	char *lastname = (char *)SvPV_nolen(ST(1));
	int level = (int) SvIV(ST(2));
	int race = (int) SvIV(ST(3));
	int botclass = (int) SvIV(ST(4));
	int gender = (int) SvIV(ST(5));

	RETVAL = quest_manager.createBot(firstname, lastname, level, race, botclass, gender);
	XSprePUSH; PUSHu((IV)RETVAL);
	XSRETURN(1);
}

#endif //BOTS

XS(XS__taskselector);
XS(XS__taskselector)
{
	dXSARGS;
	if((items >= 1) && (items <=MAXCHOOSERENTRIES)) {
		int tasks[MAXCHOOSERENTRIES];
		for(int i=0; i< items; i++) {
			tasks[i] = (int)SvIV(ST(i));
		}
		quest_manager.taskselector(items, tasks);
	} else {
		Perl_croak(aTHX_ "Usage: taskselector(taskid1, taskid2, ..., taskid%i)", MAXCHOOSERENTRIES);
	}

	XSRETURN_EMPTY;
}
XS(XS__tasksetselector);
XS(XS__tasksetselector)
{
	dXSARGS;
	if(items == 1) {
		int tasksetid = (int)SvIV(ST(0));
		quest_manager.tasksetselector(tasksetid);
	} else {
		Perl_croak(aTHX_ "Usage: tasksetselector(tasksetid)");
	}

	XSRETURN_EMPTY;
}
XS(XS__enabletask);
XS(XS__enabletask)
{
	dXSARGS;
	if((items >= 1) && (items <=10)) {
		int tasks[10];
		for(int i=0; i< items; i++) {
			tasks[i] = (int)SvIV(ST(i));
		}
		quest_manager.enabletask(items, tasks);
	} else {
		Perl_croak(aTHX_ "Usage: enabletask(taskid1, taskid2, ..., taskid10");
	}

	XSRETURN_EMPTY;
}
XS(XS__disabletask);
XS(XS__disabletask)
{
	dXSARGS;
	if((items >= 1) && (items <=10)) {
		int tasks[10];
		for(int i=0; i< items; i++) {
			tasks[i] = (int)SvIV(ST(i));
		}
		quest_manager.disabletask(items, tasks);
	} else {
		Perl_croak(aTHX_ "Usage: disabletask(taskid1, taskid2, ..., taskid10");
	}

	XSRETURN_EMPTY;
}

XS(XS__istaskenabled);
XS(XS__istaskenabled)
{
	dXSARGS;
	bool        RETVAL;
	dXSTARG;

	if(items == 1) {
		unsigned int taskid = (int)SvIV(ST(0));
		RETVAL = quest_manager.istaskenabled(taskid);
	} else {
		Perl_croak(aTHX_ "Usage: istaskenabled(taskid)");
	}

	XSprePUSH; PUSHu((IV)RETVAL);

	XSRETURN(1);
}
XS(XS__istaskactive);
XS(XS__istaskactive)
{
	dXSARGS;
	bool        RETVAL;
	dXSTARG;

	if(items == 1) {
		unsigned int task = (int)SvIV(ST(0));
		RETVAL = quest_manager.istaskactive(task);
	} else {
		Perl_croak(aTHX_ "Usage: istaskactive(task)");
	}

	XSprePUSH; PUSHu((IV)RETVAL);

	XSRETURN(1);
}
XS(XS__istaskactivityactive);
XS(XS__istaskactivityactive)
{
	dXSARGS;
	bool        RETVAL;
	dXSTARG;

	if(items == 2) {
		unsigned int task = (int)SvIV(ST(0));
		unsigned int activity = (int)SvIV(ST(1));
		RETVAL = quest_manager.istaskactivityactive(task, activity);
	} else {
		Perl_croak(aTHX_ "Usage: istaskactivityactive(task,activity)");
	}

	XSprePUSH; PUSHu((IV)RETVAL);

	XSRETURN(1);
}
XS(XS__updatetaskactivity);
XS(XS__updatetaskactivity)
{
	dXSARGS;
	unsigned int task, activity;
	int count = 1;
	if(items == 2) {
		task = (int)SvIV(ST(0));
		activity = (int)SvIV(ST(1));
		quest_manager.updatetaskactivity(task, activity, count);
	}
	else if(items == 3) {
		task = (int)SvIV(ST(0));
		activity = (int)SvIV(ST(1));
		count = (int)SvIV(ST(2));
		quest_manager.updatetaskactivity(task, activity, count);
	} else {
		Perl_croak(aTHX_ "Usage: updatetaskactivity(task, activity [,count])");
	}

	XSRETURN_EMPTY;
}

XS(XS__resettaskactivity);
XS(XS__resettaskactivity)
{
	dXSARGS;
	unsigned int task, activity;
	if(items == 2) {
		task = (int)SvIV(ST(0));
		activity = (int)SvIV(ST(1));
		quest_manager.resettaskactivity(task, activity);
	} else {
		Perl_croak(aTHX_ "Usage: resettaskactivity(task, activity)");
	}

	XSRETURN_EMPTY;
}

XS(XS__taskexploredarea);
XS(XS__taskexploredarea)
{
	dXSARGS;
	unsigned int exploreid;
	if(items == 1) {
		exploreid = (int)SvIV(ST(0));
		quest_manager.taskexploredarea(exploreid);
	} else {
		Perl_croak(aTHX_ "Usage: taskexplorearea(exploreid)");
	}

	XSRETURN_EMPTY;
}

XS(XS__assigntask);
XS(XS__assigntask)
{
	dXSARGS;
	unsigned int taskid;
	if(items == 1) {
		taskid = (int)SvIV(ST(0));
		quest_manager.assigntask(taskid);
	} else {
		Perl_croak(aTHX_ "Usage: assigntask(taskid)");
	}

	XSRETURN_EMPTY;
}

XS(XS__failtask);
XS(XS__failtask)
{
	dXSARGS;
	unsigned int taskid;
	if(items == 1) {
		taskid = (int)SvIV(ST(0));
		quest_manager.failtask(taskid);
	} else {
		Perl_croak(aTHX_ "Usage: failtask(taskid)");
	}

	XSRETURN_EMPTY;
}

XS(XS__tasktimeleft);
XS(XS__tasktimeleft)
{
	dXSARGS;
	int        RETVAL;
	dXSTARG;

	if(items == 1) {
		unsigned int taskid = (int)SvIV(ST(0));
		RETVAL = quest_manager.tasktimeleft(taskid);
	} else {
		Perl_croak(aTHX_ "Usage: tasktimeleft(taskid)");
	}

	XSprePUSH; PUSHi((IV)RETVAL);

	XSRETURN(1);
}

XS(XS__istaskcompleted);
XS(XS__istaskcompleted)
{
	dXSARGS;
	int        RETVAL;
	dXSTARG;

	if(items == 1) {
		unsigned int taskid = (int)SvIV(ST(0));
		RETVAL = quest_manager.istaskcompleted(taskid);
	} else {
		Perl_croak(aTHX_ "Usage: istaskcompleted(taskid)");
	}

	XSprePUSH; PUSHi((IV)RETVAL);

	XSRETURN(1);
}

XS(XS__enabledtaskcount);
XS(XS__enabledtaskcount)
{
	dXSARGS;
	int        RETVAL;
	dXSTARG;

	if(items == 1) {
		unsigned int taskset = (int)SvIV(ST(0));
		RETVAL = quest_manager.enabledtaskcount(taskset);
	} else {
		Perl_croak(aTHX_ "Usage: enabledtaskcount(taskset)");
	}

	XSprePUSH; PUSHi((IV)RETVAL);

	XSRETURN(1);
}

XS(XS__firsttaskinset);
XS(XS__firsttaskinset)
{
	dXSARGS;
	int        RETVAL;
	dXSTARG;

	if(items == 1) {
		unsigned int taskset = (int)SvIV(ST(0));
		RETVAL = quest_manager.firsttaskinset(taskset);
	} else {
		Perl_croak(aTHX_ "Usage: firsttaskinset(taskset)");
	}

	XSprePUSH; PUSHi((IV)RETVAL);

	XSRETURN(1);
}

XS(XS__lasttaskinset);
XS(XS__lasttaskinset)
{
	dXSARGS;
	int        RETVAL;
	dXSTARG;

	if(items == 1) {
		unsigned int taskset = (int)SvIV(ST(0));
		RETVAL = quest_manager.lasttaskinset(taskset);
	} else {
		Perl_croak(aTHX_ "Usage: lasttaskinset(taskset)");
	}

	XSprePUSH; PUSHi((IV)RETVAL);

	XSRETURN(1);
}

XS(XS__nexttaskinset);
XS(XS__nexttaskinset)
{
	dXSARGS;
	int        RETVAL;
	dXSTARG;

	if(items == 2) {
		unsigned int taskset = (int)SvIV(ST(0));
		unsigned int taskid = (int)SvIV(ST(1));
		RETVAL = quest_manager.nexttaskinset(taskset, taskid);
	} else {
		Perl_croak(aTHX_ "Usage: nexttaskinset(taskset, taskid)");
	}

	XSprePUSH; PUSHi((IV)RETVAL);

	XSRETURN(1);
}
XS(XS__activespeaktask);
XS(XS__activespeaktask)
{
	dXSARGS;
	int        RETVAL;
	dXSTARG;

	if(items == 0) {
		RETVAL = quest_manager.activespeaktask();
	} else {
		Perl_croak(aTHX_ "Usage: activespeaktask()");
	}

	XSprePUSH; PUSHi((IV)RETVAL);

	XSRETURN(1);
}

XS(XS__activespeakactivity);
XS(XS__activespeakactivity)
{
	dXSARGS;
	int        RETVAL;
	dXSTARG;

	if(items == 1) {
		unsigned int taskid = (int)SvIV(ST(0));
		RETVAL = quest_manager.activespeakactivity(taskid);
	} else {
		Perl_croak(aTHX_ "Usage: activespeakactivity(taskid)");
	}

	XSprePUSH; PUSHi((IV)RETVAL);

	XSRETURN(1);
}

XS(XS__activetasksinset);
XS(XS__activetasksinset)
{
	dXSARGS;
	int        RETVAL;
	dXSTARG;

	if(items == 1) {
		unsigned int taskset = (int)SvIV(ST(0));
		RETVAL = quest_manager.activetasksinset(taskset);
	} else {
		Perl_croak(aTHX_ "Usage: activetasksinset(taskset)");
	}

	XSprePUSH; PUSHi((IV)RETVAL);

	XSRETURN(1);
}

XS(XS__completedtasksinset);
XS(XS__completedtasksinset)
{
	dXSARGS;
	int        RETVAL;
	dXSTARG;

	if(items == 1) {
		unsigned int taskset = (int)SvIV(ST(0));
		RETVAL = quest_manager.completedtasksinset(taskset);
	} else {
		Perl_croak(aTHX_ "Usage: completedtasksinset(taskset)");
	}

	XSprePUSH; PUSHi((IV)RETVAL);

	XSRETURN(1);
}


XS(XS__istaskappropriate);
XS(XS__istaskappropriate)
{
	dXSARGS;
	bool        RETVAL;
	dXSTARG;

	if(items == 1) {
		unsigned int task = (int)SvIV(ST(0));
		RETVAL = quest_manager.istaskappropriate(task);
	} else {
		Perl_croak(aTHX_ "Usage: istaskaappropriate(task)");
	}

	XSprePUSH; PUSHu((IV)RETVAL);

	XSRETURN(1);
}

 XS(XS__popup); // prototype to pass -Wmissing-prototypes
 XS(XS__popup) {
        dXSARGS;
	int popupid = 0;
	int buttons = 0;
	int duration = 0;

	if((items < 2) || (items > 5))
                Perl_croak(aTHX_ "Usage: popup(windowtitle, text, popupid, buttons, duration)");

	if(items >= 3)
		popupid = (int)SvIV(ST(2));

	if(items >= 4)
		buttons = (int)SvIV(ST(3));

	if(items == 5)
		duration = (int)SvIV(ST(4));

        quest_manager.popup(SvPV_nolen(ST(0)), SvPV_nolen(ST(1)), popupid, buttons, duration);

        XSRETURN_EMPTY;
 }
XS(XS__clearspawntimers);
XS(XS__clearspawntimers)
{
	dXSARGS;
	if (items != 0)
		Perl_croak(aTHX_ "Usage: clearspawntimers()");

	quest_manager.clearspawntimers();

	XSRETURN_EMPTY;
}
XS(XS__ze);
XS(XS__ze)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: ze(type, str)");

	int	type = (int)SvIV(ST(0));
	char *		str = (char *)SvPV_nolen(ST(1));

	quest_manager.ze(type, str);

	XSRETURN_EMPTY;
}

XS(XS__we);
XS(XS__we)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: we(type, str)");

	int	type = (int)SvIV(ST(0));
	char *		str = (char *)SvPV_nolen(ST(1));

	quest_manager.we(type, str);

	XSRETURN_EMPTY;
}
XS(XS__getlevel);
XS(XS__getlevel)
{
	dXSARGS;
	if (items > 1)
		Perl_croak(aTHX_ "Usage: getlevel(type)");

	int	RETVAL;
	dXSTARG;

	int	type;
	if (items == 1)
		type = (int)SvIV(ST(0));
	else
		type = 0;

	RETVAL = quest_manager.getlevel(type);
	XSprePUSH; PUSHu((IV)RETVAL);

	XSRETURN(1);
}

XS(XS__CreateGroundObject);
XS(XS__CreateGroundObject)
{
	dXSARGS;
	if (items != 5 && items != 6 && items != 7 && items != 8)
		Perl_croak(aTHX_ "Usage: creategroundobject(itemid, x, y, z, heading, [decay_time], [levelreq], [qglobal])");

	int	itemid = (int)SvIV(ST(0));
	float x = (float)SvNV(ST(1));
	float y = (float)SvNV(ST(2));
	float z = (float)SvNV(ST(3));
	float heading = (float)SvNV(ST(4));
	uint16 id = 0;

	switch(items)
	{
	case 5:
		
		{
			id = quest_manager.CreateGroundObject(itemid, x, y, z, heading);
			break;
		}
	case 6:
		
		{
			int32 decay_time = (int32)SvIV(ST(5));
			id = quest_manager.CreateGroundObject(itemid, x, y, z, heading, decay_time);
			break;
		}
	case 7:
		
		{
			int32 decay_time = (int32)SvIV(ST(5));
			int32 levelrequirement = (int32)SvIV(ST(6));
			id = quest_manager.CreateGroundObject(itemid, x, y, z, heading, decay_time, levelrequirement);
			break;
		}
	case 8:
		
		{
			int32 decay_time = (int32)SvIV(ST(5));
			int32 levelrequirement = (int32)SvIV(ST(6));
			char *	qglobalvalue = (char *)SvPV_nolen(ST(7));
			id = quest_manager.CreateGroundObject(itemid, x, y, z, heading, decay_time, levelrequirement, qglobalvalue);
			break;
		}

	}

	XSRETURN_IV(id);
}

XS(XS__CreateGroundObjectFromModel);
XS(XS__CreateGroundObjectFromModel)
{
	dXSARGS;
	if (items < 5 || items > 7)
		Perl_croak(aTHX_ "Usage: creategroundobjectfrommodel(modelname, x, y, z, heading, [type], [decay_time])");

	char *		modelname = (char *)SvPV_nolen(ST(0));
	float x = (float)SvNV(ST(1));
	float y = (float)SvNV(ST(2));
	float z = (float)SvNV(ST(3));
	float heading = (float)SvNV(ST(4));
	int32 type = 0;
	int32 decay_time = 0;
	uint16 id = 0;

	if (items > 5)
		type = (int32)SvIV(ST(5));

	if (items > 6)
		decay_time = (int32)SvIV(ST(6));

	id = quest_manager.CreateGroundObjectFromModel(modelname, x, y, z, heading, type, decay_time);
	XSRETURN_IV(id);
}

XS(XS__ModifyNPCStat);
XS(XS__ModifyNPCStat)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: ModifyNPCStat(identifier, newValue)");

	quest_manager.ModifyNPCStat(SvPV_nolen(ST(0)), SvPV_nolen(ST(1)));

	XSRETURN_EMPTY;
}

XS(XS__collectitems);
XS(XS__collectitems)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: collectitems(item_id, remove?)");

	uint32 item_id = (int)SvIV(ST(0));
	bool remove = ((int)SvIV(ST(1))) == 0?false:true;

	int quantity =
		quest_manager.collectitems(item_id, remove);

	XSRETURN_IV(quantity);
}

XS(XS__UpdateSpawnTimer);
XS(XS__UpdateSpawnTimer)
{
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: UpdateSpawnTimer(Spawn2_ID, Updated_Time_Till_Repop)");

	int32 id = (int)SvIV(ST(0));
	int32 duration = (int)SvIV(ST(1));

	quest_manager.UpdateSpawnTimer(id, duration);

	XSRETURN_EMPTY;
}

XS(XS__MerchantSetItem);
XS(XS__MerchantSetItem) {
	dXSARGS;
	if (items != 2 && items != 3)
		Perl_croak(aTHX_ "Usage: MerchantSetItem(NPCid, itemid [, quantity])");

	int32 NPCid = (int)SvUV(ST(0));
	int32 itemid = (int)SvUV(ST(1));
	int32 quantity = 0;
	if (items == 3)
		quantity = (int)SvUV(ST(2));

	quest_manager.MerchantSetItem(NPCid, itemid, quantity);

	XSRETURN_EMPTY;
}

XS(XS__MerchantCountItem);
XS(XS__MerchantCountItem) {
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: MerchantCountItem(NPCid, itemid)");

	int32 NPCid = (int)SvUV(ST(0));
	int32 itemid = (int)SvUV(ST(1));
	int32 quantity = quest_manager.MerchantCountItem(NPCid, itemid);

	XSRETURN_UV(quantity);
}

XS(XS__varlink);
XS(XS__varlink) {
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: varlink(itemID)");
	dXSTARG;

	Const_char * RETVAL;
	char text[250];
	uint32 itemID;
	itemID = (int)SvUV(ST(0));

	RETVAL = quest_manager.varlink(text, itemID);

	sv_setpv(TARG, RETVAL); XSprePUSH; PUSHTARG;
	XSRETURN(1);
}

XS(XS__CreateInstance);
XS(XS__CreateInstance) {
	dXSARGS;
	if (items != 3)
		Perl_croak(aTHX_ "Usage: CreateInstance(zone_name, version, duration)");

	char * zone = (char *)SvPV_nolen(ST(0));
	int16 version = (int)SvUV(ST(1));
	int32 duration = (int)SvUV(ST(2));
	int32 id = quest_manager.CreateInstance(zone, version, duration);

	XSRETURN_UV(id);
}

XS(XS__DestroyInstance);
XS(XS__DestroyInstance) {
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: DestroyInstance(id)");

	int16 id = (int)SvUV(ST(0));
	quest_manager.DestroyInstance(id);

	XSRETURN_EMPTY;
}

XS(XS__GetInstanceID);
XS(XS__GetInstanceID) {
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: GetInstanceID(zone_name, version)");

	char * zone = (char *)SvPV_nolen(ST(0));
	int16 version = (int)SvUV(ST(1));
	int16 id = quest_manager.GetInstanceID(zone, version);

	XSRETURN_UV(id);
}

XS(XS__AssignToInstance);
XS(XS__AssignToInstance) {
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: AssignToInstance(id)");

	int16 id = (int)SvUV(ST(0));
	quest_manager.AssignToInstance(id);

	XSRETURN_EMPTY;
}

XS(XS__AssignGroupToInstance);
XS(XS__AssignGroupToInstance) {
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: AssignGroupToInstance(id)");

	int16 id = (int)SvUV(ST(0));
	quest_manager.AssignGroupToInstance(id);

	XSRETURN_EMPTY;
}

XS(XS__AssignRaidToInstance);
XS(XS__AssignRaidToInstance) {
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: AssignRaidToInstance(id)");

	int16 id = (int)SvUV(ST(0));
	quest_manager.AssignRaidToInstance(id);

	XSRETURN_EMPTY;
}

XS(XS__MovePCInstance);
XS(XS__MovePCInstance)
{
	dXSARGS;
	if (items != 5 && items != 6)
		Perl_croak(aTHX_ "Usage: MovePCInstance(zone_id, instance_id, x, y, z [,heading])");

	int	zoneid = (int)SvIV(ST(0));
	int	instanceid = (int)SvIV(ST(1));
	float	x = (float)SvNV(ST(2));
	float	y = (float)SvNV(ST(3));
	float	z = (float)SvNV(ST(4));

	if (items == 4)
	{
		quest_manager.MovePCInstance(zoneid, instanceid, x, y, z, 0.0f);
	}
	else
	{
		float heading = (float)SvNV(ST(5));
		quest_manager.MovePCInstance(zoneid, instanceid, x, y, z, heading);
	}

	XSRETURN_EMPTY;
}

XS(XS__FlagInstanceByGroupLeader);
XS(XS__FlagInstanceByGroupLeader) {
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: FlagInstanceByGroupLeader(zone, version)");

	int32 zone = (int)SvUV(ST(0));
	int16 version = (int)SvUV(ST(1));
	quest_manager.FlagInstanceByGroupLeader(zone, version);

	XSRETURN_EMPTY;
}

XS(XS__FlagInstanceByRaidLeader);
XS(XS__FlagInstanceByRaidLeader) {
	dXSARGS;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: FlagInstanceByRaidLeader(zone, version)");

	int32 zone = (int)SvUV(ST(0));
	int16 version = (int)SvUV(ST(1));
	quest_manager.FlagInstanceByRaidLeader(zone, version);

	XSRETURN_EMPTY;
}

XS(XS__saylink);
XS(XS__saylink) {
	dXSARGS;
	if (items != 1 && items != 2 && items != 3)
		Perl_croak(aTHX_ "Usage: saylink(phrase,[silent?],[linkname])");
	dXSTARG;

	Const_char * RETVAL;
	char text[250];
	char text2[250];
	strcpy(text,(char *)SvPV_nolen(ST(0)));
	bool silent = ((int)SvIV(ST(1))) == 0?false:true;
	if (items == 3)
		strcpy(text2,(char *)SvPV_nolen(ST(2)));
	else
		strcpy(text2,text);

	RETVAL = quest_manager.saylink(text, silent, text2);
	sv_setpv(TARG, RETVAL); XSprePUSH; PUSHTARG;
	XSRETURN(1);
}

XS(XS__SetRunning);
XS(XS__SetRunning)
{
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: SetRunning(val)");

	bool val = ((int)SvIV(ST(0))) == 0?false:true;

	quest_manager.SetRunning(val);

	XSRETURN_EMPTY;
}

XS(XS__IsRunning);
XS(XS__IsRunning)
{
    dXSARGS;
    if (items >= 1)
        Perl_croak(aTHX_ "Usage: IsRunning()");

    bool        RETVAL;
    dXSTARG;


	RETVAL = quest_manager.IsRunning();
	XSprePUSH; PUSHu((IV)RETVAL);

	XSRETURN(1);
}

XS(XS__IsEffectInSpell);
XS(XS__IsEffectInSpell)
{
    dXSARGS;
    if (items != 2)
        Perl_croak(aTHX_ "Usage: IsEffectInSpell(spell_id, effect_id)");

	int32		spell_id = (int32)SvUV(ST(0));
	int32		effect_id = (int32)SvUV(ST(1));
    bool        RETVAL;
    dXSTARG;


	RETVAL = IsEffectInSpell(spell_id, effect_id);
	XSprePUSH; PUSHu((IV)RETVAL);

	XSRETURN(1);
}

XS(XS__IsBeneficialSpell);
XS(XS__IsBeneficialSpell)
{
    dXSARGS;
    if (items != 1)
        Perl_croak(aTHX_ "Usage: IsBeneficialSpell(spell_id)");

	int32		spell_id = (int32)SvUV(ST(0));
    bool        RETVAL;
    dXSTARG;


	RETVAL = BeneficialSpell(spell_id);
	XSprePUSH; PUSHu((IV)RETVAL);

	XSRETURN(1);
}

XS(XS__GetSpellResistType);
XS(XS__GetSpellResistType)
{
    dXSARGS;
    if (items != 1)
        Perl_croak(aTHX_ "Usage: GetSpellResistType(spell_id)");

	int32		spell_id = (int32)SvUV(ST(0));
	sint32		spell_val = 0;
    dXSTARG;

	spell_val = GetSpellResistType(spell_id);
	XSRETURN_UV(spell_val);
}

XS(XS__GetSpellTargetType);
XS(XS__GetSpellTargetType)
{
    dXSARGS;
    if (items != 1)
        Perl_croak(aTHX_ "Usage: GetSpellTargetType(spell_id)");

	int32		spell_id = (int32)SvUV(ST(0));
	sint32		spell_val = 0;
    dXSTARG;

	spell_val = GetSpellTargetType(spell_id);
	XSRETURN_UV(spell_val);
}

XS(XS__FlyMode);
XS(XS__FlyMode) {
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: FlyMode([0/1/2])");

	int8 flymode = (int)SvUV(ST(0));
	quest_manager.FlyMode(flymode);

	XSRETURN_EMPTY;
}

XS(XS_FactionValue);
XS(XS_FactionValue) {
	dXSARGS;
	if (items != 0)
		Perl_croak(aTHX_ "Usage: factionvalue()");

	uint8 fac = quest_manager.FactionValue();
	XSRETURN_UV(fac);
}

XS(XS__enabletitle);
XS(XS__enabletitle)
{
   dXSARGS;
   if (items != 1)
      Perl_croak(aTHX_ "Usage: enabletitle(titleset)");

   int   titleset = (int)SvIV(ST(0));

   quest_manager.enabletitle(titleset);

   XSRETURN_EMPTY;
}

XS(XS__checktitle);
XS(XS__checktitle)
{
   dXSARGS;
   if (items != 1)
      Perl_croak(aTHX_ "Usage: checktitle(titleset)");

   bool RETVAL;
   int   titleset = (int)SvIV(ST(0));

   RETVAL = quest_manager.checktitle(titleset);

    ST(0) = boolSV(RETVAL);
    sv_2mortal(ST(0));
    XSRETURN(1);
}

XS(XS__removetitle);
XS(XS__removetitle)
{
   dXSARGS;
   if (items != 1)
      Perl_croak(aTHX_ "Usage: removetitle(titleset)");

   int   titleset = (int)SvIV(ST(0));

   quest_manager.removetitle(titleset);

   XSRETURN_EMPTY;
}

XS(XS__wearchange);
XS(XS__wearchange)
{
    dXSARGS;
    if (items != 2)
        Perl_croak(aTHX_ "Usage: wearchange(slot, texture)");

	int8		slot = (int)SvUV(ST(0));
	int16		texture = (int)SvUV(ST(1));

	quest_manager.wearchange(slot, texture);

	XSRETURN_EMPTY;
}

XS(XS__voicetell);
XS(XS__voicetell)
{
	dXSARGS;
	if (items != 4)
		Perl_croak(aTHX_ "Usage: voicetell(clientname, type, race, gender)");

	char *	str = (char *)SvPV_nolen(ST(0));
	int	macronum = (int)SvIV(ST(1));
	int	racenum = (int)SvIV(ST(2));
	int	gendernum = (int)SvIV(ST(3));

	quest_manager.voicetell(str, macronum, racenum, gendernum);

	XSRETURN_EMPTY;
}

XS(XS__LearnRecipe);
XS(XS__LearnRecipe)
{
    dXSARGS;
    if (items != 1)
        Perl_croak(aTHX_ "Usage: LearnRecipe(recipe_id)");

    uint32 recipe_id = (uint32)SvIV(ST(0));

    quest_manager.LearnRecipe(recipe_id);

    XSRETURN_EMPTY;
}

XS(XS__handleturnin); // prototype to pass -Wmissing-prototypes
XS(XS__handleturnin) {
	dXSARGS;
	
	if (items != 2)
		Perl_croak(aTHX_ "Usage: handleturnin(itemid, itemcharges)");
	int	itemid = (int)SvIV(ST(0));
	int	charges = (int)SvIV(ST(1));

	bool returnVal = quest_manager.TurnInItem(itemid,charges);
	
	ST(0) = boolSV(returnVal);
	sv_2mortal(ST(0));
	XSRETURN(1);
}

XS(XS__completehandin); // prototype to pass -Wmissing-prototypes
XS(XS__completehandin) {
	dXSARGS;
	
	if (items != 0)
		Perl_croak(aTHX_ "Usage: completeturnin()");

	quest_manager.CompleteHandIn();
	
	XSRETURN_EMPTY;
}

XS(XS__resethandin); // prototype to pass -Wmissing-prototypes
XS(XS__resethandin) {
	dXSARGS;
	
	if (items != 0)
		Perl_croak(aTHX_ "Usage: resetturnin()");

	quest_manager.ResetHandIn();
	
	XSRETURN_EMPTY;
}


XS(XS__GetBestZ);
XS(XS__GetBestZ) {
	dXSARGS;
	if (items != 3)
		Perl_croak(aTHX_ "Usage: GetBestZ(x,y,z)");
	{
		dXSTARG;
		double x = (double)SvNV(ST(0));
		double y = (double)SvNV(ST(1));
		double z = (double)SvNV(ST(2));

		float best_z = quest_manager.GetBestZ(x,y,z);
	
		XSprePUSH; PUSHn((double)best_z);
	}
	XSRETURN(1);
}

XS(XS__GetRule);
XS(XS__GetRule) {
	dXSARGS;
	if (items != 1)
		Perl_croak(aTHX_ "Usage: GetRule(rulename)");
	{
		dXSTARG;
		Const_char *		RETVAL;

		char*		rulename = (char *)SvPV_nolen(ST(0));
		
		std::string value("");
		if(!rules->GetRule(rulename, value))
			value = "UNDEFINED";
		
		RETVAL = value.c_str();
		sv_setpv(TARG, RETVAL); XSprePUSH; PUSHTARG;
	}
	XSRETURN(1);
}

XS(XS__SetRule);
XS(XS__SetRule) {
	dXSARGS;
	bool        RETVAL;
	dXSTARG;
	if (items != 2)
		Perl_croak(aTHX_ "Usage: SetRule(rulename,rulevalue)");
	{
		char*		rulename = (char *)SvPV_nolen(ST(0));
		char*		rulevalue = (char *)SvPV_nolen(ST(1));
		
		RETVAL = rules->SetRule(rulename, rulevalue, &database, true);

		if ( RETVAL )
			quest_manager.ReloadRules();
	}

	XSprePUSH; PUSHu((IV)RETVAL);
	XSRETURN(1);
}

/*
This is the callback perl will look for to setup the
quest package's XSUBs
*/
EXTERN_C XS(boot_quest); // prototype to pass -Wmissing-prototypes
EXTERN_C XS(boot_quest)
{
	dXSARGS;
	char file[256];
	strncpy(file, __FILE__, 256);
	file[255] = '\0';

	if(items != 1)
		LogFile->write(EQEMuLog::Error, "boot_quest does not take any arguments.");

	char buf[128];	//shouldent have any function names longer than this.

	//add the strcpy stuff to get rid of const warnings....

	XS_VERSION_BOOTCHECK ;
		newXS(strcpy(buf, "echo"), XS__echo, file);
		newXS(strcpy(buf, "say"), XS__say, file);
		newXS(strcpy(buf, "me"), XS__me, file);
		newXS(strcpy(buf, "summonitem"), XS__summonitem, file);
		newXS(strcpy(buf, "write"), XS__write, file);
		newXS(strcpy(buf, "spawn"), XS__spawn, file);
		newXS(strcpy(buf, "spawn2"), XS__spawn2, file);
		newXS(strcpy(buf, "unique_spawn"), XS__unique_spawn, file);
		newXS(strcpy(buf, "spawn_from_spawn2"), XS__spawn_from_spawn2, file);
		newXS(strcpy(buf, "enable_spawn2"), XS__enable_spawn2, file);
		newXS(strcpy(buf, "disable_spawn2"), XS__disable_spawn2, file);
		newXS(strcpy(buf, "setstat"), XS__setstat, file);
		newXS(strcpy(buf, "incstat"), XS__incstat, file);
		newXS(strcpy(buf, "castspell"), XS__castspell, file);
		newXS(strcpy(buf, "selfcast"), XS__selfcast, file);
		newXS(strcpy(buf, "addloot"), XS__addloot, file);
		newXS(strcpy(buf, "zone"), XS__zone, file);
		newXS(strcpy(buf, "settimer"), XS__settimer, file);
		newXS(strcpy(buf, "stoptimer"), XS__stoptimer, file);
		newXS(strcpy(buf, "stopalltimers"), XS__stopalltimers, file);
		newXS(strcpy(buf, "emote"), XS__emote, file);
		newXS(strcpy(buf, "shout"), XS__shout, file);
		newXS(strcpy(buf, "shout2"), XS__shout2, file);
		newXS(strcpy(buf, "gmsay"), XS__gmsay, file);
		newXS(strcpy(buf, "depop"), XS__depop, file);
		newXS(strcpy(buf, "settarget"), XS__settarget, file);
		newXS(strcpy(buf, "follow"), XS__follow, file);
		newXS(strcpy(buf, "sfollow"), XS__sfollow, file);
		newXS(strcpy(buf, "changedeity"), XS__changedeity, file);
		newXS(strcpy(buf, "exp"), XS__exp, file);
		newXS(strcpy(buf, "level"), XS__level, file);
		newXS(strcpy(buf, "traindisc"), XS__traindisc, file);
		newXS(strcpy(buf, "isdisctome"), XS__isdisctome, file);
		newXS(strcpy(buf, "safemove"), XS__safemove, file);
		newXS(strcpy(buf, "rain"), XS__rain, file);
		newXS(strcpy(buf, "snow"), XS__snow, file);
		newXS(strcpy(buf, "surname"), XS__surname, file);
		newXS(strcpy(buf, "permaclass"), XS__permaclass, file);
		newXS(strcpy(buf, "permarace"), XS__permarace, file);
		newXS(strcpy(buf, "permagender"), XS__permagender, file);
		newXS(strcpy(buf, "scribespells"), XS__scribespells, file);
		newXS(strcpy(buf, "traindiscs"), XS__traindiscs, file);
		newXS(strcpy(buf, "unscribespells"), XS__unscribespells, file);
		newXS(strcpy(buf, "untraindiscs"), XS__untraindiscs, file);
		newXS(strcpy(buf, "givecash"), XS__givecash, file);
		newXS(strcpy(buf, "pvp"), XS__pvp, file);
		// Kings & Bandits addons
		newXS(strcpy(buf, "getclientbyentityid"), XS__GetClientByEntityID, file);
		
		newXS(strcpy(buf, "pvpvalue"), XS__pvpvalue, file);
		
		newXS(strcpy(buf, "getstaticzonecount"), XS__getstaticzonecount, file);
		newXS(strcpy(buf, "getbootedzonecount"), XS__getbootedzonecount, file);
		newXS(strcpy(buf, "getavailzonecount"), XS__getavailzonecount, file);

		newXS(strcpy(buf, "setzonepvp"), XS__setzonepvp, file);
		newXS(strcpy(buf, "getzonepvp"), XS__getzonepvp, file);
		
		newXS(strcpy(buf, "summonlootgroup"), XS__summonlootgroup, file);
		newXS(strcpy(buf, "getzoneid"), XS__getzoneid, file);

		newXS(strcpy(buf, "movepc"), XS__movepc, file);
		newXS(strcpy(buf, "gmmove"), XS__gmmove, file);
		newXS(strcpy(buf, "movegrp"), XS__movegrp, file);
		newXS(strcpy(buf, "doanim"), XS__doanim, file);
		newXS(strcpy(buf, "addskill"), XS__addskill, file);
		newXS(strcpy(buf, "setlanguage"), XS__setlanguage, file);
		newXS(strcpy(buf, "setskill"), XS__setskill, file);
		newXS(strcpy(buf, "setallskill"), XS__setallskill, file);
		newXS(strcpy(buf, "attack"), XS__attack, file);
		newXS(strcpy(buf, "attacknpc"), XS__attacknpc, file);
		newXS(strcpy(buf, "attacknpctype"), XS__attacknpctype, file);
		// Kings & Bandits facenpctype addition
		newXS(strcpy(buf, "facenpctype"), XS__facenpctype, file);
		// Kings & Bandits kingdom additions
		newXS(strcpy(buf, "setcitycitizentaxrate"), XS__setcitycitizentaxrate, file);
		newXS(strcpy(buf, "setcitynewbietaxrate"), XS__setcitynewbietaxrate, file);
		newXS(strcpy(buf, "getcitycitizentaxrate"), XS__getcitycitizentaxrate, file);
		newXS(strcpy(buf, "getcitynewbietaxrate"), XS__getcitynewbietaxrate, file);
		
		newXS(strcpy(buf, "setcitybankreserve"), XS__setcitybankreserve, file);
		newXS(strcpy(buf, "getcitybankreserve"), XS__getcitybankreserve, file);
		
		newXS(strcpy(buf, "getcityupkeepcost"), XS__getcityupkeepcost, file);
		
		newXS(strcpy(buf, "getteamexpbonus"), XS__getteamexpbonus, file);
		newXS(strcpy(buf, "setteamexpbonus"), XS__setteamexpbonus, file);
		
		newXS(strcpy(buf, "addguardtype"), XS__addguardtype, file);
		newXS(strcpy(buf, "loadguardtype"), XS__loadguardtype, file);
		newXS(strcpy(buf, "findcampteamid"), XS__findcampteamid, file);
		newXS(strcpy(buf, "findguardteamid"), XS__findguardteamid, file);
		newXS(strcpy(buf, "addteamid"), XS__addteamid, file);
		newXS(strcpy(buf, "updateteamid"), XS__updateteamid, file);
		newXS(strcpy(buf, "spawninnpc"), XS__spawninnpc, file);
		newXS(strcpy(buf, "getguardfield"), XS__getguardfield, file);
		newXS(strcpy(buf, "resetguardfields"), XS__resetguardfields, file);
		
		newXS(strcpy(buf, "getisupkeeppaid"), XS__getisupkeeppaid, file);
		
		newXS(strcpy(buf, "zonepointnearby"), XS__zonepointnearby, file);
		newXS(strcpy(buf, "findnpcnear"), XS__findnpcnear, file);
		newXS(strcpy(buf, "locationnearby"), XS__locationnearby, file);
		// Kings & Bandits / LoZ / Red69 additions 4/29/2013 kingdom owner settings
		newXS(strcpy(buf, "setkingdomowner"), XS__setkingdomowner, file);
		newXS(strcpy(buf, "getkingdomownercharid"), XS__getkingdomownercharid, file);
		newXS(strcpy(buf, "getkingdomownerguildid"), XS__getkingdomownerguildid, file);
		newXS(strcpy(buf, "setkingdomownerlastvisit"), XS__setkingdomownerlastvisit, file);
		newXS(strcpy(buf, "getkingdomownerlastvisit"), XS__getkingdomownerlastvisit, file);
		
		newXS(strcpy(buf, "setcitymode"), XS__setcitymode, file);
		newXS(strcpy(buf, "getcitymode"), XS__getcitymode, file);
		
		newXS(strcpy(buf, "getteamlocation"), XS__getteamlocation, file);
		newXS(strcpy(buf, "getteamsettings"), XS__getteamsettings, file);
		newXS(strcpy(buf, "setcitylocation"), XS__setcitylocation, file);

		newXS(strcpy(buf, "getmaxlevel"), XS__getmaxlevel, file);
		newXS(strcpy(buf, "save"), XS__save, file);
		newXS(strcpy(buf, "faction"), XS__faction, file);
		newXS(strcpy(buf, "setsky"), XS__setsky, file);
		newXS(strcpy(buf, "setguild"), XS__setguild, file);
		newXS(strcpy(buf, "settime"), XS__settime, file);
		newXS(strcpy(buf, "itemlink"), XS__itemlink, file);
		newXS(strcpy(buf, "signal"), XS__signal, file);
		newXS(strcpy(buf, "signalwith"), XS__signalwith, file);
		newXS(strcpy(buf, "setglobal"), XS__setglobal, file);
		newXS(strcpy(buf, "targlobal"), XS__targlobal, file);
		newXS(strcpy(buf, "delglobal"), XS__delglobal, file);
		newXS(strcpy(buf, "ding"), XS__ding, file);
		newXS(strcpy(buf, "rebind"), XS__rebind, file);
		newXS(strcpy(buf, "start"), XS__start, file);
		newXS(strcpy(buf, "stop"), XS__stop, file);
		newXS(strcpy(buf, "pause"), XS__pause, file);
		newXS(strcpy(buf, "moveto"), XS__moveto, file);
		newXS(strcpy(buf, "resume"), XS__resume, file);
		newXS(strcpy(buf, "addldonpoints"), XS__addldonpoints, file);
		newXS(strcpy(buf, "setnexthpevent"), XS__setnexthpevent, file);
		newXS(strcpy(buf, "setnextinchpevent"), XS__setnextinchpevent, file);
		newXS(strcpy(buf, "sethp"), XS__sethp, file);
		newXS(strcpy(buf, "respawn"), XS__respawn, file);
		newXS(strcpy(buf, "getItemName"), XS_qc_getItemName, file);
		newXS(strcpy(buf, "ChooseRandom"), XS__ChooseRandom, file);
		newXS(strcpy(buf, "set_proximity"), XS__set_proximity, file);
		newXS(strcpy(buf, "clear_proximity"), XS__clear_proximity, file);
		newXS(strcpy(buf, "setanim"), XS__setanim, file);
		newXS(strcpy(buf, "showgrid"), XS__showgrid, file);
		newXS(strcpy(buf, "showpath"), XS__showpath, file);
		newXS(strcpy(buf, "pathto"), XS__pathto, file);
		newXS(strcpy(buf, "spawn_condition"), XS__spawn_condition, file);
		newXS(strcpy(buf, "get_spawn_condition"), XS__get_spawn_condition, file);
		newXS(strcpy(buf, "toggle_spawn_event"), XS__toggle_spawn_event, file);
		newXS(strcpy(buf, "has_zone_flag"), XS__has_zone_flag, file);
		newXS(strcpy(buf, "set_zone_flag"), XS__set_zone_flag, file);
		newXS(strcpy(buf, "clear_zone_flag"), XS__clear_zone_flag, file);
		newXS(strcpy(buf, "summonburriedplayercorpse"), XS__summonburriedplayercorpse, file);
		newXS(strcpy(buf, "getnextcorpsedecaytime"), XS__getnextcorpsedecaytime, file);
		newXS(strcpy(buf, "getplayerburriedcorpsecount"), XS__getplayerburriedcorpsecount, file);
		newXS(strcpy(buf, "buryplayercorpse"), XS__buryplayercorpse, file);
		newXS(strcpy(buf, "forcedooropen"), XS__forcedooropen, file);
		newXS(strcpy(buf, "forcedoorclose"), XS__forcedoorclose, file);
		newXS(strcpy(buf, "isdooropen"), XS__isdooropen, file);
		newXS(strcpy(buf, "depopall"), XS__depopall, file);
		newXS(strcpy(buf, "depopzone"), XS__depopzone, file);
		newXS(strcpy(buf, "repopzone"), XS__repopzone, file);
		newXS(strcpy(buf, "npcrace"), XS__npcrace, file);
		newXS(strcpy(buf, "npcgender"), XS__npcgender, file);
		newXS(strcpy(buf, "npcsize"), XS__npcsize, file);
		newXS(strcpy(buf, "npctexture"), XS__npctexture, file);
		newXS(strcpy(buf, "playerrace"), XS__playerrace, file);
		newXS(strcpy(buf, "playergender"), XS__playergender, file);
		newXS(strcpy(buf, "playersize"), XS__playersize, file);
		newXS(strcpy(buf, "playertexture"), XS__playertexture, file);
		newXS(strcpy(buf, "playerfeature"), XS__playerfeature, file);
		newXS(strcpy(buf, "npcfeature"), XS__npcfeature, file);

#ifdef BOTS
		newXS(strcpy(buf, "botquest"), XS__botquest, file);
		newXS(strcpy(buf, "spawnbotcount"), XS__spawnbotcount, file);
		newXS(strcpy(buf, "createbotcount"), XS__createbotcount, file);
		newXS(strcpy(buf, "createBot"), XS__createBot, file);
#endif //BOTS

		newXS(strcpy(buf, "taskselector"), XS__taskselector, file);
		newXS(strcpy(buf, "tasksetselector"), XS__tasksetselector, file);
		newXS(strcpy(buf, "enabletask"), XS__enabletask, file);
		newXS(strcpy(buf, "disabletask"), XS__disabletask, file);
		newXS(strcpy(buf, "istaskenabled"), XS__istaskenabled, file);
		newXS(strcpy(buf, "istaskactive"), XS__istaskactive, file);
		newXS(strcpy(buf, "istaskactivityactive"), XS__istaskactivityactive, file);
		newXS(strcpy(buf, "updatetaskactivity"), XS__updatetaskactivity, file);
		newXS(strcpy(buf, "resettaskactivity"), XS__resettaskactivity, file);
		newXS(strcpy(buf, "taskexploredarea"), XS__taskexploredarea, file);
		newXS(strcpy(buf, "assigntask"), XS__assigntask, file);
		newXS(strcpy(buf, "failtask"), XS__failtask, file);
		newXS(strcpy(buf, "tasktimeleft"), XS__tasktimeleft, file);
		newXS(strcpy(buf, "istaskcompleted"), XS__istaskcompleted, file);
		newXS(strcpy(buf, "enabledtaskcount"), XS__enabledtaskcount, file);
		newXS(strcpy(buf, "firsttaskinset"), XS__firsttaskinset, file);
		newXS(strcpy(buf, "lasttaskinset"), XS__lasttaskinset, file);
		newXS(strcpy(buf, "nexttaskinset"), XS__nexttaskinset, file);
		newXS(strcpy(buf, "activespeaktask"), XS__activespeaktask, file);
		newXS(strcpy(buf, "activespeakactivity"), XS__activespeakactivity, file);
		newXS(strcpy(buf, "activetasksinset"), XS__activetasksinset, file);
		newXS(strcpy(buf, "completedtasksinset"), XS__completedtasksinset, file);
		newXS(strcpy(buf, "istaskappropriate"), XS__istaskappropriate, file);
		newXS(strcpy(buf, "popup"), XS__popup, file);
		newXS(strcpy(buf, "clearspawntimers"), XS__clearspawntimers, file);
		newXS(strcpy(buf, "ze"), XS__ze, file);
		newXS(strcpy(buf, "we"), XS__we, file);
		newXS(strcpy(buf, "getlevel"), XS__getlevel, file);
		newXS(strcpy(buf, "creategroundobject"), XS__CreateGroundObject, file);
		newXS(strcpy(buf, "creategroundobjectfrommodel"), XS__CreateGroundObjectFromModel, file);
		newXS(strcpy(buf, "modifynpcstat"), XS__ModifyNPCStat, file);
		newXS(strcpy(buf, "collectitems"), XS__collectitems, file);
		newXS(strcpy(buf, "updatespawntimer"), XS__UpdateSpawnTimer, file);
		newXS(strcpy(buf, "MerchantSetItem"), XS__MerchantSetItem, file);
		newXS(strcpy(buf, "MerchantCountItem"), XS__MerchantCountItem, file);
		newXS(strcpy(buf, "varlink"), XS__varlink, file);
		newXS(strcpy(buf, "saylink"), XS__saylink, file);
		newXS(strcpy(buf, "CreateInstance"), XS__CreateInstance, file);
		newXS(strcpy(buf, "DestroyInstance"), XS__DestroyInstance, file);
		newXS(strcpy(buf, "GetInstanceID"), XS__GetInstanceID, file);
		newXS(strcpy(buf, "AssignToInstance"), XS__AssignToInstance, file);
		newXS(strcpy(buf, "AssignGroupToInstance"), XS__AssignGroupToInstance, file);
		newXS(strcpy(buf, "AssignRaidToInstance"), XS__AssignRaidToInstance, file);
		newXS(strcpy(buf, "MovePCInstance"), XS__MovePCInstance, file);
		newXS(strcpy(buf, "FlagInstanceByGroupLeader"), XS__FlagInstanceByGroupLeader, file);
		newXS(strcpy(buf, "FlagInstanceByRaidLeader"), XS__FlagInstanceByRaidLeader, file);
		newXS(strcpy(buf, "SetRunning"), XS__SetRunning, file);
		newXS(strcpy(buf, "IsRunning"), XS__IsRunning, file);
		newXS(strcpy(buf, "IsEffectInSpell"), XS__IsEffectInSpell, file);
		newXS(strcpy(buf, "IsBeneficialSpell"), XS__IsBeneficialSpell, file);
		newXS(strcpy(buf, "GetSpellResistType"), XS__GetSpellResistType, file);
		newXS(strcpy(buf, "GetSpellTargetType"), XS__GetSpellTargetType, file);
		newXS(strcpy(buf, "FlyMode"), XS__FlyMode, file);
		newXS(strcpy(buf, "factionvalue"), XS_FactionValue, file);
		newXS(strcpy(buf, "checktitle"), XS__checktitle, file);
		newXS(strcpy(buf, "enabletitle"), XS__enabletitle, file);
		newXS(strcpy(buf, "removetitle"), XS__removetitle, file);
		newXS(strcpy(buf, "wearchange"), XS__wearchange, file);
		newXS(strcpy(buf, "voicetell"), XS__voicetell, file);
        newXS(strcpy(buf, "LearnRecipe"), XS__LearnRecipe, file);

		// KaB - Zek - Red69 / Addition for multiquest
        newXS(strcpy(buf, "handleturnin"), XS__handleturnin, file);
        newXS(strcpy(buf, "completehandin"), XS__completehandin, file);
        newXS(strcpy(buf, "resethandin"), XS__resethandin, file);
        newXS(strcpy(buf, "GetBestZ"), XS__GetBestZ, file);
        newXS(strcpy(buf, "GetRule"), XS__GetRule, file);
        newXS(strcpy(buf, "SetRule"), XS__SetRule, file);
	XSRETURN_YES;
}

#endif
#endif
