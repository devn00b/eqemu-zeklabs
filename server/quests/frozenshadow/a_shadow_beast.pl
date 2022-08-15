# give 1st floorshadow beast, shadowbone a chance to spawn an undead gnoll on death
#
#Angelox

srand;

sub EVENT_DEATH {
  $gnoll = int(rand(100) + 1);
  if ($gnoll <= 10) {
    $spawngnoll = quest::spawn2(111158,0,0,$x,$y,$z,0);
    $attack = $entity_list->GetMobID($spawngnoll);
    $gnollattack = $attack->CastToNPC();
    $gnollattack->AddToHateList($client, 1);
  }
#quest::shout("-devtest- You Rolled $gnoll");
}

# EOF zone: frozenshadow

