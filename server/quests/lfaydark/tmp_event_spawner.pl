use feature qw/switch/; 

sub getPosition
{
@arrayLoc = undefined;
#max is when(x+1), eg when(2) will be 2+1=3.
$max = 13;

$rnd = int(rand($max));
given($rnd){
  when(0) {
   @arrayLoc = (-1863.1, 1220.6, 16.8, 213.9);
}
  when(1) {
   @arrayLoc = (-1865.8, 1179.3, 15.1, 169.9);
}
  when(2) {
   @arrayLoc = (-1872, 1458.7, 48.5, 116.5);
}
  when(3) {
   @arrayLoc = (-1712.9, 1180.8, 3.5, 159.8);
}
  when(4) {
   @arrayLoc = (-1534.7, 1544.9, 28.9, 198.8);
}
  when(5) {
   @arrayLoc = (-1248.9, 1807.3, -0.5, 16.5);
}
  when(6) {
   @arrayLoc = (-1280.4, 1638.7, 10.5, 132.4);
}
  when(7) {
   @arrayLoc = (-1208.6, 1300.9, 55.2, 188.1);
}
  when(8) {
   @arrayLoc = (-1275.5, 1211.1, -0.5, 172.5);
}
  when(9) {
   @arrayLoc = (-1414.8, 1138.6, -0.1, 195.8);
}
  when(10) {
   @arrayLoc = (-1634.9, 1167.3, -1.2, 177.0);
}
  when(11) {
   @arrayLoc = (-1789.9, 1762.8, -2.6, 59.8);
}
  when(12) {
   @arrayLoc = (-1716.3, 1639.3, -8.8, 34.2);
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
$npcid = 999624;
quest::spawn2($npcid,85,0,$x,$y,$z,$h);
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

	if ( !defined($entity_list->GetNPCByNPCTypeID(999624)) )
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
		if ( $n->GetNPCTypeID() == 999624 )
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
