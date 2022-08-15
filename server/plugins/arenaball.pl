sub dropBall {
my $client = shift;
my $charid = shift;
my $dropball = shift;

my $qglobals = plugin::var('qglobals');
if ( $dropball == 2 || defined $qglobals->{arena_ball_owner} )
{
$charidball = $qglobals->{arena_ball_owner};
if ( $dropball == 2 || $charid == $charidball )
{
$name = $client->GetName();
$x = $client->GetX();
$y = $client->GetY();
$z = $client->GetZ();
$h = $client->GetHeading();
$client->DelGlobal("arena_hill_allowed");
$client->SetGlobal("arena_ball_disable","1",5,"M15");
quest::setglobal("arena_ball_owner","0",7,"F");
quest::delglobal("arena_ball_ownername");
quest::delglobal("arena_ball_decay");
quest::delglobal("arena_ball_decay_enabled");
$client->NukeItem(52760);
if ( $dropball == 1 )
{
quest::creategroundobject(52760, $x, $y, $z, $h, 600000, 0, "arena_hill_allowed");
quest::signalwith(999639,1,0);
quest::ze(13,"[Arena] $name Drops the ball.");
}
else
{
quest::signalwith(999639,2,0);
quest::ze(13,"[Arena] $name Loses the ball.");
}
}
}
}