sub EVENT_SPAWN { 
my $max = 12;
my $min = 7;
my $rnd = int(rand($max)) + $min;
quest::settimer(1,$rnd); 
quest::settimer("createtrickgoblin",1); #trickster goblin loot table
}

sub EVENT_COMBAT {

	if ( $combat_state == 1 )
	{
	  quest::stoptimer(1);
	}
	else
	{
		my $max = 12;
		my $min = 7;
		my $rnd = int(rand($max)) + $min;
	  quest::settimer(1,$rnd);
	}
}

sub EVENT_TIMER { 
if ( $timer eq "createtrickgoblin" ) { quest::stoptimer("createtrickgoblin"); $npc->AddLootGroup(28); }
if ( $timer eq "1" ) { 
my $max = 3;
my $min = 0;
my $rnd = int(rand($max)) + $min;
if ( $rnd > 1 ) { $npc->DoAnim(20,0); } else { $npc->DoAnim(26,0); }
} }