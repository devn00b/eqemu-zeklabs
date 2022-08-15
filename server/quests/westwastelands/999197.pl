#dark skeletons

sub EVENT_COMBAT
{
if ( $combat_state == 1 )
{
my $max = 9;
my $min = 1;
my $rnd = int(rand($max)) + $min;
if ( $rnd > 7 ) {
quest::say("You are not welcome in my lands!!"); }
else
{
if ( $rnd > 3 ) {
quest::say("Begone traveler! You should not be here!  These lands are forbidden!!!"); }
else {
quest::say("I shall strike you down with all my might."); }
} #end of else
} #combat state
} #sub event

sub EVENT_SPAWN {
if ($moborigname=~/a captain skeleton/i) {
quest::settimer("createboneloot",1); #captain skeleton loot table
}
else
{
quest::settimer("createstdloot",1); #standard skeleton loot table
}
}

sub EVENT_TIMER {
if ( $timer eq "createboneloot" ) { quest::stoptimer("createboneloot"); $npc->AddLootGroup(9); }
if ( $timer eq "createstdloot" ) { quest::stoptimer("createstdloot"); $npc->AddLootGroup(10); }
}