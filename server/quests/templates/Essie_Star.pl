sub EVENT_SAY
{
if ($ulevel > 45 )
{
if ($text=~/Hail/i)
{
my $link = quest::saylink("Plane of Hate");
my $otherlink = quest::saylink("Plane of Sky");
quest::say("Hello $name.  Are you looking for a port to $link or perhaps $otherlink?  At a modest PVP point cost of 5 points I can send you there!");
}
elsif($text=~/Hate/i){
quest::popup("Portal to Hate","You have requested to be ported to Plane of Hate.  This will require 5 PVP points, accept if you wish to be ported.",1,60);
}
elsif($text=~/Sky/i){
quest::popup("Portal to Sky","You have requested to be ported to Plane of Sky.  This will require 5 PVP points, accept if you wish to be ported.",2,60);
}
}
else
{
quest::say("Hello $name, I am afraid you must be level 46 or higher to visit the Plane of Hate");
}
}

sub EVENT_POPUPRESPONSE
{
my $pointsToTake = 5;

if ( $popupid eq "1" ) #port to hate
{
my $points = $client->GetPVPPoints();

if ( $points < $pointsToTake || $ulevel < 46 )
{
	quest::say("You do not have enough points available ($points).");
}
else
{
	my $newpoints = $client->GetPVPPoints - $pointsToTake;
	$client->SetPVPPoints($newpoints);
	quest::say("Up you go!");
	quest::movepc(76,-353,-374,3.8,123.1);
}
}

if ( $popupid eq "2" ) #port to air
{
my $points = $client->GetPVPPoints();

if ( $points < $pointsToTake || $ulevel < 46 )
{
        quest::say("You do not have enough points available ($points).");
}
else
{
        my $newpoints = $client->GetPVPPoints - $pointsToTake;
        $client->SetPVPPoints($newpoints);
        quest::say("Up you go!");
        quest::movepc(71,614,1415,-650,159.2);
}
}



}
