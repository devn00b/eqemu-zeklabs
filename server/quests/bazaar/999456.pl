sub EVENT_SAY
{
if ($text=~/reset/i) {
$resetpts=$client->GetPVPCareerPoints();

$client->AddPVPPoints(-$resetpts);
quest::say("I have set your points to 0.");
return;
}

if ($text=~/hail/i) { 
$scribe = quest::saylink("scribe spells");
$unscribe = quest::saylink("unscribe spells");
#add max level they can goto for testing
$devmax = 55;
#quest::GetRule("Character:MaxLevel");

quest::say("I can help you by levelling you.  Say level [1-$devmax] to have your character changed. I can also [reset] your points to 0. One new thing I have figured out is how to set your [skills] just ask.");
#- OR - skills [1-255] to have your character changed.  You can also $unscribe spells that are scribed.");
}
#elsif ($text=~/scribe spells/i) {
#quest::scribespells($client->GetLevel());
#quest::say("Spells are automatically scribed as you level, other than spells sold by PVP Discord Merchants.");
#}
elsif ($text=~/unscribe/i) { 
quest::unscribespells();
}

elsif ($text=~/level /i) { 
if ( defined $qglobals{waitupdatelevel} )
{
quest::say("Wait 5 seconds $name before changing your level again.");
return;
}
$strsearch = "level ";
$result = index($text, $strsearch);
$totallength = length($text);
#quest::say("length: $totallength");
if ( $result > -1 )
{
$diff = $totallength - $result;
$resultLvl = substr($text, $result+length($strsearch), $totallength);
$devmaxplusone = $devmax + 1;
if ( ($resultLvl =~ /^[0-9]+$/) && ($resultLvl > 0) && ($resultLvl < $devmaxplusone) )
{
if ( $ulevel > $resultLvl ) 
{
quest::say("Sorry $name, you cannot go down in levels. Stop being a pussy");
return;
}
if ( $ulevel eq $resultLvl)
{
return;
}

quest::level(1);
quest::level($resultLvl);
quest::setglobal("waitupdatelevel","1",5,"S10");

$maxpvppts = $resultLvl * 200;

#quest::scribespells($resultLvl);
if ( $client->GetPVPCareerPoints() < $maxpvppts )
{
#$client->AddPVPPoints(30000);
#get current pts diff
$curdiff = $maxpvppts - $client->GetPVPCareerPoints();
#$client->AddMoneyToPP(0,0,0,1500,true);
if($curdiff eq '0')
{
return;
}
$client->AddPVPPoints($curdiff);
##Lets add some training points based on level.
$trnpts = $resultLvl * 30;
$client->SetSkillPoints($trnpts);
$client->AddAAPoints($trnpts);
}
quest::givecash(0,0,0,6000);
quest::say("You are now level $resultLvl. I also gave you $curdiff points. And because I'm so nice I gave you $trnpts training points to use at your class trainer.");
}
else
{
quest::say("$resultLvl is out of range");
}

}
else
{
quest::say("Failed to understand, say level # - where # is the level you want to be 1-$devmax");
}

}#end level text statement
##commented out images original code and replaced it with something based on level.
#elsif ( $text=~/skixlxlxs /i) {

#$strsearch = "skills ";
#$result = index($text, $strsearch);
#$totallength = length($text);
##quest::say("length: $totallength");
#if ( $result > -1 )
#{
#$diff = $totallength - $result;
#$resultLvl = substr($text, $result+length($strsearch), $totallength);
#if ( $resultLvl =~ /^[0-9]+$/ && $resultLvl > 0 && $resultLvl < 256 )
#{
#$client->IncreaseAllSkills($resultLvl);
#quest::say("Skills all set to $resultLvl.");
#}
#else
#{
#quest::say("$resultLvl is out of range (1-255)");
#}

#}
#else
#{
#quest::say("Failed to understand,"); # - where # is the skill level you want to be 1-255");
#}
#}#end skills

#################################Devs Updated Skills##############################
##################################################################################

elsif ( $text=~/skills/i) {
#max level is 55.
    if ($ulevel <= 55) {
		$client->Message( 15, "Here Have some skills $name");
		# set all available skills to maximum for race/class at current level
		foreach my $skill ( 0 .. 74 ) {
			next unless $client->CanHaveSkill($skill);
			my $maxSkill = $client->MaxSkill( $skill, $client->GetClass(), $ulevel );
			next unless $maxSkill > $client->GetRawSkill($skill);
			$client->SetSkill( $skill, $maxSkill );
		}
		# scribe all spells for current level
		quest::scribespells( $ulevel, $ulevel - 1 );
		# train all discs for current level
		#quest::traindiscs( $ulevel, $ulevel - 1 );
	}


}

##################################################################################
##################################################################################


}#end event say
