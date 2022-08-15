#west ro notify player

sub EVENT_SPAWN
{
	$x = $npc->GetX();
	$y = $npc->GetY();
	$z = $npc->GetZ();
	quest::set_proximity($x - 65, $x + 65, $y - 65, $y + 65, $z - 100, $z + 100);
}

sub EVENT_ENTER {
$npc->FaceTarget($client);
$npc->DoAnim(29,0); #wave
quest::say("Hello $name, please watch your step as I would not want you to fall into the shaft.  The path to West Ro has collapsed and workers have been forced to dig back into this old shaft.  If you must go to West Ro be careful as there have been reports of rocks falling.");
}