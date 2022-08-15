
sub EVENT_DEATH {
plugin::handlenpc_ooglyloot($client,$npc,1);
plugin::handlenpc_ooglyloot($client,$npc,1);
  my $x = $npc->GetX();
  my $y = $npc->GetY();
  my $z = $npc->GetZ();
  my $h = $npc->GetHeading();
  quest::setglobal("sirran",6,3,"M10");
 #Sirran the Lunatic
  quest::spawn2(71058,0,0,$x,$y,$z+5,$h,1);
} 
