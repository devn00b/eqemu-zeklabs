sub EVENT_SPAWN {
quest::settimer("drakeloot",1); #drake loot table
}

sub EVENT_TIMER {
if ( $timer eq "drakeloot" ) { quest::stoptimer("drakeloot"); $npc->AddLootGroup(31); }
}