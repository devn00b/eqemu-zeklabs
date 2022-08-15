#main center room portal in star room

sub EVENT_SPAWN {    $x = $npc->GetX();    $y = $npc->GetY();    quest::set_proximity($x - 10, $x + 10, $y - 10, $y + 10); }

sub EVENT_ENTER {
quest::emote("will not allow anyone to pass through at this time.");
}