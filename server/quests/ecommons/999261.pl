my $canTriggerEvent = "1";

sub EVENT_SAY
{
$link = quest::saylink("death");
if($text=~/Hail/i){
quest::say("Hello $name, what brings you to an area of $link?");
}
if($text=~/Death/i){
quest::say("This continent is full of misery.  Even more in the city of Freeport thanks to Sir Lucan.  We bestowed him something great for any one man to wield.  The consequences have hurt many and he flourishes.  If I were to have the head of Lord Grimrot I would have an army behind me.");
}
}

sub EVENT_SPAWN
{
$npc->SetSlotTint(1, 46,43, 43); #1 for chest
$npc->SetSlotTint(2, 46,43, 43); #2 for arms
$npc->SetSlotTint(3, 46,43, 43); #3 for bracers
$npc->SetSlotTint(4, 46,43, 43); #4 for hands
$npc->SetSlotTint(5, 46,43, 43); #5 for legs
$npc->SetSlotTint(6, 46,43, 43); #6 for boots
$npc->SetSlotTint(0, 116,53, 67); #0 for helm
}

sub EVENT_ITEM {
    if(plugin::check_handin(\%itemcount, 26537 => 1)) { #Test
	quest::stoptimer("resettimer");
	quest::settimer("resettimer",3600); # 6 hour wait
      quest::say("You may want to stand back -- it will be getting ugly for the Freeport Kingdom today!");
      quest::givecash(0,0,0,1);
     $canTriggerEvent = "0";
      quest::signalwith(999262,1,0); #trigger to start undead event with special npc
    }
    else {
      quest::say("I do not want this... Why do you waste my time?!?");
      plugin::return_items(\%itemcount);
    }
}

sub EVENT_TIMER {
if ( $timer eq "resettimer") {
$canTriggerEvent = "1";
quest::stoptimer("resettimer");
quest::settimer("resettimer",3600);
#quest::signalwith(999262,1,0); #trigger to start undead event with special npc
}
}