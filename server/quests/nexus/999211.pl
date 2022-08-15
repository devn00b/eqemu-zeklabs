sub EVENT_SAY {
if ( defined($qglobals{akanon_race_playerflag}) )
{
quest::say("You are currently participating in an event and cannot use the teleporter.");
return;
}

if ($text=~/Hail/i){
$saylink = quest::saylink("journey to North Karana");
$saylink2 = quest::saylink("journey to Surefall Glade");
$saylink3 = quest::saylink("journey to Lake Rathe");
quest::say("Hello $name. I do not have much time to chat. I must concentrate my energy to help others with their travel.  If you wish to $saylink , $saylink2 or $saylink3 tell me so."); 
}
elsif ($text=~/journey to North Karana/i){
$nexusLeaveIncentive = $qglobals{nexusleaveincentive};
$nexusleavecooldown = $qglobals{nexusleavecooldown};

if ( $nexusLeaveIncentive == 1 && $nexusleavecooldown != 1)
{
plugin::nexusbuff_enter($client);
}

quest::say("Good luck $name!");
quest::movepc(13,1209.0,-3685.0,-9.2,167.0);
$client->CastSpell(207,$userid,10,0,0);
}
elsif ($text=~/journey to Surefall Glade/i){
$nexusLeaveIncentive = $qglobals{nexusleaveincentive};
$nexusleavecooldown = $qglobals{nexusleavecooldown};

if ( $nexusLeaveIncentive == 1 && $nexusleavecooldown != 1)
{
plugin::nexusbuff_enter($client);
}

quest::say("Good luck $name!");
quest::movepc(3,-391,-209,4.8,192.0);
$client->CastSpell(207,$userid,10,0,0);
}
#devn00b add arena via lake rathe

elsif ($text=~/journey to Lake Rathe/i){
$nexusLeaveIncentive = $qglobals{nexusleaveincentive};
$nexusleavecooldown = $qglobals{nexusleavecooldown};

if ( $nexusLeaveIncentive == 1 && $nexusleavecooldown != 1)
{
plugin::nexusbuff_enter($client);
}

quest::say("Good luck $name!");
quest::movepc(51,1178.5,2586.0,-15.8,200.5);
#$client->CastSpell(207,$userid,10,0,0);
}




}
