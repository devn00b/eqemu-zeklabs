sub EVENT_SPAWN
{
$rnd = quest::ChooseRandom(10020,10021,10022,10023,10024,10025,143195,14176,14206);
$newRnd = quest::ChooseRandom(0,1,2,3,4);

if ( $newRnd == 1 )
{
if ( $rnd == 143195 || $rnd == 14176 )
{
$npc->AddItem($rnd,5);
}
else
{
$npc->AddItem($rnd,1);
}
}

$entid = $npc->GetID();
$npc->CastSpell(278, $entid,10,0);
quest::settimer("castinvis",2);
}

sub EVENT_TIMER
{
quest::stoptimer("castinvis");
$entid = $npc->GetID();
$npc->CastSpell(928, $entid,10,0);
}

sub EVENT_DEATH
{
quest::stoptimer("castinvis");
}