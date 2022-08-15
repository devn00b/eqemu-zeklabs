# Spawns Rathyl's_Incarnate on Rathyl's death
# Angelox

sub EVENT_DEATH
{
$rnd=int(rand(100));
$rnditm='20444';
if($rnd <=75){
$npc->AddItem($rnditm,1,0);
}

 my $a = 44104;
 my $x = $npc->GetX();
 my $y = $npc->GetY();
 my $z = $npc->GetZ(); 
 my $h = $npc->GetHeading();
  {
  quest::say("HA! You thought you killed me?");
  quest::spawn2($a,0,0,$x,$y,$z,$h);
  }
}
# EOF zone: najena NPC: Rathyl
