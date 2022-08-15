sub EVENT_SAY { 
	if($text=~/nice hat/i){
		quest::say("Thank you, It is worn with pride and is the symbol of the Rivervale Militia");
	}
}

sub EVENT_POPUPRESPONSE
{
plugin::bankpopup_response(4,$client,$popupid);
} #close event response

sub EVENT_SPAWN
{
	plugin::setupguard_enter(4,$npc);
}


sub EVENT_ENTER {
plugin::enterguard_area(4,$client,$npc);

} #end of sub event_enter

sub EVENT_DEATH
{
quest::resetguardfields(4,$npc->GetNPCTypeID(),$userid);

} #if charid and pvpflag