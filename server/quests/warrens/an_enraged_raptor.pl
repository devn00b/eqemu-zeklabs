
sub EVENT_SPAWN
{
# 2018-2019 KAB, changed jboots to travelers (2300 to 1740)
$rnd = quest::ChooseRandom(27843,1740,0,0,0,0,0,0,0,0);
if ( $rnd > 0 )
{
$npc->AddItem($rnd,1);
}
}
