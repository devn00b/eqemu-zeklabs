use Date::Parse;

sub EVENT_SAY
{

if ( $qglobals{akanon_race_notinterested} )
{
quest::say("You are already exempted from the race $name.");
return;
}
if ( defined $qglobals{akanon_racefinish_playerflag} )
{
$starttime = $qglobals{akanon_race_starttime};
$endtime = $qglobals{akanon_race_endtime};
$diff = str2time($endtime) - str2time($starttime);

$placement = $qglobals{akanon_racefinish_playerflag};

$diffMin = int($diff/60);
$diffhour = int($diffMin/60);
$diffSec = int($diff - $diffMin*60);
$diffMin = int($diffMin - $diffhour*60);

	$msg = $diffhour . " hour(s), " . $diffMin . " minute(s), " . $diffSec . " second(s)";

	if ( $timeHour < 1 )
	{
		$msg = $diffMin . " minute(s), " . $diffSec . " second(s)";
	}
quest::say("Congratulations on finishing the race $name!  You placed in position $placement.  You started the race at $starttime (EST) and finished at $endtime (EST).  The race took you $msg to complete.");
return;
}

if ( $client->GetGM() == 0 && $client->GetBaseRace() != 12 )
{
quest::say("I am not here for you $name.");
return;
}

if($text=~/Hail/i){
$link = quest::saylink("not interested");
quest::say("If you are $link in the gnome race event, let me know.  Please note you will not be able to re-enter the race once you have left.");
}
elsif($text=~/not interested/i){
$link = quest::saylink("confirm not attending the gnome race");
quest::say("Note you will not be able to re-enter the race! Please $link");
}
elsif($text=~/confirm not attending the gnome race/i){
quest::setglobal("akanon_race_notinterested","1",5,"F");
quest::delglobal("akanon_race_playerstep");
quest::delglobal("akanon_race_playerflag");
quest::delglobal("nogroupallowedflag");
quest::delglobal("nobeneficialbuffsallowed");
quest::delglobal("nobindallowed");
quest::delglobal("notranslocateallowed");
quest::delglobal("notradeallowed");
quest::say("You have been removed from the race $name.");
}

}