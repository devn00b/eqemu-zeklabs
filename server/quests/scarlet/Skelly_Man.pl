sub EVENT_SPAWN
{
quest::spawn_condition("scarlet", 1, 1);
quest::settimer("findmob",60);
}

sub EVENT_TIMER
{
if ( $timer eq "findmob" )
{
if ( !defined($entity_list->GetNPCByNPCTypeID(999476)) && 
	!defined($entity_list->GetNPCByNPCTypeID(999477)) )
{
quest::spawn_condition("scarlet", 1, 1);
}
}

}