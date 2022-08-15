sub EVENT_SAY {
#$group = $client->GetGroup();
$groupid = 0;
$groupPick = 0;

$guildnum = $client->GuildID();

if ( $guildnum > 512 )
{
$guildnum = 0;
}

$charGlobalName = "charMaalikSpawn" . $client->CharacterID();
$guildGlobalName = "guildMaalikSpawn" . $guildnum;

if ( defined $client->GetGroup() )
{
$group = $client->GetGroup();
$groupid = $group->GetID();
$groupGlobalName = "groupMaalikSpawn" . $groupid;

if ( defined $qglobals{$groupGlobalName} ) {
$groupPick = $qglobals{$groupGlobalName};
}
else
{
$getnewloc = quest::ChooseRandom(1,2,3,4,5,6,7,8);
quest::setglobal($groupGlobalName,$getnewloc,7,"H6");
$groupPick = $getnewloc;
if ( $guildnum < 10000 && undef $qglobals{$guildGlobalName} ) {
quest::setglobal($guildGlobalName,$getnewloc,7,"H6");
}
}
}
else
{
if ( $guildnum > 0 && $guildnum < 10000 )
{
if ( defined $qglobals{$guildGlobalName} ) {
$groupPick = $qglobals{$guildGlobalName};
}
else
{
$getnewloc = quest::ChooseRandom(1,2,3,4,5,6,7,8);
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
$getnewloc = quest::ChooseRandom(1,2,3,4,5,6,7,8);
quest::setglobal($charGlobalName,$getnewloc,5,"H6");
$groupPick = $getnewloc;
}
}
}

  if ($text=~/Hail/i){
   $visit = quest::saylink("visit");
$groupflag = quest::saylink("remove group flag");
$guildflag= quest::saylink("remove guild flag");
$charflag = quest::saylink("remove char flag");

  quest::say("Hello there, $name. We have scouted a new land, would you like to $visit there?  You may also $groupflag or $guildflag or $charflag.");
  }
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
  elsif($text=~/visit/i)
  {
	  quest::say("Be Safe!");
if ( $getnewloc == 1 ) {
$client->MovePC(89,665.1,-365.7,-33.5,187.9); #sebilis
}
elsif ( $getnewloc == 2 ) {
$client->MovePC(89,-73.8,-93.4,-31.5,131.5); #sebilis
}
elsif ( $getnewloc == 3 ) {
$client->MovePC(89,-0.4, 228.3, 38.5, 127.6); #sebilis
}
elsif ( $getnewloc == 4 ) {
$client->MovePC(89,536.2,-460.6,-40.5,221.9); #sebilis
}
elsif ( $getnewloc == 5 ) {
$client->MovePC(89,-98.0,-290.6,-177.4,130.4); #sebilis
}
elsif ( $getnewloc == 6 ) {
$client->MovePC(89,159.9,-439.2,-33.5,249.9); #sebilis
}
elsif ( $getnewloc == 7 ) {
$client->MovePC(89,-258.9,-407.3,-51.5,123.1); #sebilis
}
elsif ( $getnewloc == 8 ) {
$client->MovePC(89,242.7,-224.6,-177.4,66.5); #sebilis
}
else {
$client->MovePC(89,87.8,-389.3,-84.2, 5.4); #sebilis
}
  }
}