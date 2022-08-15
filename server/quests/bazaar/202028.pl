sub EVENT_SAY {
if($text=~/Hail/i){
quest::say("Hello there! I am Guardian Voelath. I'm looking to trade Ethereal Parchment and Spectral Parchment for some devnoob ears. Please hand them to me one at a time if you have any.");
quest::say("I've also got a limited stock in trade for SAVE ME FOR POINTS that dropped from The Cursed in Lfaydark. If you have any hand them over!!");
}
 }


sub EVENT_ITEM {
if($itemcount{29131}==1){
quest::say("Fantastic here have some of devnoobs ears for your trouble!");
quest::summonitem(135038, 5)
 }

elsif($itemcount{29112}==1){
quest::say("Fantastic here have some of images ears for your trouble!");
quest::summonitem(135039, 5)
 }

elsif($itemcount{22196}==1){
quest::say("Fantastic here have some points!");
$client->AddPVPPoints(200);
 }

else {
 quest::say("Hmmm..I see..this...yes this is crap...");
 quest::say("Oh what? you wanted that back?..Yeah no..");
   }
}
