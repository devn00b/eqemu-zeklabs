sub EVENT_SAY {
$togive = 0;
if ($text=~/Hail/i){
#$saylink = quest::saylink("Armor for Pets");
$saylink2 = quest::saylink("Weapons for Pets");

quest::say("Hello $name. I recently found some enchanted pet gear washed ashore while we explored kunark. Would you like some $saylink2 .");
return;
}

#if ($text=~/Armor for Pets/i){
#if($ulevel < 45) {
#quest::say("Come see me when you gain some more levels and I'll give you something better!!");
#quest::summonitem(145324);
#quest::say("You arent worth my time");
#return;
#}else{
#quest::say("Just a note from the gods, The belt is useless, but I gave you one anyway!");
#quest::say("Good luck $name!");
#quest::selfcast(10702);
#quest::summonitem(46987);
#return;
#}
#}

if ($text=~/Weapons for Pets/i){
if($ulevel < 40) {
quest::say("Come see me when you gain some more levels and I'll give you something better!");
#quest::summonitem(144153);
#quest::summonitem(144153);
quest::summonitem(145324);
quest::summonitem(145324);
return;
}

if($ulevel > 39) {
quest::summonitem(145339);
quest::summonitem(145340);
quest::say("Please $name, Take this weapon and enjoy! Its one of the best I've found yet! I think Devnoob himself used these once, durring the red69 wars.");
return;
}
}



}


