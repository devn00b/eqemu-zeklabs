$position = 0;
$reading = 0;

sub EVENT_SAY
{
if($text=~/Hail/i){
quest::doanim(29);
}
}
sub EVENT_SPAWN
{
	$x = $npc->GetX();
	$y = $npc->GetY();
	$z = $npc->GetZ();
	quest::set_proximity($x - 25, $x + 25, $y - 25, $y + 25, $z - 5, $z + 5);
	$position = 0;
	$reading = 0;
}

sub EVENT_ENTER {
	if ( $qglobals{"hudsonmemorial"} )
	{
	quest::doanim(66);
	return;
	}
	if ( $reading == 0 || undef $reading)
	{
		quest::setglobal("hudsonmemorial","1",7,"M2");
		$position = 0;
		quest::settimer("read",2);
		$reading = 1;
	}
}

sub EVENT_TIMER {
my @poem = ( "We thought of you with love today,", "But that is nothing new.", "We thought about you yesterday,", "And days before that too.", "We think of you in silence.", "We often speak your name.", "Now all we have is memories,", "And your picture in a frame.", "Your memory is our keepsake,", "With which we shall never part.", "God has you in his keeping.", "We have you in our heart.");

if ( $timer eq "read" )
{
$text = @poem[$position];
quest::say("$text");
$position += 1;
if ( ($position+1) > scalar @poem )
{
quest::stoptimer("read");
$reading = 0;
$position = 0;
return;
}
}
}