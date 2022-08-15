sub EVENT_SAY { 
#if ($text=~/Hail/i){quest::say("Hello there. There seems to be some strange problems with the boats in this area. The Academy of Arcane Sciences has sent a small team of us to investigate them. If you need to [travel to Iceclad] in the meantime, I can transport you to my companion there."); }
if ($text=~/Hail/i){
my $link = quest::saylink("travel to western wastes");
quest::say("Hello there. There seems to be some strange problems with the boats in this area. The Academy of Arcane Sciences has sent a small team of us to investigate them. If you need to [$link] in the meantime, I can transport you to my companion there."); }

#if ($text=~/travel to iceclad/i){
#quest::selfcast("2284"); }
if ($text=~/travel to western wastes/i){
quest::movepc(120,3713,1429.1,-307.8,24.4); }
}