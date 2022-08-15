sub EVENT_SPAWN
{
$chance = int(rand 4);

if($chance eq '3') {
$npc->AddItem(146322,1,0);
}

#Add 3x loot some cash.
$npc->AddItem(147801,1,0);
$npc->AddItem(147801,1,0);

$pltrnd  = int(rand 500);
$gldrnd  = int(rand 900);
$slvrnd  = int(rand 500);
$cprrnd  = int(rand 200);

$npc->AddCash($cprrnd,$silvrnd,$gldrnd,$pltrnd);


}

