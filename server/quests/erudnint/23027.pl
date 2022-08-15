sub EVENT_SAY { 
plugin::guardevent_say($npc,8,$client,$text);
}

sub EVENT_POPUPRESPONSE
{
plugin::bankpopup_response(8,$client,$popupid);
} #close event response

sub EVENT_SPAWN
{
	plugin::setupguard_enter(8,$npc);
}


sub EVENT_ENTER {
plugin::enterguard_area(8,$client,$npc);

} #end of sub event_enter

sub EVENT_DEATH
{
quest::resetguardfields(8,$npc->GetNPCTypeID(),$userid);

} #if charid and pvpflag 