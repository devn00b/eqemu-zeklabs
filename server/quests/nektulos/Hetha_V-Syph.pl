sub EVENT_SPAWN
{
	quest::settimer("try_sit", 5);

$npc->SetSlotTint(1, 170,43, 43); #1 for chest
$npc->SetSlotTint(2, 170,43, 43); #2 for arms
$npc->SetSlotTint(3, 170,43, 43); #3 for bracers
$npc->SetSlotTint(4, 170,43, 43); #4 for hands
$npc->SetSlotTint(5, 170,43, 43); #5 for legs
$npc->SetSlotTint(6, 170,43, 43); #6 for boots
$npc->SetSlotTint(0, 170,53, 67); #0 for helm
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
quest::say("I am a bit busy $name.  You are interrupting my $link.");
}
elsif($text=~/meditation/i){
quest::stoptimer("try_sit");
quest::settimer("wait_sit", 30);
$link = quest::saylink("exhausted");
quest::say("Yes I must recover my energy, I am $link.");
}
elsif($text=~/exhausted/i){
quest::stoptimer("try_sit");
quest::settimer("wait_sit", 30);
quest::say("I was in Inferno..!  Wretched place.. the large monumental markers show the way to the underworld.");
}
else
{
quest::emote("sighs.");
quest::say("What...?");
quest::stoptimer("try_sit");
quest::settimer("wait_sit", 30);
}
}
