# NPC: a_desert_madman

sub EVENT_AGGRO {
  my $random_result = int(rand(100));
  if($random_result <= 33) {
    quest::say("Damn my eyes! You're just another mirage!");
  }
  elsif($random_result <= 66) {
    quest::say("$race\'s like you are better left dead than alive.");
  }
  else {
    quest::say("It's $race\'s like you who have ruined your own lands, You'll not ruin mine!");
  }
}


#Cazel spawn
#npc - a desert madman - randomly spawns cazel round the zone
#zone - oasis
#devn00b

sub EVENT_DEATH{
 my $random_result = int(rand(100));
 my $a=37157; #npc - cazelmane
 my $b=37157; #npc - Fabled cazelmane
 my $cazel = int(rand(100));

 if(($random_result<=5) && ($cazel<=20)){
#   quest::say("Spawning Fabled1");
   quest::unique_spawn($b,0,0,1290.7,654.3,5.5,176.1);
   quest::delglobal("cazel");
   quest::setglobal("cazel",3,3,"F");
   $cazel=undef;
 }elsif(($random_result>=6) && ($random_result<=10) && ($cazel<=20)){
#   quest::say("Spawning cazelmane1");
   quest::unique_spawn($a,0,0,-747.3,429.7,5.5,70.4);
   quest::delglobal("cazel");
   quest::setglobal("cazel",3,3,"F");
   $cazel=undef;
  }elsif(($random_result>=11) && ($random_result<=20)&& ($cazel<=20)){
#    quest::say("Spawning cazelmane2");
     quest::unique_spawn($a,0,0,-64.9,-1797.3,5.5,20.6);
     quest::delglobal("cazel");
     quest::setglobal("cazel",3,3,"F");
     $cazel=undef;
  }elsif(($random_result>=21) && ($random_result<=30)&& ($cazel<=20)){
#     quest::say("Spawning cazelmane3");
     quest::unique_spawn($a,0,0,-729.4,-1031.5,5.5,14.2);
     quest::delglobal("cazel");
     quest::setglobal("cazel",3,3,"F");
     $cazel=undef;
  }elsif(($random_result>=31) && ($random_result<=36)&& ($cazel<=20)){
#     quest::say("Spawning Fabled3");
     quest::unique_spawn($b,0,0,294.4,1379.3,5.5,144.0);
     quest::delglobal("cazel");
     quest::setglobal("cazel",3,3,"F");
     $cazel=undef;
  }else{
#    quest::say("No spawn");
  };
};

# OASIS
