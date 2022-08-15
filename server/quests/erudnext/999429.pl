sub EVENT_SAY { 
if ( defined ($qglobals{akanon_race_playerflag}) )
{
quest::say("You are currently participating in an event and cannot use the teleporter.");
return;
}

my $npcid = $npc->GetNPCTypeID();
my $ownerTeam = quest::getguardfield(8,0, "ownerteamid");
if ( $ownerTeam > 0 && $client->GetPVPFlag() == $ownerTeam ) #check against pvp flag if its qeynos team
{
if($text=~/Hail/i){
my $link = quest::saylink("status");
my $trans = quest::saylink("translocate to North Karana");
my $trans2 = quest::saylink("translocate to South Ro");
my $trans3 = quest::saylink("translocate to West Commonlands");
my $trans4 = quest::saylink("translocate to Greater Faydark");
my $trans6 = quest::saylink("translocate to Iceclad");
my $trans7 = quest::saylink("translocate to Cobalt Scar");
my $trans8 = quest::saylink("translocate to Combine");
quest::say("Hello $name, are you here for a guard $link check?  Do you need a $trans, $trans2, $trans3, $trans4, $trans6, $trans7 or $trans8?");
} #end hail
elsif($text=~/translocate/i) {
$flag = $qglobals{teleportflag};
if ( defined($flag) && $flag == 1 )
{
quest::say("You must wait 10 minutes before I can translocate you again.");
}
else
{
if($text=~/translocate to North Karana/i){
quest::say("Good luck $name!");
quest::selfcast(1338);
quest::setglobal("teleportflag","1",5,"M10");
} #end hail
elsif($text=~/translocate to Combine/i){
quest::say("Good luck $name!");
quest::selfcast(1339);
quest::setglobal("teleportflag","1",5,"M10");
} #end hail
elsif($text=~/translocate to Cobalt Scar/i){
quest::say("Good luck $name!");
quest::selfcast(2025);
quest::setglobal("teleportflag","1",5,"M10");
} #end hail
elsif($text=~/translocate to Iceclad/i){
quest::say("Good luck $name!");
quest::selfcast(2022);
quest::setglobal("teleportflag","1",5,"M10");
} #end hail
elsif($text=~/translocate to South Ro/i){
quest::say("Good luck $name!");
quest::selfcast(609);
quest::setglobal("teleportflag","1",5,"M10");
} #end hail
elsif($text=~/translocate to West Commonlands/i){
quest::say("Good luck $name!");
quest::selfcast(531);
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
plugin::createguardstatus_popup($npc,$npcid,8);
} #end status
}
}

sub EVENT_SPAWN
{
	quest::setteamexpbonus(8,5); #reset qeynos team xp bonus
	plugin::setupguard_enter(8,$npc);
}


sub EVENT_ENTER {
plugin::enterguard_area(8,$client,$npc);

} #end of sub event_enter

sub EVENT_DEATH
{
#since the npc died we have to reset the values
	quest::resetguardfields(8,$npc->GetNPCTypeID(), $userid);

my $guildID = quest::getguardfield(8,0, "guildid");
#check to see if its a player, then make sure they aren't a neutral (0) player or the same team
if ( $guildID > 0 && $client->GuildID() != $guildID ) { 
quest::we(13,"The Erudin Kingdom's Morale Officer has been killed!  The Erudin Kingdom loses their experience bonus temporarily!");
quest::setteamexpbonus(8,0); #reset qeynos team xp bonus
}
}