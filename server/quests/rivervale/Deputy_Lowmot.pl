sub EVENT_SAY { 
	if($text=~/nice hat/i){
		quest::say("Thank you, It is worn with pride and is the symbol of the Rivervale Militia");
	}
else
{
plugin::guardevent_say($npc,4,$client,$text);
}
}

sub EVENT_ITEM { 
if(plugin::check_handin(\%itemcount,13959=> 1)){
quest::say("Oh good!  Hey.  You are not Jillin..  Helping out Blinza huh?  She is quite a woman..  Yes indeed.  Quite a woman..  ah..  Oh sorry.  Here you go.  Thanks again.  Mayor Gubbin will be pleased.");
quest::givecash(7,0,0,0);
quest::faction(133, 4);
quest::faction(208, 4);
quest::faction(316, 4);
quest::faction(218, 4);
quest::faction(88, -8);
quest::exp(1000); 
}
plugin::return_items(\%itemcount); # return unused items
  }
#END of FILE Zone:rivervale  ID:19110 -- Deputy_Lowmot 

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