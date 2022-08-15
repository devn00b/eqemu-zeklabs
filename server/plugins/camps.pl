use POSIX;

sub campdestroyed {
	my $teamid = shift;
	my $npc = shift;
	my $client = shift;
	my $result_npc_id = 0;
	my $charLevel = $client->GetLevel();
if ( $charLevel <= 30 )
{
	#lvl 25-35 mobs
#	$result_npc_id = quest::ChooseRandom(999581,999582,999583,999584);
}
elsif ( $charLevel <= 40 )
{
	#lvl 35-45 mobs
#	$result_npc_id = quest::ChooseRandom(999585,999586,999587,999588);
}
elsif ( $charLevel <= 50 )
{
	#lvl 45 mobs
#	$result_npc_id = quest::ChooseRandom(999588,999589,999590,999591);
}
elsif ( $charLevel <= 55 )
{
	#lvl 45-55 mobs
#	$result_npc_id = quest::ChooseRandom(999593,999594,999595,999596);
}
elsif ( $charLevel < 60 )
{
	#lvl 55-65 mobs
#	$result_npc_id = quest::ChooseRandom(999593,999594,999595,999596,999597,999598,999599);
}
elsif ( $charLevel >= 60 )
{
	#lvl 65 mobs
#	$result_npc_id = quest::ChooseRandom(221008,999597,999598,999599,999597,999598,999599,999593,999594,999595,999596);
} 

if ( $result_npc_id > 0 )
{
$x = quest::getteamlocation($teamid,"x");
$y = quest::getteamlocation($teamid,"y");
$z = quest::getteamlocation($teamid,"z");
$h = 0;

#quest::spawn2($result_npc_id,0,0,$x,$y,$z,$h);
$name = $client->GetName();
#quest::we(13,"Destruction of the guild encampment by $name has resulted in a contestable mob!");
}
}

sub checkbestz {
	my $x = shift;
	my $y = shift;
	my $z = shift;
	my $xOffset = shift;
	my $yOffset = shift;
	$bestZ = quest::GetBestZ($x+$xOffset,$y+$yOffset,$z);
	return $bestZ;
}

sub iscampsecure {
	my $npc = shift;
	my $kingdomid = shift;
	my $x = shift;
	my $y = shift;
	my $z = shift;
	my $campWidth = shift;
	my $campHeight = shift;
	my $cellSpacing = shift;
	my $secondRunSpawn = shift;
$succeed = 1;
$itr = 0;
for (my $xOffset=-$campWidth;$xOffset<$campWidth+1;$xOffset+=$cellSpacing)
{
for (my $yOffset=-$campHeight;$yOffset<$campHeight+1;$yOffset+=$cellSpacing)
{
$bestZ = plugin::checkbestz($x,$y,$z,$xOffset,$yOffset)+1.0;
$newX = $x + $xOffset;
$newY = $y + $yOffset;

$itr += 1;

if ( $secondRunSpawn == 0 )
{
if ( $bestZ < -999000 )
{
quest::say("[MAP CANNOT ESTABLISH Z COORDINATE] $itr BestZ is: $bestZ for x = $newX and y = $newY");
}
else
{
quest::say("$itr BestZ is: $bestZ for x = $newX and y = $newY, origZ = $z");
}
}
else
{
$npcid = 999578;

if ( $itr == 5 )
{
#translocator/banker
$npcid = 999580;
}
$npcname = "CAMPGUARD-" . $npcid . "-" . ceil($newX) . "-" . ceil($newY);


$spawngroup = quest::spawninnpc($kingdomid, $npcname, $npcid, $newX, $newY, $bestZ, 0.0, 600);

#quest::spawn2($npcid,0,0,$newX,$newY,$bestZ);
}

if ( $bestZ == -999999 )
{
	$succeed = 0;
}
elsif ( $secondRunSpawn == 0 )
{
$canSee = $npc->CheckLoSToLoc($newX,$newY,$bestZ);
if ( $canSee == 0 ) 
{
quest::say("guard $itr cannot be seen from this location.");
return 0;
}
}

}
}
if ( $succeed == 0 )
{
return 0;
}

return 1;

}
