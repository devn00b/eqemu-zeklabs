sub EVENT_SAY {
 if($text=~/hail/i){
  quest::say("Greetings! I can teach young rogues the ADVANCED Techniques of Poison Making.");
  quest::say("Do you wish to [LEARN]");
  }

 if ($text=~/learn/i && $class eq "Rogue") {
   quest::say("Very good, but maybe you should practice more! This is as much as I can teach you.");
   quest::setskill(56,130);
  }

 elsif($text=~/learn/i && $class ne "Rogue"){
 quest::say("What? Your not worthy of my attention. Leave now");
 }
else {
#quest::say("Back off Punk");
}
} 
