sub EVENT_SAY { 
$flag = $qglobals{teleportflag};
plugin::citytranslocator_say($npc,2,$client,$text,$flag,$zonemin);
} #end say
