sub EVENT_SPAWN
{
#need to setup the zone long names since we can only pull by ID
#50 = Rathe Mtn
#14 = SK
#36 = Befallen
#63 = Unrest
#######

#set the 1st timer for 15mins or 900 seconds
quest::settimer("repeat", 5);

#if ($zoneid eq 289) {
#$zonename = "Tipt";
#}elsif ($zoneid eq 50) {
#$zonename = "Rathe Mountains";
#}elsif ($zoneid eq 15) {
#$zonename = "Eastern Karana";
#}elsif ($zoneid eq 36) {
#$zonename = "Befallen";
#}else {
$zonename = $zoneln;
#}

#last 2 you have a chance to drop so lets get a rand for them
$lastone = int(rand 100);
$lasttwo = int(rand 100);

#rnd 1 - 4 ooglies
$rnd = quest::ChooseRandom(135006,135007,135008,135009,135010,135011,135012,135013,135014,135015,135016,135017,135018,135019,143164);
$rndtwo = quest::ChooseRandom(135006,135007,135008,135009,135010,135011,135012,135013,135014,135015,135016,135017,135018,135019,143164);
if ($lastone > 50){
$rndthree = quest::ChooseRandom(135006,135007,135008,135009,135010,135011,135012,135013,135014,135015,135016,135017,135018,135019,143164);
}
if ($lasttwo >50) {
$rndfour = quest::ChooseRandom(135006,135007,135008,135009,135010,135011,135012,135013,135014,135015,135016,135017,135018,135019,143164);
}

#Add 3x loot some cash.
$npc->AddItem($rnd,1,0);
$npc->AddItem($rndtwo,1,0);

#if ($lastone > 50){
#$npc->AddItem($rndthree,1,0);
#}

#if ($lasttwo >50) {
#$npc->AddItem($rndfour,1,0);
#}

$pltrnd  = int(rand 50); 
$gldrnd  = int(rand 200);
$slvrnd  = int(rand 500);
$cprrnd  = int(rand 200);

$npc->AddCash($cprrnd,$silvrnd,$gldrnd,$pltrnd);

$whatshout = int(rand 100);
if ($whatshout <25){
quest::shout2("$zonename is a really boring area. I've yet to find a single thing here to match my power.");
}
elsif ($whatshout>25 && $whatshout <50){
quest::shout2("You know, $zonename, is really pretty this time of year!");
}
elsif ($whatshout >50 && $whatshout <75) {
quest::shout2("I love the smell of death in the morning! Who else wants to come to $zonename and die to me?");
}
else {
quest::shout2("I pitty all of you. Wishing your loot was as good as mine. It isn't. You weaklings are too scared to even face me in combat. Here I stand in $zonename waiting.");
}

}

sub EVENT_DEATH {
   quest::faction(2010, -4000);
quest::shout2("Screw you $name, I'll be back and you will die!!");

}

sub EVENT_TIMER {
	if ($timer eq "repeat") {
$zonename = "East Karana";

#now lets setup the shout
$whatshout = int(rand 100);
#min number is 1000, we then add 6201 to it to get a max # of 7201
$rndrng = 6201;
$minrnd = 1000;
$howlong = int(rand($rndrng)) + $minrnd;

if ($whatshout <25){
quest::shout2("$zonename is a really boring area. I've yet to find a single thing here to match my power.");
}
elsif ($whatshout>25 && $whatshout <50){
quest::shout2("You know, $zonename, is really pretty this time of year!");
}
elsif ($whatshout >50 && $whatshout <75) {
quest::shout2("I love the smell of death in the morning! Who else wants to come to $zonename and die to me?");
}
else {
quest::shout2("I pitty all of you. Wishing your loot was as good as mine. It isn't. You weaklings are too scared to even face me in combat. Here I stand in $zonename waiting.");
}


		quest::stoptimer("repeat");
		quest::settimer("repeat", $howlong); 
#2hrs is 7200 using 10 for testing. Keep repeating every 60 seconds
	}
}
