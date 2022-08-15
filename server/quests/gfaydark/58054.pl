#orc legionnaire - modified for GM event only use this NPCID for the event
sub EVENT_WAYPOINT_ARRIVE {

if ( $wp > 10 ) { $npc->AssignWaypoints(255); }
}

sub EVENT_DEATH {
#enable this only for the event
$npc->AddLootGroup(35);
}