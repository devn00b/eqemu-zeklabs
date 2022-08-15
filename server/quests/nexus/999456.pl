sub EVENT_SAY
{
if ($text=~/hail/i) { 
$scribe = quest::saylink("scribe spells");
$unscribe = quest::saylink("unscribe spells");
#add max level they can goto for testing
$devmax = quest::GetRule("Character:MaxLevel");

quest::say("I can help you by levelling you.  Say level [1-$devmax] to have your character changed.");
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
if($name eq 'Nugget'){
quest::say("Sorry, you have been denied access to the beta npc");
return;
}
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
#if ( $ulevel > $resultLvl ) 
#{
#quest::say("Sorry $name, you cannot go down in levels. Stop being a pussy");
#return;
#}
if ( $ulevel eq $resultLvl)
{
return;
}

quest::level(1);
quest::level($resultLvl);
quest::setglobal("waitupdatelevel","1",5,"S10");

$maxpvppts = $resultLvl * 1000;

#quest::scribespells($resultLvl);
if ( $client->GetPVPCareerPoints() < $maxpvppts )
{
#$client->AddPVPPoints(30000);
#get current pts diff
#$curdiff eq 50000;
#$maxpvppts - $client->GetPVPCareerPoints();
#$client->AddMoneyToPP(0,0,0,1500,true);
if($curdiff eq '0')
{
return;
}
$client->AddPVPPoints(10000);
$curdiff = 10000;
#$client->AddMoneyToPP(0,0,0,1500,true);
#$client->IncreaseAllSkills(200);
##Lets add some training points based on level.
$trnpts = $resultLvl * 15;
$client->SetSkillPoints($trnpts);
#$client->AddMoneyToPP(0,0,0,1500,true);
}
quest::givecash(0,0,0,3000);
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
elsif ( $text=~/skixlxlxs /i) {

$strsearch = "skills ";
$result = index($text, $strsearch);
$totallength = length($text);
#quest::say("length: $totallength");
if ( $result > -1 )
{
$diff = $totallength - $result;
$resultLvl = substr($text, $result+length($strsearch), $totallength);
if ( $resultLvl =~ /^[0-9]+$/ && $resultLvl > 0 && $resultLvl < 256 )
{
$client->IncreaseAllSkills($resultLvl);
quest::say("Skills all set to $resultLvl.");
}
else
{
quest::say("$resultLvl is out of range (1-255)");
}

}
else
{
quest::say("Failed to understand,"); # - where # is the skill level you want to be 1-255");
}

}#end skills


}#end event say
