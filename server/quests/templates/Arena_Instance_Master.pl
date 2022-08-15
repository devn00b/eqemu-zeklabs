my $eventStarted = 0;
my $eventTriggered = 0;
my $eventStage = 0;
my $timerIteration = 0;

sub EVENT_SIGNAL
{
if ( $signal == 0 && $eventStarted == 0 && $eventTriggered == 0 )
{
$eventTriggered = 1;
quest::setzonepvp(0);
}
elsif ( $signal == 1 && $eventStarted == 0 )
{
quest::ze(13,"Awaiting all contestants to enter, please wait!");
}
elsif ( $signal == 2 && $eventStarted == 0 && $eventStage == 0 )
{
quest::ze(13,"All contestants have entered the arena, match will begin in 60 seconds!");
quest::stoptimer("timeout");
$eventStage = 1;
}
elsif ( $signal == 3 && $eventStarted == 0 && $eventStage == 1 )
{
quest::ze(13,"All contestants have entered the arena, match will begin in 10 seconds!");
$eventStage = 2;
}
elsif ( $signal == 4 && $eventStarted == 0 && $eventStage == 2 )
{
$eventStarted = 1;
$eventStage = 3;
quest::stoptimer("timeout");
quest::ze(13,"Match begins! FIGHT!!");
quest::setzonepvp(1);
}
elsif ( $signal == 5 )
{
      my @clientlist = $entity_list->GetClientList();
      foreach $ent (@clientlist) {
         plugin::handleArenaLogout($ent,$instanceid,$zoneid);
	}
}
}

sub EVENT_SPAWN
{
$eventStarted = 0;
$eventTriggered = 0;
$eventStage = 0;
$timerIteration = 0;
quest::settimer("timeout",60);
quest::settimer("teleport_timeout",1200);
}

sub EVENT_TIMER
{

if ( $timer eq "teleport_timeout" )
{
      my @clientlist = $entity_list->GetClientList();
      foreach $ent (@clientlist) {
         plugin::handleArenaLogout($ent,$instanceid,$zoneid);
	}
quest::ze(13,"Teleport restrictions lifted (instances 20 minute marker).");
quest::stoptimer("teleport_timeout");
}

if ( $timer eq "timeout" )
{

$timerIteration += 1;

if ( $timerIteration == 1 )
{
quest::ze(13,"Not all members have entered the match yet, but the match will automatically start in 2 minutes!");
}
elsif ( $timerIteration == 2 )
{
quest::ze(13,"Not all members have entered the match yet, but the match will automatically start in 1 minute!");
}
elsif ( $timerIteration > 2 )
{
quest::stoptimer("timeout");

if ( $eventStarted == 0 )
{
      my @clientlist = $entity_list->GetClientList();
      foreach $ent (@clientlist) {
         plugin::handleArenaLogout($ent,$instanceid,$zoneid);
	}
$eventTriggered = 1;
$eventStarted = 1;
$eventStage = 3;
quest::ze(13,"Match begins! FIGHT!!");
quest::setzonepvp(1);
}
}
}#end timer

}