# Attempt to limit players level 53 and over from engaging nagafen and vox.
# Banished to Lavastorm.

sub EVENT_SPAWN {
$qglobals{kerafyrm} = undef;

#$rnd = quest::ChooseRandom(44403,70746,31405,15770,23826,15770,65440,26782,25543);
#$rndtwo = quest::ChooseRandom(44403,70746,31405,15770,23826,15770,65440,1001,10152,1006,1003,1007,1010,1011,1012,1005,1004);
#$rndthree = quest::ChooseRandom(21821,21822,21823,132487,11696,6639,64193,145166,1011,1012,1005,1004);
#$rndfour = quest::ChooseRandom(21821,21822,21823,132487,11696,6639,64193,145166,1011,1012);

#Add 3x loot some cash.
#$npc->AddItem($rnd,1,0);
#$npc->AddItem($rndtwo,1,0);
#$npc->AddItem($rndthree,1,0);
#$npc->AddItem($rndfour,1,0);
#add cash
$pltrnd  = int(rand 1000);
$gldrnd  = int(rand 900);
$slvrnd  = int(rand 500);
$cprrnd  = int(rand 200);

$npc->AddCash($cprrnd,$silvrnd,$gldrnd,$pltrnd);


 if((!defined $qglobals{kerafyrm}) || (defined $qglobals{kerafyrm} && $qglobals{kerafyrm} != 3)) {
  quest::settimer("kerafyrmnag",10);
}

quest::set_proximity($x - 100, $x + 100, $y - 100, $y + 100);
quest::settimer("CheckLeash", 1);
}

sub EVENT_ENTER {
#  if (($ulevel >= 53) && ($status < 80)) {
#    quest::echo(0, "I will not fight you, but I will banish you!");
#    $client->MovePC(27,0,0,0,0);
#  }
}

sub EVENT_AGGRO {
#  if (($ulevel >= 53) && ($status < 80)) {
#    quest::echo(0, "I will not fight you, but I will banish you!");
#    $client->MovePC(27,0,0,0,0);
#  }
		quest::settimer("CheckLeash", 1);
}

sub EVENT_TIMER {

 if($timer eq "kerafyrmnag" && defined $qglobals{kerafyrm} && $qglobals{kerafyrm} == 1) {
  quest::shout("Ha! The Ring and Claws are doomed! The Sleeper has been awakened, what a glorious day! Lady Vox, I will see you soon, our long delayed nuptials can now proceed!");
  quest::stoptimer("kerafyrmnag");
}
#checkleash additions

	if ($timer eq "CheckLeash"){
		quest::set_proximity($x - 100, $x + 100, $y - 100, $y + 100);
		my $GuardX = $npc->GetGuardPointX();
		my $GuardY = $npc->GetGuardPointY();
		my $GuardZ = $npc->GetGuardPointZ();
		my $CurDist = $npc->CalculateDistance($GuardX, $GuardY, $GuardZ);
		#quest::say("Dist: $CurDist");
		if ($CurDist > 75)
		{
			$npc->SetHP(33349);
			quest::stoptimer("CheckLeash");
			# Get their current heading, since there is currently no way to get Guard Heading (yet)
			my $CurH = $npc->GetHeading();
			# Wipe the NPC's hate list as well
			$npc->WipeHateList();
			# Return them to their Guard Point
			$npc->GMMove($GuardX,$GuardY,$GuardZ,$CurH);
		}
		else
		{
		quest::stoptimer("CheckLeash");
		quest::settimer("CheckLeash", .5);
		}
	}
#end leash additions
}

sub EVENT_DEATH {
#plugin::handlenpc_epicloot($client,$npc,2);
#plugin::handlenpc_epicloot($client,$npc,1);

}


# EOF zone: soldungb ID: 32040 NPC: Lord_Nagafen
