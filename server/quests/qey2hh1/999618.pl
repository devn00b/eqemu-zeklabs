use feature qw/switch/; 

sub getPosition
{
@arrayLoc = undefined;
#max is when(x+1), eg when(2) will be 2+1=3.
$max = 3;

$rnd = int(rand($max));
given($rnd){
  when(0) {
	@arrayLoc = (-5609.3, -914.3, 22.1, 195.0);
}
  when(1) {
	@arrayLoc = (-8723.2, -3088.9, 18.3, 26.1);
}
  when(2) {
	@arrayLoc = (-14758.6, -443.0, -0.2, 221.2);
}
}

return @arrayLoc;
}

sub eventIssued
{
@pos = getPosition();

$x = $pos[0];
$y = $pos[1];
$z = $pos[2];
$h = $pos[3];
$npcid = plugin::spawnevent_mob($x,$y,$z,$h);
quest::setglobal("worldeventcooldown","1",7,"M10");
quest::setglobal("worldeventnpcspawned",$npcid,7,"M60");
quest::setglobal("worldeventmobcooldown","1",7,"M55");
quest::settimer("refreshnotice",300);
quest::shout2("A contestable mob has appeared in $zoneln!");
}

sub EVENT_SAY
{
if ( $client->GetGM() )
{
$zoneFlag = $qglobals{"worldeventzoneflag"};
$worldNPC = $qglobals{"worldeventmobcooldown"};
$npcID = $qglobals{"worldeventnpcspawned"};
$eventCooldown = $qglobals{"worldeventcooldown"};

quest::say("WorldNPCID: $npcID, NPC cooldown: $worldNPC, zoneID: $zoneFlag, event cooldown: $eventCooldown");
}
#$zoneFlag = $qglobals{"worldeventzoneflag"};
#quest::say("$zoneFlag");
#quest::setglobal("worldeventcooldown","0",7,"M10");
#quest::setglobal("worldeventzoneflag","37",7,"M5");
}

sub EVENT_SPAWN
{
quest::settimer("worldevent",60);
}

sub EVENT_TIMER
{
$eventCooldown = $qglobals{"worldeventmobcooldown"};

if ( $timer eq "refreshnotice" )
{
if ( defined $qglobals{"worldeventnpcspawned"} && $qglobals{"worldeventnpcspawned"} > 0 )
{
$npcid = $qglobals{"worldeventnpcspawned"};
if ( defined($entity_list->GetNPCByNPCTypeID($npcid)))
{
quest::shout2("A contestable mob is still available to contest in $zoneln!");
}
}
}


if ( $timer eq "worldevent" )
{

if ( !defined $qglobals{"worldeventmobcooldown"} )
{
if ( defined $qglobals{"worldeventnpcspawned"} )
{
$npcid = $qglobals{"worldeventnpcspawned"};
if ( $npcid > 0 && defined($entity_list->GetNPCByNPCTypeID($npcid)))
{
quest::shout2("The contestable mob in $zoneln has fled!");
quest::setglobal("worldeventnpcspawned","0",7,"M60");
quest::stoptimer("refreshnotice");
quest::depop($npcid);
plugin::worldmob_resetzone();
}
}
}

if ( defined $qglobals{"worldeventcooldown"} && $qglobals{"worldeventcooldown"} > 0 )
{
# no action cooldown is active
return;
}
if ( defined $qglobals{"worldeventzoneflag"} )
{
$zoneFlag = $qglobals{"worldeventzoneflag"};
if ( $zoneFlag > 0 && $zoneFlag == $zoneid )
{
quest::setglobal("worldeventzoneflag","0",7,"M5");
eventIssued();
}
}
}

}
