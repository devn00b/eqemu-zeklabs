#Kobold loot
sub EVENT_SPAWN {
quest::settimer("createkoboldloot",1);
}

sub EVENT_TIMER {
if ( $timer eq "createkoboldloot" ) { quest::stoptimer("createkoboldloot"); $npc->AddLootGroup(19); }
}