sub EVENT_SPAWN
{
#quest::stoptimer("spawn");
#quest::settimer("spawn",60);
}
sub EVENT_SAY
{
quest::spawn2(999545,quest::ChooseRandom(999477,999478,999479),0,-398.9,684.5,155.3,90.0);
}

sub EVENT_TIMER
{
if ( $timer eq "spawn" )
{
my @nlist = $entity_list->GetNPCList();
my $size = scalar @nlist;
# original max was 800
if ( $size > 800 )
{
return;
}

quest::spawn2(999545,quest::ChooseRandom(999477,999478,999479),0,-398.9,684.5,155.3,90.0);

} #end timer
}