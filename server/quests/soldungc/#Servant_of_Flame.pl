sub EVENT_SPAWN
{

$rnd = quest::ChooseRandom(0,10401,2883,10032,22503);
$npc->AddItem($rnd,1);

}