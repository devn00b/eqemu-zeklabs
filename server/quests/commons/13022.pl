sub EVENT_SPAWN {
   $x = $npc->GetX();
   $y = $npc->GetY();
   quest::set_proximity($x - 120, $x + 120, $y - 120, $y + 120);
}

sub EVENT_ENTER {
   quest::setglobal("spire_wc",1,1,"F"); #when we enter the spires, set global true.
   $spire_wc = undef;
} 

sub EVENT_EXIT {
   quest::delglobal("spire_wc"); #when we leave spires, set global false.
   $spire_wc  = undef;
} 

sub EVENT_SIGNAL {
if ($signal == 101) {
$npc->CastSpell(13022,2935); #flavor
}
 }