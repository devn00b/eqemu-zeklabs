sub EVENT_SAY { 
if ( $client->GetPVPFlag() == 3 ) #check against pvp flag if its qeynos team
{
if($text=~/Hail/i){
my $link = quest::saylink("status");
quest::say("Hello $name, are you here for a guard $link check?");
} #end hail

if($text=~/status/i){
$hp = quest::getguardfield(3,10143, "hp");
$mana = quest::getguardfield(3,10143, "mana");
$hpregen = quest::getguardfield(3,10143, "hpregen");
$manaregen = quest::getguardfield(3,10143, "manaregen");
$levels = quest::getguardfield(3,10143, "level");
$ac = quest::getguardfield(3,10143, "ac");
quest::popup("NPC Guard Status","<c \"#479eab\">NPC Guard Card Bonuses</c><br>  <c \"#4fd613\">HP</c>: $hp<br> <c \"#4fd613\">Mana</c>: $mana<br> <c \"#4fd613\">HP Regen</c>: $hpregen<br><c \"#4fd613\">Mana Regen</c>: $manaregen<br> <c \"#4fd613\">AC</c>: $ac<br> <c \"#4fd613\">Additional Levels</c>: $levels<br>",0,0,120);
} #end status

} #end friendly

} #end say

sub EVENT_DEATH
{
#since the npc died we have to reset the values
quest::resetguardfields(3,10143);
}

sub EVENT_SPAWN
{
	quest::loadguardtype(3,10143);
}