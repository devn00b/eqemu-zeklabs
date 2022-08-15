sub EVENT_SAY { 
plugin::guardevent_say($npc,7,$client,$text);
}

sub EVENT_POPUPRESPONSE
{
plugin::bankpopup_response(7,$client,$popupid);
} #close event response

sub EVENT_SPAWN
{
	plugin::setupguard_enter(7,$npc);
}


sub EVENT_ENTER {
plugin::enterguard_area(7,$client,$npc);

} #end of sub event_enter

sub EVENT_DEATH
{
quest::resetguardfields(7,$npc->GetNPCTypeID(),$userid);

} #if charid and pvpflag