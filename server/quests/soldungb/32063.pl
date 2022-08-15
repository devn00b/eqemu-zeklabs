sub EVENT_SPAWN
{
$chance = int(rand 100);
if($chance < '10') {
$npc->AddItem(5057,1,0);
}



}


