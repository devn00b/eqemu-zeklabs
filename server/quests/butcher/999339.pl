sub EVENT_SAY { 
if ( $client->GetPVPFlag() == 5 ) #check against pvp flag if its qeynos team
{
if($text=~/Hail/i){
my $link = quest::saylink("status");
my $trans = quest::saylink("translocate to North Karana");
my $trans2 = quest::saylink("translocate to South Ro");
my $trans3 = quest::saylink("translocate to West Commonlands");
my $trans4 = quest::saylink("translocate to Greater Faydark");
my $trans5 = quest::saylink("translocate to Tox");
quest::say("Hello $name.  Do you need a $trans? or $trans2? or $trans3? or $trans4? or $trans5?");
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
elsif($text=~/translocate to Tox/i){
quest::say("Good luck $name!");
quest::selfcast(1337);
quest::setglobal("teleportflag","1",5,"M10");
} #end hail
}
}
}
}