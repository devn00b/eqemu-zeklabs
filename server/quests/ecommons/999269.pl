#escort npc merchant for quest npc event
my $entityID = "0";

sub EVENT_SIGNAL {
$entityID = $signal; # track the character's entity id
quest::follow($entityID,10);
quest::settimer("depoptimer",599);
quest::settimer("checkclient",30);
quest::say("Thank you very much for helping me.  Lets hurry this place is giving me the creeps!  My home borders the west commonlands south of the inn.");
quest::set_proximity(4641, 4790, -163, -50, -60, -30);
}

sub EVENT_TIMER {
if ( $timer eq "checkclient" )
{
$character = quest::getclientbyentityid($entityID);
if ( defined($character) == 0 )
{
quest::stoptimer("checkclient");
quest::say("Oh no.. my escort is gone!  I must flee now!!");
quest::signalwith(999267,1,0); #reset captain
quest::depop(999269);
}
}
elsif ( $timer eq "depoptimer" )
{
quest::stoptimer("depoptimer");
quest::say("We are taking too long!  I will make the travel on my own.");
quest::signalwith(999267,1,0); #reset captain
quest::depop(999269);
}
}

sub EVENT_ENTER {
quest::say("I appreciate your assistance traveler.  Here are some gold pieces for your journey back.");
$character = quest::getclientbyentityid($entityID);
if ( defined($character) )
{
$character->AddMoneyToPP(0,0,0,5,true);
$character->AddEXP(500000);
}
quest::signalwith(999267,1,0); #reset captain
quest::depop(999269);
}