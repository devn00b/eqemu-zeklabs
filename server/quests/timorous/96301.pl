#maiden's voyage

sub EVENT_WAYPOINT_ARRIVE
{
	if ( $wp == 0 )
	{
		#quest::say("Arrived, signalling Island Shuttle.");
		quest::signalwith(96075,1,0);
	}
}