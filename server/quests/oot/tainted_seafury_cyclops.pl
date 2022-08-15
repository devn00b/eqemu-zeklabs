my $x;
my $y;
my $z;
my $h;

sub EVENT_DEATH {
  my $x = $npc->GetX();
  my $y = $npc->GetY();
  my $z = $npc->GetZ();
  my $h = $npc->GetHeading();
#  quest::spawn2(69142,0,0,$x,$y,$z,$h);
}

sub EVENT_SPAWN
{
#$rnd = quest::ChooseRandom(144482,144481,144480,144479,4201,4202,4203,4204,4205,4206);
#$npc->AddItem($rnd,1);
}
