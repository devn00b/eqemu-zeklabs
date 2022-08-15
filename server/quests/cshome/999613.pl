sub EVENT_SAY {
my $ptz = $client->GetPVPPoints();

 if($text=~/hail/i && $ulevel >19){
#  quest::say("Greetings! Can you feel that? It feels like a hole in space/time is going to appear. I wonder if I open a [portal] where it would lead?");
quest:say("test: $ptz");
  }
else {
} 
if($ulevel <20){
quest::say("I'm sorry $name, $ulevel is too low you have to be level 20 to even bother talking to me.");
}
}

 if ($text=~/portal/i && $ptz <1000) {
#my $pts = $client->GetPVPPoints();

   quest::say("There is no telling what has caused this energy to converge like this. What I do know is only those that are weak may enter. So, If you wish to see what is contained within just say '[I am weak]' and I will open a portal.");
  } else {
#quest::say("I'm sorry $name, $ulevel is too low you have to be level 20 to even bother talking to me.");
}

if ($text=~/i am weak/i && $ulevel >19 && $client->GetPVPPoints() <1000) {
quest::say("Well here you go weakling $name");
}

 if ($text=~/group/i && $ulevel >19) {
   quest::say("Here we go! And good luck!!");
   quest::movegrp(111, 200, 120, -2.2);
  } else {
#quest::say("I'm sorry $name, $ulevel is too low you have to be level 20 to even bother talking to me.");
}

} 
