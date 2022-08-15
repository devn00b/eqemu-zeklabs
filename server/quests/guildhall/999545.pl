sub EVENT_SPAWN
{

$rnd = quest::ChooseRandom(13401,2300,1138,1139,1140,1152,0,0,0,0,0,0,0,0,0,0);

$npc->AddItem($rnd,1);


$entid = $npc->GetID();
$npc->CastSpell(928, $entid,10,0);
}

sub EVENT_COMBAT
{
	if ( $combat_state == 0 )
	{
		$entid = $npc->GetID();
		$npc->CastSpell(928, $entid,10,0);
	}
}