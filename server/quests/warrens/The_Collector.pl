sub EVENT_SPAWN
{
$rand = quest::ChooseRandom(1,1,2,2,2,2,2,3,3);
for($i = 0; $i < $rand; $i++)
{
$rnd = quest::ChooseRandom(69240,69241,69242,69243,69244,69245,69246,69247,69248,69249);
$npc->AddItem($rnd,1);
}

}

sub EVENT_DEATH
{
quest::ze(13,"[Warrens] Collector items can be turned in to The Preservationist in the Bazaar arena.");
}