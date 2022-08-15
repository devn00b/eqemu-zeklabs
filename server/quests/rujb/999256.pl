sub EVENT_SPAWN {
quest::settimer("butcherloot",1); #butcher's loot table
}

sub EVENT_TIMER {
if ( $timer eq "butcherloot" ) { quest::stoptimer("butcherloot"); $npc->AddLootGroup(30); }
}