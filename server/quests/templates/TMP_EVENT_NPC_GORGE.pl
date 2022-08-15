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
			#gorge approach is option 1, highpass to EK is option 0
			quest::setglobal("akanon_race_approach","1",5,$ToBeForeverFlag);

			#quest::setglobal("akanon_race_playerstep","7",5,$ToBeForeverFlag);
			$client->Message(14,"$startMsg You have passed through the Gorge and can continue to make your way to Qeynos Hills!");
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



}

}

sub EVENT_SAY {
quest::say("Hi");
}