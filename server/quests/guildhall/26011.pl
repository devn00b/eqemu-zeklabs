sub EVENT_SAY { 
if($text=~/Hail/i){
quest::say("Welcome to my Casino! If you would like to try your luck at a game of chance, I can offer you a game of devn00b's Court with prizes best suited for any warrior, monk, or rogue. To play, buy a token, for points, or for cash from Reefman. Then just hand it to me when ready."); 
quest::say("**PLEASE NOTE** Only one token at a time or I will eat them all");
}
 }
 
sub EVENT_ITEM {
$random_result = 0;
my $random_result = int(rand(5000));
#good stuff
if($itemcount{64193} && $random_result<600 ){  
quest::summonitem(quest::ChooseRandom(134996,17123,96041,1740,64193,24890,52356,2469));
quest::say("A special prize just for you!");
 }

 #stat food x20
elsif($itemcount{64193} && $random_result>601 && $random_result<1300){  
quest::summonitem(quest::ChooseRandom(66602,66603,66601),20);
quest::say("Congrats!");

 }
#gems and cash
elsif($itemcount{64193} && $random_result>1301 && $random_result<1900){  
quest::summonitem(quest::ChooseRandom(10037,22503,10032,21823,21822,21821,21820));
quest::say("Congrats!");

 }
#fireworks
elsif($itemcount{64193} && $random_result>1901 && $random_result<2400){  
quest::summonitem(16880,16877,16879,16881,69230,69231,69233,69232,69234);
quest::say("Congrats!");

 }
 #potions
elsif($itemcount{64193} && $random_result>2401 && $random_result<2800){  
quest::summonitem(quest::ChooseRandom(17966,66614,66613,14521,11631,14402,96454,96456,96465,96453,96455,96459,96458,96462,96457,96463,96451,96461,96464,96452,96460),5);
quest::say("Congrats!");

 }
 #stat items resistance stones, beer goggles
elsif($itemcount{64193} && $random_result>2801 && $random_result<3000){  
quest::summonitem(quest::ChooseRandom(7466,1139,1152,1140,7479,1138));
quest::say("Congrats!");

 }
 #golden ticket
elsif($itemcount{64193} && $random_result==3001){  
quest::summonitem(64193,6639,12265);
#quest::say("Congrats!");
quest::shout ("Big Winner!!");
 } 

elsif($itemcount{64193} && $random_result>3002){
quest::say("Sorry you won nothing this time. Try again maybe you will have better luck next Time!");

 }

elsif($itemcount{66616}){
quest::say("Sorry, I'm no longer accepting coins from reefman. They are of no value anymore.");

 }


else {
 quest::say("I do not want this... Why do you waste my time?!?");
 quest::say("Your items are now mine... Thanks!");
   }
}
