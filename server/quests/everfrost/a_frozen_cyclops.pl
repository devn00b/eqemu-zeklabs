sub EVENT_SPAWN
{
$rnd = quest::ChooseRandom(8117,8118,8118,54713,54713,54714,54714,54715,54715,54716,54716,10032,2300,2300,10032,132844,132845);
$rnd2 = quest::ChooseRandom(8118,8118,54713,54713,54714,54714,54715,54715,54716,54716);
$npc->AddItem($rnd,1);
$npc->AddItem($rnd2,1);
}