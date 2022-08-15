############################################
# ZONE: Plane of Sky (airplane)
# LAST EDIT DATE: May 17, 2009
# VERSION: 1.0
# DEVELOPER: Congdar
#
# NAME: player.pl
# 
# *** QUESTS INVOLVED IN ***
#
# Plane of Sky Quirkiness
#
# *** QUESTS AVAILABLE TO ***
# 
# All Players
#
############################################
sub EVENT_LEVEL_UP {
    plugin::PlayerLevelUpHandler();
}

sub EVENT_DEATH {
    plugin::PlayerDeathHandler();
}

sub EVENT_ENTERZONE {
    #airplane specific
    $client->BuffFadeAll();
    
    plugin::EnterZoneHandler();
}

sub EVENT_POPUPRESPONSE {
    plugin::PlayerPopupResponseHandler($popupid);
}

sub EVENT_TIMER {
    plugin::PlayerTimerHandler($timer);
}

#END of FILE Zone:airplane  ID:player.pl

