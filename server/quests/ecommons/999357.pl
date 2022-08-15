sub EVENT_SAY {
if($text=~/Hail/i){
quest::say("Hello, I am the keeper of ancient crafting knowledge. If you wish to learn what I know you must first aquire the proper paperwork");
quest::say("Fibrus should be able to help you with that. Last I saw him he was in the Highkeep Casino");
}
 }


sub EVENT_ITEM {
if($itemcount{12265}==1){
quest::say("Congrats! Your Jewelcrafting skill is now ALMOST equal to mine. Now go forth an Craft.");
quest::setskill(68,200);
 }

if($itemcount{146667}==1){
quest::say("Congrats! Your Alchemy skill is now ALMOST equal to mine. Now go forth an Craft.");
quest::setskill(59,200);
 }

if($itemcount{146668}==1){
quest::say("Congrats! Your Baking skill is now ALMOST equal to mine. Now go forth an Craft.");
quest::setskill(60,200);
 }

if($itemcount{146669}==1){
quest::say("Congrats! Your Tinkering skill is now ALMOST equal to mine. Now go forth an Craft.");
quest::setskill(57,200);
 }

if($itemcount{146670}==1){
quest::say("Congrats! Your Tailoring skill is now ALMOST equal to mine. Now go forth an Craft.");
quest::setskill(61,200);
 }

if($itemcount{146671}==1){
quest::say("Congrats! Your Blacksmithing skill is now ALMOST equal to mine. Now go forth an Craft.");
quest::setskill(63,200);
 }

if($itemcount{146672}==1){
quest::say("Congrats! Your Fletching skill is now ALMOST equal to mine. Now go forth an Craft.");
quest::setskill(64,200);
 }

if($itemcount{146673}==1){
quest::say("Congrats! Your Taunting Skill is now 250.");
quest::setskill(73,250);
 }

if($itemcount{146674}==1){
quest::say("Congrats! Your Lockpicking skill is now 200.");
quest::setskill(35,200);
 }



else {
 quest::say("Hmmm..I see..this...yes this is crap...");
 quest::say("Oh what? you wanted that back?..Yeah no..");
   }
}
