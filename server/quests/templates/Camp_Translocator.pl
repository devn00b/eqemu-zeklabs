my $teamid = 0;

	sub EVENT_SAY { 
	$flag = $qglobals{teleportflag};
	$teamid = quest::findguardteamid($npc->GetNPCTypeID(),$npc->GetSp2());

	if ( $teamid > 0 )
	{
		plugin::citytranslocator_say($npc,$teamid,$client,$text,$flag,$zonemin);
	}
}

	sub EVENT_POPUPRESPONSE
	{
	$teamid = quest::findguardteamid($npc->GetNPCTypeID(),$npc->GetSp2());
	if ( $teamid > 0 )
	{
	plugin::bankpopup_response($teamid,$client,$popupid);
	}

} #close event response

	sub EVENT_SPAWN
	{
	$teamid = quest::findguardteamid($npc->GetNPCTypeID(),$npc->GetSp2());

	if ( $teamid > 0 )
	{
		plugin::setupguard_enter($teamid,$npc);
	}
}


	sub EVENT_ENTER {
		$teamid = quest::findguardteamid($npc->GetNPCTypeID(),$npc->GetSp2());

	if ( $teamid > 0 )
	{
	plugin::enterguard_area($teamid,$client,$npc);
	}
} #end of sub event_enter

	sub EVENT_DEATH
	{

		$teamid = quest::findguardteamid($npc->GetNPCTypeID(),$npc->GetSp2());

	if ( $teamid > 0 )
	{
	$allDead = quest::resetguardfields($teamid,$npc->GetNPCTypeID(), $userid);
	if ( $allDead == 1 )
	{
		plugin::campdestroyed($teamid,$npc,$client);
	}
	}
} #if charid and pvpflag
