sub EVENT_SPAWN { quest::settimer(1,3); quest::settimer("createloot",1); }

sub EVENT_COMBAT
{
if ( $combat_state > 0 )
{
quest::say("Just leave me alone!!  What do you want?!?  I do not have anything of value!");
quest::stoptimer(1);
}
else
{
quest::settimer(1,3);
}
}

sub EVENT_TIMER
{
if ( $timer eq "1" )
{ $npc->DoAnim(12,0); }

if( $timer eq "2" )
{
quest::stoptimer(2);
my $max = 4;
my $rnd = int(rand($max));

if ( $rnd > 1 )
{
quest::say("Oh jeeze... oh jeeze... hes going to come for me this time I know it.... SOMEONE please help me!!");
$npc->DoAnim(24,0);
}
if ( $rnd < 2 )
{
quest::say("AHH!! HE IS COMING!! I know it!  Someone needs to stop him now!!");
$npc->DoAnim(40,0);
}
}

if ( $timer eq "createloot" ) { quest::stoptimer("createloot"); $npc->AddLootGroup(5); }
}

sub EVENT_WAYPOINT_ARRIVE
{
my $max = 12;
my $min = 4;
my $rnd = int(rand($max)) + $min;
quest::settimer(2,$rnd);
quest::stoptimer(1);
}

sub EVENT_WAYPOINT_DEPART {
quest::settimer(1,3);
}

sub EVENT_DEATH {
quest::stoptimer(1);
}