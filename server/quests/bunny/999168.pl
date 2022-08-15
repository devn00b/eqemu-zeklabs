sub EVENT_SAY
{
if ($text=~/hail/i) {
$stewLink = quest::saylink("stew");
quest::say("Hey there $name, wanna get down?!? How about some $stewLink?!?");
$npc->DoAnim(23,0);
quest::settimer(1,1);
}
if ($text=~/stew/i) {
quest::say("You know... Stew!! I just love stew! Get me some already you slow fool... The bigger rabbits make yummy stew around these parts..");
}
}

sub EVENT_TIMER
{
if ( $timer eq "1" )
{
quest::stoptimer(1);
$npc->DoAnim(23,0);
}
}
sub EVENT_ITEM
{
my $var = $itemcount{11350};
if ( $var > 0 )
{
quest::exp(150);
quest::givecash(0,5,0,0);
quest::say("Ooooo!! Thanks!! Here take this, I hate having it around.");
$client->SummonLootGroup(3);
}
}