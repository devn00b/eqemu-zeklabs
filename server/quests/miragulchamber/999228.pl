#Enraged Ice elemental in miragul
sub EVENT_SPAWN {
quest::settimer("createiceloot",1);
}

sub EVENT_TIMER {
if ( $timer eq "createiceloot" ) { quest::stoptimer("createiceloot"); $npc->AddLootGroup(20); }
}