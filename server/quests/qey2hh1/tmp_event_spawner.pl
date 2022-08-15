use feature qw/switch/; 

sub getPosition
{
@arrayLoc = undefined;
#max is when(x+1), eg when(2) will be 2+1=3.
$max = 16;

$rnd = int(rand($max));
given($rnd){
  when(0) {
   @arrayLoc = (-856.3, -8.7, -3.2, 190.0);
}
  when(1) {
   @arrayLoc = (-883.5, 45.0, -3.2, 188.1);
}
  when(2) {
   @arrayLoc = (-807.0, -14.2, -3.2, 98.4);
}
  when(3) {
   @arrayLoc = (-705.9, 46.1, -3.2, 177.0);
}
  when(4) {
   @arrayLoc = (-605.3, -14.2, -3.2, 189.0);
}
  when(5) {
   @arrayLoc = (-589.6, 37.0, -3.2, 185.4);
}
  when(6) {
   @arrayLoc = (-518.7, 32.2, -3.2, 150.0);
}
  when(7) {
   @arrayLoc = (-494.1, -66.8, -3.2, 233.2);
}
  when(8) {
   @arrayLoc = (-435.0, -40.0, -3.2, 212.9);
}
  when(9) {
   @arrayLoc = (-451.5, -140.6, -3.2, 243.6);
}
  when(10) {
   @arrayLoc = (-1033.3, -22.2, -3.2, 192.0);
}
  when(11) {
   @arrayLoc = (-1033.3, 44.5, -3.2, 192.0);
}
  when(12) {
   @arrayLoc = (-1120.9, 44.5, -3.2, 192.0);
}
  when(13) {
   @arrayLoc = (-1163.7, -23.2, -3.2, 192.0);
}
  when(14) {
   @arrayLoc = (-1278.5, 48.4, -3.2, 190.0);
}
  when(15) {
   @arrayLoc = (-1329.6, -47.6, 1.5, 190.0);
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
$npcid = 999626;
quest::spawn2($npcid,0,0,$x,$y,$z,$h);
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
quest::settimer("refreshnotice",10);
}

sub EVENT_TIMER
{
if ( $timer eq "refreshnotice" )
{
if ( !defined($qglobals{akanon_racestarted}) )
{
#quest::say("no event started");
}
if ( defined($qglobals{akanon_racestarted}))
{

	if ( !defined($entity_list->GetNPCByNPCTypeID(999626)) )
	{
		eventIssued();
		eventIssued();
		eventIssued();
	}
	else
	{
	$count = 0;
        my @nlist = $entity_list->GetNPCList();
        foreach my $n (@nlist){
		if ( $n->GetNPCTypeID() == 999626 )
		{
			$count += 1;
		}
	}
	if ( $count < 5 )
	{
		eventIssued();
	}
	}
}
}

}
