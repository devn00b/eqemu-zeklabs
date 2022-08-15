sub EVENT_WAYPOINT_ARRIVE {

if ( $wp > 10 ) { $npc->AssignWaypoints(255); }
}

sub EVENT_DEATH {
if ( $charid > 0 ) { $npc->AddLootGroup(35); }
}