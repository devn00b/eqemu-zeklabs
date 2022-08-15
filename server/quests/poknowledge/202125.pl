sub EVENT_SAY {
$togive = 0;
if ($text=~/Hail/i){
$saylink = quest::saylink("Armor for Pets");
$saylink2 = quest::saylink("Weapons for Pets");

quest::say("Hello $name. I recently found some enchanted pet gear washed ashore while we explored kunark. Do you want $saylink or $saylink2, tell me so."); 
}

elsif ($text=~/Armor for Pets/i){
quest::say("Good luck $name!");
quest::selfcast(10702);
quest::summonitem(46987);

}

elsif ($text=~/Weapons for Pets/i){
$togive = quest::ChooseRandom(77508,52882,52797,77509);
quest::summonitem($togive);
quest::summonitem($togive);
quest::say("Enjoy $name!");

}


else
{
}
}
