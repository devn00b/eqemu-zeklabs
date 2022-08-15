#magmite

sub EVENT_DEATH {
#item template 99, 101, 102
$npc->AddLootGroup(46);
quest::spawn2(209102,0,0,-1087.8,-1736.9,1267.8,62.6);
}