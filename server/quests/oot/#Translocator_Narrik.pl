sub EVENT_SAY { 
if ($text=~/Hail/i){
$link1 = quest::saylink("travel to Freeport");
$link2 = quest::saylink("travel to Butcherblock");

quest::say("Hello there. There seems to be some strange problems with the boats in this area. The Academy of Arcane Sciences has sent a small team of us to investigate them. If you need to [$link1] or [$link2] in the meantime, I can transport you to my companion there."); 

}
elsif ($text=~/travel to freeport/i){quest::selfcast("2283"); }
elsif ($text=~/travel to butcherblock/i){quest::selfcast("2281"); } 
else
{
quest::selfcast("2283");
}
} 