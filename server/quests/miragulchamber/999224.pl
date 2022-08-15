#Miragul spider script
sub EVENT_SPAWN {
quest::settimer("createspiderloot",1);
}

sub EVENT_TIMER {
if ( $timer eq "createspiderloot" ) { quest::stoptimer("createspiderloot"); $npc->AddLootGroup(18); }
}