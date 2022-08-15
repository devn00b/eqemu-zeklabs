sub EVENT_DEATH
{
quest::ze(13,"Deeeeaaaaaaaathhhhhhh!");
}

sub EVENT_TIMER
{
if ( $timer == 1 )
{
quest::stoptimer(1);
quest::level(6);
}
}

sub EVENT_LEVEL_UP
{
if ( $client->Admin() > 0 )
{
return;
}
if ( $ulevel < 3 )
{
quest::scribespells($ulevel,$ulevel);
}
if ( $ulevel > 5 )
{
quest::echo(4,"You cannot exceed past level 6 in Bunny Island.  Please speak to Destiny in the Bunny Village Compound to head to Norrath.");
}
if ( $ulevel > 6 )
{
quest::echo(4,"You cannot exceed past level 6 and your level has been reset to 6.  See Destiny in the Bunny Village Compound to leave Bunny Island.");
quest::settimer(1,2);
}
}