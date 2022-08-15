sub EVENT_SAY { 
$flag = $qglobals{teleportflag};
plugin::citytranslocator_say($npc,4,$client,$text,$flag,$zonemin);
} #end say
