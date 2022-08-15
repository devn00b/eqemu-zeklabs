#Devnoob: not sure what was up with this quest file, but was summoning mage armor.
#And wasnt porting correctly to faydark...both fixed.
sub EVENT_SAY {

if ( defined($qglobals{akanon_race_playerflag}) )
{
quest::say("You are currently participating in an event and cannot use the teleporter.");
return;
}

if ($text=~/Hail/i){
$saylink = quest::saylink("journey to Greater Faydark");
$saylink2 = quest::saylink("journey to Butcher Block");
$saylink3 = quest::saylink("journey to Everfrost");
quest::say("Hello $name. I do not have much time to chat. I must concentrate my energy to help others with their travel.  If you wish to $saylink or $saylink2 or $saylink3, tell me so."); 
}
elsif ($text=~/journey to Everfrost/i){

$nexusLeaveIncentive = $qglobals{nexusleaveincentive};
$nexusleavecooldown = $qglobals{nexusleavecooldown};

if ( $nexusLeaveIncentive == 1 && $nexusleavecooldown != 1)
{
plugin::nexusbuff_enter($client);
}

quest::say("Good luck $name!");
quest::movepc(30,-1962.9, 1643.9, 46.0, 0.0);
#quest::selfcast(6214);
$client->CastSpell(207,$userid,10,0,0);
}
elsif ($text=~/journey to Greater Faydark/i){

$nexusLeaveIncentive = $qglobals{nexusleaveincentive};
$nexusleavecooldown = $qglobals{nexusleavecooldown};

if ( $nexusLeaveIncentive == 1 && $nexusleavecooldown != 1)
{
plugin::nexusbuff_enter($client);
}

quest::say("Good luck $name!");
quest::movepc(54,-441.0,-2023.0,-4.8,222.0);
#quest::selfcast(6214);
$client->CastSpell(207,$userid,10,0,0);
}
elsif ($text=~/journey to Butcher Block/i){
$nexusLeaveIncentive = $qglobals{nexusleaveincentive};
$nexusleavecooldown = $qglobals{nexusleavecooldown};

if ( $nexusLeaveIncentive == 1 && $nexusleavecooldown != 1)
{
plugin::nexusbuff_enter($client);
}

quest::say("Good luck $name!");
quest::movepc(68,1984.0,-2135.0,-1.0,58.0);
$client->CastSpell(207,$userid,10,0,0);
}
}