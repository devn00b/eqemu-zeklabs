	sub EVENT_SAY { 
	$var = plugin::guardevent_say($npc,11,$client,$text);
	if ( $var == 0 )
	{
	quest::say("Hail, $name!  I am sorry but I do not have time to chat as I must be on watch for any approaching enemies.");
	}
	}

	sub EVENT_SPAWN
	{
		plugin::setupguard_enter(11,$npc);
	}


	sub EVENT_ENTER {
	plugin::enterguard_area(11,$client,$npc);

	} #end of sub event_enter

	sub EVENT_DEATH
	{
	quest::resetguardfields(11,$npc->GetNPCTypeID(), $userid);

	} #if charid and pvpflag