sub EVENT_SPAWN
{
plugin::worldmob_spawnhandler($npc,0);
}

sub EVENT_TIMER
{
plugin::worldmob_timerhandler($npc,$timer);
}
sub EVENT_DEATH
{
plugin::worldmob_deathhandler($npc);
}