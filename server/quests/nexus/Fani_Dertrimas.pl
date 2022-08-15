sub EVENT_SAY { 
###########
  my $guildCamp = quest::findcampteamid($client->GuildID());

if ($text =~/Guild Camp/i){
        if ( $guildCamp > 0 )
        {
                my $campAlive = quest::getcitymode($guildCamp);
                if ( $campAlive == 96 )
                {
                        my $x = quest::getteamlocation($guildCamp,"x");
                        my $y = quest::getteamlocation($guildCamp,"y");
                        my $z = quest::getteamlocation($guildCamp,"z");
                        my $zoneid = quest::getteamlocation($guildCamp,"zoneid");
                        $client->MovePC($zoneid,$x, $y, $z, 0);
                }
                else
                {
                        quest::say("Guild camp is unavailable.");
                }
        }
        else
        {
                quest::say("No guild camp exists for you.");
        }
}
###########

if($text=~/Hail/i){
  my $guildCamp = quest::findcampteamid($client->GuildID());
if ( $client->GetPVPFlag() == 0 )
{
quest::say("Hello $name!  I am here to send players to their city but it seems you are not one!  Maybe you should talk to the Priest of Order.");
}
else
{
if ( $client->GetPVPFlag() == 5 )
{
#$link = quest::saylink("head to Butcherblock Camp");
#$link2 = quest::saylink("head to Eruds Crossing Camp");
$camps = quest::saylink("Guild Camp");
quest::say("Hello $name, you are a bandit, thus you have no home!  SAD FOR YOU!!!! You can however goto your [$camps], should you be lucky enough to own one");
}
else
{
$link = quest::saylink("return");
$camps = quest::saylink("Guild Camp");
quest::say("Hello $name!  Would you like to $link home? Or Perhaps port to your [$camps]?");
}
}
}
#elsif($text=~/head to Eruds Crossing Camp/i){
#if ( $client->GetPVPFlag() == 5 ) { $client->MovePC(98,768.8,-1520.4,7.7,184.1); }
#}
#elsif($text=~/head to Butcherblock Camp/i){
#if ( $client->GetPVPFlag() == 5 ) { $client->MovePC(68,2753.5,1139.2,0.8,156.4); }
#}
elsif($text=~/return/i){

if ( $client->GetPVPFlag() == 1 ) { $client->MovePC(2,176.2,293.6,47.7,75.2); }
elsif ( $client->GetPVPFlag() == 3 ) { $client->MovePC(9,-52.1,-108,-10.2,130.9); }
elsif ( $client->GetPVPFlag() == 2 ) { $client->MovePC(29,-290.6,116.2,3.8,127.1); }
elsif ( $client->GetPVPFlag() == 4 ) { $client->MovePC(19,-172.2,52.7,-.3,213); }
elsif ( $client->GetPVPFlag() == 5 ) { $client->MovePC(68,2753.5,1139.2,0.8,156.4); }
}

}
#END of FILE Zone:nexus  ID:152013 -- Fani_Dertrimas 

