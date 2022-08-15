sub EVENT_SAY {

if ( defined($qglobals{akanon_race_playerflag}) )
{
quest::say("You are currently participating in an event and cannot use the teleporter.");
return;
}

if ($text=~/Hail/i){
$saylink = quest::saylink("journey to South Ro");
$saylink2 = quest::saylink("journey to Toxxulia Forest");
$saylink3 = quest::saylink("journey to Innothule Swamp");
quest::say("Hello $name. I do not have much time to chat. I must concentrate my energy to help others with their travel.  If you wish to $saylink or $saylink2 or $saylink3, tell me so."); 
}
elsif ($text=~/journey to Innothule Swamp/i){
$nexusLeaveIncentive = $qglobals{nexusleaveincentive};
$nexusleavecooldown = $qglobals{nexusleavecooldown};

if ( $nexusLeaveIncentive == 1 && $nexusleavecooldown != 1)
{
plugin::nexusbuff_enter($client);
}

quest::say("Good luck $name!");
quest::movepc(46,-12.2, -736.5, -24.0, 7.9);
$client->CastSpell(207,$userid,10,0,0);
}
elsif ($text=~/journey to South Ro/i){
$nexusLeaveIncentive = $qglobals{nexusleaveincentive};
$nexusleavecooldown = $qglobals{nexusleavecooldown};

if ( $nexusLeaveIncentive == 1 && $nexusleavecooldown != 1)
{
plugin::nexusbuff_enter($client);
}

quest::say("Good luck $name!");
quest::movepc(35,317.0, -2034.0, -23.9, 10.0);
$client->CastSpell(207,$userid,10,0,0);
}
elsif ($text=~/journey to Toxxulia Forest/i){
$nexusLeaveIncentive = $qglobals{nexusleaveincentive};
$nexusleavecooldown = $qglobals{nexusleavecooldown};

if ( $nexusLeaveIncentive == 1 && $nexusleavecooldown != 1)
{
plugin::nexusbuff_enter($client);
}

quest::say("Good luck $name!");
quest::movepc(38,-916.0,-1510.0,-37.9,10.0);
$client->CastSpell(207,$userid,10,0,0);
}
}