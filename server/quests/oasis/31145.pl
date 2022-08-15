
sub EVENT_SAY { 
if($text=~/Hail/i){
$link1 = quest::varlink(146929);
$link2 = quest::varlink(146928);
$saylnk = quest::saylink("want");
quest::say("Pssst hey $name You [$saylnk] $link1 or $link2? I got the stuff man.");
}
if($text=~/want/i){
quest::say("Then here is what I want. I want 4 Black Tickets. They can be found all over this god forsaken desert. Bring me 4....");
}
 }
 
sub EVENT_ITEM {
#my $random_result = 0;

#100 is highest, above that adds chance to fail (no loot at all).
my $random_result = int(rand(250));

$checkHandIn = plugin::check_handin(\%itemcount,51862=> 4);

if ( $checkHandIn ) {

if ( $client->GetLevel() < 5 )
{
quest::summonitem(64193);
#64193); #original non beta it
quest::say("$name please come back when you are level 5 to play the casino you are too young!");
return;
}

#quest::say("You Rolled $random_result.");
############################ ULTRA RARE
if ($random_result eq '0'){
quest::say("You Get an ULTRA rare 2 Potions! one of each!");
quest::summonitem(146929);
quest::summonitem(146928);
return;
}
if ($random_result <= 25 and $random_result >0){
quest::say("Congrats $name!");
quest::summonitem(146929);
return;
}

if ($random_result>25 and $random_result<=100){
quest::summonitem(146928);
quest::say("Woohoo Here you go $name!");
return
}
############################## END COMMON
 
 ##if we got here our roll was greater than the highest. So, they get nothing.
 if($random_result>100) {
quest::say("Sorry $name, you won nothing this time. Try again maybe you will have better luck next Time! **Mumble** Probably not.");
return;
 }
} else {
 quest::say("I do not want this... Why do you waste my time?!?");
 quest::say("Your items are now mine... Thanks!");
   }
   return;
}
