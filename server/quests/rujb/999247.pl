sub EVENT_SPAWN {
quest::settimer("createskeleloot",1); #defender loot table
}

sub EVENT_TIMER {
if ( $timer eq "createskeleloot" ) { quest::stoptimer("createskeleloot"); $npc->AddLootGroup(27); }
}