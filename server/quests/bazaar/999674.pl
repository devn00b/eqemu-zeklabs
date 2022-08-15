sub EVENT_SAY {
$togive = 0;
if ($text=~/Hail/i){
$saylink = quest::saylink("Armor for Pets");
$saylink2 = quest::saylink("Weapons for Pets");
$saylink3 = quest::saylink("perhaps BOTH");

if($class eq 'Beastlord'){
quest::say("Hello $name, I see you are a $class. Since we are of the same Blood, I can offer you something the others only wish they had");
quest::say("Would you like $saylink or $saylink2 or $saylink3 I'm sure you'll be the envy of all the mages.");
}else{
quest::say("Why would I want to deal with the likes of you $name. You aren't even worth the shit stuck to my fur.");
}


}

elsif ($text=~/Armor for Pets/i){
if($ulevel < 15 & $class eq 'Beastlord' ) {
        quest::say("Come see me when you gain some more levels!");
}else{
if($class eq 'Beastlord'){
quest::say("Good luck $name! Kill with pride!");
quest::summonitem(144135);
}else{
quest::say("Why would I want to deal with the likes of you $name. You aren't even worth the shit stuck to my fur.");
}


}
}

elsif ($text=~/Weapons for Pets/i){
if($ulevel < 15 & $class eq 'Beastlord') {
        quest::say("Come see me when you gain some more levels!");
}else{
if($class eq 'Beastlord'){
quest::summonitem(144134);
quest::summonitem(144134);
quest::say("Well, $name!, That should help you get more kills!");
}else{
quest::say("Why would I want to deal with the likes of you $name. You aren't even worth the shit stuck to my fur.");
}


}
}


elsif ($text=~/perhaps BOTH/i){
if($ulevel < 15 & $class eq 'Beastlord') {
        quest::say("Come see me when you gain some more levels!");
}else{
if($class eq 'Beastlord'){
quest::summonitem(144134);
quest::summonitem(144134);
quest::summonitem(144135);
quest::say("Enjoy $name! I hope these help.");
}else{
quest::say("Why would I want to deal with the likes of you $name. You aren't even worth the shit stuck to my fur.");
}


}
}


else
{
}
}

