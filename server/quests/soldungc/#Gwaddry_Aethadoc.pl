sub EVENT_SPAWN
{

$rnd = quest::ChooseRandom(0,31862,51596,10032,22503);
$npc->AddItem($rnd,1);

}