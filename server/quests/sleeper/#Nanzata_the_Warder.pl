sub EVENT_DEATH {
$ventani = $entity_list->GetMobByNpcTypeID(128091);
$tukaarak = $entity_list->GetMobByNpcTypeID(128092);
$hraashna = $entity_list->GetMobByNpcTypeID(128093);
if (!$ventani && !$tukaarak && !$hraashna) {
quest::signalwith(128094,66,0);
quest::we(13,"Nanzata the Warder shouts \"Warders, I have fallen. Prepare yourselves, these fools are determined to unleash doom!\"");
}
else { 
quest::we(13,"Nanzata the Warder shouts \"Warders, I have fallen. Prepare yourselves, these fools are determined to unleash doom!\"");
}
 }
