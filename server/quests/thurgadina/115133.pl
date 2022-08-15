sub EVENT_SAY { 
my $val = plugin::guardevent_say($npc,6,$client,$text);
if ( $val == 0 )
{
  if($text=~/hail/i) {
    quest::say("Greetings and welcome to Thurgadin, $name. Ye best watch yer step on the bridge there, it can be a bit slippery."); 
    if (defined($entity_list->GetMobByNpcTypeID(115132))) { #Make sure the mob is there or you get an undefined error. 
       $entity_list->GetMobByNpcTypeID(115132)->Say("Aye, we wouldn't want ye fallin into the pit now would we."); #Make Guard Baldvin also talk. 
    } 
  }
}
}

sub EVENT_POPUPRESPONSE
{
plugin::bankpopup_response(6,$client,$popupid);
} #close event response

sub EVENT_SPAWN
{
	plugin::setupguard_enter(6,$npc);
}


sub EVENT_ENTER {
plugin::enterguard_area(6,$client,$npc);

} #end of sub event_enter

sub EVENT_DEATH
{
quest::resetguardfields(6,$npc->GetNPCTypeID(),$userid);

} #if charid and pvpflag