#cave broodmother


sub EVENT_SPAWN
{
$rnd = quest::ChooseRandom(19700,1544,10032,10022,10023,10024,10025);
$npc->AddItem($rnd,1);
quest::stoptimer("spawn");
quest::settimer("spawn",45);
}

sub EVENT_TIMER
{
my $spawnnpcid = 999544;
my $count = 0;
my @nlist = $entity_list->GetNPCList();
foreach my $n (@nlist){
 if ( $n->GetNPCTypeID() == $spawnnpcid )
	{
		$count+=1;
	}
}

if ( $count > 1 )
{
return;
}

if ( $timer eq "spawn" )
{
quest::spawn2($spawnnpcid,quest::ChooseRandom(105,106),0,-815.5,342.1,-92.2,60.1);
}
}

sub EVENT_DEATH
{
quest::stoptimer("spawn");
}