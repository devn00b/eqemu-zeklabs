sub EVENT_SAY { 
if($text=~/Hail/i){
quest::say("Welcome to my Casino! If you would like to try your luck at a game of chance, I can offer you a game of devn00b's Court with prizes best suited for those who like fun. To play, buy a token, for points, or for cash from Reefman. Then just hand it to me when ready."); 
quest::say("**PLEASE NOTE** Only one token at a time or I will eat them all");
quest::say("***** THIS IS A BETA TEST EXPECT FAILURE ****sdasd*");
}
 }
 
sub EVENT_ITEM {
my $random_result = 0;
my $random_result = int(rand(1901));
quest::say("Burp! $random_result");
$checkHandIn = plugin::check_handin(\%itemcount,64193=> 1);
quest::say("Burp!");
if ( $checkHandIn ) {
quest::say("Burp!");
if ( $client->GetLevel() < 20 )
{
quest::summonitem(64193);
quest::say("$name please come back when you are level 20 to play the casino you are too young!");
return;
}

## ULTRA RARE
if( $random_result <= 10){
quest::say("Burp!");
my $prze = quest::ChooseRandom(6639,11696,64193);
$winning =  quest::varlink($prze);
quest::say("Burp!");
if(plugin::check_hasitem($client, $prze)){
quest::summonitem(64193);
quest::say("Sorry $name, you already had that item ($winning), please roll again!");
return;
}else{
if($prze eq '64193') {
quest::summonitem($prze,10);
quest::say("Congrats on your $winning!");
return;
}
quest::summonitem($prze);
quest::say("A Special Mega RARE WINNER ($winning)!");
return;
}

 }



## RARE
if( $random_result>10 && $random_result<600 ){
my $prze = quest::ChooseRandom(134996,17123,1740,64193,24890,52356,2469,146667,146668,146669,146670,146671,146672,146673,146674);
$winning =  quest::varlink($prze);
if(plugin::check_hasitem($client, $prze)){
quest::summonitem(64193);
quest::say("Sorry $name, you already had that item ($winning), please roll again!");
return;
}else{
quest::summonitem($prze);
}
quest::say("A special prize just for you ($winning)!");
 }

 ## SEMI RARE
elsif( $random_result>600 && $random_result<1300){  
my $prze = quest::ChooseRandom(15957,66603,66601,36927,93473,46122,46121,75843,98238,53677,61993,37539,9766,58694,58693,96538,58692,58691,58701,58700,15994,5805,58687,58696,58697);
$winning =  quest::varlink($prze);
if(plugin::check_hasitem($client, $prze)){
quest::summonitem(64193);
quest::say("Sorry $name, you already had that item ($winning), please roll again!");
return;
}else{
quest::summonitem($prze,2);
}
#$winning = quest::varlink($prze);
quest::say("Congrats on your $winning!");

 }
 
## COMMON
elsif( $random_result>1300 && $random_result<1900){  
my $prze = quest::ChooseRandom(10037,22503,52883,21821,21820,146667,146668,146669,146670,146671,146672,146673,146674);
$winning =  quest::varlink($prze);
if(plugin::check_hasitem($client, $prze)){
quest::summonitem(64193);
quest::say("Sorry $name, you already had that item ($winning), please roll again!");
return;
}else{
quest::summonitem($prze);
}
#$winning = quest::varlink($prze);
quest::say("Congrats you won $winning!");
return;
 } else {
quest::say("Sorry $name, you won nothing this time. Try again maybe you will have better luck next Time!");

 }
}
else {
 quest::say("I do not want this... Why do you waste my time?!?");
 quest::say("Your items are now mine... Thanks!");
   }
}
