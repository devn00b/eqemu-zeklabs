sub EVENT_SAY { 
if($text=~/Hail/i){
$link1 = quest::saylink("Travel to Eruds Crossing");

quest::say("Hello there. There seem to be some strange problems with the boats in this area. The Academy of Arcane Sciences has sent a small team of us to investigate them. If you need to $link1 in the meantime, I can transport you to my companion there."); 
}
if($text=~/crossing/i){
$client->MovePC(98,690,-1773,3.8,63.9); }
if($text=~/erud/i){
$client->MovePC(98,690,-1773,3.8,63.9); }
}
 

