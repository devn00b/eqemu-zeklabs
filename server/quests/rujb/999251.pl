sub EVENT_SPAWN {
quest::settimer("werewolfloot",1); #werewolf loot table
}

sub EVENT_TIMER {
if ( $timer eq "werewolfloot" ) { quest::stoptimer("werewolfloot"); $npc->AddLootGroup(34); }
}