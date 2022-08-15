# little boat that goes to maidens voyage (Island Shuttle)

sub EVENT_WAYPOINT_ARRIVE
{
	if ( $wp == 0 )
	{
		quest::say("Awaiting next ship arrival in approximately 5 minutes.");
		quest::pause(360);
	}
}

sub EVENT_SIGNAL
{
	if ( $signal == 1 )
	{
		quest::say("Shuttle will be departing in 45 seconds!  Be sure to depart the shuttle when it arrives on the barge.");
		quest::resume();
		quest::pause(40);
	}
}