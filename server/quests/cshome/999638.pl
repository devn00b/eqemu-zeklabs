sub EVENT_SAY
{
 
if ($text=~/hail/i) { 

quest::say("Hello $name, Are you an [early access] player?");
}#end hail

if ($text=~/early access/i) {
my $accid = $client->AccountID();
#if($accid eq '2' or $accid eq '7' or $accid eq '77' or $accid eq '532' or $accid eq '1874' or $accid eq '2874' or $accid eq '3123' or $accid eq '3231' or $accid eq '3422' or $accid eq '3494' or $accid eq '' or $accid eq '3599' or $accid eq '3603' or $accid eq '3649' or $accid eq '3869' or $accid eq '4249' or $accid eq '4436' or $accid eq '4443' or $accid eq '4848' or $accid eq '5849' or $accid eq '5878') 
#{
#so now that we know they are a donator lets break it down some more
#bronze ($50)
if($accid eq '5878' or $accid eq '3869' or $accid eq '3422' or $accid eq '3422' or $accid eq '33231' or $accid eq '77' or $accid eq '4443' or $accid eq '4249' or $accid eq '7' or $accid eq '532' or $accid eq '3123' or $accid eq '3494' or $accid eq '3649' or $accid eq '4848' or $accid eq '5849') {	
quest::say("!!Bronzer!");
return;
}

#silver
if($accid eq '2' or $accid eq '1874' or $accid eq '3599' $accid eq '2874') {
quest::say("Silverer!");
return;
}

#plat
if($accid eq '4436'  ) {
quest::say("Platinuminum!");
return;
}

quest::say("Hey devn00b.");
return;
}
quest::say("Sorry $name you arent an EA Player, so fuck off.");
return;
#}

}#end event say
