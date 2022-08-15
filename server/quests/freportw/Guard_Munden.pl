sub EVENT_SAY { 
plugin::guardevent_say($npc,3,$client,$text);
}

sub EVENT_SPAWN
{
	plugin::setupguard_enter(3,$npc);
}


sub EVENT_ENTER {
plugin::enterguard_area(3,$client,$npc);

} #end of sub event_enter

sub EVENT_DEATH
{
	quest::resetguardfields(3,$npc->GetNPCTypeID(), $userid);

} #if charid and pvpflag