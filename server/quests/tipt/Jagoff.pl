sub EVENT_SAY
{

if($text=~/Hail/i){
$var = quest::saylink("team experience bonus");

quest::say("Hello! Are you looking for $var information?");
}

if($text=~/team experience bonus/i){

my $evilBonus = quest::getteamexpbonus(2);
my $goodBonus = quest::getteamexpbonus(3);

$evilPaid = "<c \"#00FF00\">OK</c>";
$goodPaid = "<c \"#00FF00\">OK</c>";

my $evilUpkeepPaid = quest::getisupkeeppaid(2);
my $goodUpkeepPaid = quest::getisupkeeppaid(3);

if ( $evilBonus < 1 ) { $evilPaid = "<c \"#FCFF00\">Kingdom Under Attack</c>"; }
if ( $goodBonus < 1 ) { $goodPaid = "<c \"#FCFF00\">Kingdom Under Attack</c>"; }

if ( $evilUpkeepPaid < 1 ) { $evilPaid = "<c \"#FF0000\">Upkeep Not Paid</c>"; }
if ( $goodUpkeepPaid < 1 ) { $goodPaid = "<c \"#FF0000\">Upkeep Not Paid</c>"; }

quest::popup("Team Information","<c \"#FF0000\">Team XP Bonus Rate</c><br>Evil Team: $evilBonus% - ($evilPaid)<br>Good Team: $goodBonus% - ($goodPaid)<br>",0,0,120);
}
}