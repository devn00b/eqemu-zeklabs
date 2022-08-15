sub EVENT_SPAWN {
quest::say("All to me!");
quest::settimer(1,3600);
}

sub EVENT_TIMER  {
if($timer == 1) {
quest::stoptimer(1);
quest::depop();
}
}

sub EVENT_SAY {
if($text=~/hail/i) {
$client->Message(9,"One more foe remains!  Prepare yourself for.. COCYTUS!");
quest::movepc(209,-371,-1680,2356);
}
}