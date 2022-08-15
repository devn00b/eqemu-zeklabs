#starts the GM event for gfaydark (orc raiders)
sub EVENT_SAY {
if ( $client->GetGM() )
{
	quest::settimer("spawn",1);
	quest::settimer("stopspawn",300);
quest::say("ok");
}
}

#999281 - orc leader
#58055 - orc oracle
#58037 - orc centurion
#58030 - orc warrior

#grids to 'lead out of tunnel'
#grid 252 (sample) - 254, 256, 257, 259

#grids to path randomly afterwards
#grid 253 (sample) - 255

sub EVENT_TIMER {
if ( $timer eq "stopspawn" )
{
quest::stoptimer("stopspawn");
quest::stoptimer("spawn");
quest::spawn2(58055,256,0,-55.7,2695,19.3,123.5); #oracle
#quest::spawn2(999281,258,0,-45.7,2695,19.3,123.5); #leader
quest::spawn2(58055,254,0,-35.7,2695,19.3,123.5); #oracle

quest::spawn2(58055,257,0,-55.7,2715,19.3,123.5);
quest::spawn2(58055,254,0,-45.7,2715,19.3,123.5);
quest::spawn2(58055,256,0,-35.7,2715,19.3,123.5);
}
elsif ( $timer eq "spawn" ) #orc centurion spawn
{
#do rain here cause setting sky color at the same time borks it
quest::rain(1);
quest::stoptimer("spawn");
quest::settimer("spawn",55);
quest::settimer("spawn2",15);
#rabies werewolves
quest::spawn2(58037,254,0,-55.7,2675,19.3,123.5);
quest::spawn2(58037,256,0,-45.7,2675,19.3,123.5);
quest::spawn2(58037,257,0,-35.7,2675,19.3,123.5);
}
elsif( $timer eq "spawn2" ) #orc centurion spawn
{
quest::stoptimer("spawn2");
quest::settimer("spawn3",15);
quest::spawn2(58037,257,0,-55.7,2675,19.3,123.5);
quest::spawn2(58037,254,0,-45.7,2675,19.3,123.5);
quest::spawn2(58055,259,0,-35.7,2675,19.3,123.5); #oracle
}
elsif( $timer eq "spawn3" )
{
quest::stoptimer("spawn3");
quest::settimer("spawn4",15);
quest::spawn2(58030,254,0,-55.7,2675,19.3,123.5);
quest::spawn2(58030,256,0,-45.7,2675,19.3,123.5);
quest::spawn2(58055,259,0,-35.7,2675,19.3,123.5); #oracle
}
elsif( $timer eq "spawn4" )
{
quest::stoptimer("spawn4");
quest::spawn2(58055,257,0,-55.7,2675,19.3,123.5);
quest::spawn2(58055,259,0,-45.7,2675,19.3,123.5);
quest::spawn2(58055,256,0,-35.7,2675,19.3,123.5);

quest::spawn2(58054,257,0,-55.7,2715,19.3,123.5);
quest::spawn2(58054,254,0,-45.7,2715,19.3,123.5);
quest::spawn2(58054,259,0,-35.7,2715,19.3,123.5);
}

}