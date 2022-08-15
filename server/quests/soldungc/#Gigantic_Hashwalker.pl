sub EVENT_SPAWN
{

$rnd = quest::ChooseRandom(1197,51600,10032,22503);
$npc->AddItem($rnd,1);

}