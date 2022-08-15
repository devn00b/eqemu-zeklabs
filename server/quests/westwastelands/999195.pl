sub EVENT_SPAWN {
$rnd = quest::ChooseRandom(4989,4984,4984,4985,4985,4986,4987,4988,4988,4990,4991);
$rndnew = quest::ChooseRandom(0,1,2,3,4,5,6,7,8,9,10);
if ( $rndnew == 1 )
{
$npc->AddItem($rnd,1);
}

quest::settimer("createnewloot",1);
}

sub EVENT_TIMER {
if ( $timer eq "createnewloot" ) { quest::stoptimer("createnewloot"); $npc->AddLootGroup(14); }
}