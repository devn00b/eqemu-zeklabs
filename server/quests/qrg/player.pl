sub EVENT_LEVEL_UP {
    plugin::PlayerLevelUpHandler();
}

sub EVENT_DEATH {
    plugin::PlayerDeathHandler();
    plugin::handleArenaDeath($client,$killedby_charid,$legitkill,$zoneid,$instanceid,$entity_list);
}

sub EVENT_ENTERZONE {
    plugin::arePlayersReady($entity_list,$client,$instanceid,$zoneid);
    plugin::EnterZoneHandler();
}

sub EVENT_POPUPRESPONSE {
    plugin::PlayerPopupResponseHandler($popupid);
}

sub EVENT_TIMER {
    plugin::PlayerTimerHandler($timer);
}

sub EVENT_LOGOUT
{
plugin::handleArenaLogout($client,$instanceid,$zoneid);
}