sub EVENT_SPAWN {

$rnd = quest::ChooseRandom(4977,4976,4978,4979,4980,4980,4981,4981,4982,4982,4983,4983);
$rndnew = quest::ChooseRandom(0,1,2,3,4,5,6,7,8,9,10);
if ( $rndnew == 1 )
{
$npc->AddItem($rnd,1);
}

quest::settimer("createorcloot",1); #orc loot table
}

sub EVENT_TIMER {
if ( $timer eq "createorcloot" ) { quest::stoptimer("createorcloot"); $npc->AddLootGroup(15); }
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