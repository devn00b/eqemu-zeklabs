# quest pvp merchant for warriors / monks
sub EVENT_SPAWN {
$npc->SetSlotTint(1, 33,29, 31); #1 for chest
$npc->SetSlotTint(2, 33,29, 31); #2 for arms
$npc->SetSlotTint(3, 33,29, 31); #3 for bracers
$npc->SetSlotTint(4, 33,29, 31); #4 for hands
$npc->SetSlotTint(5, 33,29, 31); #5 for legs
$npc->SetSlotTint(6, 33,29, 31); #6 for boots
$npc->SetSlotTint(0, 33,29, 31); #0 for helm
}
sub EVENT_SAY {
$pts = $client->GetPVPPoints();
  if($text=~/hail/i) {
	if ( $pts > 4 )
	{
		$link = quest::saylink("sacrifice");
   		quest::say("Hello $name!  I see you have come to look for some new equipment to replace those rags.  You currently have a total of $pts PVP available to spend.  You must $link 5 of your points, be warned you may only receive a token and will have to try again to get armor.");
	}
	else
	{
		quest::say("Hello $name!  Come back when you have more PVP points.");
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
$client->SummonLootGroup(43);
$client->SetPVPPoints($newpoints);
quest::say("Let us see what the gods have for you today..  Here you go $name!  You have $newpoints left."); }
}

}
