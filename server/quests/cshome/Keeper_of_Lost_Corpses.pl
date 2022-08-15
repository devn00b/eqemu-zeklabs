sub EVENT_SAY {
$charid = 0;
$corpse = 0;
$charid = $client->CharacterID();
$x = $npc->GetX();
$y = $npc->GetY();
$z = $npc->GetZ();

if($text=~/Hail/i) {
   $client->Message(4, "The spectral figure stares into your soul with 
cold eyes, its voice entering your mind. We keep all that has been lost. In times past, nothing be returned once it was mine. Time have changed. The bodies, your [bodies], that were lost to you can now be returned.");
}
if($text=~/bodies/i) {
	$var = 1;
	$count = 0;
	while($var == 1 )
	{
	$count += 1;
	if ( $count > 10 ) { quest::say("There are too many corpses to retrieve right now try again in a moment."); return; }
	$var = quest::summonburriedplayercorpse($charid, $x, $y, $z, 0);
	if ( $var == 0 ) { 
		$nextCount = quest::getnextcorpsedecaytime($charid);
		if ( $nextCount > 0 && $nextCount < 1800 ) {
		$min = int(((1800 - $nextCount)/60))+1;
		quest::say("I cannot recover any further corpses for you right now, another should be available in about $min minute(s).");
		}
		else
		{
		quest::say("You have no further corpses at this time.");
		}
	} 

	}
}

}