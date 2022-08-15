sub EVENT_SAY { 
if ($text=~/Hail/i){
$saylink = quest::saylink("journey to Nexus");
quest::say("Hello $name. I do not have much time to chat. I must concentrate on the portal that is due to open soon. If you wish to $saylink, tell me so."); 
}
elsif ($text=~/journey to Nexus/i){
quest::say("Good luck $name!");
quest::selfcast(2943);
}
 }
