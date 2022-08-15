sub EVENT_SPAWN
{

$rnd = quest::ChooseRandom(1138,1139,1140,1152,10032,22503);
$npc->AddItem($rnd,1);

}