use feature qw/switch/; 
$alternatePop = 0;
sub getPosition
{
@arrayLoc = undefined;

given($alternatePop){
  when(0) {
   @arrayLoc = (335.1, -199.1, 2.5, 217.8, 12);
}
  when(1) {
   @arrayLoc = (442.4, 483.1, 17.8, 130.6, 13);
}

$alternatePop += 1;

if ( $alternatePop > 1 )
{
$alternatePop = 0;
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
$grid = $pos[4];
$npcid = 999627;
quest::spawn2($npcid,$grid,0,$x,$y,$z,$h);
}

sub EVENT_SAY
{
if ( $client->GetGM() )
{
quest::say("Hello");
}

}

sub EVENT_SPAWN
{
$alternatePop = 0;
quest::settimer("refreshnotice",10);
}

sub EVENT_TIMER
{
if ( $timer eq "refreshnotice" )
{
if ( !defined($qglobals{akanon_racestarted}) )
{
	if ( defined($entity_list->GetNPCByNPCTypeID(999631)) ) #ping fuzzlecutter
	{
		quest::depop(999631);
	}
	if ( defined($entity_list->GetNPCByNPCTypeID(999627)) )
	{
		quest::depop(999627);
	}
#quest::say("no event started");
}
if ( defined($qglobals{akanon_racestarted}))
{
	if ( !defined($entity_list->GetNPCByNPCTypeID(999631)) ) #ping fuzzlecutter
	{
		quest::spawn2(999631,0,0,-799.8,512.2,20.2,137.0);
	}
	if ( !defined($entity_list->GetNPCByNPCTypeID(999627)) )
	{
		eventIssued();
	}
}
}

}
