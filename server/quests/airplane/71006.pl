
sub EVENT_DEATH {
  my $x = $npc->GetX();
  my $y = $npc->GetY();
  my $z = $npc->GetZ();
  my $h = $npc->GetHeading();
  quest::spawn2(71106,0,0,$x+10,$y,$z+5,$h,1);
  quest::spawn2(71106,0,0,$x-10,$y,$z+5,$h,1);
  quest::spawn2(71106,0,0,$x,$y,$z+5,$h,1);
}