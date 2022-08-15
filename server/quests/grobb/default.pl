	sub EVENT_SAY { 
	  if($npc->GetClass() != 41 && ( $npc->GetName()=~/Basher/i ) ) {
	$var = plugin::guardevent_say($npc,20,$client,$text);
	if ( $var == 0 )
	{
	quest::say("Hello $name... Please refrain from combat while in the city.");
	}
	}

	}

	sub EVENT_SPAWN
	{
	  if($npc->GetClass() != 41 && ( $npc->GetName()=~/Basher/i ) ) {
		plugin::setupguard_enter(20,$npc);
	  }
	}
	
	sub EVENT_AGGRO {
	  if($npc->GetClass() != 41 && ( $npc->GetName()=~/Basher/i ) ) {
  quest::say("I shall pluck you limb from limb!!");
		}
	}

	sub EVENT_ENTER {
	  if($npc->GetClass() != 41 && ( $npc->GetName()=~/Basher/i ) ) {
	plugin::enterguard_area(20,$client,$npc);
	  }
	} #end of sub event_enter

	sub EVENT_DEATH
	{
	  if($npc->GetClass() != 41 && ( $npc->GetName()=~/Basher/i ) ) {
	quest::resetguardfields(20,$npc->GetNPCTypeID(),$userid);
	  }

	} #if charid and pvpflag

sub EVENT_SLAY {
  quest::say("Those who play with da Basher always gets bashed good!!");
}