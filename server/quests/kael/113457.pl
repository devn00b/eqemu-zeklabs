sub EVENT_SPAWN
{
quest::settimer("despawn",3600);
}

sub EVENT_TIMER
{
if ( $timer eq "despawn" )
{
quest::stoptimer("despawn");
quest::depop();
}
}

sub EVENT_DEATH
{
quest::stoptimer("despawn");
}