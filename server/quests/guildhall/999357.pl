sub EVENT_SAY {
if($text=~/Hail/i){
quest::say("Hello, I am the keeper of ancient jewelcraft knowledge. If you wish to learn what I know you must first aquire the proper paperwork");
quest::say("Fibrus should be able to help you with that.");
}
 }


sub EVENT_ITEM {
if($itemcount{12265}==1){
quest::say("Congrats! Your skill is now ALMOST equal to mine. Now go forth an Craft");
quest::setskill(68,200);
 }
else {
 quest::say("Hmmm..I see..this...yes this is crap...");
 quest::say("Oh what? you wanted that back?..Yeah no..");
   }
}
