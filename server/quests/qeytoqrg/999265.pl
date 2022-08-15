my $qeynosShield = "25008";

sub EVENT_SPAWN {
quest::SetRunning(true);
quest::selfcast(424);
quest::settimer("depop",90);
}

sub EVENT_WAYPOINT_ARRIVE{
quest::creategroundobject($qeynosShield, $x, $y, $z, $h, 300000);
}

sub EVENT_TIMER {
if ( $timer eq "depop" )
{
quest::depop(999265);
}
}