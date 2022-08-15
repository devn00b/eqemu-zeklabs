sub EVENT_SPAWN
{
	$x = $npc->GetX();
	$y = $npc->GetY();
	$z = $npc->GetZ();
	quest::set_proximity($x - 45, $x + 45, $y - 45, $y + 45, $z - 5, $z + 5);
}

sub EVENT_TIMER {
if ( $timer eq "doanimchuckle" )
{
quest::stoptimer("doanimchuckle");
$npc->DoAnim(54,0);
}
}

sub EVENT_ENTER {
if ( $npc->CheckLoS($client) == 1 )
{
$npc->DoAnim(63,0);
$npc->FaceTarget($client);
quest::say("Hello there! Hahaha!!! Don't mind me... I am just having too much fun... Like my tree? I made it myself!  It has a mystical power heheh...");
quest::settimer("doanimchuckle",1);

}#check los

} #end of sub event_enter

sub EVENT_WAYPOINT_ARRIVE {
quest::facenpctype(999175);

my $max = 9;
my $min = 1;
my $rnd = int(rand($max)) + $min;
if ( $rnd == 5 )
{ $npc->DoAnim(30,0); quest::say("Do not give me that attitude tree!  Grr... you always cause me trouble."); }
if ( $rnd == 4 )
{ $npc->DoAnim(55,0); quest::say("I knew I forgot to do something this morning!! Do not worry I will get some water for you tree!"); }
if ( $rnd == 3 )
{ $npc->DoAnim(58,0); quest::say("You make me so happy tree!! Stay alive and strong!"); }
if ( $rnd < 3 ) { $npc->DoAnim(27,0); }
}