
sub EVENT_SAY {
$togive = 0;
if ($text=~/Hail/i){
$saylink = quest::saylink("Armor for Pets");
$saylink2 = quest::saylink("Weapons for Pets");

quest::say("Hello $name. I recently found some enchanted pet gear washed ashore while we explored kunark. Do you want $saylink or $saylink2, tell me so."); 
}

elsif ($text=~/Armor for Pets/i){
if($ulevel < 45) {
	quest::say("Come see me when you gain some more levels and I'll give you something better!!");
quest::summonitem(144135);
}else{
quest::say("Just a note from the gods, The belt is useless, but I gave you one anyway!");
quest::say("Good luck $name!");
quest::selfcast(10702);
quest::summonitem(46987);
}
}

elsif ($text=~/Weapons for Pets/i){
if($ulevel < 45) {
        quest::say("Come see me when you gain some more levels and I'll give you something better!");
quest::summonitem(144153);
quest::summonitem(144153);
}else{
#$togive = quest::ChooseRandom(77508,52882,52797,77509);
quest::summonitem(77508);
quest::summonitem(77508);
quest::say("Enjoy $name!");
}
}


else
{
}
}
