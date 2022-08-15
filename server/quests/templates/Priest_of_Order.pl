sub EVENT_SAY { 
$flag = $qglobals{teampvpflag};
$flag2 = $qglobals{LeaveBunny};

if ( $client->GetLevel() < 30 && $client->GetPVPFlag() != 0  && defined($flag) && $flag == 1 )
{
if($text=~/Hail/i){
$link = quest::saylink("revoke");
quest::say("Greetings. $name.  You have recently aligned yourself to another faction and can only $link your status at this time.");
return;
}
elsif($text=~/revoke/i){
#quest::say("I have revoked your team status.");
#quest::pvpvalue("0");
quest::say("Sorry this feature is disabled");
}
}
elsif( $client->GetLevel() > 29  && $client->GetPVPFlag() != 0  && defined($flag) && $flag == 1 )
{
quest::say("Greetings. $name.  You have recently aligned yourself to another faction and must wait five days from changing your faction.");
return;
}

if($text=~/Hail/i){
my $sayjoin = quest::saylink("join");
quest::say("Greetings. $name.  Would you like to $sayjoin a kingdom and sway the battles that plague Norrath?");
}
else
{
$hitJoin = 0;
if($text=~/join/i){
$qeynos = quest::saylink("Qeynos");
$freeport = quest::saylink("Freeport");
$rivervale = quest::saylink("Rivervale");
$halas = quest::saylink("Halas");
$bandit = quest::saylink("Bandits");

quest::say("If you are interested in joining the kingdom of $freeport or $rivervale or $halas or $qeynos or $bandit. I can let you know more about each kingdom.");
}

if($text=~/Kingdom of Qeynos/i){
$hitJoin = 1;
if ( $client->GetPVPFlag() == 1 ) { quest::say("Well you seem to already be a part of this kingdom!"); }
else
{
quest::say("Good luck citizen of Qeynos, go to uphold your kingdom's honor.");
quest::pvpvalue("1");
quest::rebind(2,172.5,519.3,3.1);
quest::emote("binds $name to Qeynos");
quest::setglobal("teampvpflag","1",5,"D5");
}
}
elsif($text=~/Kingdom of Freeport/i){
$hitJoin = 1;
if ( $client->GetPVPFlag() == 3 ) { quest::say("Well you seem to already be a part of this kingdom!"); }
else
{
quest::say("Good luck citizen of Freeport, go and support your kingdom's struggle for domination.");
quest::pvpvalue("3");
quest::rebind(9,248.1,16.2,-24.9);
quest::emote("binds $name to Freeport");
quest::setglobal("teampvpflag","1",5,"D5");
}
}
elsif($text=~/Kingdom of Halas/i){
$hitJoin = 1;
if ( $client->GetPVPFlag() == 2 ) { quest::say("Well you seem to already be a part of this kingdom!"); }
else
{
quest::say("Good luck citizen of Halas, spread peace through the land and defend your kingdom.");
quest::pvpvalue("2");
quest::rebind(29,31.3,-532.6,3.0);
quest::emote("binds $name to Halas");
quest::setglobal("teampvpflag","1",5,"D5");
}
}
elsif($text=~/Kingdom of Rivervale/i){
$hitJoin = 1;
if ( $client->GetPVPFlag() == 4 ) { quest::say("Well you seem to already be a part of this kingdom!"); }
else
{
quest::say("Good luck citizen of Rivervale!");
quest::pvpvalue("4");
quest::rebind(33,-2315.4,536.1,-5.2);
quest::emote("binds $name to Rivervale");
quest::setglobal("teampvpflag","1",5,"D5");
}
}

elsif($text=~/Bandit Scourge/i){
$hitJoin = 1;
if ( $client->GetPVPFlag() == 5 ) { quest::say("Well you seem to already be a part of this kingdom!"); }
else
{
quest::say("..Ugh another joins the filth. Good luck $name");
quest::pvpvalue("5");
quest::rebind(152,-1.5,9.5,-30.9);
quest::emote("binds $name to Nexus");
quest::setglobal("teampvpflag","1",5,"D5");
}
}


if ( $hitJoin < 1 )
{
if($text=~/qeynos/i){
$qeynos = quest::saylink("Kingdom of Qeynos");
quest::say("Qeynos is a respectable kingdom demanding honor and respect of all their citizens.  If you would like to join the $qeynos let me know.");
}
elsif($text=~/freeport/i){
$freeport = quest::saylink("Kingdom of Freeport");
quest::say("Freeport is a kingdom based on power and glory.  If you would like to join the $freeport let me know.");
}
elsif($text=~/halas/i){
$halas = quest::saylink("Kingdom of Halas");
quest::say("Halas is a calm and secluded kingdom in the cold north.  If you would like to join the $halas let me know.");
}
elsif($text=~/rivervale/i){
$rivervale = quest::saylink("Kingdom of Rivervale");
quest::say("Rivervale is a simple but robust kingdom with many riches.  If you would like to join the $rivervale let me know.");
}
elsif($text=~/bandits/i){
$bandit = quest::saylink("Bandit Scourge");
quest::say("Bandits are the scourge of all the kingdoms. They align with none but their own kind. They may not own cities, and are the hardest of all the choices.  If you would like to join the $bandit let me know.");
}

}

}

}



sub EVENT_ITEM {

$checkHandIn = plugin::check_handin(\%itemcount,148139=> 1);

if ( $checkHandIn ) {
quest::say("Thank you $name for this ticket, I have reset your timer, you can now change your team.");
quest::delglobal("teampvpflag");
return;
}
quest::say("What the...Yeah this is trash, I'll just delete it");
return;
}
