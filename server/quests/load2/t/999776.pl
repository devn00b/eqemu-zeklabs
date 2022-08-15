sub EVENT_SAY {

if($text=~/hail/i){
quest::say("111Since you all are giant pussies, the gods have sent me to [toughen] you up. I can set your DEFENSIVE skills to be somewhat acceptable.");
return;
 }

if(!defined $qglobals{"manastone1"} || !defined $qglobals{"manastone2"} || !defined $qglobals{"manastone3"} || !defined $qglobals{"manastone4"} ){
quest::say("You Failed");
return;
}


if($text=~/completed/i && defined $qglobals{"manastone1"} && defined $qglobals{"manastone2"} && defined $qglobals{"manastone3"} && defined $qglobals{"manastone4"} ){
quest::summonitem(147804);
quest::say("You Completed them all! So take this! Take this to A Shady Genkai He will help you finish this!");
return;
}

}

sub EVENT_ITEM {
$checkHandIn  = plugin::check_handin(\%itemcount,147786=> 1 ,147787=> 1 ,147788=> 1 ,147789=> 1);
$checkHandIn2 = plugin::check_handin(\%itemcount,147790=> 1 ,147791=> 1 ,147792=> 1 ,147793=> 1);
$checkHandIn3 = plugin::check_handin(\%itemcount,147794=> 1 ,147795=> 1 ,147796=> 1 ,147797=> 1);
$checkHandIn4 = plugin::check_handin(\%itemcount,147798=> 1 ,147799=> 1 ,147800=> 1 ,147801=> 1);


if ( $checkHandIn ) {
quest::say("You should now have an extra spell slot!");
quest::setglobal("manastone1", 1, 5, "F");

return;
}

if ( $checkHandIn2 ) {
quest::say("You should now have an extra spell slot! 22222222");
quest::setglobal("manastone2", 1, 5, "F");

return;
}

if ( $checkHandIn3 ) {
quest::say("You should now have an extra spell slot! 33333333333333");
quest::setglobal("manastone3", 1, 5, "F");

return;
}

if ( $checkHandIn4 ) {
quest::say("You should now have an extra spell slot! 444444444444444");
quest::setglobal("manastone4", 1, 5, "F");
return;
}


quest::say("Whats this crap? I guess I can wipe my ass with it.");
}
