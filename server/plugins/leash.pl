sub leash_handlespawn {
  my $npc = shift;
  my $x = shift;
  my $y = shift;
  my $z = shift;
  quest::set_proximity($x - 100, $x + 100, $y - 100, $y + 100);
}

sub leash_handleaggro {
  my $npc = shift;
  quest::stoptimer("CheckLeash");
  quest::settimer("CheckLeash", 10);
}

sub leash_handletimer {
  my $npc = shift;
  my $timer = shift;
  my $x = shift;
  my $y = shift;
  my $z = shift;
  my $range = shift;
  if ($timer eq "CheckLeash"){
		my $GuardX = $npc->GetGuardPointX();
		my $GuardY = $npc->GetGuardPointY();
		my $GuardZ = $npc->GetGuardPointZ();
		my $CurDist = $npc->CalculateDistance($GuardX, $GuardY, $GuardZ);
		#quest::say("Dist: $CurDist");
		if ($CurDist > $range)
		{
			#$npc->SetHP(33349);
			quest::stoptimer("CheckLeash");
			# Get their current heading, since there is currently no way to get Guard Heading (yet)
			my $CurH = $npc->GetHeading();
			# Wipe the NPC's hate list as well
			$npc->WipeHateList();
			# Return them to their Guard Point
			$npc->GMMove($GuardX,$GuardY,$GuardZ,$CurH);
		}
	}
}

sub leash_handledeath {
  my $npc = shift;
  quest::stoptimer("CheckLeash");
}