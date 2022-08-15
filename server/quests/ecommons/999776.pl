sub EVENT_SAY {

if(defined $qglobals{"manastonecompleted"}) {
quest::say("You have completed this quest, I no longer want anything to do with you. Get Lost.");
return;
}

if($text=~/hail/i){
$fuck = quest::saylink("Completed");
$fuck2 = quest::saylink("Upgrading");
$fuck3 = quest::varlink(143069);

quest::say("Hello $name, are you interested in [$fuck2] your manastone to $fuck3. Or have you [$fuck] the quest?");
return;
 }

if($text=~/upgrading/i){
quest::say("There are four steps to this quest. All must be turned in correctly and in the right order");
quest::say("");
quest::say("Step 1: Bring me tokens from the following NPCs: Emperor Crush, D'vinn, Master Brewer, Lord Elgnub. You must bring me the token of truth #1-4");
quest::say("");
quest::say("Step 2: Bring me tokens from the following NPCs: Najena, Drelzna, Garanel Rucksif, Torklar Battlemaster. You must bring me the token of truth #5-8");
quest::say("");
quest::say("Step 3: Bring me tokens from the following NPCs: Ancient Croc, Tempus, Princess Lenia, Mistress Anna. You must bring me the token of truth #9-12");
quest::say("");
quest::say("Step 4: Bring me tokens from the following NPCs: Raster of Guk, Froglok King, Guano Harvester, Efreeti Lord Djarn. You must bring me the token of truth #13-16");
quest::say("");
quest::say("Once all of this is completed I'll reward you with an item you can then talk to A Shady Genkai for an upgrade to your manastone.");

return;
}

if(!defined $qglobals{"manastone1"} || !defined $qglobals{"manastone2"} || !defined $qglobals{"manastone3"} || !defined $qglobals{"manastone4"} ){
quest::say("You have not completed the required kills.");
return;
}


if($text=~/completed/i && defined $qglobals{"manastone1"} && defined $qglobals{"manastone2"} && defined $qglobals{"manastone3"} && defined $qglobals{"manastone4"} ){
quest::summonitem(147804);
quest::delglobal(manastone1);
quest::delglobal(manastone2);
quest::delglobal(manastone3);
quest::delglobal(manastone4);

quest::say("You Completed them all! So take this! Take this and your normal manastone to A Shady Genkai He will help you finish this!");
return;
}

}

sub EVENT_ITEM {
$checkHandIn  = plugin::check_handin(\%itemcount,147786=> 1 ,147787=> 1 ,147788=> 1 ,147789=> 1);
$checkHandIn2 = plugin::check_handin(\%itemcount,147790=> 1 ,147791=> 1 ,147792=> 1 ,147793=> 1);
$checkHandIn3 = plugin::check_handin(\%itemcount,147794=> 1 ,147795=> 1 ,147796=> 1 ,147797=> 1);
$checkHandIn4 = plugin::check_handin(\%itemcount,147798=> 1 ,147799=> 1 ,147800=> 1 ,147801=> 1);


if ( $checkHandIn ) {
quest::say("Congrats $name, You have completed Step 1.");
quest::say("");
quest::say("Step 2: Bring me tokens from the following NPCs: Najena, Drelzna, Garanel Rucksif, Torklar Battlemaster. You must bring me the token of truth #5-8");
quest::setglobal("manastone1", 1, 5, "F");

return;
}

if ( $checkHandIn2 ) {
quest::setglobal("manastone2", 1, 5, "F");
quest::say("Step 3: Bring me tokens from the following NPCs: Ancient Croc, Tempus, Princess Lenia, Mistress Anna. You must bring me the token of truth #9-12");

return;
}

if ( $checkHandIn3 ) {
quest::setglobal("manastone3", 1, 5, "F");
quest::say("Step 4: Bring me tokens from the following NPCs: Raster of Guk, Froglok King, Guano Harvester, Efreeti Lord Djarn. You must bring me the token of truth #13-16");
return;
}

if ( $checkHandIn4 ) {
$fuck = quest::saylink("Completed");
quest::say("Step 4 is completed, If you have completed the other 3 steps you should have [$fuck] the whole thing and are ready to claim your prize!");
quest::setglobal("manastone4", 1, 5, "F");
return;
}


quest::say("Whats this crap? I guess I can wipe my ass with it.");
}
