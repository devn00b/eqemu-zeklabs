sub EVENT_TIMER
{
	if ( $timer eq "fear" )
	{
		#make him run!
		$npc->CastSpell(229,$npc->GetID(),10,0,0);
	}
}

sub EVENT_SPAWN
{
	quest::settimer("fear",1);
}