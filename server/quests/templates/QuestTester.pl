sub EVENT_SAY
{
$lvl = quest::GetRule("PVPCombat:WorldPoints");
quest::say("WorldPts: $lvl");
if ( $lvl eq 'true' )
{
quest::say("it is so!");
}
}