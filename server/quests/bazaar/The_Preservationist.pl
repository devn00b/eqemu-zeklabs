sub EVENT_SAY { 
if($text=~/Hail/i){
quest::say("Hello!  I am here to preserve collector items which have been stolen or lost, turn in a Collector's item to me and you will be rewarded!"); 
quest::say("**PLEASE NOTE** Only one item at a time");
}
 }
 
sub EVENT_ITEM {
$random_result = 0;
my $random_result = int(rand(3500));
#good stuff
$checkHandIn = plugin::check_handin(\%itemcount,69240=> 1) || plugin::check_handin(\%itemcount,69241=> 1) || plugin::check_handin(\%itemcount,69242=> 1) || plugin::check_handin(\%itemcount,69243=> 1) || plugin::check_handin(\%itemcount,69244=> 1) || plugin::check_handin(\%itemcount,69245=> 1) || plugin::check_handin(\%itemcount,69246=> 1) || plugin::check_handin(\%itemcount,69247=> 1) || plugin::check_handin(\%itemcount,69248=> 1) || plugin::check_handin(\%itemcount,69249=> 1);
if ( $checkHandIn ) {
if( $random_result<600 ){  
$client->AddPVPPoints(3);
quest::say("You have been given 3 pvp points for your trouble.");
 }

 #stat food x20
elsif( $random_result>600 && $random_result<1300){  
quest::summonitem(quest::ChooseRandom(66602,66603,66601),20);
quest::say("Thank you for your trouble I have provided you some food.");

 }
#gems and cash
elsif( $random_result>1300 && $random_result<1900){  
quest::summonitem(quest::ChooseRandom(10037,22503,10032,21823,21822,21821,21820));
quest::say("Thank you for your time, take this in return.");

 }
 #potions
elsif( $random_result>1900  && $random_result<2800){  
quest::summonitem(quest::ChooseRandom(17966,66614,66613,14521,11631,14402,96454,96456,96465,96453,96455,96459,96458,96462,96457,96463,96451,96461,96464,96452,96460),5);
quest::say("Thank you for your time, take this in return.");

 }
 #stat items resistance stones, beer goggles
elsif( $random_result>2800 && $random_result<3000){  
$client->AddPVPPoints(5);
quest::say("You have been given 5 pvp points for your trouble.");

 }
 #golden ticket
elsif( $random_result>3001 && $random_result<3031){  
$client->AddPVPPoints(25);
quest::say("You have been given 25 pvp points for your trouble.");
 } 
#junk/fireworks
elsif( $random_result<3032 && $random_result<3400){
quest::summonitem(quest::ChooseRandom(16880,16877,16879,16881,69230,69231,69233,69232,69234,10032,21823,21822));
quest::say("Thank you for your assistance, take this for your trouble.");

 }
#PVP Ears
elsif( $random_result<3401 && $random_result>3500){
$client->AddPVPPoints(10);
quest::say("You have been given 10 pvp points for your trouble.");

 }



else {
quest::say("Sorry you won nothing this time. Try again maybe you will have better luck next Time!");

 }
}
else {
 quest::say("I do not want this... Why do you waste my time?!?");
 quest::say("Your items are now mine... Thanks!");
   }
}
