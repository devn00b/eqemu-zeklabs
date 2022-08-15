$triggered = 0;

sub EVENT_SPAWN { quest::settimer(1,1); quest::settimer(2,270); quest::settimer(3,5); }

sub EVENT_TIMER
{
if ( $timer==1 )
{ $npc->DoAnim(34,0); }
if ( $timer==2 )
{ quest::ze(13,"The merciless dragon disappears into the air as swiftly as he arrived.",13); quest::depop(999169); quest::stoptimer(1); quest::stoptimer(2); }
if ( $timer==3 )
{
quest::unique_spawn(999171,282,0,470.5,-3811.2,168.0,29.5);
quest::stoptimer(3);
quest::ze(13,"You hear a loud disturbance in the air and realize something is making a swift descent.",13);
}

}

sub EVENT_WAYPOINT_ARRIVE
{
if ( $wp >= 144 && $triggered == 0 )
{
$triggered = 1;
quest::stoptimer(1);
quest::stoptimer(2);
quest::ze(13,"The merciless dragon disappears into the air as swiftly as he arrived.");
quest::depop(999169);
}

}