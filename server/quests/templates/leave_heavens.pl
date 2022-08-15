sub EVENT_SPAWN
{
quest::settimer("checkclients",120);
}

sub EVENT_TIMER
{
if ( $timer eq "checkclients" ) { $entity_list->SignalAllClients(1); }
}