# EPIC CLERIC -skyfire-

sub EVENT_DEATH {
    my $x = $npc->GetX();
    my $y = $npc->GetY();
    my $z = $npc->GetZ();
    my $h = $npc->GetHeading();
  quest::spawn2(91090,0,0,$x,$y,-146.3,$h); # Zordakalicus_Ragefire
}


#End of File, Zone:skyfire  NPC:32038 -- Zordak Ragefire
