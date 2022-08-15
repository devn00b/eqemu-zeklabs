sub EVENT_DEATH
{
plugin::handleArenaDeath($client,$killedby_charid,$legitkill,$zoneid,$instanceid,$entity_list);
}

sub EVENT_ENTERZONE
{
plugin::arePlayersReady($entity_list,$client,$instanceid,$zoneid);
}

sub EVENT_LOGOUT
{
plugin::handleArenaLogout($client,$instanceid,$zoneid);
}