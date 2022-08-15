sub EVENT_SPAWN
{
quest::settimer("L1",1);
}

sub EVENT_TIMER
{
if ($timer eq "L1")
{
$npc->SetAppearance(3);
quest::stoptimer("L1");
quest::settimer("L1",4);
}
}