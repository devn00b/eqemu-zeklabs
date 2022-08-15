sub EVENT_SAY {
if($text=~/Hail/i){
quest::say("Hello there $name! Wonderful day isnt it? Sorry I got distracted");
quest::say("But anyway, If you happen to have any PVP Point Vouchers from the new lands I hear recently opened up **PLEASE** can I have them?");
quest::say("I would be the envy of all the other aviaks in the village! I'll Give you 200/300/500 PVP points depending on what voucher you have.");
quest::say("Just give them to me **ONE** at a time!");
}
 }


sub EVENT_ITEM {

if(plugin::check_handin(\%itemcount, 144586 => 1)){
#$itemcount{144586}==1){
quest::say("This isnt bad! I know there are better so here take 200pts and please try and find the better ones. Thats what I REALLY want.");
$client->AddPVPPoints(200); 
}

elsif(plugin::check_handin(\%itemcount, 144587 => 1)){
#$itemcount{144587}==1){
quest::say("Wonderful This is simply wonderful! Here have 300pts $name. You have earned it. If you manage to find anything better than this, I will be stunned.");
$client->AddPVPPoints(300);
 }

elsif(plugin::check_handin(\%itemcount, 144588 => 1)){
#$itemcount{144588}==1){
quest::emote("Grabs his chest");
quest::doanim(16);
quest::say("I...I...have....wow....PLEASE TAKE THIS 500points with my EXTREME Thanks. I've also put in a good word with the rest of the tribe about you.");
$client->AddPVPPoints(500);
quest::faction(991,2000);
 }

#else {
# quest::say("Hmmm..I see..this...yes this is crap...");
# quest::say("Oh what? you wanted that back?..Yeah no..Dont waste my time!");
#   }
}
