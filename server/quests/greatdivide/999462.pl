	sub EVENT_SAY { 
	$var = plugin::guardevent_say($npc,9,$client,$text);
	if ( $var == 0 )
	{
	quest::say("Hail, $name!  I am sorry but I do not have time to chat, as I must get back to my patrol.");
	}
	}

	sub EVENT_SPAWN
	{
		plugin::setupguard_enter(9,$npc);
	}


	sub EVENT_ENTER {
	plugin::enterguard_area(9,$client,$npc);

	} #end of sub event_enter

	sub EVENT_DEATH
	{
	quest::resetguardfields(9,$npc->GetNPCTypeID(), $userid);

	} #if charid and pvpflag