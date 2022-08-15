sub EVENT_SPAWN
{
#Add 3x loot some cash.
$npc->AddItem(147798,1,0);
$npc->AddItem(147798,1,0);

$pltrnd  = int(rand 500);
$gldrnd  = int(rand 900);
$slvrnd  = int(rand 500);
$cprrnd  = int(rand 200);

$npc->AddCash($cprrnd,$silvrnd,$gldrnd,$pltrnd);


}

