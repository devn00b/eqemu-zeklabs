sub EVENT_SPAWN {    $x = $npc->GetX();    $y = $npc->GetY(); $z = $npc->GetZ();   quest::set_proximity($x - 55, $x + 55, $y - 55, $y + 55, $z - 5,$z + 5); }

sub EVENT_ENTER {    
if ( !( defined $qglobals{magmiteopen} || $magmiteopen == 1 ) ) { quest::movepc(209,-720,-1707,1727); }
}