sub EVENT_SPAWN
{

$rnd = quest::ChooseRandom(0,6692,2924,10032,22503);
$npc->AddItem($rnd,1);

}