sub EVENT_SPAWN
{
$rnd = quest::ChooseRandom(10020,10021,10022,10023,6813,6814,6815,6816,6817);
$newRnd = quest::ChooseRandom(0,1,2,3,4);
if ( $newRnd == 1 )
{
$npc->AddItem($rnd,1);
}
}