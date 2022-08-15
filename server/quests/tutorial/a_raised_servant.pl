sub EVENT_SPAWN
{
$var = quest::ChooseRandom(1,2);
if ( $var == 1 )
{
quest::npctexture(1);
}

$rnd = quest::ChooseRandom(1879,1879,12208,10012,10053,143195,14176);
if ( $rnd == 143195 || $rnd == 14176 )
{
$npc->AddItem($rnd,5);
}
else
{
$npc->AddItem($rnd,1);
}
}

sub EVENT_AGGRO
{
$spid = $npc->GetSpawnPointID();
$rnd = quest::ChooseRandom(0,1,2,3,4);
if ( $spid != 139767 && $rnd < 2 )
{
quest::ze(13,"[Warrens] The Collector has retreated deeper in the zone as to avoid intruders.");
}
}