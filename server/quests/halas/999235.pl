sub EVENT_SAY {
 if($text=~/hail/i && $ulevel < 11){
quest::say("Greetings! I can send you to join the rest of your team if you wish. I can only do this for players level 10 and under.");
  quest::say("Do you wish to [JOIN] them?");

 }
 elsif($text=~/join/i && $ulevel < 11){
 quest::say("Fantastic! Off you go!");
 quest::movepc(68,-2452.4,-1100,-0.3,132.0); 
 }
else {
quest::say("Sorry only available for players under level 11.");
}
} 
