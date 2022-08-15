#There's a gate in the Neriak Commons that is actually three separate doors, but the gates should all lift together as one. This CLICKDOOR script accomplishes that. The ENTERZONE command was put together by Joligario, all credit to him.

sub EVENT_CLICKDOOR {
 if($doorid == 1 || $doorid == 257) {
  quest::forcedooropen(2);
  quest::forcedooropen(49);
  }
 if($doorid == 2 || $doorid == 258) {
  quest::forcedooropen(1);
  quest::forcedooropen(49);
 }
 if($doorid == 49 || $doorid == 305) {
  quest::forcedooropen(1);
  quest::forcedooropen(2);
 }
}

sub EVENT_LEVEL_UP {
    plugin::PlayerLevelUpHandler();
}

sub EVENT_DEATH {
    plugin::PlayerDeathHandler();
}

sub EVENT_ENTERZONE {
    plugin::EnterZoneHandler();
}

sub EVENT_POPUPRESPONSE {
    plugin::PlayerPopupResponseHandler($popupid);
}

sub EVENT_TIMER {
    plugin::PlayerTimerHandler($timer);
}
