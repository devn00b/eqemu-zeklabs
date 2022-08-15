#Mysti Drache - bone event quest

my $canTriggerEvent = "1";

sub EVENT_SAY
{
#quest::say("If only I had the head of the famous Lord Grimrot.. The power he holds could put me in the books forever!!");
}

sub EVENT_SPAWN
{
$canTriggerEvent = "1";
quest::signalwith(999260,1,1);
quest::stoptimer("resettimer");
quest::settimer("resettimer",600);
$npc->SetSlotTint(1, 78,93, 124); #1 for chest
}

sub EVENT_TIMER
{
if ( $timer eq "resettimer") {
$canTriggerEvent = "0";
quest::signalwith(999260,1,0);
}
}

sub EVENT_ITEM {
if ( $canTriggerEvent eq "0" )
{
quest::say("Now is not a good time.");
}
    elsif(plugin::check_handin(\%itemcount, 26537 => 1)) { #Test
      quest::say("Ah. Thank you for bringing this to me! I will make very good use of it..  I suggest you move along before more trouble comes to you.");
      quest::givecash(0,0,0,1);
	$canTriggerEvent = "0";
      quest::signalwith(999260,1,0); #trigger to start undead event with special npc
	quest::stoptimer("resettimer");
	quest::settimer("resettimer",21720);

    }
    else {
      quest::say("I do not want this... Why do you waste my time?!?");
      plugin::return_items(\%itemcount);
    }
}