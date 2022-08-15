sub EVENT_SPAWN
{
$npc->AddItem(142740,2,"0");

}

sub EVENT_DEATH
{
if ( defined $client && (defined $qglobals{akanon_race_playerflag} || defined $qglobals{akanon_racefinish_playerflag}) && $client->HasItem(142754) == -1)
{
$client->SummonItem(142754);
}

quest::emote("has it's essence fade away.");
$npc->SetTexture(1);
}