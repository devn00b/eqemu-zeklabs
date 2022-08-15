my $entityID = "0";
my $pastX = "0";
my $pastY = "0";
my $pastZ = "0";


sub EVENT_SAY
{
if ( $userid != $entityID && $client->GetGM() == 0 )
{
quest::say("You do not own me!");
return;
}
#if ( $client->GetGM() == 0 )
#{
#quest::say("Only GM's may use this npc at this time.");
#return;
#}
if ( $uguild_id < 1 )
{
quest::say("You are not guilded.");
return;
}
if($text=~/hail/i){
$link = quest::saylink("setup camp");
$link2 = quest::saylink("follow");
$link3 = quest::saylink("npcnear");
$link4 = quest::saylink("zonenear");
$link5 = quest::saylink("locationnear");
quest::say("Hail! Would you like me to $link here?  I can also $link2 you.  I can also notify you if a zone is near ($link4) or npc is near ($link3) or another location is near ($link5) which will prevent you from spawning the camp here.");
}
elsif($text=~/setup camp/i){

$zoneNear = quest::zonepointnearby($x,$y,$z,500);
if($zoneNear==1){
quest::say("Zonepoint is too close by.");
return;
}

$diameter = 100;
$cellspace = 75;
$npcNear = quest::findnpcnear($x,$y,$z,150);
if ( $npcNear == 1 )
{
quest::say("Camp cannot be secured here, there are NPC's nearby.");
return;
}

$locationNear = quest::locationnearby($x,$y,$z,$zoneid,1000);
if ( $locationNear == 1 )
{
quest::say("Camp cannot be secured here, there is another location nearby that conflicts.");
return;
}

$campSecure = plugin::iscampsecure($npc,0,$x,$y,$z,$diameter,$diameter,$cellspace,0);
quest::say("Camp secure: $campSecure");
if ($campSecure == 0 )
{
quest::say("Camp cannot be secured here.");
}
else
{
$teamid = $client->GetPVPFlag();
$guildid = $client->GuildID();
$kingdom = quest::addteamid($guildid,$teamid,$charid,$x,$y,$z,100,$zoneid);
$citymode = quest::getcitymode($kingdom);

if ( $citymode == 96 )
{
quest::say("Camp already exists, cannot spawn another.");
return;
}
quest::say("result: $kingdom");
if ( $kingdom == 0 )
{
quest::say("No team id found failed to install camp.");
return;
}

$campInstalled = plugin::iscampsecure($npc,$kingdom,$x,$y,$z,$diameter,$diameter,$diameter,1);
if ( $campInstalled == 1 )
{
quest::updateteamid($guildid,$teamid,$charid,$x,$y,$z,100,$zoneid);
quest::setcitymode($kingdom,96);
quest::say("Camp was successfully installed.");
$client->NukeItem(39014);
quest::depop(999577);
}
else
{
quest::say("Camp failed to be installed here.");
}
}
}
elsif($text=~/zonenear/i){
$zoneNear = quest::zonepointnearby($x,$y,$z,500);
if ( $zoneNear == 1 )
{
quest::say("Camp cannot be secured here, there is a zone point nearby.");
return;
}
else
{
quest::say("No zonepoint is nearby here.");
return;
}
}
elsif($text=~/npcnear/i){
$npcNear = quest::findnpcnear($x,$y,$z,150);
if ( $npcNear == 1 )
{
quest::say("Camp cannot be secured here, there is a npc nearby.");
return;
}
else
{
quest::say("No npc is nearby here.");
return;
}
}
elsif($text=~/locationnear/i){
$locationNear = quest::locationnearby($x,$y,$z,$zoneid,1000);
if ( $locationNear == 1 )
{
quest::say("Camp cannot be secured here, there is another location nearby that conflicts.");
return;
}
else
{
quest::say("No locations conflict with the current location.");
return;
}
}
elsif($text=~/findteam/i){
$guildid = $client->GuildID();
$team = quest::findcampteamid($guildid);
if ( $team > 0 )
{
quest::say("Team found: $team");
}
else
{
quest::say("No team for guild $guildid");
}
}
elsif($text=~/follow/i){
quest::follow($userid);
}
#elsif($text=~/addteam/i){
#$teamid = $client->GetPVPFlag();
#$guildid = $client->GuildID();
#$kingdom = quest::addteamid($guildid,$teamid,$charid,$x,$y,$z,100,$zoneid);
#quest::say("result: $kingdom");
#}
#elsif($text=~/citymode/i){
#quest::setcitymode(15,97);
#}
}

sub EVENT_SIGNAL {
$entityID = $signal; # track the character's entity id
quest::follow($entityID,10);
quest::say("I am here to assist!");
quest::settimer("depoptimer",599);
quest::settimer("checkclient",5);
}

sub EVENT_TIMER {
if ( $timer eq "checkclient" )
{
$character = quest::getclientbyentityid($entityID);
if ( defined($character) == 0 )
{
quest::say("Client is not around $entityID.");
quest::stoptimer("checkclient");
quest::depop(999577);
}
}
elsif ( $timer eq "depoptimer" )
{
quest::stoptimer("depoptimer");
quest::say("This is taking a great deal of time, I can return to assist when you are ready.");
quest::depop(999577);
}
}