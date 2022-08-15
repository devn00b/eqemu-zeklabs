	sub EVENT_SAY { 
	$var = plugin::guardevent_say($npc,8,$client,$text);
	if ( $var == 0 )
	{
	quest::say("Hail, $name!  I am sorry but I do not have time to chat as I must be on watch for any approaching enemies.");
	}
	}

	sub EVENT_SPAWN
	{
		plugin::leash_handlespawn($npc, $x, $y, $z);
		plugin::setupguard_enter(8,$npc);
	}


	sub EVENT_ENTER {
	plugin::enterguard_area(8,$client,$npc);

	} #end of sub event_enter

	sub EVENT_DEATH
	{

	plugin::leash_handledeath($npc);
	quest::resetguardfields(8,$npc->GetNPCTypeID(), $userid);

	} #if charid and pvpflag

sub EVENT_AGGRO {
	plugin::leash_handleaggro($npc);
}
sub EVENT_TIMER {
	plugin::leash_handletimer($npc, $timer, $x, $y, $z, 150);
}