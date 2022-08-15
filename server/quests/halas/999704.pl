sub EVENT_SPAWN
{
#lets do 3 items drops, so 3 randoms so we dont drop same item 3 times

#rnd 1 is generic or junkish stuff. 
$rnd = quest::ChooseRandom(7466,26526,2737,62230,62229,1625,14742,8957,8958,8959,8971,8972,8973,8974,145407,145378,145379,145380,145381,145382,145383,145385,145386,145387,145494,145495);

#rnd 2 is weapons.
$rndtwo = quest::ChooseRandom(26001,6715,5646,7250,7328,7371,5163,145405,13280,7943,7942,7941,7940,44222,40312);

#rnd 3 is weaps jewlery and armor stuff
$rndthree = quest::ChooseRandom(62227,14751,2738,1414,2739,62235,1607,5163,6603,6604,6608,7939,7938,25572,25095,62056);

# Add 3 junk loots (cloth)
$rndfour = quest::ChooseRandom(1001,10152,1006,1003,1007,1010,1011,1012,1005,1004,1008,1002,1009,1025,1030,1027,1031,1034,1035,1036,1029,1028,1032,1026,1033);
$rndfive = quest::ChooseRandom(1001,10152,1006,1003,1007,1010,1011,1012,1005,1004,1008,1002,1009,1025,1030,1027,1031,1034,1035,1036,1029,1028,1032,1026,1033);
$rndsix = quest::ChooseRandom(1001,10152,1006,1003,1007,1010,1011,1012,1005,1004,1008,1002,1009,1025,1030,1027,1031,1034,1035,1036,1029,1028,1032,1026,1033);


#Add 3x loot
$npc->AddItem($rnd,1,0);
$npc->AddItem($rndtwo,1,0);
$npc->AddItem($rndthree,1,0);
#add the crap loot
$npc->AddItem($rndfour,1,0);
$npc->AddItem($rndfive,1,0);
$npc->AddItem($rndsix,1,0);


#add all randomized cash
$pltrnd  = int(rand 1500); 
$gldrnd  = int(rand 900);
$slvrnd  = int(rand 500);
$cprrnd  = int(rand 200);
#add the cash 
$npc->AddCash($cprrnd,$silvrnd,$gldrnd,$pltrnd);

#decide what shout to use
$whatshout = int(rand 100);
#$whatshout = 0;
if ($whatshout <25){
#quest::shout2("The world will burn, and you all with it!");
}
elsif ($whatshout>25 && $whatshout <50){
#quest::shout2("These Barbarians are such crude people. I Think I shall kill them first! Think you can stop me?");
}
elsif ($whatshout >50 && $whatshout <75) {
#quest::shout("Lord Bayle, you cannot stop my world domination. You are nothing.");
}
else {
#quest::shout2("Ha! Norrath is filled with pussies. All of you are worthless.");
}

}

sub EVENT_DEATH {
   quest::faction(2010, -4000);
}

sub EVENT_AGGRO
{
$whatshout2 = int(rand 100);
if ($whatshout2 <25){
quest::shout2("So $name you think your hot shit do you? Lets see how you like this.");
return;
}
elsif ($whatshout2 >25 && $whatshout2 <50){
quest::shout2("-WARNING- -WARNING- -WARNING- $name is attacking me! Come kill them!");
return;
}
elsif ($whatshout2 >50 && $whatshout2 <75) {
quest::shout2("So this is all you bring $name ? And you think this is enough to kill me?? Pfft.");
return;
}
else {
quest::shout2("$name you really do have a death wish dont you?");
return;
}
}

