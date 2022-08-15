sub EVENT_ITEM {
if(plugin::check_handin(\%itemcount,16165=> 1)){
quest::say("Daleen sent you? She is so sweet. If she wasn't married.. ehem.. Thanks.");
quest::faction(133, 5);
quest::faction(208, 5);
quest::faction(316, 5);
quest::faction(218, 5);
quest::faction(88, -10);
quest::givecash(0,0,1,0);
quest::exp(500);
}
  }
sub EVENT_SAY { 
	if($text=~/nice hat/i){
		quest::say("Thank you, It is worn with pride and is the symbol of the Rivervale Militia");
	}
else
{
plugin::guardevent_say($npc,4,$client,$text);
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