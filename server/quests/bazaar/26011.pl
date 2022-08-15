sub EVENT_SAY { 
if($text=~/Hail/i){
quest::say("Welcome to my Casino! If you would like to try your luck at a game of chance, I can offer you a game of devn00b's Court with prizes best suited for those who like fun. To play, buy a token, for points, or for cash from Reefman. Then just hand it to me when ready."); 
quest::say("**PLEASE NOTE** Only one token at a time or I will eat them all");
}
 }
 
sub EVENT_ITEM {
$random_result = 0;
my $random_result = int(rand(6500));
#$random_result=3004;
#good stuff
$checkHandIn = plugin::check_handin(\%itemcount,64193=> 1);
if ( $checkHandIn ) {

if ( $client->GetLevel() < 20 )
{
quest::summonitem(64193);
quest::say("$name please come back when you are level 20 to play the casino you are too young!");
return;
}


if( $random_result<600 ){
my $prze = quest::ChooseRandom(134996,17123,1740,64193,24890,52356,2469);
#quest::summonitem(quest::ChooseRandom(134996,17123,1740,64193,24890,52356,2469));

if(plugin::check_hasitem($client, $prze)){
quest::summonitem(64193);
quest::say("Sorry $name, you already had that item, please roll again!");
return;
}else{
quest::summonitem($prze);
}

quest::say("A special prize just for you!");
 }

 #stat food x20
elsif( $random_result>600 && $random_result<1300){  
quest::summonitem(quest::ChooseRandom(15957,66603,66601,36927),20);
quest::say("Congrats!");

 }
#gems and cash
elsif( $random_result>1300 && $random_result<1900){  
quest::summonitem(quest::ChooseRandom(10037,22503,52883,21823,21822,21821,21820));
quest::say("Congrats!");

 }
#fireworks
elsif( $random_result>1900 && $random_result<2400){  
quest::summonitem(quest::ChooseRandom(16880,16877,16879,16881,69230,69231,69233,69232,69234));
quest::say("Congrats!");

 }
 #potions
elsif( $random_result>2400 && $random_result<2800){  

#if( $random_result<600 ){
my $prze = quest::ChooseRandom(17966,66614,66613,14521,11631,14402,96454,96456,96465,96453,96455,96459,96458,96462,96457,96463,96451,96461,96464,96452,96460);
if(plugin::check_hasitem($client, $prze)){
quest::summonitem(64193);
quest::say("Sorry $name, you already had that item, please roll again!");
return;
}else{
#quest::summonitem(quest::ChooseRandom(17966,66614,66613,14521,11631,14402,96454,96456,96465,96453,96455,96459,96458,96462,96457,96463,96451,96461,96464,96452,96460),5);
quest::summonitem($prze,5);
quest::say("Congrats!");
 }
}

 #stat items resistance stones, beer goggles
elsif( $random_result>2800 && $random_result<3000){  
my $prze = quest::ChooseRandom(7466,1139,1152,1140,7479,1138);
if(plugin::check_hasitem($client, $prze)){
quest::summonitem(64193);
quest::say("Sorry $name, you already had that item, please roll again!");
return;
}else{
quest::summonitem($prze,5);
quest::say("Congrats!");
 }
}

 #golden ticket
elsif( $random_result>3001 && $random_result<3010){  
my $prze = quest::ChooseRandom(8137,64193,6639,12265,11696,145166);
if(plugin::check_hasitem($client, $prze)){
quest::summonitem(64193);
quest::say("Sorry $name, you already had that item, please roll again!");
return;
}else{
quest::say("Congrats big winner!");
quest::shout("!!$name Is A Big Casino Winner!!");
quest::summonitem($prze);
}
 } 

#junk/fireworks
elsif( $random_result<3011 && $random_result<3400){
quest::summonitem(quest::ChooseRandom(16880,16877,16879,16881,69230,69231,69233,69232,69234,10032,21823,21822));
quest::say("Congrats!!");

 }
#PVP Ears
elsif( $random_result<3401 && $random_result>3500){
quest::summonitem(quest::ChooseRandom(135038,135039),10);
quest::say("Yay PVP Points!!!!!!");

 }
elsif( $random_result<3501 && $random_result>6500){
quest::summonitem(quest::ChooseRandom(15957,66603,66601,36927),20);
quest::say("Hey! Congrats!!!!!!");

 }



else {
quest::say("Sorry $name, you won nothing this time. Try again maybe you will have better luck next Time!");

 }
}
else {
 quest::say("I do not want this... Why do you waste my time?!?");
 quest::say("Your items are now mine... Thanks!");
   }
}
