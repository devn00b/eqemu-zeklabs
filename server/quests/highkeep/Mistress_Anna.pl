sub EVENT_SPAWN
{
#Add 3x loot some cash.
$npc->AddItem(147797,1,0);
$npc->AddItem(147797,1,0);

$pltrnd  = int(rand 500);
$gldrnd  = int(rand 900);
$slvrnd  = int(rand 500);
$cprrnd  = int(rand 200);

$npc->AddCash($cprrnd,$silvrnd,$gldrnd,$pltrnd);


}


sub EVENT_SAY { 
if($text=~/Hail/i){
quest::say("Yes? I don't often converse with the servants of Highkeep. but what do you want? I am a very good friend of Carson McCabe's. If I wished it so. you could be sent to the dungeons to converse with the artisans of pain."); }
}
#END of FILE Zone:highkeep  ID:6068 -- Mistress_Anna 

