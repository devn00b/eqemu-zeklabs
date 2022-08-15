sub EVENT_SPAWN
{

$rnd = quest::ChooseRandom(0,26513,5713,10032,22503);
$npc->AddItem($rnd,1);

}