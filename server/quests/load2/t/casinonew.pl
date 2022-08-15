sub EVENT_SAY { 
if($text=~/Hail/i){
quest::say("Welcome to my Casino! If you would like to try your luck at a game of chance, I can offer you a game of devn00b's Court with prizes best suited for those who like fun. To play, buy a token, for points, or for cash from Reefman. Then just hand it to me when ready."); 
quest::say("**PLEASE NOTE** Only one token at a time or I will eat them all");
quest::say("***** THIS IS A BETA TEST EXPECT FAILURE *****");
}
 }
 
sub EVENT_ITEM {
quest::say("Burp");
#my $random_result = 100;

#1900 is highest, above that adds chance to fail (no loot at all).
my $random_result = int(rand(2000));

$checkHandIn = plugin::check_handin(\%itemcount,4328=> 1);
#                                               4328=> 1); #original non beta item turn in

if ( $checkHandIn ) {

if ( $client->GetLevel() < 20 )
{
quest::summonitem(4328);
#4328); #original non beta it
quest::say("$name please come back when you are level 20 to play the casino you are too young!");
return;
}

############################ ULTRA RARE
if( $random_result <= 10){

my $prze = quest::ChooseRandom(6639,11696,4328,13401,2300,52356,24890,12265,146667,146668,146671,146672,146674,146670,146673,146669);
$winning =  quest::varlink($prze);

if(plugin::check_hasitem($client, $prze)){
quest::summonitem(4328);
quest::say("Sorry $name, you already had that item ($winning), please roll again!");
return;
}else{
if($prze eq '4328') {
quest::summonitem($prze,10);
quest::say("Congrats on your $winning! x10");
return;
}
quest::summonitem($prze);
quest::say("A Special Mega RARE WINNER ($winning)!");
return;
}

 }
############################# END ULTRA RARE

############################# RARE
#if ultra is 10, then 50 is 5x chance right???...right???
if( $random_result>10 && $random_result<=50 ){
my $prze = quest::ChooseRandom(146928,146929,146930,141320,142122,15991,1138,1139,1140,1152,145492,145493,7475,7299,145166);
$winning =  quest::varlink($prze);
if(plugin::check_hasitem($client, $prze)){
quest::summonitem(4328);
quest::say("Sorry $name, you already had that item ($winning), please roll again!");
return;
}else{
quest::summonitem($prze);
quest::say("A special prize just for you ($winning)!");
return;
}

 }
############################# END RARE 

############################ SEMI RARE
elsif( $random_result>50 && $random_result<=500){  
my $prze = quest::ChooseRandom(36927,66603,1740,17123,21820,66614,66613,7466,135039,135038);
$winning =  quest::varlink($prze);
if(plugin::check_hasitem($client, $prze)){
quest::summonitem(4328);
quest::say("Sorry $name, you already had that item ($winning), please roll again!");
return;
}else{
quest::summonitem($prze);
quest::say("Congrats on your $winning!");
return;
}
 }
 ############################# END SEMI RARE 

 
############################## COMMON
elsif( $random_result>500 && $random_result<=1900){  
my $prze = quest::ChooseRandom(66601,15957,16880,16877,16879,96454,96456,96459,96462,96463,96464);
$winning =  quest::varlink($prze);
if(plugin::check_hasitem($client, $prze)){
quest::summonitem(4328);
quest::say("Sorry $name, you already had that item ($winning), please roll again!");
return;
}else{
quest::summonitem($prze);
quest::say("Congrats you won $winning!");
return;
}
} 
############################## END COMMON
 
 ##if we got here our roll was greater than the highest. So, they get nothing.
 else {
quest::say("Sorry $name, you won nothing this time. Try again maybe you will have better luck next Time!");
return;
 }
}
else {
 quest::say("I do not want this... Why do you waste my time?!?");
 quest::say("Your items are now mine... Thanks!");
   }
   return;
}
