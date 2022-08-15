sub EVENT_SPAWN
{
quest::npctexture(quest::ChooseRandom(1,2,3));
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

sub EVENT_AGGRO
{
$castspellid = quest::ChooseRandom(0,0,0,216,216,216,216,216,216,216,216,967,967,967,1308);
if ( $castspellid > 0 )
{
$npc->CastSpell($castspellid,$userid,10,0);
}
}