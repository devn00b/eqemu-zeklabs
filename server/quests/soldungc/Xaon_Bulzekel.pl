sub EVENT_SPAWN
{

$rnd = quest::ChooseRandom(0,20861,10032,22503,14174,14176);
if ( $rnd == 14174 || $rnd == 14176 )
{
$npc->AddItem($rnd,5);
}
else
{
$npc->AddItem($rnd,1);
}

}