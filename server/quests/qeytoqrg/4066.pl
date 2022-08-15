sub EVENT_SAY { 
if ( $client->GetPVPFlag() == 1 ) #check against pvp flag if its qeynos team
{
if($text=~/Hail/i){
my $link = quest::saylink("status");
quest::say("Hello $name, are you here for a guard $link check?");
} #end hail

if($text=~/status/i){
$hp = quest::getguardfield(1,4066, "hp");
$mana = quest::getguardfield(1,4066, "mana");
$hpregen = quest::getguardfield(1,4066, "hpregen");
$manaregen = quest::getguardfield(1,4066, "manaregen");
$levels = quest::getguardfield(1,4066, "level");
$ac = quest::getguardfield(1,4066, "ac");
quest::popup("NPC Guard Status","<c \"#479eab\">NPC Guard Card Bonuses</c><br>  <c \"#4fd613\">HP</c>: $hp<br> <c \"#4fd613\">Mana</c>: $mana<br> <c \"#4fd613\">HP Regen</c>: $hpregen<br><c \"#4fd613\">Mana Regen</c>: $manaregen<br> <c \"#4fd613\">AC</c>: $ac<br> <c \"#4fd613\">Additional Levels</c>: $levels<br>",0,0,120);
} #end status

} #end friendly

} #end say

sub EVENT_DEATH
{
#since the npc died we have to reset the values
quest::resetguardfields(1,4066);
}

sub EVENT_SPAWN
{
	quest::loadguardtype(1,4066);
}