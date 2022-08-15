sub EVENT_SAY { 

my $npcid = $npc->GetNPCTypeID();
my $ownerTeam = quest::getguardfield(1,0, "ownerteamid");
if ( $ownerTeam > 0 && $client->GetPVPFlag() == $ownerTeam ) #check against pvp flag if its qeynos team
{
if($text=~/Hail/i){
my $link = quest::saylink("status");
my $trans = quest::saylink("translocate to North Ro");
my $trans4 = quest::saylink("translocate to Greater Faydark");
my $sow = quest::saylink("Spirit of Wolf");
quest::say("Hello $name, are you here for a guard $link check?  Do you need a teleport to $trans or $trans4?  Would you need $sow for your travels?");
} #end hail
elsif($text=~/Spirit of Wolf/i){
quest::say("Good luck $name!");
quest::selfcast(1776);
} #end hail
elsif($text=~/translocate/i) {
$flag = $qglobals{teleportflag};
if ( defined($flag) && $flag == 1 )
{
quest::say("You must wait 10 minutes before I can translocate you again.");
}
else
{
if($text=~/translocate to North Ro/i){
quest::say("Good luck $name!");
quest::selfcast(1373);
quest::setglobal("teleportflag","1",5,"M10");
} #end hail
elsif($text=~/translocate to Greater Faydark/i){
quest::say("Good luck $name!");
quest::selfcast(1336);
quest::setglobal("teleportflag","1",5,"M10");
} #end hail
}
}
elsif($text=~/status/i){
plugin::createguardstatus_popup($npc,$npcid,1);
} #end status

} #end friendly

} #end say

sub EVENT_DEATH
{
#since the npc died we have to reset the values

quest::resetguardfields(1,$npc->GetNPCTypeID(), $userid);
	
my $guildID = quest::getguardfield(1,0, "guildid");
#check to see if its a player, then make sure they aren't a neutral (0) player or the same team
if ( $guildID > 0 && $client->GuildID() != $guildID ) { 
quest::we(13,"The Qeynos Kingdom's Morale Officer has been killed!  The Qeynos Kingdom loses their experience bonus temporarily!");
quest::setteamexpbonus(1,0); #reset qeynos team xp bonus
}
}
sub EVENT_ENTER
{
plugin::enterguard_area(1,$client,$npc);
}

sub EVENT_SPAWN
{
	quest::setteamexpbonus(1,5);
	plugin::setupguard_enter(1,$npc);
}