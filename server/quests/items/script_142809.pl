sub EVENT_ITEM_CLICK_CAST {
	$target = $client->GetTarget();
	if ( defined $target && $target->IsNPC() )
	{
		$npc = $target->CastToNPC();
		$teamid = quest::findguardteamid($npc->GetNPCTypeID(),$npc->GetSp2());
		if ( $teamid > 0 )
		{
			$citymode = quest::getcitymode($teamid);
			if ( $npc->IsEngaged() == 1 )
			{
		      		    $client->CastSpell(8558, 0, 10, 0, 0); 
				$client->Message(14,"This NPC is in combat and cannot be moved at this time.");
				return;
			}
			if ( $citymode != 96 )
			{
		      		    $client->CastSpell(8558, 0, 10, 0, 0); 
				$client->Message(14,"This is not a constructed guild camp guard.");
				return;
			}
			$teamx = quest::getteamlocation($teamid,"x");
			$teamy = quest::getteamlocation($teamid,"y");
			$teamz = quest::getteamlocation($teamid,"z");
			$diameter = 200;
			$newDist = $client->CalculateDistance($teamx,$teamy,$teamz);
			if ( $newDist > $diameter )
			{
				    $client->CastSpell(8558, 0, 10, 0, 0); 
				$client->Message(14,"This extends beyond your camp diameter of $diameter, you are currently $newDist units away.");
				return;
			}

			$guildid = quest::getguardfield($teamid,$npc->GetNPCTypeID(),"guildid");
			if ( $guildid > 0 && $guildid == $client->GuildID() )
			{
				$bestZ = quest::GetBestZ($client->GetX(),$client->GetY(),$client->GetZ());
				if ( $bestZ < -999000 )
				{
				    $client->CastSpell(8558, 0, 10, 0, 0); 
				$client->Message(14,"Could not determine new Z coordinate.");
				return;
				}
				$npcNear = quest::findnpcnear($client->GetX(),$client->GetY(),$bestZ,5);
				if ( $npcNear )
				{
				    $client->CastSpell(8558, 0, 10, 0, 0); 
				$client->Message(14,"There is another NPC near this one, try giving some space!");
				return;
				}
				$success = $npc->SpawnFix($client->GetX(),$client->GetY(),$bestZ,$client->GetHeading());
				if ( $success == 1 )
				{
				$npc->Depop();
				$npc->Repop(0);
				$client->Message(14,"Placed guard at your location.");
				}
				else
				{
				$client->Message(14,"Failed guard placement.");
				}
			}
			else
			{
				$client->Message(14,"This guard does not belong to your guild.");
			}
		}
	}
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
}