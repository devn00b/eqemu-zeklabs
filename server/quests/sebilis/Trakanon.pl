sub EVENT_DEATH
{
#plugin::handlenpc_ooglyloot($client,$npc,2);
#plugin::handlenpc_epicloot($client,$npc,2);
#plugin::handlenpc_epicv2loot($client,$npc,1);
#$npc->AddItem(142842,1);
#$npc->AddItem(142842,1);
#$npc->AddItem(142842,1);
#$npc->AddItem(142842,1);
#$npc->AddItem(142842,1);
#$npc->AddItem(142842,1);
#$npc->AddItem(142851,1);
#$npc->AddItem(142852,1);
}

sub EVENT_COMBAT
{
if ( $combat_state == 0 )
{
$npc->Heal();
}
}
