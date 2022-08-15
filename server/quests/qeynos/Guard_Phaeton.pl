	sub EVENT_SAY { 
	$var = plugin::guardevent_say($npc,1,$client,$text);
	if ( $var == 0 )
	{
	quest::say("Hail, $name.  If you are not busy, we can always use help keeping the beetle population under control in front of the gates.  But be careful, we have had sightings of a large queen beetle out there.");
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