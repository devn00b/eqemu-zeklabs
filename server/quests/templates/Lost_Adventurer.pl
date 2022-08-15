use Date::Parse;
my @charListArray = ();
my @groupListArray = ();

my $maxInstanceCount = 5;

sub EVENT_SPAWN
{
	quest::settimer("rebuildlist",30);
}

sub EVENT_SAY
{
	if($text=~/hail/i) {
		if ( defined $client->GetGroup() )
		{
			$isPlayerOnCharList = plugin::hasArenaPlayer(\@charListArray, $client->CharacterID());
			$isPlayerOn = plugin::hasArenaPlayer(\@groupListArray, $client->CharacterID());
			$isGroupPlayerOn = plugin::isPlayerInGroupQueued(\@groupListArray, $client);
			$isSinglePlayerOn = plugin::isPlayerInGroupQueued(\@charListArray, $client);
			if ( $isGroupPlayerOn == 1 || $isSinglePlayerOn == 1 )
			{
				quest::say("One of your group members is already queued.");
			}
			elsif ( $isPlayerOnCharList == 1 )
			{
				quest::say("$name you are already in the queue as a single player (not as a group).");
			}
			elsif ( $isPlayerOn == 0 )
			{
				$link = quest::saylink("accept the group challenge");
				quest::say("Do you wish to enter a group vs group challenge? Do you $link");
			}
			else
			{
				$link = quest::saylink("decline the group challenge");
				quest::say("Are you abandoning your group challenge? Ask to $link");
			}
		}
		else
		{
			$isPlayerOn = plugin::hasArenaPlayer(\@charListArray, $client->CharacterID());
			$isGroupPlayerOn = plugin::isPlayerInGroupQueued(\@groupListArray, $client);
			if ( $isGroupPlayerOn == 1 )
			{
				quest::say("One of your group members is already queued.");
			}
			elsif ( $isPlayerOn == 0 )
			{
				$link = quest::saylink("accept the challenge");
				quest::say("Do you wish to enter a 1v1 challenge? Do you $link");
			}
			else
			{
				$link = quest::saylink("decline the challenge");
				quest::say("Are you abandoning your challenge? Ask to $link");
			}
		}
	}
	elsif($text=~/accept the challenge/i) {
		if ( defined $client->GetGroup() )
		{
			$client->Message(13,"You are in a group, leave first before joining the queue.");
			return;
		}
		$isPlayerOn = plugin::hasArenaPlayer(\@charListArray, $client->CharacterID());
		if ( $isPlayerOn == 0 )
		{
			if ( defined $qglobals{arena_instance_disable} )
			{
			$client->Message(13,"You are currently on cooldown from instanced matches at this time, max instances allowed $maxInstanceCount over a 6 hour period.");
			return;
			}
			else
			{
				$count = $qglobals{arena_instance_count};
				$count += 1;
				quest::setglobal("arena_instance_count",$count,5,"H6");
				if ( $count >= $maxInstanceCount )
				{
					$client->Message(13,"You have reached the maximum times you can enter an instanced arena match ($maxInstanceCount), you will now be on a 6 hour cooldown (if you decline you will not be able to re-enter).");
					quest::setglobal("arena_instance_disable","1",5,"H6");
				}
				else
				{
					$client->Message(13,"Accepted into queue, match number $count of $maxInstanceCount allowed in a 6 hour period.");
				}
			}
		my @thisid = ( $client->CharacterID() );

		@charListArray = &plugin::addArenaPlayer(\@charListArray,\@thisid);
		quest::say("You have been added to the list!");
		}
		else
		{
		quest::say("You are already present on the list!");
		}
	}
	elsif($text=~/decline the challenge/i) {
		$isPlayerOn = plugin::hasArenaPlayer(\@charListArray, $client->CharacterID());
		if ( $isPlayerOn == 1 )
		{
		my @thisid = ( $client->CharacterID() );

		@charListArray = &plugin::removeArenaPlayer(\@charListArray,$client->CharacterID());
		quest::say("You have been removed to the list!");
		}
		else
		{
		quest::say("You are not present on the list!");
		}
	}
	elsif($text=~/accept the group challenge/i && defined $client->GetGroup()) {
		$isPlayerOn = plugin::hasArenaPlayer(\@groupListArray, $client->CharacterID());
		if ( $isPlayerOn == 0 )
		{
		my @thisid = ( $client->CharacterID() );
		my $disabledName = plugin::getGroupInstanceStatus($client->GetGroup());
			if ( length $disabledName > 1 )
			{
			$client->Message(13,"You cannot queue up at this time as a group member is disabled from instance access, $disabledName.");
			return;
			}
			else
			{
				plugin::updateGroupInstanceStatus($client->GetGroup(),$maxInstanceCount);
			}
		@groupListArray = &plugin::addArenaPlayer(\@groupListArray,\@thisid);
		quest::say("You have been added to the list!");
		}
		else
		{
		quest::say("You are already present on the list!");
		}
	}
	elsif($text=~/decline the group challenge/i && defined $client->GetGroup()) {
		$isPlayerOn = plugin::hasArenaPlayer(\@groupListArray, $client->CharacterID());
		if ( $isPlayerOn == 1 )
		{
		my @thisid = ( $client->CharacterID() );

		@groupListArray = &plugin::removeArenaPlayer(\@groupListArray,$client->CharacterID());
		quest::say("You have been removed to the list!");
		}
		else
		{
		quest::say("You are not present on the list!");
		}
	}
	elsif($text=~/list/i && $client->GetGM() == 1)
	{
		@list = plugin::getArenaList(\@charListArray,$entity_list);
		quest::say("@list");
		$availzones = quest::getavailzonecount();
		$staticzones = quest::getstaticzonecount();
		$bootedzones = quest::getbootedzonecount();
		quest::say("Zones Available: $availzones, Static: $staticzones, Booted: $bootedzones");
	}
	elsif($text=~/groups/i && $client->GetGM() == 1)
	{
		@list = plugin::getArenaList(\@groupListArray,$entity_list);
		quest::say("@list");
	}
	#elsif($text=~/details/i)
	#{
	#	quest::say("$zoneid $instanceid");
	#}
	#elsif($text=~/instance/i)
	#{
	#	$instid = $client->CreateInstance("arena",1,300);
	#	$client->AddToInstance($instid); 
	#	$client->MovePCInstance(77,$instid,0,0,0,0);
	#}

	#$lastid = plugin::getArenaPlayer(\@charListArray);
	#quest::say("last id: $lastid");
	#quest::say("@charListArray - count $amt");

	#$bindid = $client->GetBindZoneID();
	#quest::say("BindZone: $bindid");
	#$alcohol = $client->GetIntoxication();
	#quest::say("Tox: $alcohol");
	#if ( $client->GetGM() )
	#{
		#$zoneFlag = $qglobals{"worldeventzoneflag"};
		#$worldNPC = $qglobals{"worldeventmobcooldown"};
		#$npcID = $qglobals{"worldeventnpcspawned"};
		#$eventCooldown = $qglobals{"worldeventcooldown"};

		#quest::say("WorldNPCID: $npcID, NPC cooldown: $worldNPC, zoneID: $zoneFlag, event cooldown: $eventCooldown");
		#quest::setglobal("worldeventmobcooldown","0",7,"S1");
		#quest::setglobal("worldeventnpcspawned","0",7,"S1");
		#quest::setglobal("testcharflag","someval",5,"M5");
	#}
	
}

sub EVENT_TIMER
{
	if ( $timer eq "rebuildlist")
	{
		$availzones = quest::getavailzonecount("test");
		$skipAssignments = 0;
		if ( $availzones < 6 )
		{
			$skipAssignments = 1;
			quest::ze(13, "[1v1/Group Arena Instances] Available instances is below threshold ($availzones left), new matches must wait until instances are available.");
		}

		$zonesAssigned = 0;

		@charListArray = &plugin::rebuildArenaList(\@charListArray,$entity_list);
		$count = scalar @charListArray;
		if ( $count > 0 && $skipAssignments == 0 )
		{
			for($i=0;$i<$count;$i++)
			{
				if ( $zonesAssigned > $availzones )
				{
					break;
				}
				my $front = @charListArray[$i];
				#quest::say("Front: $front");
				my $opponent = plugin::findRandomArenaOpponent(\@charListArray,$entity_list,$front);
				if ( $opponent > 0 )
				{
					$zonesAssigned += 1;
					@charListArray = &plugin::removeArenaPlayer(\@charListArray,$front);
					@charListArray = &plugin::removeArenaPlayer(\@charListArray,$opponent);
					$i = 0;
				}
				else
				{
					$opponent = plugin::findArenaOpponent(\@charListArray,$entity_list,$front);
					if ( $opponent > 0 )
					{
						$zonesAssigned += 1;
						@charListArray = &plugin::removeArenaPlayer(\@charListArray,$front);
						@charListArray = &plugin::removeArenaPlayer(\@charListArray,$opponent);
						$i = 0;
					}
				}
				$count = scalar @charListArray;
			}
		}
		
		@groupListArray = &plugin::rebuildArenaList(\@groupListArray,$entity_list);
		$count = scalar @groupListArray;
		if ( $count > 0 && $skipAssignments == 0 )
		{
			for($i=0;$i<$count;$i++)
			{
				if ( $zonesAssigned > $availzones )
				{
					break;
				}
				my $front = @groupListArray[$i];
				#quest::say("Front: $front");
				$opponent = plugin::findGroupArenaOpponent(\@groupListArray,$entity_list,$front);
				if ( $opponent > 0 )
				{
					$zonesAssigned += 1;
					@groupListArray = &plugin::removeArenaPlayer(\@groupListArray,$front);
					@groupListArray = &plugin::removeArenaPlayer(\@groupListArray,$opponent);
					$i = 0;
				}
				$count = scalar @groupListArray;
			}
		}
	}
}