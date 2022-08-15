#Miragul skeletons script
sub EVENT_SPAWN {
if ($moborigname=~/An_Enchanted_Frost_Skeleton/i) { quest::npcsize(8); quest::settimer("createboneloot",1);  }
}

sub EVENT_TIMER {
if ( $timer eq "createboneloot" ) { quest::stoptimer("createboneloot"); $npc->AddLootGroup(16); }
}