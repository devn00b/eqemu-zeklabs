sub EVENT_SAY
{
$nexusLeaveIncentive = $qglobals{nexusleaveincentive};
$nexusleavecooldown = $qglobals{nexusleavecooldown};

if ( $nexusLeaveIncentive == 1 && $nexusleavecooldown != 1)
{
$client->AddPVPPoints(1);
quest::setglobal("nexusprobationperiod","1",5,"M5");
quest::setglobal("nexusleavecooldown","1",5,"M30");
$client->Message(15,"***You have been awarded a PVP point for leaving Nexus during the incentive period.");
}
quest::say("Nexus: $val");
}