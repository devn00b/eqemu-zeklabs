# Zone to Friona event
# Zone: timorous
# AngeloX

sub EVENT_SPAWN
{
	$x = $npc->GetX();
	$y = $npc->GetY();
	quest::set_proximity($x - 250, $x + 250, $y - 250, $y + 250);
}

sub EVENT_ENTER
{
	quest::movepc(84,1408.9,-4334.3,-103.6);
}