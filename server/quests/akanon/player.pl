sub EVENT_LEVEL_UP {
    plugin::PlayerLevelUpHandler();
}

sub EVENT_TIMER {
    plugin::PlayerTimerHandler($timer);

    #gnome race
    if ( $timer eq "akanonrace" ) {
        $ToBeForeverFlag = "F";
        $raceDuration    = "H49";

        $startMsg = "[GNOME RACE]";

        #REMOVE FOR OFFICIAL RACE
        if ( $client->GetGM() == 0 ) {
            return;
        }

        #REMOVE FOR OFFICIAL RACE
        if ( defined $qglobals{akanon_race_notinterested} ) {
            $client->Message( 14,
"$startMsg You have noted you do not wish to participate in the race, you cannot re-enter the race."
            );
            quest::stoptimer("akanonrace");
            return;
        }

        if ( defined $qglobals{akanon_racefinish_playerflag} ) {
            $client->Message( 14,
                "$startMsg You have already completed the AkAnon Gnome Race." );
            quest::stoptimer("akanonrace");
            return;

        }

        if (   defined $qglobals{akanon_racestarted}
            && defined $qglobals{akanon_race_playerflag} )
        {
            $client->Message( 14,
                "$startMsg You are flagged for the race, GO GO GO [STEAMFONT]!!"
            );
            quest::stoptimer("akanonrace");
            return;
        }

        #no flag exists yet
        $race = $client->GetBaseRace();

        if ( $client->GetGM() == 0 && $client->GetBaseRace() != 12 ) {
            $client->Message( 15,
"$startMsg The official race today is gnomes only!  You will not be able to participate."
            );
            quest::stoptimer("akanonrace");
            return;
        }

        if ( $client->GetGM() == 0 && $ulevel > 1 ) {
            $client->Message( 15,
"$startMsg The official race today is gnomes only and you must be level 1!  You will not be able to participate currently as you are level $ulevel."
            );
            quest::stoptimer("akanonrace");
            return;
        }

        if ( $client->GetBindZoneID() != 55 ) {
            if ( $qglobals{akanon_notice} == undef ) {
                $client->Message( 15,
"$startMsg If you wish to participate in today's official race be sure to be bound in Ak'Anon and prepare for the official word!"
                );
                $client->Message( 13,
"WARNING: PARTICIPANTS WILL HAVE THEIR INVENTORY AND BANK DELETED AT THE START OF THE RACE AND YOU WILL BE REPOPULATED WITH STARTER ITEMS!!  SPEAK TO CORRI RINE NEAR ENTRANCE OF AKANON OR NEXUS TO BE EXEMPTED FROM THE GNOME RACE!"
                );
                quest::setglobal( "akanon_notice", "1", 5, "M5" );
            }
        }
        else {
            if (
                !defined $qglobals{akanon_racestarted}
                && ( !defined $qglobals{akanon_notice}
                    || $qglobals{akanon_notice} == "1" )
              )
            {
                $client->Message( 15,
"$startMsg You are bound to Ak'Anon and should be good to go for today's race!  Wait in Ak'Anon for the announcement to head towards Steamfont!"
                );
                $client->Message( 13,
"WARNING: PARTICIPANTS WILL HAVE THEIR INVENTORY AND BANK DELETED AT THE START OF THE RACE AND YOU WILL BE REPOPULATED WITH STARTER ITEMS!!  SPEAK TO CORRI RINE NEAR ENTRANCE OF AKANON OR NEXUS TO BE EXEMPTED FROM THE GNOME RACE!"
                );

                if ( $client->HasItem(142745) == -1 ) {
                    $client->Message( 14,
                        "$startMsg No spirits on hand? No problem!" );
                    $client->SummonItem( 142745, 20 );
                }
                quest::setglobal( "akanon_notice", "2", 5, "M1" );

                #quest::setglobal("akanon_racestarted","1",7,"H1");
            }
            elsif ( defined $qglobals{akanon_racestarted} ) {
                $requiredIntoxication = 20;
                $yourIntox            = $client->GetIntoxication();
                if ( !defined $qglobals{akanon_race_playerflag} ) {
                    if ( $yourIntox < $requiredIntoxication ) {
                        if ( !defined $qglobals{akanon_notice}
                            || $qglobals{akanon_notice} != "3" )
                        {
                            $client->Message( 14,
"$startMsg The race has started! You are not intoxicated enough to start this race ($yourIntox < $requiredIntoxication), GET DRINKING!"
                            );
                            quest::setglobal( "akanon_notice", "3", 5, "S30" );
                        }
                        if ( $client->HasItem(142745) == -1 ) {
                            $client->Message( 14,
                                "$startMsg No spirits on hand? No problem!" );
                            $client->SummonItem( 142745, 20 );
                        }
                    }
                    else {
                        #UNCOMMENT FOR RELEASE
                        #if ( $client->GetGM() == 0 ) {
                        $client->SetPVPValue(2);   #no discord players allowed!!
                        $client->NukeInventory();
                        $client->SetStartingItems();
                        $client->BuffFadeAll();

                        #}
                        quest::delglobal("akanon_race_playerstep");
                        $client->Message( 14,
"$startMsg You are flagged for the race, GO GO GO [STEAMFONT] ($yourIntox)!!"
                        );
                        $client->Message( 14,
"$startMsg You are flagged for the race, GO GO GO [STEAMFONT]!!"
                        );
                        $client->Message( 14,
"$startMsg You are flagged for the race, GO GO GO [STEAMFONT]!!"
                        );
                        quest::setglobal( "akanon_race_playerflag", "1", 5,
                            $ToBeForeverFlag );
                        quest::level(1);
                        if ( defined $client->GetGroup() ) {
                            $client->LeaveGroup();
                            $client->Message( 13,
"$startMsg Disbanded player from group as you cannot group during the race."
                            );
                        }
                        $guild = $client->GuildID();
                        if ( $guild > 0 && $guild < 10000 ) {
                            quest::setguild( -1, 0 );
                            $client->Message( 13,
"$startMsg You have been disbanded from your guild as you cannot be in one during the race."
                            );
                        }
                        quest::setglobal( "nogroupallowedflag", "1", 5,
                            $raceDuration );
                        quest::setglobal( "nobeneficialbuffsallowed", "1", 5,
                            $raceDuration );
                        quest::setglobal( "nobindallowed", "1", 5,
                            $raceDuration );
                        quest::setglobal( "notranslocateallowed", "1", 5,
                            $raceDuration );
                        quest::setglobal( "noteleportallowed", "1", 5,
                            $raceDuration );
                        quest::setglobal( "notradeallowed", "1", 5,
                            $raceDuration );
                        $timestamp = localtime(time);
                        quest::setglobal( "akanon_race_starttime", $timestamp,
                            5, $ToBeForeverFlag );

                    }
                }
                else {
                    #quest::delglobal("akanon_race_playerflag");
                    $client->Message( 14,
"$startMsg You are flagged for the race, GO GO GO [STEAMFONT]!!"
                    );
                }
            }
            elsif ( !defined $globals{akanon_racestarted}
                && defined $qglobals{akanon_race_playerflag} )
            {
                quest::delglobal("akanon_race_playerflag");
            }
        }

    }

    #end of gnome race timer
}

sub EVENT_ENTERZONE {
    plugin::EnterZoneHandler();
}

sub EVENT_POPUPRESPONSE {
    plugin::PlayerPopupResponseHandler($popupid);
}

sub EVENT_DEATH {
    plugin::PlayerDeathHandler();
}