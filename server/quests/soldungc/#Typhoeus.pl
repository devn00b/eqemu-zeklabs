sub EVENT_SPAWN
{

$rnd = quest::ChooseRandom(0,4341,4342,4343,4344,4345,4346,4407,10032,22503);
$npc->AddItem($rnd,1);

}