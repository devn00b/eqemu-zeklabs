sub EVENT_DEATH {
$nanzata = $entity_list->GetMobByNpcTypeID(128090);
$ventani = $entity_list->GetMobByNpcTypeID(128091);
$hraashna = $entity_list->GetMobByNpcTypeID(128093);
if (!$nanzata && !$ventani && !$hraashna) {
quest::signalwith(128094,66,0);
quest::we(13,"Tukaarak the Warder shouts \"Warders, I have fallen. Prepare yourselves, these fools are determined to unleash doom!\"");
}
else { 
quest::we(13,"Tukaarak the Warder shouts \"Warders, I have fallen. Prepare yourselves, these fools are determined to unleash doom!\"");
}
 }
