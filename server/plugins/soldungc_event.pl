sub handlesoldungc_loot {
  my $npc = shift;
$rnd = quest::ChooseRandom(0,0,7186,8131,3393,10032,10048,10037,10165,10007,22503,10012,10053,14174,14176);
$rnd2 = quest::ChooseRandom(0,132487);
if ( $rnd2 > 0 ) { $npc->AddItem($rnd2,1); }

if ( $rnd == 14174 || $rnd == 14176 )
{
$npc->AddItem($rnd,5);
}
else
{
$npc->AddItem($rnd,1);
}
}