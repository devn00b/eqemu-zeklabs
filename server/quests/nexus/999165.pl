sub EVENT_SAY {

if ($text=~/hail/i) {
$link = quest::saylink("portal");
quest::say("Good to see you again $name!  Nice to see you made your trip alright.  Would you like a $link item for your travels?");
}
if ($text=~/portal/i) {
$client->SummonLootGroup(24); #nexus portal item, disabled for now
quest::say("Good luck young adventurer!");
}

}