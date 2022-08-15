sub EVENT_TIMER {
if ( $timer eq "doanimblush" )
{
quest::stoptimer("doanimblush");
$npc->DoAnim(52,0);
}
if ( $timer eq "doanimtwo" )
{
quest::stoptimer("doanimtwo");
my $max = 9;
my $min = 1;
my $rnd = int(rand($max)) + $min;
if ( $rnd > 4 )
{
$npc->DoAnim(57,0);
}
else
{
$npc->DoAnim(49,0);
}
quest::settimer("doanimthree",1);
}
if ( $timer eq "doanimthree" )
{
quest::stoptimer("doanimthree");
$npc->DoAnim(58,0);
}
}

sub EVENT_WAYPOINT_ARRIVE {
quest::facenpctype(999185);
$npc->DoAnim(58,0);
quest::settimer("doanimtwo",1);
}