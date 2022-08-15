# Zone to timorous event
# Zone: butcher
# AngeloX

sub EVENT_SPAWN
{
	$x = $npc->GetX();
	$y = $npc->GetY();
	quest::set_proximity($x - 100, $x + 100, $y - 100, $y + 100);
}

sub EVENT_ENTER
{
	quest::movepc(96,-3260.10,-4544.56,19.47);
}