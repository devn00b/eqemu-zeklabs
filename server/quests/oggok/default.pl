	sub EVENT_SAY { 
	  if($npc->GetClass() != 41 && ( $npc->GetName()=~/Bouncer/i ) ) {
	$var = plugin::guardevent_say($npc,19,$client,$text);
	if ( $var == 0 )
	{
	quest::say("Hello $name... Please refrain from combat while in the city.");
	}
	}

	}

	sub EVENT_SPAWN
	{
	  if($npc->GetClass() != 41 && ( $npc->GetName()=~/Bouncer/i ) ) {
		plugin::setupguard_enter(19,$npc);
	  }
	}
	
	sub EVENT_AGGRO {
	  if($npc->GetClass() != 41 && ( $npc->GetName()=~/Bouncer/i ) ) {
  			quest::say("Bouncer smash you!!");
		}
	}

	sub EVENT_ENTER {
	  if($npc->GetClass() != 41 && ( $npc->GetName()=~/Bouncer/i ) ) {
	plugin::enterguard_area(19,$client,$npc);
	  }
	} #end of sub event_enter

	sub EVENT_DEATH
	{
	  if($npc->GetClass() != 41 && ( $npc->GetName()=~/Bouncer/i ) ) {
	quest::resetguardfields(19,$npc->GetNPCTypeID(),$userid);
	  }

	} #if charid and pvpflag

sub EVENT_SLAY {
  quest::say("Ha!! Bouncers best. I am victorioo.. Victer.. I win!!");
}