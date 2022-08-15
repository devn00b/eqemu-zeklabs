sub EVENT_SPAWN
{
#quest::shout2("I have returned mortals, and I've got some new toys to try on you. Think you can take them from me?");
#main items
#$rnd = quest::ChooseRandom(51271,44330,62108,25907,1558,69403,9878,44332,44082,82700,1608,32909,60536,60535,6406,29608);
#main items plus some cloth to lessen chance
#$rndtwo = quest::ChooseRandom(51271,44330,62108,25907,1558,69403,9878,44332,44082,82700,1608,32909,60536,60535,6406,29608,1012,1005,1004,1006,1003,1007,1010,1011,1012,1005,1004);
#money bags + junk + a few ph
#$rndthree = quest::ChooseRandom(21821,21822,21823,132487,11696,6639,64193,145166,1011,1012,1005,1004);
#same as above but less cloth so better chance
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

sub EVENT_DEATH {
#plugin::handlenpc_epicloot($client,$npc,2);
#plugin::handlenpc_epicloot($client,$npc,1);

}
