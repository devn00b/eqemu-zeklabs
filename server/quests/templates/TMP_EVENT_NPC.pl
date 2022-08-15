use feature qw/switch/;

sub getStep
{
	my $num = shift;

	my $returnval = "Unknown $num";
	given($num){
	when(0) {
		$returnval = "Starting in Akanon and Getting Drunk";
	}
	when(1) {
		$returnval = "Entering into Steamfont from Akanon";
	}
	when(2) {
		$returnval = "Entering into Lesser Faydark from Steamfont";
	}
	when(3) {
		$returnval = "Entering into Greater Faydark from Lesser Faydark";
	}
	when(4) {
		$returnval = "Entering into Butcherblock from Greater Faydark";
	}
	when(5) {
		$returnval = "Speaking to Translocator Fithop at the Butcherblock Docks";
	}
	when(6) {
		$returnval = "Entering into East Freeport Dock Port from Ocean of Tears";
	}
	when(7) {
		$returnval = "Entering into Qeynos Hills from West Karana";
	}
	when(8) {
		$returnval = "Entering into Toxxulia Forest from Erudin";
	}
	when(9) {
		$returnval = "Entering into Kerra Isle from Toxxulia Forest";
	}
	}
	return $returnval;
}

sub EVENT_SPAWN
{
	$x = $npc->GetX();
	$y = $npc->GetY();
	$z = $npc->GetZ();
	quest::set_proximity($x - 125, $x + 125, $y - 125, $y + 125, $z - 50, $z + 50);
}

	sub EVENT_ENTER {
	$ToBeForeverFlag = "F";
	$eventDurationFlag = "H49";

	$startMsg = "[GNOME RACE]";

if ( defined $qglobals{akanon_racefinish_playerflag} )
{
return;
}

#quest::say("test $zoneid");
$racestart = $qglobals{akanon_racestarted};
$playerflag = $qglobals{akanon_race_playerflag};
$playerstep = $qglobals{akanon_race_playerstep};

#quest::shout("Steamfont: race: $racestart player: $playerflag");
given($zoneid){
#start steamfont
  when(56) {
	if ( defined $qglobals{akanon_racestarted} && defined $qglobals{akanon_race_playerflag})
	{
		if ( defined $qglobals{akanon_race_playerstep} && $qglobals{akanon_race_playerstep} == "1" )
		{
			$client->Message(14,"$startMsg Travel to Lesser Faydark!!");
		}
		else
		{
			$timestamp = localtime(time);
			quest::setglobal("akanon_race_starttime",$timestamp,5,$ToBeForeverFlag);
			quest::setglobal("akanon_race_playerstep","1",5,$ToBeForeverFlag);
			$client->Message(14,"$startMsg So your journey begins, make your way towards Lesser Faydark!!");
			$client->Message(13,"$startMsg Race start time is $timestamp.");
		}
	}
}
#end steamfont

#start lfaydark
  when(57) {
	if ( defined $qglobals{akanon_racestarted} && defined $qglobals{akanon_race_playerflag})
	{
		if ( !defined $qglobals{akanon_race_playerstep} )
		{
			$client->Message(14,"$startMsg You were not properly flagged when leaving AkAnon into Steamfont!!");
		}
		elsif ( $playerstep < 1 )
		{
			$nextStepText = getStep($playerstep+1);
			$client->Message(14,"$startMsg You have missed a step on your adventure ($nextStepText)!");
		}
		elsif ( defined $qglobals{akanon_race_playerstep} && $qglobals{akanon_race_playerstep} == "1" )
		{
			quest::delglobal("nobindallowed");
			#do not forget to add 'false' as a final argument
			$client->SetBindPoint($zoneid,$client->GetX(),$client->GetY(),$client->GetZ(),"0");
			quest::setglobal("nobindallowed","1",5,$eventDurationFlag);
			$client->Message(13,"You have been rebound to this checkpoint.");

			quest::setglobal("akanon_race_playerstep","2",5,$ToBeForeverFlag);
			$client->Message(14,"$startMsg The journey continues! Make your way towards Greater Faydark!!");
			$client->SendSound();
		}
		elsif ( $qglobals{akanon_race_playerstep} == "2" )
		{
			$client->Message(14,"$startMsg Travel to Greater Faydark!!");
		}
		else
		{
			$client->Message(14,"$startMsg Continue forward to Greater Faydark!!");
		}
	}
}
#end lfaydark

#start gfaydark
  when(54) {
	if ( defined $qglobals{akanon_racestarted} && defined $qglobals{akanon_race_playerflag})
	{
		if ( !defined $qglobals{akanon_race_playerstep} )
		{
			$client->Message(14,"$startMsg You were not properly flagged when leaving AkAnon into Steamfont!!");
		}
		elsif ( $playerstep < 2 )
		{
			$nextStepText = getStep($playerstep+1);
			$client->Message(14,"$startMsg You have missed a step on your adventure ($nextStepText)!");
		}
		elsif ( defined $qglobals{akanon_race_playerstep} && $qglobals{akanon_race_playerstep} == "2" )
		{
			quest::delglobal("nobindallowed");
			#do not forget to add 'false' as a final argument
			$client->SetBindPoint($zoneid,$client->GetX(),$client->GetY(),$client->GetZ(),"0");
			quest::setglobal("nobindallowed","1",5,$eventDurationFlag);
			$client->Message(13,"You have been rebound to this checkpoint.");

			quest::setglobal("akanon_race_playerstep","3",5,$ToBeForeverFlag);
			$client->Message(14,"$startMsg You are making good progress, keep it up! Make your way towards Butcherblock!!");
			$client->SendSound();
		}
		elsif ( $qglobals{akanon_race_playerstep} == "3" )
		{
			$client->Message(14,"$startMsg Travel to Butcherblock!!");
		}
		else
		{
			$client->Message(14,"$startMsg Continue forward to Butcherblock!!");
		}
	}
}
#end gfaydark

#start butcher
  when(68) {
	if ( defined $qglobals{akanon_racestarted} && defined $qglobals{akanon_race_playerflag})
	{
		if ( !defined $qglobals{akanon_race_playerstep} )
		{
			$client->Message(14,"$startMsg You were not properly flagged when leaving AkAnon into Steamfont!!");
		}
		elsif ( $playerstep < 3 )
		{
			$nextStepText = getStep($playerstep+1);
			$client->Message(14,"$startMsg You have missed a step on your adventure ($nextStepText)!");
		}
		elsif ( defined $qglobals{akanon_race_playerstep} && $qglobals{akanon_race_playerstep} == "3" )
		{
			quest::delglobal("nobindallowed");
			#do not forget to add 'false' as a final argument
			$client->SetBindPoint($zoneid,$client->GetX(),$client->GetY(),$client->GetZ(),"0");
			quest::setglobal("nobindallowed","1",5,$eventDurationFlag);
			$client->Message(13,"You have been rebound to this checkpoint.");

			quest::setglobal("akanon_race_playerstep","4",5,$ToBeForeverFlag);
			$client->Message(14,"$startMsg Well done adventurer! Make your way to the Butcherblock dock and travel to Ocean of Tears!!");
			$client->SendSound();
		}
		elsif ( $qglobals{akanon_race_playerstep} == "4" )
		{
			$client->Message(14,"$startMsg Travel to Ocean of Tears!!");
		}
		else
		{
			$client->Message(14,"$startMsg Continue forward to Ocean of Tears!!");
		}
	}
}
#end butcher


#next step of event handled with #Translocator_Fithop in butcher, they go to OOT then East Freeport, flag # will be set to akanon_race_playerstep=5


#start east freeport
  when(10) {
	if ( defined $qglobals{akanon_racestarted} && defined $qglobals{akanon_race_playerflag})
	{
		if ( !defined $qglobals{akanon_race_playerstep} )
		{
			$client->Message(14,"$startMsg You were not properly flagged when leaving AkAnon into Steamfont!!");
		}
		elsif ( $playerstep < 5 )
		{
			$nextStepText = getStep($playerstep+1);
			$client->Message(14,"$startMsg You have missed a step on your adventure ($nextStepText)!");
		}
		elsif ( defined $qglobals{akanon_race_playerstep} && $qglobals{akanon_race_playerstep} == "5" )
		{
			quest::delglobal("nobindallowed");
			$client->SetBindPoint($zoneid,$client->GetX(),$client->GetY(),$client->GetZ(),"0");
			quest::setglobal("nobindallowed","1",5,$eventDurationFlag);
			$client->Message(13,"You have been rebound to this checkpoint.");

			quest::setglobal("akanon_race_playerstep","6",5,$ToBeForeverFlag);
			$client->Message(14,"$startMsg Now that you are done with the ocean breeze you have a long way to Qeynos Hills!!");

			#drunken trinket
			$client->SummonItem(142744);
			$client->Message(14,"$startMsg You have earned a Drunken Trinket for reaching this checkpoint!");

			$client->SendSound();
		}
		elsif ( $qglobals{akanon_race_playerstep} == "6" )
		{
			$client->Message(14,"$startMsg Travel to Qeynos Hills!!");
		}
		else
		{
			$client->Message(14,"$startMsg Continue forward to Qeynos Hills!!");
		}
	}
}
#end east freeport


#start highpass to east karana (EK zone)
  when(15) {
	if ( defined $qglobals{akanon_racestarted} && defined $qglobals{akanon_race_playerflag})
	{
		if ( !defined $qglobals{akanon_race_playerstep} )
		{
			$client->Message(14,"$startMsg You were not properly flagged when leaving AkAnon into Steamfont!!");
		}
		elsif ( $playerstep < 6 )
		{
			$nextStepText = getStep($playerstep+1);
			$client->Message(14,"$startMsg You have missed a step on your adventure ($nextStepText)!");
		}
		elsif ( !defined $qglobals{akanon_race_approach} && defined $qglobals{akanon_race_playerstep} && $qglobals{akanon_race_playerstep} == "6" )
		{
			quest::delglobal("nobindallowed");
			$client->SetBindPoint($zoneid,$client->GetX(),$client->GetY(),$client->GetZ(),"0");
			quest::setglobal("nobindallowed","1",5,$eventDurationFlag);
			$client->Message(13,"You have been rebound to this checkpoint.");
			quest::setglobal("akanon_race_approach","0",5,$ToBeForeverFlag);

			#quest::setglobal("akanon_race_playerstep","7",5,$ToBeForeverFlag);
			$client->Message(14,"$startMsg You have made it through the pass, you have a long path to descend towards Qeynos Hills!");
			#$earnItemID = quest::ChooseRandom(25928,25929,25931,25932,25937,25938);
			#$client->SummonItem($earnItemID);
			#$client->Message(14,"$startMsg You have earned a Prominent Adventurer item for reaching this checkpoint!");
			$client->SendSound();
			
		}
		else
		{
			$client->Message(14,"$startMsg Continue forward to Qeynos Hills!!");
		}
	}
}
#end highpass to east karana

#start lake rathe to south karana (SK zone)
  when(14) {
	if ( defined $qglobals{akanon_racestarted} && defined $qglobals{akanon_race_playerflag})
	{
		if ( !defined $qglobals{akanon_race_playerstep} )
		{
			$client->Message(14,"$startMsg You were not properly flagged when leaving AkAnon into Steamfont!!");
		}
		elsif ( $playerstep < 6 )
		{
			$nextStepText = getStep($playerstep+1);
			$client->Message(14,"$startMsg You have missed a step on your adventure ($nextStepText)!");
		}
		elsif ( !defined $qglobals{akanon_race_approach} && defined $qglobals{akanon_race_playerstep} && $qglobals{akanon_race_playerstep} == "6" )
		{
			quest::delglobal("nobindallowed");
			$client->SetBindPoint($zoneid,$client->GetX(),$client->GetY(),$client->GetZ(),"0");
			quest::setglobal("nobindallowed","1",5,$eventDurationFlag);
			$client->Message(13,"You have been rebound to this checkpoint.");
			quest::setglobal("akanon_race_approach","2",5,$ToBeForeverFlag);

			#quest::setglobal("akanon_race_playerstep","7",5,$ToBeForeverFlag);
			$client->Message(14,"$startMsg You have made a long journey to get here through the deserts of Ro and the Mountains of Rathe, you must continue to Qeynos Hills to complete your adventure!");
			#$earnItemID = quest::ChooseRandom(25928,25929,25931,25932,25937,25938);
			#$client->SummonItem($earnItemID);
			#$client->Message(14,"$startMsg You have earned a Prominent Adventurer item for reaching this checkpoint!");
			$client->SendSound();
			
		}
		else
		{
			$client->Message(14,"$startMsg Continue forward to Qeynos Hills!!");
		}
	}
}
#end lake rathe to south karana


#start qeynos hills
  when(4) {
	if ( defined $qglobals{akanon_racestarted} && defined $qglobals{akanon_race_playerflag})
	{
		if ( !defined $qglobals{akanon_race_playerstep} )
		{
			$client->Message(14,"$startMsg You were not properly flagged when leaving AkAnon into Steamfont!!");
		}
		elsif ( $playerstep < 6 )
		{
			$nextStepText = getStep($playerstep+1);
			$client->Message(14,"$startMsg You have missed a step on your adventure ($nextStepText)!");
		}
		elsif ( defined $qglobals{akanon_race_playerstep} && $qglobals{akanon_race_playerstep} == "6" )
		{
			quest::delglobal("nobindallowed");
			$client->SetBindPoint($zoneid,$client->GetX(),$client->GetY(),$client->GetZ(),"0");
			quest::setglobal("nobindallowed","1",5,$eventDurationFlag);
			$client->Message(13,"You have been rebound to this checkpoint.");

			quest::setglobal("akanon_race_playerstep","7",5,$ToBeForeverFlag);
			$client->Message(14,"$startMsg You have made it through some fierce lands to arrive here, you are almost there, make way to Toxxulia Forest!!");
			$earnItemID = quest::ChooseRandom(25928,25929,25931,25932,25937,25938);
			$client->SummonItem($earnItemID);
			$client->Message(14,"$startMsg You have earned a Prominent Adventurer item for reaching this checkpoint!");
			$client->SendSound();
			
		}
		elsif ( $qglobals{akanon_race_playerstep} == "7" )
		{
			$client->Message(14,"$startMsg Travel to Toxxulia Forest!!");
		}
		else
		{
			$client->Message(14,"$startMsg Continue forward to Toxxulia Forest!!");
		}
	}
}
#end qeynos hills


#start tox
  when(38) {
	if ( defined $qglobals{akanon_racestarted} && defined $qglobals{akanon_race_playerflag})
	{
		if ( !defined $qglobals{akanon_race_playerstep} )
		{
			$client->Message(14,"$startMsg You were not properly flagged when leaving AkAnon into Steamfont!!");
		}
		elsif ( $playerstep < 7 )
		{
			$nextStepText = getStep($playerstep+1);
			$client->Message(14,"$startMsg You have missed a step on your adventure ($nextStepText)!");
		}
		elsif ( defined $qglobals{akanon_race_playerstep} && $qglobals{akanon_race_playerstep} == "7" )
		{
			quest::delglobal("nobindallowed");
			$client->SetBindPoint($zoneid,$client->GetX(),$client->GetY(),$client->GetZ(),"0");
			quest::setglobal("nobindallowed","1",5,$eventDurationFlag);
			$client->Message(13,"You have been rebound to this checkpoint.");

			quest::setglobal("akanon_race_playerstep","8",5,$ToBeForeverFlag);
			$client->Message(14,"$startMsg Much progress has been made, you are reaching the end!! Get to Kerra Isle!!");

			$client->SummonItem(142842);
			$client->Message(14,"$startMsg You have earned a Palladium PVP Ticket for reaching this checkpoint!");

			$client->SendSound();
		}
		elsif ( $qglobals{akanon_race_playerstep} == "8" )
		{
			$client->Message(14,"$startMsg Travel to Kerra Isle!!");
		}
		else
		{
			$client->Message(14,"$startMsg Continue forward to Kerra Isle!!");
		}
	}
}
#end tox


#start kerraridge
  when(74) {
	if ( defined $qglobals{akanon_racestarted} && defined $qglobals{akanon_race_playerflag})
	{
		if ( !defined $playerstep )
		{
			$client->Message(14,"$startMsg You were not properly flagged when leaving AkAnon into Steamfont!!");
		}
		elsif ( $playerstep < 8 )
		{
			$nextStepText = getStep($playerstep+1);
			$client->Message(14,"$startMsg You have missed a step on your adventure ($nextStepText)!");
		}
		elsif ( defined $qglobals{akanon_race_playerstep} && $qglobals{akanon_race_playerstep} == "8" )
		{
			quest::setglobal("akanon_race_playerstep","9",5,$ToBeForeverFlag);
			$client->Message(14,"$startMsg Congratulations you made your way through Norrath, what a fantastic feat!!!");
			$client->Message(13,"$startMsg ..But what is this...? Something is wrong!  Screams can be heard from Kerra Isle!!");
			#potion of spite
			$client->SummonItem(142757,2);
			#potion of blissful healing
			$client->SummonItem(142755,2);
			$client->Message(14,"$startMsg You have earned two special potions for reaching this checkpoint!");
			$client->SendSound();
		}
		elsif ( $qglobals{akanon_race_playerstep} == "9" )
		{
			$client->Message(13,"$startMsg Investigate the screams in Kerra Isle!!");
		}
		else
		{
			$client->Message(14,"$startMsg You are not properly flagged, uh oh ($playerflag)!!");
		}
	}
}
#end kerraridge

}

}

sub EVENT_SAY {
quest::say("Hi");
}