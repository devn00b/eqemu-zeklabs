#orc leader
sub EVENT_WAYPOINT_ARRIVE {
if ( $wp > 2 ) { $npc->SetGrid(0); } #stop the leader at the ent
}

sub EVENT_DEATH {
if ( $charid > 0 ) {
$npc->AddLootGroup(35);
$npc->AddLootGroupEx(36,true);
}
}