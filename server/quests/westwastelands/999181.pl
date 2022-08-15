#make rocks fall

sub EVENT_SPAWN
{
	$x = $npc->GetX();
	$y = $npc->GetY();
	$z = $npc->GetZ();
	quest::set_proximity($x - 65, $x + 65, $y - 65, $y + 65, $z - 100, $z + 100);
}

sub EVENT_ENTER {
quest::castspell(3944,$mobid);
}