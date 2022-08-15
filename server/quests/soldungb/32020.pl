sub EVENT_SPAWN
{
$chance = int(rand 100);
if($chance < '40') {
$npc->AddItem(12544,1,0);
}
}


