sub EVENT_SPAWN
{

$rnd = quest::ChooseRandom(0,24037,3554,10032,22503);
$npc->AddItem($rnd,1);
$npc->AddItem(55538,1);
}