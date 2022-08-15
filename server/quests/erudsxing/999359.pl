#bandit guard in mseru
sub EVENT_SPAWN
{
	#set to bandit team
	quest::loadguardtype(5, 999359);
}

sub EVENT_DEATH
{
	quest::resetguardfields(5, 999359);
}