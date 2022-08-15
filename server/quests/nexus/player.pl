# Guild Hall Regen Pool Script - Player Portion

sub EVENT_SIGNAL {
    quest::settimer("DoRegen", 6);
}

sub EVENT_ENTERZONE {
    plugin::EnterZoneHandler();
    plugin::EnterZoneBanditHandler();

  quest::settimer("ZoneIn", 3);
}

sub EVENT_ZONE {
#  quest::stoptimer("DoRegen"); #Stop the timer if they zone
}

sub EVENT_TIMER {
    plugin::PlayerTimerHandler($timer);

    quest::stoptimer("DoRegen");
    my $RegenPercent = 30; # Set Regen Percent per tic here
    if ($client) {
      my $HPRatio = $client->GetHPRatio();
      if ($HPRatio < 100) {
        my $HPMax = $client->GetMaxHP();
        my $HPNew = (($HPRatio + $RegenPercent) / 100) * $HPMax;
        $client->SetHP($HPNew);

      }
      my $ManaRatio = $client->GetManaRatio();
      if ($ManaRatio < 100) {
        my $ManaMax = $client->GetMaxMana();
        my $ManaNew = (($ManaRatio + $RegenPercent) / 100) * $ManaMax;
        $client->SetMana($ManaNew);
      }
      my $EnduranceRatio = $client->GetEnduranceRatio();
      if ($EnduranceRatio < 100) {
        my $EnduranceMax = $client->GetMaxEndurance();
        my $EnduranceNew = (($EnduranceRatio + $RegenPercent) / 100) * $EnduranceMax;
        $client->SetEndurance($EnduranceNew);

      }
      quest::settimer("DoRegen", 6);
    }
}

####
sub EVENT_LEVEL_UP {
    plugin::PlayerLevelUpHandler();
}

sub EVENT_DEATH {
    plugin::PlayerDeathHandler();
}

sub EVENT_POPUPRESPONSE {
    plugin::PlayerPopupResponseHandler($popupid);
}

