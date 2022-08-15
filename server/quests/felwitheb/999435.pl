sub EVENT_SAY { 

if ( defined ($qglobals{akanon_race_playerflag}) )
{
quest::say("You are currently participating in an event and cannot use the teleporter.");
return;
}

my $npcid = $npc->GetNPCTypeID();
my $ownerTeam = quest::getguardfield(7,0, "ownerteamid");
if ( $ownerTeam > 0 && $client->GetPVPFlag() == $ownerTeam ) #check against pvp flag if its qeynos team
{
if($text=~/Hail/i){
my $link = quest::saylink("status");
my $trans = quest::saylink("Circle of Iceclad");
my $trans2 = quest::saylink("Circle of Toxxulia");
my $trans3 = quest::saylink("Circle of Butcher");
my $trans4 = quest::saylink("Circle of Lavastorm");
my $trans5 = quest::saylink("Circle of the Combines");
my $trans6 = quest::saylink("Circle of Cobalt Scar");
quest::say("Hello $name, are you here for a guard $link check?  Do you need a teleport via $trans, $trans2, $trans3, $trans4, $trans5 or $trans6?");
} #end hail
elsif($text=~/Circle/i) {
$flag = $qglobals{teleportflag};
if ( defined($flag) && $flag == 1 )
{
quest::say("You must wait 10 minutes before I can translocate you again.");
}
else
{
if($text=~/Circle of Iceclad/i){
quest::say("Good luck $name!");
quest::selfcast(1434);
quest::setglobal("teleportflag","1",5,"M10");
} #end hail
elsif($text=~/Circle of Toxxulia/i){
quest::say("Good luck $name!");
quest::selfcast(552);
quest::setglobal("teleportflag","1",5,"M10");
} #end hail
elsif($text=~/Circle of Butcher/i){
quest::say("Good luck $name!");
quest::selfcast(553);
quest::setglobal("teleportflag","1",5,"M10");
} #end hail
elsif($text=~/Circle of Lavastorm/i){
quest::say("Good luck $name!");
quest::selfcast(554);
quest::setglobal("teleportflag","1",5,"M10");
} #end hail
elsif($text=~/Circle of the Combines/i){
quest::say("Good luck $name!");
quest::selfcast(1517);

quest::setglobal("teleportflag","1",5,"M10");
} #end hail
elsif($text=~/Circle of Cobalt Scar/i){
quest::say("Good luck $name!");
quest::selfcast(1440);
quest::setglobal("teleportflag","1",5,"M10");
} #end hail
}
}
elsif($text=~/status/i){
plugin::createguardstatus_popup($npc,$npcid,7);
} #end status

} #end friendly

} #end say

sub EVENT_DEATH
{
#since the npc died we have to reset the values

quest::resetguardfields(7,$npc->GetNPCTypeID(), $userid);
	
my $guildID = quest::getguardfield(7,0, "guildid");
#check to see if its a player, then make sure they aren't a neutral (0) player or the same team
if ( $guildID > 0 && $client->GuildID() != $guildID ) { 
quest::we(13,"The Felwithe Kingdom's Morale Officer has been killed!  The Felwithe Kingdom loses their experience bonus temporarily!");
quest::setteamexpbonus(7,0); #reset qeynos team xp bonus
}
}
sub EVENT_ENTER
{
plugin::enterguard_area(7,$client,$npc);
}

sub EVENT_SPAWN
{
	quest::setteamexpbonus(7,5);
	plugin::setupguard_enter(7,$npc);
}