sub EVENT_DEATH

{

   $x = $npc->GetX();

   $y = $npc->GetY();

   $z = $npc->GetZ();

   $sirran= undef;

   
   quest::setglobal("sirran",5,3,"M10");
#   plugin::handlenpc_epicloot($client,$npc,1);
#   plugin::handlenpc_epic15loot($client,$npc,1);
#   plugin::handlenpc_epic15loot($client,$npc,1);
   quest::spawn(71058,0,0,$x,$y,$z);
} 
