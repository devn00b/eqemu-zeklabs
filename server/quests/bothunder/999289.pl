my $portalSwitch = 0;

sub EVENT_SPAWN {    $x = $npc->GetX();    $y = $npc->GetY();    quest::set_proximity($x - 10, $x + 10, $y - 10, $y + 10); }

sub EVENT_ENTER {

if ( defined $qglobals{infernoportalsopen} && $infernoportalsopen == 1 ) {  quest::setglobal("magmiteopen",1,3,"D1"); quest::movepc(209,-765,-1735,1270); quest::signalwith(999290,1,1); }
else
{
quest::emote("will not allow anyone to pass through at this time.");
}

} 

sub EVENT_SIGNAL { if ( $signal == 1 ) { quest::depop(); } }