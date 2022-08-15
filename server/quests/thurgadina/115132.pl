sub EVENT_SAY { 
my $val = plugin::guardevent_say($npc,6,$client,$text);
if ( $val == 0 )
{
  if($text=~/hail/i) {
    quest::say("Beyond these gates is the city of our people. Be respectful and obey our laws else I'll toss yer carcass into the pit meself.");
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