sub EVENT_SPAWN {
quest::settimer("createbearclaw",1); #bear loot table
}

sub EVENT_TIMER {
if ( $timer eq "createbearclaw" ) { quest::stoptimer("createbearclaw"); $npc->AddLootGroup(25); }
}