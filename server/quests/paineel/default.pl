	sub EVENT_SAY { 
	  if($npc->GetClass() != 41 && ( $npc->GetName()=~/Guard/i ) ) {
	$var = plugin::guardevent_say($npc,16,$client,$text);
	if ( $var == 0 )
	{
	quest::say("Hello $name... Please refrain from combat while in the city.");
	}
	}

	}

	sub EVENT_SPAWN
	{
	  if($npc->GetClass() != 41 && ( $npc->GetName()=~/Guard/i ) ) {
		plugin::setupguard_enter(16,$npc);
	  }
	}
	
	sub EVENT_AGGRO {
	  if($npc->GetClass() != 41 && ( $npc->GetName()=~/Guard/i ) ) {
		quest::say("Cease hostilities!");
		}
	}

	sub EVENT_ENTER {
	  if($npc->GetClass() != 41 && ( $npc->GetName()=~/Guard/i ) ) {
	plugin::enterguard_area(16,$client,$npc);
	  }
	} #end of sub event_enter

	sub EVENT_DEATH
	{
	  if($npc->GetClass() != 41 && ( $npc->GetName()=~/Guard/i ) ) {
	quest::resetguardfields(16,$npc->GetNPCTypeID(),$userid);
	  }

	} #if charid and pvpflag