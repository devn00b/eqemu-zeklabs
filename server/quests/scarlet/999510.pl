sub EVENT_SPAWN
{
quest::stoptimer("spawn");
quest::settimer("spawn",60);
}

sub EVENT_SAY
{
if ( $client->GetGM() != 0 )
{
quest::spawn2(999545,quest::ChooseRandom(999477,999478,999479,999480,999481,999482),0,-398.9,684.5,155.3,90.0);
}
}

sub EVENT_TIMER
{
if ( $timer eq "spawn" )
{
my @nlist = $entity_list->GetNPCList();
my $size = scalar @nlist;
# original max was 800
if ( $size > 770 )
{
return;
}

$rndspawn = quest::ChooseRandom(0,1,2,3);
if ($rndspawn == 0 )
{ quest::spawn2(999545,quest::ChooseRandom(999477,999478,999479,999480,999481,999482),0,-398.9,684.5,155.3,90.0); }
elsif ($rndspawn == 1 )
{ quest::spawn2(999545,quest::ChooseRandom(999477,999478,999479,999480,999481,999482),0,-343.1,629.5,155.3,90.0); }
elsif ($rndspawn == 2 )
{ quest::spawn2(999545,quest::ChooseRandom(999477,999478,999479,999480,999481,999482),0,-289.4,724.7,155.3,90.0); }
elsif ($rndspawn == 3 )
{ quest::spawn2(999545,quest::ChooseRandom(999477,999478,999479,999480,999481,999482),0,-360.2,885.5,155.3,90.0); }


} #end timer
}
