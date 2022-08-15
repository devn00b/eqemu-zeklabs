sub EVENT_DEATH {
$rnd = quest::ChooseRandom(1,1,2,3,4);
if ( $rnd > 2 ) {
$npc->AddItem(9433,1);
}
}