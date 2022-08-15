## This is devn00bs s5 point catch up zones entry point.
## Goes on The Portal Master
## Also using for ports to other locations I may use

sub EVENT_SAY {

if($ulevel <10){
quest::say("Get to level 11 then lets talk noob");
return;
}

if(($text=~/hail/i) && ($ulevel >10) ){
quest::say("Greetings! I can send you to a few zones that may be avaialbe at launch. Note Single Port in at the moment.");
#quest::say("[frozenshadow]");
quest::say("[karnor]");
quest::say("[templeveeshan]");
quest::say("[velketor]");
quest::say("[kael]");
quest::say("[necropolis]");
#quest::say("[greatdivide]");
#quest::say("[growthplane] Please Note Port in not always safe.");
quest::say("[sleeper] - Level 50+");
quest::say("");
quest::say("And Finally [sebilis]");

return;
  }

if($text=~/sleeper/i && $ulevel >49){
quest::say("..Good Luck.");
quest::movepc(128,9.8,-22.9,4.4,67.9);
return;
}

if($text=~/karnor/i && $ulevel >10){
quest::say("..Good Luck.");
quest::movepc(102,302, 18, 6, 192);
return;
}

if($text=~/templeveeshan/i && $ulevel >10){
quest::say("..Good Luck.");
quest::movepc(124, -504, -2111, -36, 2.2);
return;
}

if($text=~/velketor/i && $ulevel >10){
quest::say("..Good Luck.");
quest::movepc(112, -120, 587, -152, 129.0);
return;
}

if($text=~/kael/i && $ulevel >10){
quest::say("..Good Luck.");
quest::movepc(113, -633, -47, 128, 126.5);
return;
}

if($text=~/necropolis/i && $ulevel >10){
quest::say("..Good Luck.");
quest::movepc(123, 1984, -95, 3, 203.1);
return;
}

#if($text=~/greatdivide/i && $ulevel >10){
#quest::say("..Good Luck.");
#quest::movepc(90,-853.4,-0.6,3.1,63.6);
#return;
#}
#if($text=~/growthplane/i && $ulevel >10){
#quest::say("..Good Luck.");
#quest::movepc(127, 3016, -2522, -19, 0);
#return;
#}

if($text=~/sebilis/i && $ulevel >50){
quest::say("I do hope you enjoy, sebilis");
quest::movepc(89,0,250,44,128.2);
return;
}


} 
