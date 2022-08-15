sub EVENT_DEATH
{
plugin::handleArenaDeath($client,$killedby_charid,$legitkill,$zoneid,$instanceid,$entity_list);
plugin::dropBall($client,$charid,1);
}

sub EVENT_ZONE
{
#quest::ze(13,"$name zoned out.");
plugin::dropBall($client,$charid,1);
quest::delglobal("arena_hill_allowed");
}

sub EVENT_ENTERZONE
{
quest::delglobal("arena_ball_decay");
quest::delglobal("arena_ball_decay_enabled");
quest::settimer("earnpt",60);
if ( defined $qglobals{arena_ball_disable} )
{
$client->Message(13,"You are currently on cooldown from King of the Hill at this time.");
}
else
{
quest::setglobal("arena_hill_allowed","1",5,"F");
}

plugin::arePlayersReady($entity_list,$client,$instanceid,$zoneid);

$approved = $qglobals{popup_allowautoscribe};

if ( !defined $approved )
{
quest::popup("Auto Scribe and Skills","Auto scribe and auto skill ups is currently ENABLED by default.  Click 'Yes' to DISABLE this feature.  See Heimdall in Nexus for more information about managing auto scribe and skill-ups or manually triggering the feature.  Ignoring this prompt will leave the auto scribe and skill feature enabled.",5555,120);
quest::setglobal("popup_allowautoscribe",1,5,"F");
}
}

sub EVENT_LOGOUT
{
#quest::ze(13,"$name logged out.");
plugin::dropBall($client,$charid,1);
quest::delglobal("arena_hill_allowed");

plugin::handleArenaLogout($client,$instanceid,$zoneid);
}

sub EVENT_PLAYER_PICKUP
{
if ( $client->GetGM() == 1 )
{
#return;
}
if ( $picked_up_id == 52760 )
{

if ( defined $qglobals{arena_ball_disable} )
{
$client->Message(13,"You are currently on cooldown from King of the Hill at this time.");
plugin::dropBall($client,$charid,1);
quest::delglobal("arena_hill_allowed");
return;
}

quest::signalwith(999639,0,0);
quest::delglobal("arena_ball_decay");
quest::delglobal("arena_ball_decay_enabled");
quest::setglobal("arena_ball_owner",$charid,7,"M30");
quest::setglobal("arena_ball_ownername",$name,7,"M30");
if ( !defined $qglobals{arena_ball_count} )
{
quest::setglobal("arena_ball_count","0",5,"H6");
}

quest::we(13,"[Arena] $name has the ball!");
}
}

sub EVENT_TIMER
{
if ( $timer eq "earnpt" )
{
	$charidball = $qglobals{arena_ball_owner};
	if ( $charidball > 0 && $charid == $charidball )
	{
		#my @clist = $entity_list->GetClientList();
		#$count = scalar @clist;

		$onhill = $qglobals{arena_onhill};

		if ( (!defined $onhill || $onhill != "1") && defined $qglobals{arena_ball_decay_enabled} && $qglobals{arena_ball_decay_enabled} == "1" )
		{

		if ( !defined $qglobals{arena_ball_decay} )
		{
		plugin::dropBall($client,$charid,0);
		quest::delglobal("arena_hill_allowed");
		return;
		}
		}

		if ( !defined $onhill || $onhill != "1" )
		{
		quest::we(13,"[Arena] $name has the ball!");
		if ( !defined $qglobals{arena_ball_decay} && !defined $qglobals{arena_ball_decay_enabled} )
		{
		quest::setglobal("arena_ball_decay",$charid,5,"M3");
		quest::setglobal("arena_ball_decay_enabled","1",5,"F");
		}
		$client->Message(13,"You did not receive a ticket as you are not on the hill in the center of the arena!  You are given no more than 3 minutes to return to the platform!");
		return;
		}

		if ( !defined $qglobals{arena_ball_announcement} || $qglobals{arena_ball_announcement} != $charid ){
		quest::we(13,"[Arena] $name has the ball!");
		quest::delglobal("arena_ball_decay");
		quest::delglobal("arena_ball_decay_enabled");
		quest::setglobal("arena_ball_announcement",$charid,5,"M5");
			if ( $client->GetLevel() > 44 )
			{
			$client->PutItemInOpenSlot(142842);
			$client->Message(15,"[Arena] You receive a Palladium PVP Ticket!");
			}
			else
			{
			$client->PutItemInOpenSlot(132487);
			$client->Message(15,"[Arena] You receive a Golden PVP Ticket!");
			}
		}
		else
		{
			$client->PutItemInOpenSlot(132487);
			$client->Message(15,"[Arena] You receive a Golden PVP Ticket!");
		}
		
	$count = $qglobals{arena_ball_count};
	$count += 1;
	quest::setglobal("arena_ball_count",$count,5,"H6");
	if ( $count > 20 )
	{
	$client->Message(13,"You have received the maximum tickets allowed for King of the Hill, you are now on a 6 hour cooldown beginning now!");
	quest::setglobal("arena_ball_disable","1",5,"H6");
	plugin::dropBall($client,$charid,2);
	quest::delglobal("arena_hill_allowed");
	}
	else
	{
	$client->Message(13,"You have received $count of your 20 tickets at which time will require a 6 hour cooldown.");
	return;
	}
	} #char id match

	if ( !defined $qglobals{arena_hill_allowed} && !defined $qglobals{arena_ball_disable} )
	{
	$client->Message(13,"You can now pick up the arena ball again.");
	quest::setglobal("arena_hill_allowed","1",5,"F");
	}
	elsif ( defined $qglobals{arena_ball_disable} )
	{
	$client->Message(13,"You are currently on cooldown from becoming King of the Hill.");
	}
}#end timer earnpt
}#end event_timer
