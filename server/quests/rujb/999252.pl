sub EVENT_SPAWN {
quest::settimer("elitegoblinloot",1); #elite goblin loot table
}

sub EVENT_TIMER {
if ( $timer eq "elitegoblinloot" ) { quest::stoptimer("elitegoblinloot"); $npc->AddLootGroup(32); }
}