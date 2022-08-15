sub EVENT_SAY {
if (defined $qglobals{"betagrant"}) {
quest::say("I want no more to do with you $name!!");
return;
}

  if($text =~ /Hail/i) {
#  $interestedLink = quest::saylink("interested"); 
quest::say("Hello $name! If you were the winner of the beta bash gimme your ticket. If not why are you talking to me?");
  }
}

sub EVENT_ITEM {
if (defined $qglobals{"betagrant"}) {
quest::say("Nooope!");
return;
}


if($itemcount{149286}==1){
quest::say("Congrats $name on killing the most people! Your AWESOME");
quest::summonitem(64193,15);
quest::summonitem(146930);
quest::enabletitle(900);
quest::setglobal("betagrant", 1, 5, "F");
return;
 }

if($itemcount{149287}==1){
quest::say("Congrats $name on getting the most points...you probably cheated.");
quest::summonitem(64193,10);
quest::enabletitle(901);
#quest::setglobal("betagrant", 1, 5, "F");
return;
 }

if($itemcount{149288}==1){
quest::say("Wow do you even know how to pvp $name? what a joke you are!");
quest::enabletitle(902);
#quest::setglobal("betagrant", 1, 5, "F");
return;
 }

if($itemcount{149301}==1){
quest::say("Well done $name you found the most items. On a PVP server....sad little bluebie");
quest::enabletitle(903);
#quest::setglobal("betagrant", 1, 5, "F");
return;
 }


else {
 quest::say("Hmmm..I see..this...yes this is crap...");
 quest::say("Oh what? you wanted that back?..Yeah no..");
   }
}




