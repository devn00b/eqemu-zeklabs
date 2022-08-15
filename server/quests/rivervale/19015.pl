sub EVENT_SPAWN
{
#$rnd = quest::ChooseRandom(44403,70746,31405,15770,23826,15770,65440,26782,25543);
#$rndtwo = quest::ChooseRandom(44403,70746,31405,15770,23826,15770,65440,1001,10152,1006,1003,1007,1010,1011,1012,1005,1004);
#$rndthree = quest::ChooseRandom(21821,21822,21823,132487,11696,6639,64193,145166,1011,1012,1005,1004);
#$rndfour = quest::ChooseRandom(21821,21822,21823,132487,11696,6639,64193,145166,1011,1012);

#Add 3x loot some cash.
#$npc->AddItem($rnd,1,0);
#$npc->AddItem($rndtwo,1,0);
#$npc->AddItem($rndthree,1,0);
#$npc->AddItem($rndfour,1,0);
#add cash
$pltrnd  = int(rand 1000);
$gldrnd  = int(rand 900);
$slvrnd  = int(rand 500);
$cprrnd  = int(rand 200);

$npc->AddCash($cprrnd,$silvrnd,$gldrnd,$pltrnd);
}

