#sub EVENT_SPAWN
#{
#quest::settimer("runtime",5);
#}

sub EVENT_SAY
{
$zoneFlag = $qglobals{"worldeventzoneflag"};
$worldNPC = $qglobals{"worldeventmobcooldown"};
$npcID = $qglobals{"worldeventnpcspawned"};
$eventCooldown = $qglobals{"worldeventcooldown"};
#quest::setglobal("worldeventmobcooldown","0",7,"S1");
quest::say("$localTime WorldNPCID: $npcID, NPC cooldown: $worldNPC, zoneID: $zoneFlag, event cooldown: $eventCooldown");
}

#sub EVENT_TIMER
#{
#$localTime = localtime();
#my $curMaxLevel = quest::GetRule("Character:MaxLevel");
#  if($curMaxLevel == "50" && index($localTime, "Thu Jan  3 09:28") != -1 ) {
#	quest::say("Current Level: $curMaxLevel");
#	quest::SetRule("Character:MaxLevel","60");
#  }
#quest::say("Current Time: $localTime");
#}
