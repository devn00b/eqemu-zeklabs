sub EVENT_DEATH{
 my $random_result = int(rand(100));
 my $a=14139; #npc - Quillmane
# my $quill = $qglobals{quill};

 if(($random_result<5)){
#   quest::say("Spawning Quillmane1");
   quest::unique_spawn($a,0,0,3210,-6821.6,38.3,193.3);
   quest::delglobal("quill");
   quest::setglobal("quill",3,3,"F");
   $quill=undef;
  }elsif(($random_result>=5) && ($random_result<10)){
#     quest::say("Spawning Quillmane2");
     quest::unique_spawn($a,0,0,-2761.8,-6076,0.7,4.0);
     quest::delglobal("quill");
     quest::setglobal("quill",3,3,"F");
     $quill=undef;
  }elsif(($random_result>=10) && ($random_result<15)){
#     quest::say("Spawning Quillmane3");
     quest::unique_spawn($a,0,0,-2333.8,1296.5,38.7,247.3);
     quest::delglobal("quill");
     quest::setglobal("quill",3,3,"F");
     $quill=undef;
  }else{
#    quest::say("No spawn roll was $random_result Global is $quill");
  }
}

