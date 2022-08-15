sub EVENT_SPAWN
{
#need to setup the zone long names since we can only pull by ID
#50 = Rathe Mtn

if ($zoneid eq 289) {
$zonename = "Tipt";
}elsif ($zoneid eq 50) {
$zonename = "Rathe Mountains";
}else {
$zonename = "Unknown Zone";
}

#last 2 you have a chance to drop so lets get a rand for them
$lastone = int(rand 100);
$lasttwo = int(rand 100);

#rnd 1 - 4 ooglies
$rnd = quest::ChooseRandom(135006,135007,135008,135009,135010,135011,135012,135013,135014,135015,135016,135017,135018,135019);
$rndtwo = quest::ChooseRandom(135006,135007,135008,135009,135010,135011,135012,135013,135014,135015,135016,135017,135018,135019);
if ($lastone > 50){
$rndthree = quest::ChooseRandom(135006,135007,135008,135009,135010,135011,135012,135013,135014,135015,135016,135017,135018,135019);
}
if ($lasttwo >50) {
$rndfour = quest::ChooseRandom(135006,135007,135008,135009,135010,135011,135012,135013,135014,135015,135016,135017,135018,135019);
}

#Add 3x loot some cash.
$npc->AddItem($rnd,1,0);
$npc->AddItem($rndtwo,1,0);

#make sure we check that we are actually adding an item so we dont crash.
if ($lastone > 50){
$npc->AddItem($rndthree,1,0);
}

if ($lasttwo >50) {
$npc->AddItem($rndfour,1,0);
}

$pltrnd  = int(rand 500); 
$gldrnd  = int(rand 900);
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
}


