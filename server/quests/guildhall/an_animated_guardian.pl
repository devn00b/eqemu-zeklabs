sub EVENT_SPAWN
{
$rnd = quest::ChooseRandom(10032,10048,10037,10165,10007,16766,16764,22503);
$npc->AddItem($rnd,1);
$itemid = $npc->GetEquipment(7);
$itemid2 = $npc->GetEquipment(8);

if ( $itemid == 6913 )
{
$npc->SetEquipment(7,0);
}
if ($itemid2 == 6913 )
{
$npc->SetEquipment(8,0);
}

if ( $itemid == 6913 || $itemid2 == 6913  )
{
$npc->RemoveItem(6913);
$npc->AddItem(6913,2);
}

}