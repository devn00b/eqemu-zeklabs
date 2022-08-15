sub EVENT_SPAWN {
quest::settimer("trescreateloot",1);
}

sub EVENT_TIMER {
if ( $timer eq "trescreateloot" ) { quest::stoptimer("trescreateloot"); $npc->AddLootGroup(7); }
}