sub EVENT_SPAWN
{
$npc->SetSlotTint(1, 1,43, 43); #1 for chest
}

sub EVENT_SAY
{
$link = quest::saylink("bind");
#$gate = quest::saylink("gate");
if($text=~/Hail/i){
quest::say("Hello do you need a $link?");
}
elsif($text=~/bind/i){
quest::selfcast(35);
quest::say("Here you go!");
}
#elsif($text=~/gate/i){
#quest::selfcast(36);
#quest::say("Safe travels!");
#}
}
