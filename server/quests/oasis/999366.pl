sub EVENT_SPAWN
{
quest::settimer("spawn",1);
quest::settimer("spawnboss",1);
}

sub EVENT_TIMER
{
if ( $timer eq "spawn" )
{
quest::rain(1);
quest::stoptimer("spawn");
quest::settimer("spawn",600);
quest::spawn2(999364,102,0,-1084,1317,-31.8,180.5);
quest::spawn2(999364,102,0,-1084,1317,-31.8,180.5);
quest::spawn2(999364,103,0,-1054,1379,-32,180.5);
quest::spawn2(999364,102,0,-1049.9,1404,-32,180.5);
quest::spawn2(999364,103,0,-1052, 1432, -32,180.5);
quest::spawn2(999364,103,0,-1052, 1432, -32,180.5);

quest::spawn2(999364,102,0,-1024,1317,-31.8,180.5);
quest::spawn2(999364,103,0,-1014,1379,-32,180.5);
quest::spawn2(999364,102,0,-1099.9,1404,-32,180.5);

#npcid,gridid,unused,x,y,z,heading

}
if ( $timer eq "spawnboss" )
{
quest::rain(1);
quest::stoptimer("spawnboss");
quest::settimer("spawnboss",21600);
quest::spawn2(999365,103,0,-1084,1317,-31.8,180.5);
#npcid,gridid,unused,x,y,z,heading

}
}