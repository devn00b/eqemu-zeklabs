sub EVENT_SPAWN
{

$rnd = quest::ChooseRandom(0,4831,4832,4833,4834,4835,4836,4837,10032,22503);
$npc->AddItem($rnd,1);
}