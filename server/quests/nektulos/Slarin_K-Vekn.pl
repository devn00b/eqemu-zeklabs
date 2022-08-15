sub EVENT_SPAWN
{
	quest::settimer("try_sit", 5);
$npc->SetSlotTint(1, 43,43, 43); #1 for chest
$npc->SetSlotTint(2, 43,43, 43); #2 for arms
$npc->SetSlotTint(3, 43,43, 43); #3 for bracers
$npc->SetSlotTint(4, 43,43, 43); #4 for hands
$npc->SetSlotTint(5, 43,43, 43); #5 for legs
$npc->SetSlotTint(6, 43,43, 43); #6 for boots
$npc->SetSlotTint(0, 43,53, 67); #0 for helm
}

sub EVENT_TIMER
{
	if($timer eq "try_sit")
	{
		$npc->SetAppearance(1);
		quest::facenpctype(999304);
	}
	elsif($timer eq "wait_sit")
	{
		quest::stoptimer("wait_sit");
		quest::settimer("try_sit", 5);
	}
}

sub EVENT_DEATH
{
	quest::stoptimer("try_sit");
}

sub EVENT_SAY { 
$npc->SetAppearance(0);
if($text=~/Hail/i){
quest::stoptimer("try_sit");
quest::settimer("wait_sit", 30);
$link = quest::saylink("meditation");
quest::say("What do you want $name?  Can't you see I am in $link?");
}
elsif($text=~/meditation/i){
quest::stoptimer("try_sit");
quest::settimer("wait_sit", 30);
$link = quest::saylink("return");
quest::say("Yes I must collect my energy together before I $link.");
}
elsif($text=~/return/i){
quest::stoptimer("try_sit");
quest::settimer("wait_sit", 30);
quest::say("Return to the other realm ofcourse!  I have lost ones which must be saved.  The passage is nearby, the markers lead the path.");
}
else
{
quest::emote("grunts with anger.");
quest::stoptimer("try_sit");
quest::settimer("wait_sit", 30);
}
}
