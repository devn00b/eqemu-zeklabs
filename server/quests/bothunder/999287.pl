sub EVENT_DEATH
{
#item template 92,95,96,97
$npc->AddLootGroup(45);
quest::setglobal("infernoportalsopen",1,3,"D1"); #when the portal is open, global is set true for 1 day.
}