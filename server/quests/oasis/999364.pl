sub EVENT_SPAWN
{
quest::settimer("despawn",600);
quest::settimer("loot",1);
}

sub EVENT_TIMER
{
if ( $timer eq "loot" )
{
quest::addloot(6576);
$rnd = quest::ChooseRandom(13073,13073,13073,10019);
$rnd2 = quest::ChooseRandom(13073,13073,13073,12729);
quest::addloot($rnd,1,false);
quest::addloot($rnd2,1,false);

quest::addloot(13073,1,false);
quest::stoptimer("loot");
}
if ( $timer eq "despawn" )
{
quest::stoptimer("despawn");
$npc->Depop();
}

}

sub EVENT_COMBAT
{
if ( $combat_state == 1 )
{
quest::stoptimer("despawn");
}
else
{
quest::settimer("despawn",600);
}
}