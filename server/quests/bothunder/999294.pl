sub EVENT_DEATH
{
plugin::handlenpc_epicloot($client,$npc,2);
$rnd = quest::ChooseRandom(25850,25851,25852,25853,25854,25855,25856);
$rnd2 = quest::ChooseRandom(25850,25851,25852,25853,25854,25855,25856);
$npc->AddItem($rnd,1);
$npc->AddItem($rnd2,1);

$npc->AddItem(132487,1);
$npc->AddItem(132487,1);
$npc->AddItem(132487,1);
$npc->AddItem(132487,1);
$npc->AddItem(132487,1);
$npc->AddItem(132487,1);
$npc->AddItem(132487,1);
$npc->AddItem(132487,1);
$npc->AddItem(132487,1);
$npc->AddItem(132487,1);

$npc->AddItem(45920,1);
#item template 73
$npc->AddLootGroup(40);
$npc->AddLootGroupEx(40,true);
$npc->AddLootGroupEx(40,true);
}