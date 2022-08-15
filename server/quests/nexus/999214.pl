sub EVENT_SAY {

if ( defined($qglobals{akanon_race_playerflag}) )
{
quest::say("You are currently participating in an event and cannot use the teleporter.");
return;
}

if ($text=~/Hail/i){
$saylink = quest::saylink("journey to West Commonlands");
$saylink2 = quest::saylink("journey to Nek");
quest::say("Hello $name. I do not have much time to chat. I must concentrate my energy to help others with their travel.  If you wish to $saylink or $saylink2 tell me so."); 
}
elsif ($text=~/journey to West Commonlands/i){
$nexusLeaveIncentive = $qglobals{nexusleaveincentive};
$nexusleavecooldown = $qglobals{nexusleavecooldown};

if ( $nexusLeaveIncentive == 1 && $nexusleavecooldown != 1)
{
plugin::nexusbuff_enter($client);
}

quest::say("Good luck $name!");
quest::movepc(21,1427.0,478.0,-52.6,69.0);
$client->CastSpell(207,$userid,10,0,0);
}
elsif ($text=~/journey to Nek/i){
$nexusLeaveIncentive = $qglobals{nexusleaveincentive};
$nexusleavecooldown = $qglobals{nexusleavecooldown};

if ( $nexusLeaveIncentive == 1 && $nexusleavecooldown != 1)
{
plugin::nexusbuff_enter($client);
}

quest::say("Good luck $name!");
quest::movepc(25,-772,432,33,0);
$client->CastSpell(207,$userid,10,0,0);
}
}