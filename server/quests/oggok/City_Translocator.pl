sub EVENT_SAY { 
$flag = $qglobals{teleportflag};
plugin::citytranslocator_say($npc,19,$client,$text,$flag,$zonemin);
} #end say
