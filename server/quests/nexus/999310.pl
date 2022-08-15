sub EVENT_SPAWN
{
$npc->SetSlotTint(1, 1,43, 43); #1 for chest
}

sub EVENT_SAY
{
$gate = quest::saylink("gate");
if($text=~/Hail/i){
quest::say("Hello do you need a $gate?");
}
elsif($text=~/gate/i){
quest::selfcast(36);
quest::say("Safe travels!");
}
}