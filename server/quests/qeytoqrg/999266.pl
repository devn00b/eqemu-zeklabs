my $entityID = "0";
my $pastX = "0";
my $pastY = "0";
my $pastZ = "0";

sub EVENT_SIGNAL {
$entityID = $signal; # track the character's entity id
quest::follow($entityID,10);
quest::settimer("depoptimer",599);
quest::settimer("checkclient",30);
quest::say("Thank you Corporal Gravlin for finding me a suitable escort.  We must move with haste so I arrive to the meeting on schedule!");
quest::set_proximity(195, 200, 4933.5, 5082.0, -5, 0);
}

sub EVENT_TIMER {
if ( $timer eq "checkclient" )
{
$character = quest::getclientbyentityid($entityID);
if ( defined($character) == 0 )
{
quest::stoptimer("checkclient");
quest::say("Lazy traveler... Some escort that was hmph!");
quest::signalwith(999264,1,0); #reset corporal
quest::depop(999266);
}
elsif ( $pastX == $x && $pastY == $y && $pastZ == $z )
{
my $max = 20;
my $min = 1;
my $rnd = int(rand($max)) + $min;
if ( $rnd < 7 ) { quest::say("What are we standing around for?"); }
elsif ( $rnd < 15 ) { quest::say("We are wasting time just standing around here..."); }
elsif ( $rnd < 20 ) { quest::say("There are a lot of dangerous creatures around us! We should get moving now!!"); }
}

$pastX = $x;
$pastY = $y;
$pastZ = $z;
}
elsif ( $timer eq "depoptimer" )
{
quest::stoptimer("depoptimer");
quest::say("We have taken too long!  You should turn back I must go as we have missed our meeting.");
quest::signalwith(999264,1,0); #reset corporal
quest::depop(999266);
}
}

sub EVENT_DEATH {
quest::signalwith(999264,1,0); #reset corporal
}

sub EVENT_ENTER {
quest::say("Thank you very much for your effort!  Have a few gold pieces for your troubles.  I believe I am safe from here traveler.");
$character = quest::getclientbyentityid($entityID);
if ( defined($character) )
{
$character->AddMoneyToPP(0,0,5,1,true);
$character->AddEXP("15000");
$character->SummonItem(quest::ChooseRandom(132487,10032,16791,1590));
}
quest::signalwith(999264,1,0); #reset corporal
quest::depop(999266);
}