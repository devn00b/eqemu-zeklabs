sub EVENT_SAY {

if($text=~/hail/i){
$itm = quest::varlink(11571);
$upgrd = quest::varlink(146723);
$qst = quest::saylink("Down");
quest::say("Hello $name, I'm Bobnober Thingamajig I can take your $itm and I can give you $upgrd. You [$qst] to get started?");

 }
if($text=~/down/i){
$itm = quest::varlink(11571);
$smr = quest::varlink(1360);
quest::say("I knew it, I just knew you would be interested $name!");
quest::say("So here is what I need. I need you to goto Lower Guk, and bring me one $smr, your old $itm, *AND* 500 PvP Points.");
quest::say("Just bring me that stuff and have enough points and I can work some magick for you!!!");
}
}

sub EVENT_ITEM {
$checkHandIn = plugin::check_handin(\%itemcount,1360=> 1);
$hooz = plugin::check_handin(\%itemcount,11571=> 1);
if ( $checkHandIn and $hooz) {

my $pointsToTake = 500;
my $points = $client->GetPVPPoints();

if ( $points < $pointsToTake || $ulevel < 30 )
{
        quest::say("You do not have enough points available ($points) or you are under level 30");
	quest::summonitem(1360);
	quest::summonitem(11571);
return;
}
else
{
        my $newpoints = $client->GetPVPPoints - $pointsToTake;
        $client->SetPVPPoints($newpoints);
        quest::say("Here you go, one upgraded pet enhancing thingamabob. I also took $pointsToTake from your total, you now have $newpoints.");
	quest::summonitem(146723);
return;
}

}
}
quest::say("Whats this crap? I guess I can wipe my ass with it.");
}
