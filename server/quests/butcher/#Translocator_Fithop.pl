##Translocator_Fithop.pl
#Zone: butcher

sub EVENT_SAY{
  if ($text=~/Hail/i){
  $link1 = quest::saylink("travel to Ocean of Tears");
  quest::say("Hello there, $name. There seem to be some strange problems with the boats in this area. The Academy of Arcane Sciences has sent a small team of us to investigate them. If you need to [$link1] in the meantime, I can transport you to my companion there");
  }
  else
  {
	if ( defined $qglobals{akanon_racestarted} && defined $qglobals{akanon_race_playerflag})
	{
		$ToBeForeverFlag = "F";
		$startMsg = "[GNOME RACE]";
		if ( $qglobals{akanon_race_playerstep} == undef )
		{
			$client->Message(14,"$startMsg You were not properly flagged when leaving AkAnon into Steamfont!!");
		}
		elsif ( defined $qglobals{akanon_race_playerstep} && $qglobals{akanon_race_playerstep} == "4" )
		{
			quest::setglobal("akanon_race_playerstep","5",5,$ToBeForeverFlag);
			$client->Message(14,"$startMsg Another great leap forward for you! Make your way to East Freeport!!");
		}
		elsif ( $qglobals{akanon_race_playerstep} == "5" )
		{
			$client->Message(14,"$startMsg Travel to East Freeport!!");
		}
		else
		{
			$client->Message(14,"$startMsg Continue forward!!");
		}
	}

if($text=~/ocean of tears/i) {
  quest::say("Off you go!");  
} 
  quest::selfcast(2279);
  }
}
