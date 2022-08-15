sub EVENT_SPAWN
{
$npc->SetSlotTint(1, 106,0,0); #1 for chest
}

sub EVENT_DEATH
{
quest::say("Natural selection at work.");
}