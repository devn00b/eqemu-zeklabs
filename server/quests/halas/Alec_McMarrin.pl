sub EVENT_SAY { 
plugin::guardevent_say($npc,2,$client,$text);
}

sub EVENT_POPUPRESPONSE
{
plugin::bankpopup_response(2,$client,$popupid);
} #close event response

sub EVENT_SPAWN
{
	plugin::setupguard_enter(2,$npc);
}


sub EVENT_ENTER {
plugin::enterguard_area(2,$client,$npc);

} #end of sub event_enter

sub EVENT_DEATH
{
quest::resetguardfields(2,$npc->GetNPCTypeID(),$userid);

} #if charid and pvpflag