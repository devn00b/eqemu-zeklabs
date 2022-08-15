sub EVENT_SPAWN
{
quest::settimer("loot",1);
}

sub EVENT_TIMER
{
if ( $timer eq "loot" )
{
$rnd = quest::ChooseRandom(12729, 10344, 1365, 1296);
if ( $rnd == 10344 ){
quest::addloot($rnd,5,false); }
else
{
quest::addloot($rnd);
}

quest::addloot(13073,1,false);
quest::addloot(13073,1,false);

quest::stoptimer("loot");
}

}