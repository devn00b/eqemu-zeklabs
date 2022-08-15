sub EVENT_DEATH
{
$min = 0;
$max = 100;
my $rnd = int(rand($max)) + $min;
my $bonus = $npc->GetLevel() - 51;

if ( $bonus < 0 )
{
$bonus = 0;
}

my $base = 1 + $bonus;
if ( $rnd < $base )
{

my $randItem = 0;
$npcname = $npc->GetName();
if ( $npc->GetBaseRace() == 160 )
{
$randItem = quest::ChooseRandom(142855,142842,142849,142850);
}
elsif ( $npcname =~/shaman/i && $npcname =~/froglok/i )
{
$randItem = quest::ChooseRandom(142856,142842,142849,142850);
}
else
{
$randItem = quest::ChooseRandom(142842,142849,142850);
}

#$npc->AddItem($randItem,1);
}#end rnd

}#end death
