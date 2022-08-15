sub EVENT_SAY { 
if ( defined($qglobals{akanon_race_playerflag}) )
{
quest::say("You are currently participating in an event and cannot use the teleporter.");
return;
}

$group = $client->GetGroup();
$groupid = 0;
$groupPick = 0;

$guildnum = $client->GuildID();

$charGlobalName = "charSpawn" . $client->CharacterID();
$guildGlobalName = "guildSpawn" . $guildnum;

if ( defined $group )
{
$groupid = $group->GetID();
$groupGlobalName = "groupSpawn" . $groupid;

if ( defined $qglobals{$groupGlobalName} ) {
$groupPick = $qglobals{$groupGlobalName};
}
else
{
$getnewloc = quest::ChooseRandom(1,2,3,4,5,6,7,8,9,10);
quest::setglobal($groupGlobalName,$getnewloc,7,"H6");
$groupPick = $getnewloc;
if ( $guildnum < 10000 && undef $qglobals{$guildGlobalName} ) {
quest::setglobal($guildGlobalName,$getnewloc,7,"H6");
}
}
}
else
{
if ( $guildnum < 10000 )
{
if ( defined $qglobals{$guildGlobalName} ) {
$groupPick = $qglobals{$guildGlobalName};
}
else
{
$getnewloc = quest::ChooseRandom(1,2,3,4,5,6,7,8,9,10);
quest::setglobal($guildGlobalName,$getnewloc,7,"H6");
$groupPick = $getnewloc;
}
}
else
{
#no guild
if ( defined $qglobals{$charGlobalName} ) {
$groupPick = $qglobals{$charGlobalName};
}
else
{
$getnewloc = quest::ChooseRandom(1,2,3,4,5,6,7,8,9,10);
quest::setglobal($charGlobalName,$getnewloc,5,"H6");
$groupPick = $getnewloc;
}
}
}



if ( $client->GetPVPFlag() < 1 )
{
quest::say("Hello $name.  You must join a team before you can enter a pvp battleground.");
return;
}
elsif($text=~/Hail/i){
#$bg1 = quest::saylink("The Desert Highlands");
$arena = quest::saylink("The Arena");
$warrens = quest::saylink("The Warrens");

$groupflag = quest::saylink("remove group flag");
$guildflag= quest::saylink("remove guild flag");
$charflag = quest::saylink("remove char flag");
quest::say("Hello $name!  There is a PVP Free For All battleground, $arena available.  A PVE/PVP dungeon -- ENTER AT YOUR OWN RISK -- $warrens.  You may also $groupflag or $guildflag or $charflag.");
#There is a PVP team battleground at $bg1.
}
#elsif($text=~/The Desert Highlands/i){
#$client->MovePC(168,-1668,540,-6,0); #mseru
#}
elsif($text=~/remove group flag/i){
quest::delglobal($groupGlobalName);
quest::say("$name your group flag has been removed.");
}
elsif($text=~/remove guild flag/i){
quest::delglobal($guildGlobalName);
quest::say("$name your guild flag has been removed.");
}
elsif($text=~/remove char flag/i){
quest::delglobal($charGlobalName);
quest::say("$name your char flag has been removed.");
}
elsif($text=~/The Arena/i){
$nexusLeaveIncentive = $qglobals{nexusleaveincentive};
$nexusleavecooldown = $qglobals{nexusleavecooldown};

if ( $nexusLeaveIncentive == 1 && $nexusleavecooldown != 1)
{
$client->AddPVPPoints(1);
quest::setglobal("nexusprobationperiod","1",5,"M5");
quest::setglobal("nexusleavecooldown","1",5,"M30");
$client->Message(15,"***You have been awarded a PVP point for leaving Nexus during the incentive period.");
}

$guildid = $client->GuildID();
if ( $guildid > 512 )
{
$guildid = 0;
}

$locTime = 0;
if ( $zonehour < 4 )
{
$locTime = 1;
}
elsif( $zonehour < 8 )
{
$locTime = 2;
}
elsif( $zonehour < 12 )
{
$locTime = 3;
}
elsif( $zonehour < 16 )
{
$locTime = 4;
}
elsif( $zonehour < 20 )
{
$locTime = 5;
}
elsif( $zonehour < 25 )
{
$locTime = 6;
}
else
{
$locTime = 7;
}

$guildComp = $guildid % 9;
$guildComp += $locTime;
$getnewloc = $guildComp;

#$getnewloc = quest::ChooseRandom(1,2,3,4,5,6,7,8,9);
quest::selfcast("207"); #Kings protection for pvp zones - divine aura

if ( $getnewloc == 1 ) {
$client->MovePC(77,1167.8,-43.5,1.9,193.9); #arena
}
elsif ( $getnewloc == 2 ) {
$client->MovePC(77,433.5,-40.5,22.8,63.9); #arena
}
elsif ( $getnewloc == 3 ) {
$client->MovePC(77,706.2,-373.2,3.8,232.4); #arena
}
elsif ( $getnewloc == 4 ) {
$client->MovePC(77,477.9,732.1,3.8,125.1); #arena
}
elsif ( $getnewloc == 5 ) {
$client->MovePC(77,-73.9,-656.9,3.8,82.5); #arena
}
elsif ( $getnewloc == 6 ) {
$client->MovePC(77,903.5,-623.8,3.8,250.4); #arena
}
elsif ( $getnewloc == 7 ) {
$client->MovePC(77,567.6,373.9,3.8,136.4); #arena
}
elsif ( $getnewloc == 8 ) {
$client->MovePC(77,153.3,-95.5,3.8,51.9); #arena
}
elsif ( $getnewloc == 9 ) {
$client->MovePC(77,436.2,496.2,3.8,13.5); #arena
}
elsif ( $getnewloc == 10 ) {
$client->MovePC(77,-291.5,364.0,3.8,82.2); #arena
}
elsif ( $getnewloc == 11 ) {
$client->MovePC(77,-276.3,-427.4,3.8,48.0); #arena
}
elsif ( $getnewloc == 12 ) {
$client->MovePC(77,37.3,-357.9,3.8,169.5); #arena
}
elsif ( $getnewloc == 13 ) {
$client->MovePC(77,771.6,-750.9,3.8,252.4); #arena
}
elsif ( $getnewloc == 14 ) {
$client->MovePC(77,1088.3,-536.6,3.8,33.9); #arena
}
elsif ( $getnewloc == 15 ) {
$client->MovePC(77,1039.1,569.3,3.8,99.6); #arena
}
else {
$client->MovePC(77,473.4,-374.6,3.8,253.6); #arena
}
}
elsif($text=~/The Warrens/i){
if ( $client->GetLevel() < 15 )
{
quest::say("Hello $name.  You are below level 15 and must wait before being able to enter The Warrens!");
return;
}

$nexusLeaveIncentive = $qglobals{nexusleaveincentive};
$nexusleavecooldown = $qglobals{nexusleavecooldown};

if ( $nexusLeaveIncentive == 1 && $nexusleavecooldown != 1)
{
plugin::nexusbuff_enter($client);
}

#groupPick determined at the top of the script
$getnewloc = $groupPick;

quest::selfcast("207"); #Kings protection for pvp zones - divine aura

if ( $getnewloc == 1 ) {
$client->MovePC(101,-895.1,747.5,-33.2,191.8); #warrens
}
elsif ( $getnewloc == 2 ) {
$client->MovePC(101,-853.4, 656.9, -75.2, 62.4); #warrens
}
elsif ( $getnewloc == 3 ) {
$client->MovePC(101,-831.5, 453.1, -67.5, 19.4); #warrens
}
elsif ( $getnewloc == 4 ) {
$client->MovePC(101,937.7, -295.9, -107.2, 17.5); #warrens
}
elsif ( $getnewloc == 5 ) {
$client->MovePC(101,1145.4, -92.5, -107.2, 189.9); #warrens
}
elsif ( $getnewloc == 6 ) {
$client->MovePC(101,363.7, 105.8, -89.2, 107.2); #warrens
}
elsif ( $getnewloc == 7 ) {
$client->MovePC(101,-540.7, 757.9, -73.7, 105.5); #warrens
}
elsif ( $getnewloc == 8 ) {
$client->MovePC(101,174.1, 330.4, -78.2, 125.2); #warrens
}
elsif ( $getnewloc == 9 ) {
$client->MovePC(101,-983.6, 326.2, -99.2, 162.9); #warrens
}
else {
$client->MovePC(101,-117.0, 327.5, -72.1, 58.0); #warrens
}
}

#elsif($text=~/The Mystery/i && $client->GetGM() == 1){
#$nexusLeaveIncentive = $qglobals{nexusleaveincentive};
#$nexusleavecooldown = $qglobals{nexusleavecooldown};

#if ( $nexusLeaveIncentive == 1 && $nexusleavecooldown != 1)
#{
#plugin::nexusbuff_enter($client);
#}

#groupPick determined at the top of the script
#$getnewloc = $groupPick;

#quest::selfcast("207"); #Kings protection for pvp zones - divine aura

#if ( $getnewloc == 1 ) {
#$client->MovePC(89,665.1,-365.7,-33.5,187.9); #sebilis
#}
#elsif ( $getnewloc == 2 ) {
#$client->MovePC(89,-73.8,-93.4,-31.5,131.5); #sebilis
#}
#elsif ( $getnewloc == 3 ) {
#$client->MovePC(89,-0.4, 228.3, 38.5, 127.6); #sebilis
#}
#elsif ( $getnewloc == 4 ) {
#$client->MovePC(89,536.2,-460.6,-40.5,221.9); #sebilis
#}
#elsif ( $getnewloc == 5 ) {
#$client->MovePC(89,-98.0,-290.6,-177.4,130.4); #sebilis
#}
#elsif ( $getnewloc == 6 ) {
#$client->MovePC(89,159.9,-439.2,-33.5,249.9); #sebilis
#}
#elsif ( $getnewloc == 7 ) {
#$client->MovePC(89,-258.9,-407.3,-51.5,123.1); #sebilis
#}
#elsif ( $getnewloc == 8 ) {
#$client->MovePC(89,242.7,-224.6,-177.4,66.5); #sebilis
#}
#else {
#$client->MovePC(89,87.8,-389.3,-84.2, 5.4); #sebilis
#}
#}


}
