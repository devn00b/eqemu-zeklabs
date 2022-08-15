# Zone to iceclad event
# Zone: nro
# AngeloX

sub EVENT_SPAWN
{
	$x = $npc->GetX();
	$y = $npc->GetY();
	quest::set_proximity($x - 55, $x + 55, $y - 55, $y + 55);
}

sub EVENT_ENTER
{
#west wastes:
#enable when opening velious
#quest::movepc(120,3713,1429.1,-307.8,24.4)


#iceclad:
#quest::movepc(110,-20395.3,3777.7,-34.1);
}