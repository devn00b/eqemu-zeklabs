	sub EVENT_SAY { 
	  if($npc->GetRace() == 112 && $npc->GetClass() != 41 && ( $npc->GetName()=~/Guard/i ) ) {
	$var = plugin::guardevent_say($npc,17,$client,$text);
	if ( $var == 0 )
	{
	quest::say("Hello $name... Please refrain from combat while in the city.");
	}
	}

	}

sub EVENT_COMBAT {
   if($combat_state == 1 && ( $npc->GetName()=~/Guard/i ) ) {
   quest::say("For the defense of Kelethin!!");
   }
}

	sub EVENT_SPAWN
	{
	  if($npc->GetRace() == 112 && $npc->GetClass() != 41 && ( $npc->GetName()=~/Guard/i ) ) {
		plugin::setupguard_enter(17,$npc);
	  }
	}

	sub EVENT_ENTER {
	  if($npc->GetRace() == 112 && $npc->GetClass() != 41 && ( $npc->GetName()=~/Guard/i ) ) {
	plugin::enterguard_area(17,$client,$npc);
	  }
	} #end of sub event_enter

	sub EVENT_DEATH
	{
	  if($npc->GetRace() == 112 && $npc->GetClass() != 41 && ( $npc->GetName()=~/Guard/i ) ) {
	quest::resetguardfields(17,$npc->GetNPCTypeID(),$userid);
	  }

	} #if charid and pvpflag