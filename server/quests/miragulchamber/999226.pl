sub EVENT_SPAWN { 
my $max = 60;
my $min = 30;
my $rnd = int(rand($max)) + $min;

if ( $rnd > 55 ) { quest::npctexture(19); }
if ( $rnd > 45 && $rnd < 51 ) { quest::npctexture(18); }
quest::settimer("bow",$rnd); }

sub EVENT_COMBAT {
if ( combat_state == 1 )
{ quest::stoptimer("bow"); }
else { 
my $max = 60;
my $min = 30;
my $rnd = int(rand($max)) + $min;

quest::settimer("bow",$rnd);
}
}

sub EVENT_TIMER {
if ($timer eq "bow")
{
my $max = 7;
my $min = 1;
my $rnd = int(rand($max)) + $min;

if ( $rnd == 1 ) {
quest::say("Master Miragul teach us your secrets!"); $npc->DoAnim(50,0); }
if ( $rnd == 3 ) {
quest::say("We serve you Master Miragul!"); $npc->DoAnim(70,0); }
if ( $rnd == 5 ) {
quest::say("What must we do to serve you master!"); $npc->DoAnim(70,0); }
if ( $rnd == 5 ) {
quest::say("Please teach us master!"); $npc->DoAnim(50,0); }
}
}