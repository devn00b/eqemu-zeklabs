	sub EVENT_SAY { 
	$var = plugin::guardevent_say($npc,1,$client,$text);
	if ( $var == 0 )
	{
if($text=~/Hail/i){
my $link = quest::saylink("status");
quest::say("Hello $name.  My name is Hezlan Nur and I am a member of the Qeynos Guard. serving the will of Antonius Bayle.  Anything or anyone trying to get into Qeynos has to go through me. first.  Heh!  I sure love it when those dirty Sabertooths try. though!  Nothing is as gratifying as the death wail of a gnoll.");
} #end hail
	}
	}

	sub EVENT_POPUPRESPONSE
	{
	plugin::bankpopup_response(1,$client,$popupid);
	} #close event response

	sub EVENT_SPAWN
	{
		plugin::setupguard_enter(1,$npc);
	}


	sub EVENT_ENTER {
	plugin::enterguard_area(1,$client,$npc);

	} #end of sub event_enter

	sub EVENT_DEATH
	{
	quest::resetguardfields(1,$npc->GetNPCTypeID(), $userid);

	} #if charid and pvpflag