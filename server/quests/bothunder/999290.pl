my $portalSwitch = 0;

sub EVENT_SPAWN {    $x = $npc->GetX();    $y = $npc->GetY();    quest::set_proximity($x - 10, $x + 10, $y - 10, $y + 10); }

sub EVENT_ENTER {    

#if ( defined $qglobals{infernoportalsopen} && $infernoportalsopen == 1 ) { quest::movepc(209,265,525,-1420); quest::signalwith(999289,1,1); } 
#else
#{
quest::emote("will not allow anyone to pass through at this time.");
#}

}

sub EVENT_SIGNAL { if ( $signal == 1 ) { quest::depop(); } }