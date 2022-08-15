sub EVENT_SPAWN
{
#lets do 3 items drops, so 3 randoms so we dont drop same item 3 times

#engraved Royal velium
$rnd =  quest::ChooseRandom(7466,26526,2737,62230,62229,1625,14742,144117);
#rnd 2 is weapons.
$rndtwo =  quest::ChooseRandom(26001,6715,5646,7250,7328,7371,5163,145405,145378,145379,145380,145381,145382,145383,145385,145386,145387,145494,145495);
#rnd 3 is weaps jewlery and armor stuff
$rndthree =  quest::ChooseRandom(62227,14751,2738,1414,2739,62235,1607,5163,6603,6604,6608);
# Add 3 junk loots (cloth)
$rndfour = quest::ChooseRandom(1001,10152,1006,1003,1007,1010,1011,1012,1005,1004,1008,1002,1009,1025,1030,1027,1031,1034,1035,1036,1029,1028,1032,1026,1033);
$rndfive = quest::ChooseRandom(1001,10152,1006,1003,1007,1010,1011,1012,1005,1004,1008,1002,1009,1025,1030,1027,1031,1034,1035,1036,1029,1028,1032,1026,1033);
$rndsix = quest::ChooseRandom(1001,10152,1006,1003,1007,1010,1011,1012,1005,1004,1008,1002,1009,1025,1030,1027,1031,1034,1035,1036,1029,1028,1032,1026,1033);

#Add 3x loot some cash.
$npc->AddItem($rnd,1,0);
$npc->AddItem($rndtwo,1,0);
$npc->AddItem($rndthree,1,0);
#add the crap loot
$npc->AddItem($rndfour,1,0);
$npc->AddItem($rndfive,1,0);
$npc->AddItem($rndsix,1,0);
#add random cash
$pltrnd  = int(rand 1500); 
$gldrnd  = int(rand 900);
$slvrnd  = int(rand 500);
$cprrnd  = int(rand 200);
$npc->AddCash($cprrnd,$silvrnd,$gldrnd,$pltrnd);
#decide what shout to use
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


sub EVENT_AGGRO
{
$whatshout2 = int(rand 100);
if ($whatshout2 <25){
quest::shout2("Ugh $name not you again. Didn't I already kill you?!");
return;
}
elsif ($whatshout2 >25 && $whatshout2 <50){
quest::shout2("$name there is nothing you can do to stop me. Kill me now I'll come back more powerful than you can possibly imagine!");
return;
}
elsif ($whatshout2 >50 && $whatshout2 <75) {
quest::shout2("Wait, $name you actually have friends?! I thought you were some neckbeard basement dweller! No matter you'll die just the same!");
return;
}
else {
quest::shout2("Why do I even put up with plebs like you $name Anyone want pvp? $name is here in felwithe come get em!!!");
return;
}
}

