sub EVENT_SPAWN
{
$chance = int(rand 100);
$chance2 = int(rand 100);
quest::say("Grrr..What is that smell?");

if($chance <= 50) {
$npc->AddItem(7573,1,0);
}

if($chance2 <=50){
$npc->AddItem(149441,1,0);
}

}

