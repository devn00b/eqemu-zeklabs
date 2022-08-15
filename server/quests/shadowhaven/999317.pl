# quest pvp merchant for warriors / monks
sub EVENT_SPAWN {
$npc->SetSlotTint(1, 128,0, 0); #1 for chest
$npc->SetSlotTint(2, 128,0, 0); #2 for arms
$npc->SetSlotTint(3, 128,0, 0); #3 for bracers
$npc->SetSlotTint(4, 128,0, 0); #4 for hands
$npc->SetSlotTint(5, 128,0, 0); #5 for legs
$npc->SetSlotTint(6, 128,0, 0); #6 for boots
$npc->SetSlotTint(0, 128,0, 0); #0 for helm
}
sub EVENT_SAY {
$pts = $client->GetPVPPoints();
  if($text=~/hail/i) {
	if ( $pts > 4 )
	{
		$link = quest::saylink("sacrifice");
   		quest::say("Hey there fella!  I see you have come to look for something new.  You currently have a total of $pts PVP available to spend.  You have a chance at my light weight armor!  You must $link 5 of your points, be warned you may only receive a token and will have to try again to get armor.");
	}
	else
	{
		quest::say("Hey there fella!  Come back when you have more PVP points.");
	}
  }
elsif($text=~/sacrifice/i) {
$link = quest::saylink("take the points");
quest::say("Are you positive you wish me to $link?");
}
elsif($text=~/take the points/i) {
if ( $pts < 5 )
{ quest::say("You cannot do this right now!"); }
else {
#subtract points
$newpoints = $pts - 5;
#provide them the item
$client->SummonLootGroup(37);
$client->SetPVPPoints($newpoints);
quest::say("Sure lets see what I got in my bag here!!  Ahh yes here you go!  You have $newpoints left."); }
}

}
