sub EVENT_SPAWN { quest::settimer("1",270); quest::settimer("2",65); quest::settimer("3",15); }

sub EVENT_COMBAT
{
quest::stoptimer("2");
}

sub EVENT_TIMER
{
if ( $timer eq "1" )
{ quest::stoptimer("1"); quest::depop(); }
if ( $timer eq "2" )
{ quest::attacknpctype(999169); quest::settimer("2",1); quest::stoptimer("3"); }

if ( $timer eq "3" )
{
quest::say("Everyone should get inside immediately!! Stay inside the compound and away from the outer walls!");
}

}

sub EVENT_DEATH
{
quest::stoptimer("1");
quest::stoptimer("2");
quest::stoptimer("3");
}