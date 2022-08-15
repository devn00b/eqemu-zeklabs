sub EVENT_SAY {


if(defined $qglobals{"manastonecompleted"}) {
quest::say("You have completed this quest, I no longer want anything to do with you. Get Lost.");
return;
}


if($text=~/hail/i){
$fuck = quest::varlink(147804);
$fuck3 = quest::varlink(13401);

quest::say("Hello $name if you were sent to me by the Manastone Exchange give me your $fuck3 and $fuck.");
return;
 }


}

sub EVENT_ITEM {
$checkHandIn  = plugin::check_handin(\%itemcount,13401=> 1 ,147804=> 1);


if ( $checkHandIn ) {
quest::say("Congrats $name");
quest::setglobal("manastonecompleted", 1, 5, "F");
quest::summonitem(143069);
return;
}


quest::say("Whats this crap? I guess I can wipe my ass with it.");
}
