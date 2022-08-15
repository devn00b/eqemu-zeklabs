use Date::Parse;
use feature qw/switch/;

sub getPlace
{
	my $placement = shift;
	my $client = shift;
	my $bestHit = shift;
	my $timeHour = shift;
	my $timeMin = shift;
	my $timeSec = shift;
	my $route = shift;

	$msg = $timeHour . " hour(s), " . $timeMin . " minute(s), " . $timeSec . " second(s).";

	if ( $timeHour < 1 )
	{
		$msg = $timeMin . " minute(s), " . $timeSec . " second(s).";
	}
	my $routeName = "Unknown";
	given($route){
	when(-1) {
		$routeName = "Undetermined";
	}
	when(0) {
		$routeName = "Highpass to East Karana";
	}
	when(1) {
		$routeName = "Gorge to East Karana";
	}
	when(2) {
		$routeName = "Rathe Mountains to Karanas";
	}
	}

	my $returnval = "$placement";
	given($placement){
	when(1) {
		$returnval = "First";
	}
	when(2) {
		$returnval = "Second";
	}
	when(3) {
		$returnval = "Third";
	}
	when(4) {
		$returnval = "Fourth";
	}
	when(5) {
		$returnval = "Fifth";
	}
	when(6) {
		$returnval = "Sixth";
	}
	when(7) {
		$returnval = "Seventh";
	}
	when(8) {
		$returnval = "Eighth";
	}
	when(9) {
		$returnval = "Ninth";
	}
	when(10) {
		$returnval = "Tenth";
	}
	}

	if ( $placement < 2 || $bestHit )
	{
		$client->Message(15,"You have been awarded a Ethereal PVP Ticket!");
		if ( $placement < 2 )
		{
			$client->SummonItem(145653);
		}
		else
		{
			$client->SummonItem(145652);
		}

		$client->SummonItem(142841);
	}
	elsif ( $placement < 3 ) #2nd place
	{
		$client->Message(15,"You have been awarded a Platinum and Titanium PVP Ticket!");
		$client->SummonItem(142799);
		$client->SummonItem(142798);
	}
	elsif ( $placement < 4 ) #3rd place
	{
		$client->Message(15,"You have been awarded a Palladium and Titanium PVP Ticket!");
		$client->SummonItem(142842);
		$client->SummonItem(142798);
	}
	elsif ( $placement < 21 )
	{
		$client->Message(15,"You have been awarded a Titanium PVP Ticket!");
		$client->SummonItem(142798);
	}
	else
	{
		$client->Message(15,"You have been awarded a Platinum PVP Ticket!");
		$client->SummonItem(142799);
	}

	if ( $bestHit == 1 && $placement < 11 )
	{
		$client->Message(14,"Congratulations!!  You were $returnval in the race, you completed the race in $msg!!");
	}
	elsif ( $bestHit == 1 )
	{
		$client->Message(14,"Congratulations!!  You have made the best time in the race with $msg!!  You placed in position $returnval.");
	}
	else
	{
		$client->Message(14,"Congratulations!!  You made position $returnval in the race, you completed the race in $msg!!");
	}

	$name = $client->GetName();
	$frontMsg = "[GNOME RACE]";
	if ( $bestHit == 1 )
	{
		$frontMsg = "[GNOME RACE - BEST COMPLETION TIME!]";
	}
	if ( $placement < 11 )
	{
	quest::we(14,"$frontMsg Congratulations to $name coming in $returnval place!!  Completed race in $msg.  Route used: $routeName.");
	}
	else
	{
	quest::we(14,"$frontMsg Congratulations to $name coming in at position $returnval!  Completed race in $msg.  Route used: $routeName.");
	}
}

sub findTarget {

my $npc = shift;
my $entity_list = shift;
my $count = 0;
        my @nlist = $entity_list->GetNPCList();
        foreach my $n (@nlist){
	if ( ( $n->GetBaseRace() == 23 || $n->GetBaseRace() == 50 ) && $n->CheckLoS($npc) && $n->CalculateDistance($npc->GetX(),$npc->GetY(),$npc->GetZ()) < 75 )
	{
		return $n;
	}
}
return undef;
}

sub EVENT_SPAWN
{
#quest::delglobal("akanon_race_besthour");


#quest::delglobal("akanonrace_placement");
$npc->AddItem(142802,1,"0");
quest::settimer("checkarea",1);
}

sub EVENT_COMBAT
{
if ( defined($target) && ( $target->GetBaseRace() == 23 || $target->GetBaseRace() == 50 ))
{
	if ( $npc->GetHPRatio() < 75 )
	{
	$dmgHeal = $npc->GetMaxHP() - $npc->GetHP();
		if ( $dmgHeal > 0 )
		{
			$npc->HealDamage($dmgHeal);
		}
	}
}
}

sub EVENT_CAST_ON
{
if ( defined($target) && ( $target->GetBaseRace() == 23 || $target->GetBaseRace() == 50 ))
{
	if ( $npc->GetHPRatio() < 75 )
	{
	$dmgHeal = $npc->GetMaxHP() - $npc->GetHP();
		if ( $dmgHeal > 0 )
		{
			$npc->HealDamage($dmgHeal);
		}
	}
}
}
sub EVENT_TIMER
{
if ( $timer eq "checkarea" )
{
$hate = $npc->GetHateRandom();
if ( defined $hate && ( $hate->GetBaseRace() == 23 || hate->GetBaseRace() == 50 ))
{
	if ( $npc->GetHPRatio() < 75 )
	{
	$dmgHeal = $npc->GetMaxHP() - $npc->GetHP();
		if ( $dmgHeal > 0 )
		{
			$npc->HealDamage($dmgHeal);
		}
	}
if ( $hate->GetHP() < 10 )
{
$hate->Damage($npc, 10, 0, 1, false);
}
else
{
$hate->Damage($npc, $hate->GetHP()-1, 0, 1, false);
}

}
elsif ( !defined $hate )
{
	$target = findTarget($npc, $entity_list);
	if ( defined $target)
	{
		$newname = $target->GetName();
		#quest::say("Found $newname");
		if ( $npc->IsOnHatelist($target) == 0 )
		{
			$npc->AddToHateList($target,1000);
		}
		if ( $target->IsOnHatelist($npc) == 0 )
		{
			$target->AddToHateList($npc,1);
		}
	}
}#close elsif
}#close timer checkarea
}#close event timer



sub EVENT_DEATH
{
if ( defined $client && defined $qglobals{akanon_race_playerflag} && $qglobals{akanon_race_playerstep} == 9 && $client->HasItem(142801) == -1)
{
$client->SummonItem(142801);

if ( defined $qglobals{akanon_racefinish_playerflag} )
{
$client->Message(14,"You have already finished the race!");
return;
}

quest::delglobal("notranslocateallowed");
quest::delglobal("noteleportallowed");
quest::delglobal("nogroupallowedflag");
quest::delglobal("nobeneficialbuffsallowed");
quest::delglobal("nobindallowed");
quest::delglobal("notradeallowed");
quest::delglobal("akanon_race_playerflag");

$placement = $qglobals{akanonrace_placement};
if ( !defined $placement )
{
$placement = 0;
}

$placement += 1;

quest::setglobal("akanonrace_placement",$placement,7,"F");
quest::setglobal("akanon_race_justfinished",1,5,"M10");

quest::setglobal("akanon_racefinish_playerflag",$placement,5,"F");
$endtime = localtime(time);
quest::setglobal("akanon_race_endtime",$endtime,5,"F");

$starttime = $qglobals{akanon_race_starttime};
$diff = str2time($endtime) - str2time($starttime);

$diffMin = int($diff/60);
$diffhour = int($diffMin/60);
$diffSec = int($diff - $diffMin*60);
$diffMin = int($diffMin - $diffhour*60);

$bestHour = $qglobals{akanon_race_besthour};
$bestMin = $qglobals{akanon_race_bestmin};
$bestSec = $qglobals{akanon_race_bestsec};

$bestHit = 0;
$route = 0;

if ( defined $qglobals{akanon_race_approach} )
{
$route = $qglobals{akanon_race_approach};
}

if ( $route == 0 && ( !defined $bestHour || (defined $bestHour && ( ($diffhour <= $bestHour && $diffMin <= $bestMin && $diffSec < $bestSec) || ($diffhour < $bestHour && $diffMin >= $bestMin) || ($diffhour <= $bestHour && $diffMin < $bestMin && $diffSec >= $bestSec) ) ) ) )
{
quest::setglobal("akanon_race_besthour",$diffhour,7,"F");
quest::setglobal("akanon_race_bestmin",$diffMin,7,"F");
quest::setglobal("akanon_race_bestsec",$diffSec,7,"F");
quest::setglobal("akanon_race_bestroute",$route,7,"F");
$pname = $client->GetName();
quest::setglobal("akanon_race_bestname",$pname,7,"F");
quest::setglobal("akanon_race_bestcharid",$client->CharacterID(),7,"F");
$bestHit = 1;
}

$bestHour = $qglobals{akanon_race_besthourgorge};
$bestMin = $qglobals{akanon_race_bestmingorge};
$bestSec = $qglobals{akanon_race_bestsecgorge};

if ( $route == 1 && ( !defined $bestHour || (defined $bestHour && ( ($diffhour <= $bestHour && $diffMin <= $bestMin && $diffSec < $bestSec) || ($diffhour < $bestHour && $diffMin >= $bestMin) || ($diffhour <= $bestHour && $diffMin < $bestMin && $diffSec >= $bestSec) ) ) ) )
{
quest::setglobal("akanon_race_besthourgorge",$diffhour,7,"F");
quest::setglobal("akanon_race_bestmingorge",$diffMin,7,"F");
quest::setglobal("akanon_race_bestsecgorge",$diffSec,7,"F");
quest::setglobal("akanon_race_bestroutegorge",$route,7,"F");
$pname = $client->GetName();
quest::setglobal("akanon_race_bestnamegorge",$pname,7,"F");
quest::setglobal("akanon_race_bestcharidgorge",$client->CharacterID(),7,"F");
$bestHit = 1;
}

$bestHour = $qglobals{akanon_race_besthourrathe};
$bestMin = $qglobals{akanon_race_bestminrathe};
$bestSec = $qglobals{akanon_race_bestsecrathe};

if ( $route == 2 && ( !defined $bestHour || (defined $bestHour && ( ($diffhour <= $bestHour && $diffMin <= $bestMin && $diffSec < $bestSec) || ($diffhour < $bestHour && $diffMin >= $bestMin) || ($diffhour <= $bestHour && $diffMin < $bestMin && $diffSec >= $bestSec) ) ) ) )
{
quest::setglobal("akanon_race_besthourrathe",$diffhour,7,"F");
quest::setglobal("akanon_race_bestminrathe",$diffMin,7,"F");
quest::setglobal("akanon_race_bestsecrathe",$diffSec,7,"F");
quest::setglobal("akanon_race_bestrouterathe",$route,7,"F");
$pname = $client->GetName();
quest::setglobal("akanon_race_bestnamerathe",$pname,7,"F");
quest::setglobal("akanon_race_bestcharidrathe",$client->CharacterID(),7,"F");
$bestHit = 1;
}


getPlace($placement,$client,$bestHit,$diffhour,$diffMin,$diffSec,$route);
quest::exp(9000000);
$client->Message(14,"Be sure to speak to Ping Fuzzlecutter of your accomplishment either in Kerra Ridge or Nexus!!!  YOU MUST LEAVE THE ZONE TO TOX TO BE ABLE TO PORT TO NEXUS.  YOU HAVE 10 MINUTES TO LEAVE THE ZONE BEFORE BEING ATTACKED!!");
}
}


sub EVENT_SAY
{

}