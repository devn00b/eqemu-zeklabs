sub EVENT_SPAWN {
quest::settimer("doscreateorcloot",1); #orc loot table
}

sub EVENT_TIMER {
if ( $timer eq "doscreateorcloot" ) { quest::stoptimer("doscreateorcloot"); $npc->AddLootGroup(11); }
}

sub EVENT_COMBAT
{
if ( $combat_state == 1 )
{
my $max = 9;
my $min = 1;
my $rnd = int(rand($max)) + $min;
if ( $rnd > 7 ) {
quest::say("We no welcome your kind here stranger."); }
else
{
if ( $rnd > 3 ) {
quest::say("This land is fer the orcs!"); }
else {
quest::say("Mmm dinner smells good tonight.."); }
} #end of else
} #combat state
} #sub event