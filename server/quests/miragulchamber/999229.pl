#Tinr spawn table
sub EVENT_SPAWN {
quest::settimer("createtinrloot",1);
}

sub EVENT_TIMER {
if ( $timer eq "createtinrloot" ) { quest::stoptimer("createtinrloot"); $npc->AddLootGroup(23); }
}