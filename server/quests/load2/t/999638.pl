sub EVENT_SAY
{
if (!defined $qglobals{"donate"}) {
quest::setglobal("donate", 0, 5, "F");
}
$shitt = $client->GetGlobal("donate");
quest::say("$shitt");
 
if ($text=~/hail/i) { 

quest::say("Hello $name, Are you an [early access] player?");
}#end hail

if ($text=~/early access/i) {
$accid = $client->AccountID();
$shit = $client->GetGlobal("donate");

if($accid eq '2' or $accid eq '7' or $accid eq '77' or $accid eq '532' or $accid eq '1874' or $accid eq '2874' or $accid eq '3123' or $accid eq '3231' or $accid eq '3422' or $accid eq '3494' or $accid eq '3599' or $accid eq '3603' or $accid eq '3649' or $accid eq '3869' or $accid eq '4249' or $accid eq '4436' or $accid eq '4443' or $accid eq '4848' or $accid eq '5849' or $accid eq '5878') {
#so now that we know they are a donator lets break it down some more
#bronze ($50)
}

if($accid eq '5878' or $accid eq '3869' or $accid eq '3422' or $accid eq '3422' or $accid eq '3231' or $accid eq '77' or $accid eq '4443' or $accid eq '4249' or $accid eq '7' or $accid eq '532' or $accid eq '3123' or $accid eq '3494' or $accid eq '3649' or $accid eq '4848' or $accid eq '5849') {
quest::say("!!Bronzer!");
if($shit>0) {
quest::say("Sorry $name you can only claim this once");
return;
}
quest::summonitem("146596");
quest::summonitem("64193",5);
return;
}

#silver
if($accid eq '2' or $accid eq '1874' or $accid eq '3599' or $accid eq '2874') {
quest::say("Silverer!");
if($shit>0) {
quest::say("Sorry $name you can only claim this once");
return;
}

quest::summonitem("146597");
quest::summonitem("64193",10);
quest::setglobal("donate", 1, 5, "F");
return;
}

#plat
if($accid eq '4436') {
quest::say("Platinuminum!");
quest::summonitem("146598");
quest::summonitem("64193",20);
return;
}



}

}#end event say
