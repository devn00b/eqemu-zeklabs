sub EVENT_SPAWN
{
$rnd = quest::ChooseRandom(54017,54017,62163,10023,10024,10025,143195,14176);

if ( $rnd == 143195 || $rnd == 14176 )
{
$npc->AddItem($rnd,5);
}
else
{
$npc->AddItem($rnd,1);
}
}