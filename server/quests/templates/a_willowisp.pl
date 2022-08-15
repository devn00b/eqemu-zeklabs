#this will fix the willowisp getting into a 'stuck' state while moving
sub EVENT_WAYPOINT_DEPART
{
quest::settimer("pulseanim",5);
}

sub EVENT_WAYPOINT_ARRIVE
{
quest::stoptimer("pulseanim");
}

sub EVENT_TIMER
{
if ( $timer eq "pulseanim" ) { $npc->DoAnim(26,0); } #this anim seems to do the trick
}