sub EVENT_SPAWN
{
#lets do 3 items drops, so 3 randoms so we dont drop same item 3 times

#$rnd = quest::ChooseRandom(8957,8958,8959,8971,8972,8973,8974);
$rnd =  quest::ChooseRandom(7466,26526,2737,62230,62229,1625,14742,8957,8958,8959,8971,8972,8973,8974);

#$rndtwo = quest::ChooseRandom(25287,25026,25027,25098,14745,1283,145366,30041,145378,145379,145380,145381,145382,145383,145385,145386,145387);
$rndtwo =  quest::ChooseRandom(26001,6715,5646,7250,7328,7371,5163,145405);

#$rndthree = quest::ChooseRandom(25283,25284,25285,25286,25287,25288,25289,25290,25291,5727,29206,22816,30069,2061,143063);
$rndthree =  quest::ChooseRandom(62227,14751,2738,1414,2739,62235,1607,5163,6603,6604,6608);

#Add 3x loot some cash.
$npc->AddItem($rnd,1,0);
$npc->AddItem($rndtwo,1,0);
$npc->AddItem($rndthree,1,0);
$pltrnd  = int(rand 1500); 
$gldrnd  = int(rand 900);
$slvrnd  = int(rand 500);
$cprrnd  = int(rand 200);

$npc->AddCash($cprrnd,$silvrnd,$gldrnd,$pltrnd);

$whatshout = int(rand 100);
if ($whatshout <25){
quest::shout("The Orcs will soon plunder Kelethin. I shall enjoy that day like no other.");
}
elsif ($whatshout>25 && $whatshout <50){
quest::shout("I Thought there might be a champion here amongst these high elves. I was mistaken.");
}
elsif ($whatshout >50 && $whatshout <75) {
quest::shout("I'm bored of these high elves. Anyone want to die? I'll take you all on at once! Any Takers?");
}
else {
quest::shout("I pitty all of you. Wishing your loot was as good as mine.");
}

}

sub EVENT_DEATH {
   quest::faction(2010, -4000);
}


