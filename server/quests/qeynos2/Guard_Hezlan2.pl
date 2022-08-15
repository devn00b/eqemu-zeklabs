sub EVENT_SAY { 
if($text=~/Hail/i){
quest::say("Hello. $name.  My name is Hezlan Nur and I am a member of the Qeynos Guard. serving the will of Antonius Bayle.  Anything or anyone trying to get into Qeynos has to go through me. first.  Heh!  I sure love it when those dirty Sabertooths try. though!  Nothing is as gratifying as the death wail of a gnoll."); }
}

sub EVENT_SPAWN
{
	$x = $npc->GetX();
	$y = $npc->GetY();
	$z = $npc->GetZ();
	quest::set_proximity($x - 45, $x + 45, $y - 45, $y + 135, $z - 5, $z + 5);
}


sub EVENT_ENTER {
if ( $npc->CheckLoS($client) == 1 )
{

if ($client->GetPVPFlag() > 0) #check if they are pvp enabled
{

if ( $client->GetPVPFlag() == 1 ) #check against pvp flag if its qeynos team
{
if ( $client->IsKing() > 0 )
{
quest::say("Your majesty!!!!");
$npc->FaceTarget($client);
$npc->DoAnim(62,0);
}
else
{
$npc->FaceTarget($client);
my $max = 3;
my $min = 1;
my $rnd = int(rand($max)) + $min;
if ( $rnd > 1 )
{ $npc->DoAnim(66,0); } else { $npc->DoAnim(29,0); }
}
}
else
{
$npc->FaceTarget($client);
my $max = 3;
my $min = 1;
my $rnd = int(rand($max)) + $min;
if ( $rnd > 1 )
{ $npc->DoAnim(59,0); } else { $npc->DoAnim(57,0); }

}

}#end of pvp flag check area

}#check los

} #end of sub event_enter