sub EVENT_LEVEL_UP {
    plugin::PlayerLevelUpHandler();
}

sub EVENT_DEATH {
    plugin::PlayerDeathHandler();
}

sub EVENT_ENTERZONE {
    plugin::EnterZoneHandler();
    plugin::EnterZoneBanditHandler();

}

sub EVENT_POPUPRESPONSE {
    plugin::PlayerPopupResponseHandler($popupid);
}

sub EVENT_TIMER {
    plugin::PlayerTimerHandler($timer);
}
