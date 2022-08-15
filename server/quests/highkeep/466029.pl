 sub EVENT_SPAWN
{
#lets do 3 items drops, so 3 randoms so we dont drop same item 3 times

#engraved Royal velium
$rnd = quest::ChooseRandom(8957,8958,8959,8971,8972,8973,8974,8407);
#Misc Items CC shield, coldain skin boots/goves, OOIV, book of obo,tonpo shoulders, robe of azure, Ice Forged Shackles.
$rndtwo = quest::ChooseRandom(25287,25026,25027,25098,14745,1283,145366,30041,145378,145379,145380,145381,145382,145383,145385,145386,145387,145494,145495);
#crystal chitin armor, lodi armor, regal band, shaded torch,infestation
$rndthree = quest::ChooseRandom(25283,25284,25285,25286,25287,25288,25289,25290,25291,5727,29206,22816,30069,2061,143063,4314,1619,6648,11058,4579,6646);
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

$pltrnd  = int(rand 1000); 
$gldrnd  = int(rand 900);
$slvrnd  = int(rand 500);
$cprrnd  = int(rand 200);

$npc->AddCash($cprrnd,$silvrnd,$gldrnd,$pltrnd);

$whatshout = int(rand 100);
if ($whatshout <25){
quest::shout2("Freport WILL bow to me some day!");
}
elsif ($whatshout>25 && $whatshout <50){
quest::shout2("No one is worthy of my attention. Not a single one of you!");
}
elsif ($whatshout >50 && $whatshout <75) {
quest::shout2("Lord Lucan D`Lere was harder to defeat than you pathetic mortals.");
}
else {
quest::shout2("Looking for 1v1. Someone 1v1 me bro! No? Cowards, Pussies, Loosers All of you.");
}

}

sub EVENT_DEATH {
   quest::faction(1073, -4000);
}

sub EVENT_AGGRO 
{
$whatshout2 = int(rand 100);
if ($whatshout2 <25){
quest::shout2("Screw you $name . I'm going to kill you then eat your children.");
return;
}
elsif ($whatshout2 >25 && $whatshout2 <50){
quest::shout2("Someone, get the batphone and let grokmol know $name is here and trying to kill me!");
return;
}
elsif ($whatshout2 >50 && $whatshout2 <75) {
quest::shout2("So this is all you bring $name ? And you think this is enough to kill me?? Pfft.");
return;
}
else {
quest::shout2("So you think you got what it takes $name ?? HAHAHAHA I'll make short work of you, and your entire guild. That is if grokmol doesn't train you first");
return;
}
}
