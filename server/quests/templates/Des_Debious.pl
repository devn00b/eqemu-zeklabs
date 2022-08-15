sub EVENT_SPAWN
{
$npc->SetSlotTint(1, 31,0, 200); #1 for chest
$npc->SetSlotTint(6, 31,0, 200); #6 for boots
}

sub EVENT_SAY
{
if($text=~/Hail/i){
quest::say("Hello $name! If you are looking to get some cheaper discounts on guard upgrades you have come to the right place! Consultants to work with you in your kingdom, purchase upgrades at a cheaper cost!");
}
}