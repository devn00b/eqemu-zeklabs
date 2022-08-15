sub EVENT_SPAWN
{
$gotloot = int(rand(100));
if($gotloot <40){
$rnd = quest::ChooseRandom(135119,31859,31858,31757,34040);
$npc->AddItem($rnd,1);
return;
}
}

