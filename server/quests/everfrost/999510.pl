sub EVENT_SPAWN
{
#quest::stoptimer("spawn");
#quest::settimer("spawn",60);
#quest::snow(1);
}

sub EVENT_SAY
{
#quest::say("test");
#$rnd8 = quest::ChooseRandom(999495,999496,999498,999500);
#quest::spawn2($rnd8,101,0,-5394.7,681.8,-13.0,61.5);
}

sub EVENT_TIMER
{
if ( $timer eq "spawn" )
{
my @nlist = $entity_list->GetNPCList();
my $size = scalar @nlist;
# original max was 320
if ( $size > 300 )
{
return;
}

$rnd = quest::ChooseRandom(999495,999496,999498);
$rnd2 = quest::ChooseRandom(999497,999499,999501,999495,999498);
$rnd3 = quest::ChooseRandom(999497,999500,999502,999496,999498);
$rnd4 = quest::ChooseRandom(999497,999500,999502);

$rnd5 = quest::ChooseRandom(999495,999496,999498);
$rnd6 = quest::ChooseRandom(999500,999502,999496,999498);
$rnd7 = quest::ChooseRandom(999499,999501,999495,999498);

$rnd8 = quest::ChooseRandom(999495,999496,999498,999500);

quest::spawn2($rnd,81,0,-6325.5,1882.4,-60.2,-60);
quest::spawn2($rnd2,82,0,-6264.5,2059.4,-60.2,93.5);
quest::spawn2($rnd3,85,0,-6179.5,1828.9,-69.2,106.5);
quest::spawn2($rnd,88,0,-6081.4,1468.7,-97.0,105.0);
quest::spawn2($rnd4,quest::ChooseRandom(89,100),0,-5988.9,1090.1,-88.0,57.0);

quest::spawn2($rnd5,93,0,-5987.9,942.1,-62.4,58.0);
quest::spawn2($rnd6,94,0,-5520.5,1816.4,-61.4,73.0);
quest::spawn2($rnd7,95,0,-5719.0,2300.4,-64.3,58.0);

quest::spawn2($rnd8,96,0,-5676.4,1205.5,-24.4,146.0);

quest::spawn2($rnd8,quest::ChooseRandom(97,99,100,101),0,-5394.7,681.8,-13.0,61.5);
quest::spawn2($rnd4,quest::ChooseRandom(98,99,100,101),0,-5275.0,383.4,-30.2,54);

$rndSpawn = quest::ChooseRandom(0,1,2,3);
if ( $rndSpawn == 1 )
{
quest::snow(1);
quest::spawn2($rnd3,quest::ChooseRandom(83,92,99),0,-6105.3,2207.0,-60.5,66.0);
quest::spawn2($rnd,quest::ChooseRandom(84,90,92,99,100),0,-6244.2,1887.0,-60.1,65.0);
quest::spawn2($rnd2,quest::ChooseRandom(86,91,92),0,-6047.7,2491.5,-73.6,63.0);
quest::spawn2($rnd4,quest::ChooseRandom(87,92,99),0,-6263.9,2086.9,-60.2,90.0);
}

} #end timer
}