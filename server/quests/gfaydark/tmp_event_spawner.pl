use feature qw/switch/; 

sub getPosition
{
@arrayLoc = undefined;
#max is when(x+1), eg when(2) will be 2+1=3.
$max = 13;

$rnd = int(rand($max));
given($rnd){
  when(0) {
   @arrayLoc = (2351.7, -1667.6, -22.3, 233.8);
}
  when(1) {
   @arrayLoc = (2292.5, -1442.1, -19.6, 120.2);
}
  when(2) {
   @arrayLoc = (2197.8, -1512.0, -0.1, 153.9);
}
  when(3) {
   @arrayLoc = (2198.8, -1686.1, -0.3, 248.1);
}
  when(4) {
   @arrayLoc = (2241.4, -1626.9, -25.2, 241.1);
}
  when(5) {
   @arrayLoc = (2014.3, -1631.8, 26.0, 152.9);
}
  when(6) {
   @arrayLoc = (2098.8, -1764.3, -1.8, 165.2);
}
  when(7) {
   @arrayLoc = (2020.7, -1743.6, 13.0, 233.5);
}
  when(8) {
   @arrayLoc = (2042.8, -1591.6, 22.8, 131.5);
}
  when(9) {
   @arrayLoc = (1973.1, -1646.7, 39.1, 119.5);
}
  when(10) {
   @arrayLoc = (2013.0, -1754.8, 14.6, 190.8);
}
  when(11) {
   @arrayLoc = (2158.7, -1785.8, -7.7, 130.2);
}
  when(12) {
   @arrayLoc = (2312.8, -1784.6, -36.2, 173.9);
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
$npcid = 999625;
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

	if ( !defined($entity_list->GetNPCByNPCTypeID(999625)) )
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
		if ( $n->GetNPCTypeID() == 999625 )
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
