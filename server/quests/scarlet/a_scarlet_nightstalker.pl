my $combatstate = 0;

sub EVENT_SPAWN
{

$rnd = quest::ChooseRandom(13401,2300,1138,1139,1140,1152,0,0,0,0,0,0,0,0,0,0);

$npc->AddItem($rnd,1);

quest::settimer("check",1);
$entid = $npc->GetID();
$npc->CastSpell(80, $entid,10,0);
}

sub EVENT_COMBAT
{
quest::stoptimer("check");
quest::settimer("check",1);

$combatstate = $combat_state;
	if ( $combat_state == 0 )
	{
		$entid = $npc->GetID();
		$npc->CastSpell(80, $entid,10,0);
	}
}

sub EVENT_TIMER
{
$hasInvis = $npc->FindBuff(928);

if ( $hasInvis == 0 && $combatstate == 0 )
{
$entid = $npc->GetID();
$npc->CastSpell(928, $entid,10,0);
}
}
