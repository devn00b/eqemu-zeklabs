sub EnterZoneHandler {
    my $client   = plugin::val('$client');
    my $qglobals = plugin::var('qglobals');
    my $zoneid = plugin::val('$zoneid');

    plugin::GnomeRaceEnterZone();



#####

    #if ($client->GetPVPFlag() != 5  && $client->GetPVPFlag() != 2  && $client->Admin() < 1 )
    #{
    #    $client->SetPVPValue(2);
    #}
	
	#bind players to nexus that were in scarlet
	if ( $client->GetBindZoneID() == 175 )
	{
	$client->SetBindPoint(152,0,0,-24,"0");
	}
        #no paludal binds
        if($client->GetBindZoneID() == 156){
        $client->SetBindPoint(152,0,0,-24,"0");
        }

	plugin::HandleLocationBonuses();
	
	if ( $zoneid == 152 && $client->GetX() == 0 && $client->GetY() == 0 )
	{
		$max = 6;
		$rndX = int(rand($max)) * 5;
		$rndY = int(rand($max)) * 5;

		$mix = int(rand(2));
		if ( $mix == 1 )
		{
			$rndX *= -1;
		}
		$mix = int(rand(2));
		if ( $mix == 1 )
		{
			$rndY *= -1;
		}

		$client->MovePC(152,$rndX,$rndY,-24.0,0.0);
	}

	if (!defined $qglobals->{akanon_racestarted} || ( defined $qglobals->{akanon_racestarted} && !defined $qglobals->{akanon_race_playerflag} ) )
	{
		if ( $zoneid == 151 || $zoneid == 155 || $zoneid == 152 ) {
			$probation = $qglobals->{nexusprobationperiod};
			if ( $probation == 1 ) {
				$client->Message( 15,"***You have returned before the Nexus probation period expired from a previously provided incentive, thus the PVP point provided before has been taken away.");
				$client->AddPVPPoints(-1);
				quest::setglobal( "nexusprobationperiod", "0", 5, "M5" );
			}
			elsif ( defined $qglobals->{nexusleavecooldown} ) {
				$client->Message( 15,"***Nexus incentive cooldown is set, you cannot earn an incentive for leaving Nexus at this time.");
			}
			else {
				#quest::settimer("nexusleave",60);
				#quest::setglobal("nexusleaveincentive","1",5,"M5");
				#quest::setglobal("nexuscount","0",5,"M5");
			}
		}
		else {
			quest::settimer( "randomreward", 60 );
		}
	}


####### DEVN00B STUFF HERE #######
#
# For S5 giving starter guild. Once they hit 30 they get removed (so similar to above sortaish).
# Guild ID for starter guild will be 69.
#
# Also, added check for discord players surname. Takes 100pts no buff and sends them to nexus if they try and change it. each time.
#    if ( $client->GuildID() == 69 && $ulevel > 29 && $client->Admin() < 1 ) {
#        $client->Message( 13,
#"You have reached level 30. This has caused you to be evicted from Soulblighters Dominion. Good Luck Player."
#        );
#        quest::setguild( -1, 0 );
#    }

#devnoob removed discord handler for s7 since teamid 5 is not discord, its bandit!
#    plugin::EnterZoneDiscordHandler();

##devn00b s7 bandit check
#if($client->GetPVPFlag() == 5) {
#plugin::EnterZoneBanditHandler();
#}

    $client->SummonLootGroup(24);

    $approved = $qglobals->{popup_allowautoscribe};

    if ( !defined $approved ) {
        quest::popup("Auto Scribe and Skills","Auto scribe and auto skill ups is currently ENABLED by default.  Click 'Yes' to DISABLE this feature.  See Heimdall in Nexus for more information about managing auto scribe and skill-ups or manually triggering the feature.  Ignoring this prompt will leave the auto scribe and skill feature enabled.",5555,120);
        quest::setglobal( "popup_allowautoscribe", 1, 5, "F" );
    }
}

#end EnterZoneHandler

##### DEVN00B: Handle S7 Bandit Buff ####
sub EnterZoneBanditHandler {
my $client   = plugin::val('$client');
#my $name = plugin::val('$name');
if ( $client->GetPVPFlag() == 5){
$banditbuff = 4171;
quest::selfcast($banditbuff);
}
}

#### END S7 BANDIT BUFF  FUNC ####

sub EnterZoneDiscordHandler {
#disable for s7
$devnoobdisable = 10;
if($devnoobdisable > 1) {
return;
}
    my $client   = plugin::val('$client');
    my $qglobals = plugin::var('qglobals');

##devn00b force discord players to have a title/lastname so they can be targeted.
    if ( $client->GetPVPFlag() == 5 && $client->Admin() < 1 ) {

        #give discord players spell haste.
        #$selfbuff = 3531;

        $casterbuff  = 5193;
        $casterbuffb = 5194;

        #SK/PAL/WAR
        $tankbuff  = 5196;
        $tankbuffb = 5195;

        #MONK/ROG/RNG/BRD
        $dpsbuff  = 5191;
        $dpsbuffb = 5192;

        if ( defined $qglobals->{akanon_racestarted}  && defined $qglobals->{akanon_race_playerflag} && $client->Admin < 1 )
       {
            $casterbuff  = 0;
            $casterbuffb = 0;

            #SK/PAL/WAR
            $tankbuff  = 0;
            $tankbuffb = 0;

            #MONK/ROG/RNG/BRD
            $dpsbuff  = 0;
            $dpsbuffb = 0;
        }

#set title every time they zone since it doesnt notify client its not annoying. Also we cant check for specific title being shown =/
        $client->SetTitleSuffix( "of Discord", 1 );

        #get player lastname. only updates on zone...lame
        $bonkers = $client->GetLastName();

        #this is the lastname we expect
        $bonked = "Discord Player";

        #this is the # of points we take for being a dick.
        $imadick = -100;

#if their lastname doesnt match what we expect (IE THEY CHANGED IT) Then we need to set it and punish them for being dicks.
        if ( $bonkers ne $bonked ) {
            quest::say("Your last name $bonkers needs to remain $bonked otherwise....");
            quest::say("Since you are attempting to break the rules, I have taken $imadick points! You also get no buff.");
            quest::say("Oh since I am an asshole, im sending you to nexus too");
            quest::shout2("Hello Server, Please note that I'm Sorta dumb and changed my last name while I was a discord player. Please laugh at me cuz I lost 100 points." );

            #punish them harshly for attempting to cheat.
            $client->AddPVPPoints($imadick);

            #set lastname
            quest::surname("Discord Player");

            #force them back to nexus
            $backtonexus = 2734;
            quest::selfcast($backtonexus);
        }

        #lastname is OK then lets do this shit
        #lets give them the spell haste
        #quest::selfcast($selfbuff);

#lets decide who gets what buff since whats good for casters sucks for tanks etc.
        if (   $class eq "Necromancer"
            or $class eq "Enchanter"
            or $class eq "Magician"
            or $class eq "Wizard"
            or $class eq "Druid"
            or $class eq "Shaman"
            or $class eq "Cleric" )
        {

          #if less than 31 give them low buff. over 30 so 31, give them highbuff
            if ( $ulevel < 31 ) {
                quest::selfcast($casterbuff);
            }
            if ( $ulevel > 30 ) {
                quest::selfcast($casterbuffb);
            }
        }

        if (   $class eq "Warrior"
            or $class eq "Shadowknight"
            or $class eq "Paladin" )
        {
          #quest::selfcast($tankbuff);
          #if less than 31 give them low buff. over 30 so 31, give them highbuff
            if ( $ulevel < 31 ) {
                quest::selfcast($tankbuff);
            }
            if ( $ulevel > 30 ) {
                quest::selfcast($tankbuffb);
            }
        }

        if (   $class eq "Monk"
            or $class eq "Rogue"
            or $class eq "Ranger"
            or $class eq "Bard" )
        {

          #if less than 31 give them low buff. over 30 so 31, give them highbuff
            if ( $ulevel < 31 ) {
                quest::selfcast($dpsbuff);
            }
            if ( $ulevel > 30 ) {
                quest::selfcast($dpsbuffb);
            }

            #quest::selfcast($dpsbuff);
        }

    }

###### END DEVS SHIT ##########

}

#end EnterZoneDiscordHandler

sub PlayerPopupResponseHandler {
    my $popupid  = shift;
    my $client   = plugin::val('$client');
    my $qglobals = plugin::var('qglobals');

    if ( $popupid eq "5555" ) {
        $client->Message( 13,"Auto scribe and auto skill ups DISABLED.  Go to Nexus and speak to Heimdall to manage (re-enable or disable) this feature, as well as manually auto-scribing and skill ups.");
        quest::setglobal( "disabledlevelupdates", 1, 5, "F" );
    }
}

#end PlayerPopupResponseHandler

sub PlayerTimerHandler {
    my $timer    = shift;
    plugin::GnomeRaceEventTimer($timer);
    plugin::NexusTimerHandler($timer);
    plugin::InFieldPointTimer($timer);
    plugin::HandleLocationBonuses($timer);
}

#end PlayerTimerHandler


#nexus timer start
sub NexusTimerHandler {
    my $client   = plugin::val('$client');
    my $qglobals = plugin::var('qglobals');
    my $timer    = shift;

    if ( $timer eq "nexusleave" ) {
        $nexusIncentive = $qglobals->{nexusleaveincentive};

        $probation  = $qglobals->{nexusprobationperiod};
        $nexusCount = $qglobals->{nexuscount};
        $nexusCount += 1;
        $timeLeft = 5 - $nexusCount;
        if ( $timeLeft < 1 ) {
            $timeLeft = 1;
        }
        quest::setglobal( "nexuscount", $nexusCount, 5, "M5" );

        if ( $probation == 1 ) {
            $client->Message( 15,"***You have returned before the Nexus probation period expired from a previously provided incentive, thus the PVP point provided before has been taken away.");
            $client->AddPVPPoints(-1);
            quest::stoptimer("nexusleave");
            quest::setglobal( "nexusprobationperiod", "0", 5, "M5" );
        }
        elsif ( $nexusIncentive == 1 ) {
            $client->Message( 15,"***You will be awarded one PVP point and be given a hp/regen buff along with Spirit of the Wolf if you use one of the spire teleporters to leave Nexus in the next $timeLeft minute(s).  WARNING! returning before the 5 minute probation will result in the loss of that pvp point!");
        }
        else {
            $client->Message( 15,"***Incentive period has expired to earn the PVP point.  No new incentive will be provided until you leave nexus and return at a later time.");
            quest::stoptimer("nexusleave");
            quest::setglobal( "nexusleavecooldown", "1", 5, "M30" );
        }
    }
}
#end NexusTimerHandler


#InFieldPointPosition, randomreward timer is set in EnterZoneHandler, only if not bazaar/nexus/shar vahl
sub InFieldPointTimer {
    my $client   = plugin::val('$client');
    my $qglobals = plugin::var('qglobals');
    my $timer    = shift;
    if ( $timer eq "randomreward" ) {
        $worldptsenabled = quest::GetRule("PVPCombat:WorldPoints");

        if ( $worldptsenabled ne 'true' ) {
            quest::stoptimer("randomreward");
            return;
        }

        $probation   = $qglobals->{nexusprobationperiod};
        $fieldpoints = $qglobals->{fieldpoints};
        $reduction   = ( $client->GetLevel() / 40 );

        $rnd = quest::ChooseRandom( 0, 1, 2, 3, 4 );
        if ( $client->Admin() == 0 && ( $rnd == 1 && $instanceid < 10000 ) ) {
            if ( defined $fieldpoints && $fieldpoints >= 20 ) {
                $client->Message( 13,"[INFIELD-SYSTEM] You are currently on cooldown from field points, this cooldown will expire after 30 minutes you last received points.");
                return;
            }
            if ( $client->GetLevel() < 20 ) {
                $client->Message( 13,"[INFIELD-SYSTEM] You are too low in level to receive points in the field.");
                return;
            }

            $prevx = $qglobals->{fieldpoints_x};
            $prevy = $qglobals->{fieldpoints_y};
            quest::setglobal( "fieldpoints_x", int( $client->GetX() ),
                5, "M30" );
            quest::setglobal( "fieldpoints_y", int( $client->GetY() ),
                5, "M30" );

            $diffx = $prevx - int( $client->GetX() );
            $diffy = $prevy - int( $client->GetY() );

            if ( $diffx < 0 ) { $diffx = -$diffx; }
            if ( $diffy < 0 ) { $diffy = -$diffy; }

            $clientcount = 0;
            $mult        = 0;
            $allies      = 0;
            my @clientlist = $entity_list->GetClientList();
            foreach $ent (@clientlist) {
                if (   $ent->Admin() == 0  && $ent->GuildID() != $client->GuildID() && $ent->GetLevel() == $client->GetLevel() )
            {
                    if ( $mult < 2 ) {
                        $mult += 1;
                    }
                    $clientcount += 1;
                }
                elsif ($ent->Admin() == 0
                    && $ent->GuildID() == $client->GuildID() )
                {
                    $clientcount -= 0.25;
                    if ( $ent->GuildID() > 0 && $ent->GuildID() < 10000 ) {
                        $allies += 1;
                    }
                }
            }
            if ( $clientcount < 1 ) {
                $rnd2 = quest::ChooseRandom( 0, 1, 2, 3, 4 );
                if ( $rnd2 != 1 ) {
                    if ( $allies > 0 ) {
                        $client->Message( 15,"[INFIELD-SYSTEM] You have many allies here ($allies) and thus your point earning has slowed.");
                        return;
                    }
                   $client->Message( 15,"[INFIELD-SYSTEM] Not much going on here.. you are slowing down your PVP point earnings from being in an inactive area of the field.");
                    return;
                }
            }
            $ptclients = 0;
            if ( $clientcount > 5 ) {
                $ptclients = 1;
            }

            $pts = ( $mult + $ptclients ) / $reduction;

            if ( $pts > 3 ) {
                $pts = 3;
            }
            if ( $pts < 1 || $probation == 1 || $client->GetPVPPoints() > 1000 )
            {
                $pts = 1;
            }

            if ( $diffx < 100 && $diffy < 100 ) {
                $pts = 1;
            }

            $newpts = $fieldpoints + $pts;
            quest::setglobal( "fieldpoints", $newpts, 5, "M30" );

            $client->AddPVPPoints($pts);
            if ( $probation == 1 ) {
                $client->Message( 15,"[INFIELD-SYSTEM] You have earned $pts point(s) for remaining in the field (NEXUS PROBATION PERIOD LIMITED POINT EARNING)!");
            }
            elsif ( $diffx < 100 && $diffy < 100 ) {
                $client->Message( 15,"[INFIELD-SYSTEM] Not working those legs much are we!  You received $pts point(s) for remaining in the field!");
            }
            else {
               $client->Message( 15,"[INFIELD-SYSTEM] You have earned $pts point(s) for remaining in the field!");
            }
        }
    }
}
#end InFieldPointTimer

sub PlayerDeathHandler {
    my $client   = plugin::val('$client');
    my $qglobals = plugin::var('qglobals');
    quest::delglobal("skyflag");
    quest::delglobal("skydisableflag");
}

#end PlayerDeathHandler

sub PlayerLevelUpHandler {
    my $client   = plugin::val('$client');
    my $qglobals = plugin::var('qglobals');
    my $class    = plugin::val('$class');
    my $ulevel   = plugin::val('$ulevel');

    if ( $class eq "Berserker" ) {
        quest::traindiscs( $ulevel, 1 );
    }
}

sub CheckLocationBonus {
    my $teamID    = shift;
    my $teamSpell = shift;
    my $locationText  = shift;
    my $teamSpellText  = shift;
    my $client   = plugin::val('$client');
    my $qglobals = plugin::var('qglobals');
    if ( $teamID > 0 && $client->GetPVPFlag() == $teamID )
    {
	$hasSpell = $client->FindBuff($teamSpell);
###
#devn00b: Check for Spirit of Eagle (2517) since it screws stuff up.
###
	$hasconflict = $client->FindBuff(2517);

	if ( $hasSpell == 0 )
	{
           if ($hasconflict == 1 && $teamSpell eq '1776') {
		#if we have SoE and we are trying to cast sow, just stop. otherwise go ahead.
		return 0;
           }
    		$client->CastSpell($teamSpell,$client->GetID(),10,0,0);
    		$client->Message(5,"You are currently receiving $teamSpellText due to team control of $locationText.");
	}
	return 1;
    }
	return 0;
}

sub HandleLocationBonuses {
    my $timer    = shift;
    my $client   = plugin::val('$client');
    my $qglobals = plugin::var('qglobals');
    my $zoneid = plugin::val('$zoneid');

    #bandits get NOTHING, neutral players get NOTHING or if the client is casting we wait
    if ( $client->IsCasting() || $client->GetPVPFlag() == 0 || $client->GetPVPFlag() == 5 )
    {
      return;
    }

    $matchAnyOwnedCities = 0; #count if this player belongs to any owned cities
    $customCitiesOwned = 0; #set to 1 if there are any cities owned

    $grobbOwnerTeam = quest::getguardfield(20,0,"ownerteamid");
    $oggokOwnerTeam = quest::getguardfield(19,0,"ownerteamid");
    $kaladimOwnerTeam = quest::getguardfield(15,0,"ownerteamid");
    $neriakOwnerTeam = quest::getguardfield(14,0,"ownerteamid");
    $erudinOwnerTeam = quest::getguardfield(8,0,"ownerteamid");
    $felwitheOwnerTeam = quest::getguardfield(7,0,"ownerteamid");

    if ( !defined $timer )
    {
	quest::settimer("locationbonus",1);
    }
    elsif ( $timer eq "locationbonus" )
    {
    if ( $grobbOwnerTeam > 0 && $client->GetPVPFlag() == $grobbOwnerTeam )
    {
    	if ( defined $qglobals->{locationbonus_grobb} )
    	{
		#cooldown
	}
	else
	{
		if ( $zoneid == 151 || $zoneid == 155 || $zoneid == 152 ) {
			quest::setglobal( "locationbonus_grobb", "1", 5, "M10" );
    			$client->Message(5,"You are currently in a safe zone and do not receive the 5 platinum bonus from team control of Grobb.");
		}
		else
		{
			$plat = 5;
			$client->AddMoneyToPP(0,0,0,$plat,1);
			$matchAnyOwnedCities += 1;
			quest::setglobal( "locationbonus_grobb", "1", 5, "M10" );
    			$client->Message(5,"You are currently receiving $plat platinum due to team control of Grobb.");
		}
	}
    }
    $matchAnyOwnedCities += plugin::CheckLocationBonus($oggokOwnerTeam, 1776, "Oggok", "Spirit of Wolf");
    $matchAnyOwnedCities += plugin::CheckLocationBonus($felwitheOwnerTeam, 2887, "Felwithe", "Mask of the Stalker");
    #enduring breath = 86

    $cureAttempt = 0;


    if ( defined $qglobals->{locationbonus_cures} )
    {
	#cooldown
    }
    else
    {
	$cureCooldown = "S10"; #10 seconds
	$locationBonusCureFlag = "locationbonus_cures";
    #slot type 20 = blindness
    if ( $client->GetBuffSlotFromType(20) > -1 )
    {
	    $cureAttempt += plugin::CheckLocationBonus($kaladimOwnerTeam, 212, "Kaladim", "Cure Blindness");
	    if ( $cureAttempt > 0 ) { quest::setglobal( $locationBonusCureFlag, "1", 5, $cureCooldown ); }
    }
    #slot type 35 = disease
    if ( $cureAttempt == 0 && $client->GetBuffSlotFromType(35) > -1 && $client->FindBuff(871) == 0)
    {
		if ( $client->GetLevel() > 39 )
		{
		    $cureAttempt += plugin::CheckLocationBonus($erudinOwnerTeam, 96, "Neriak", "Counteract Disease");
		}
		else
		{
		    $cureAttempt += plugin::CheckLocationBonus($neriakOwnerTeam, 213, "Neriak", "Cure Disease");
		}
	    if ( $cureAttempt > 0 ) { quest::setglobal( $locationBonusCureFlag, "1", 5, $cureCooldown ); }
    }
    #slot type 36 = poison
    if ( $cureAttempt == 0 && $client->GetBuffSlotFromType(36) > -1 )
    {
		if ( $client->GetLevel() > 39 )
		{
		    $cureAttempt += plugin::CheckLocationBonus($erudinOwnerTeam, 95, "Erudin", "Counteract Poison");
		}
		else
		{
		    $cureAttempt += plugin::CheckLocationBonus($erudinOwnerTeam, 203, "Erudin", "Cure Poison");
		}
	    if ( $cureAttempt > 0 ) { quest::setglobal( $locationBonusCureFlag, "1", 5, $cureCooldown ); }
    }
    } #end else statement for locationbonus_cures

    $matchAnyOwnedCities += $cureAttempt;

    # if any locations are owned then we want to flip the flag to note there is competition
    if ( $oggokOwnerTeam > 0 || $grobbOwnerTeam > 0 || $kaladimOwnerTeam > 0 || $neriakOwnerTeam > 0 || $erudinOwnerTeam > 0 || $felwitheOwnerTeam > 0)
    {
	$customCitiesOwned = 1;
    }


    if ( $customCitiesOwned > 0 && $matchAnyOwnedCities == 0 && $client->GetPVPFlag() > 0 )
    {
	if ( defined $qglobals->{locationbonus_losers} || $client->GetHPRatio() >= 100)
	{
	#cooldown since they have received it recently or their hp ratio is 100%
	}
	else
	{
		quest::setglobal( "locationbonus_losers", "1", 5, "M1" );

		$levelSpell = 200;#minor healing
		$dispText = "Minor Healing";
		if ( $client->GetLevel() > 39 )
		{
			$levelSpell = 17;#light healing
			$dispText = "Light Healing";
		}
    		$client->CastSpell($levelSpell,$client->GetID(),10,0,0);
    		$client->Message(5,"You are currently receiving $dispText due to being on the underdog team.");
	}
	if ( defined $qglobals->{locationbonus_losers2} || $client->GetManaRatio() >= 100)
	{
	#cooldown since they have received it recently or their mana ratio is 100%
	}
	else
	{
		quest::setglobal( "locationbonus_losers2", "1", 5, "M1" );

		$levelSpell = 723;#Cassindra's Chorus of Clarity
		$dispText = "Cassindra's Chorus of Clarity";
    		$client->CastSpell($levelSpell,$client->GetID(),10,0,0);
    		$client->Message(5,"You are currently receiving $dispText due to being on the underdog team.");
	}
    }#end of locationbonus for losers

   }#end elsif of $timer eq "locationbonus"


}
