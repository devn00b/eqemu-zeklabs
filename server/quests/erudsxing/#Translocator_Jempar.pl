sub EVENT_SAY { 
if($text=~/Hail/i){
$link1 = quest::saylink("Travel to Erudin");
$link2 = quest::saylink("Travel to Qeynos");

quest::say("Hello there. There seem to be some strange problems with the boats in this area. The Academy of Arcane Sciences has sent a small team of us to investigate them. If you need to [$link1] or [$link2] in the meantime, I can transport you to my companion there."); 
}
if($text=~/erudin/i){
$client->MovePC(24,-341,95,23.8,45); }

if($text=~/qeynos/i){
$client->MovePC(1,176,15,3.8,90); }
}
  