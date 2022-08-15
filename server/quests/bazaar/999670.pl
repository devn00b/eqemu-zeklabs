sub EVENT_SAY {

 if($text=~/hail/i && $ulevel >19){
  quest::say("Greetings! Can you feel that? It feels like a hole in space/time is going to appear. I wonder if I open a [portal] where it would lead?");
  }
else {
} 

 if($text=~/hail/i && $ulevel >59){
  quest::say(" ");
  quest::say("I Knew you would come back. You felt it too didnt you? Another hole has opened up. Do you want to try [another] portal and see where it will lead?");
  }
else {

if($ulevel <20){
quest::say("I'm sorry $name, $ulevel is too low you have to be level 20 to even bother talking to me.");
}
}

 if ($text=~/portal/i && $ulevel >19) {
   quest::say("I cannot promise where you will land. I'm sure it will change from time to time. If you are [willing] to go just let me know. Or I can open a [group] portal if you wish. Either way be ready as I'll port you now!");
  } else {
#quest::say("I'm sorry $name, $ulevel is too low you have to be level 20 to even bother talking to me.");
}


 if ($text=~/willing/i && $ulevel >19) {
   quest::say("Here we go! And good luck!!");
   quest::movepc(111, 200, 120, -2.2, 0.0);
  } else {
#quest::say("I'm sorry $name, $ulevel is too low you have to be level 20 to even bother talking to me.");
}


 if ($text=~/group/i && $ulevel >19) {
   quest::say("Here we go! And good luck!!");
   quest::movegrp(111, 200, 120, -2.2);
  } else {
#quest::say("I'm sorry $name, $ulevel is too low you have to be level 20 to even bother talking to me.");
}

 if ($text=~/another/i && $ulevel >59) {
   quest::say("Lets do this!!!!");
   quest::movepc(212, 1, -2917, -766.1);
  } else {
#quest::say("I'm sorry $name, $ulevel is too low you have to be level 20 to even bother talking to me.");
}

} 
