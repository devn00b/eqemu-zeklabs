sub EVENT_SPAWN
{

$rnd = quest::ChooseRandom(0,6639,11696,52356,51598,51601);
$npc->AddItem($rnd,1);

$rnd2 = quest::ChooseRandom(10032,10048,10037,10165,10007,16766,16764,22503,10012,10053,14174,14176);
if ( $rnd2 == 14174 || $rnd2 == 14176 )
{
$npc->AddItem($rnd2,5);
}
else
{
$npc->AddItem($rnd2,1);
}

}