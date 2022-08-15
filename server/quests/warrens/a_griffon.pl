sub EVENT_SPAWN
{
$rnd = quest::ChooseRandom(2463,3557,8103);
$newRnd = quest::ChooseRandom(0,1,2,3,4);
if ( $newRnd == 1 )
{
$npc->AddItem($rnd,1);
}
}