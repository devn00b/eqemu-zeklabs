sub EVENT_SPAWN {
quest::settimer("barbloot",1); #barb loot table
}

sub EVENT_TIMER {
if ( $timer eq "barbloot" ) { quest::stoptimer("barbloot"); $npc->AddLootGroup(33); }
}