#various ice spores in miragul
sub EVENT_SPAWN {
quest::settimer("createsporeloot",1);
}

sub EVENT_TIMER {
if ( $timer eq "createsporeloot" ) { quest::stoptimer("createsporeloot"); $npc->AddLootGroup(21); }
}