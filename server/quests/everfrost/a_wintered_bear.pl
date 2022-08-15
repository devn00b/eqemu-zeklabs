sub EVENT_SPAWN
{
$rnd = quest::ChooseRandom(8118,54713,54713,54714,54714,54715,54715,54716,54716,2912);
$npc->AddItem($rnd,1);
}