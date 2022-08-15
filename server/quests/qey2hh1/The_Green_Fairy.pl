sub EVENT_SPAWN
{
$npc->AddItem(142796,3,"0");
}

sub EVENT_DEATH
{
quest::emote("explodes into dust!");
$npc->CastSpell(1363, $npc->GetID(), 10, 0, 0);
}