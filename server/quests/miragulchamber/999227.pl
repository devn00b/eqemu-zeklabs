#Frantic Observer
sub EVENT_SPAWN {
quest::settimer("createobserveloot",1);
}

sub EVENT_TIMER {
if ( $timer eq "createobserveloot" ) { quest::stoptimer("createobserveloot"); $npc->AddLootGroup(17); }
}