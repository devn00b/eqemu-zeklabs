sub EVENT_SPAWN
{
$rnd = quest::ChooseRandom(143195,14176);
$newRnd = quest::ChooseRandom(0,1,2,3,4);
if ( $newRnd == 1 )
{
$npc->AddItem($rnd,5);
}

$itemid = $npc->GetEquipment(7);
$itemid2 = $npc->GetEquipment(8);

if ( $itemid == 6913 || $itemid2 == 6913  )
{
$npc->RemoveItem(6913);
$npc->AddItem(6913,2);
}

}