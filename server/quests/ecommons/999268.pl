my $freeportWeapon = "22260";

sub EVENT_SPAWN {
quest::SetRunning(true);
quest::selfcast(424);
quest::settimer("depop",240);
}

sub EVENT_WAYPOINT_ARRIVE{
quest::creategroundobject($freeportWeapon, $x, $y, $z, $h, 300000);
}

sub EVENT_TIMER {
if ( $timer eq "depop" )
{
quest::depop(999268);
}
}