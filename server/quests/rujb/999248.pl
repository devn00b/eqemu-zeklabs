sub EVENT_SPAWN {
quest::settimer("createbasiliskloot",1); #basilisk loot table
}

sub EVENT_TIMER {
if ( $timer eq "createbasiliskloot" ) { quest::stoptimer("createbasiliskloot"); $npc->AddLootGroup(26); }
}