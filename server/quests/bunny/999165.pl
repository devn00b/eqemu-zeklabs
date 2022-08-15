sub EVENT_SPAWN { quest::settimer("notify",60); }

sub EVENT_TIMER {
if ($timer eq "notify")
{
quest::say("Hello everyone!! If you want to help citizens of Bunny Island see Wizbang Bobnober inside the housing to the south.  The hermit Draupner to the north-east of the front gate may have something for you to do as well.  I will be here for anyone who wishes to [return] to the world of Norrath.  I will not be going anywhere so do not be afraid!");
}
}


sub EVENT_SAY {

if ($text=~/hail/i) {
$returnLink = quest::saylink("return");
quest::say("Hello $name!! Let me know if you would like to $returnLink the world of Norrath.  I will not be going anywhere, take your time.");
}
if ($text=~/return/i) {
$leaveLink = quest::saylink("leave to");
quest::say("Hello $name!  Please understand I cannot let you return if you leave.  Knowing that if you still wish to $leaveLink Norrath let me know.");
}
if ($text=~/leave to/i) {
#if they dont have this defined they havent left yet so lets make them pick a team before they leave.
if (!defined $qglobals{"LeaveBunny"}) {
$kingLink = quest::saylink("Kings");
$banditLink = quest::saylink("Bandits");
quest::say("You haven't picked a team yet $name. In order to leave you must either pick [$kingLink] or [$banditLink]. Well...Whats it going to be $name...[$kingLink] or [$banditLink]???");
return;
}
return;
}

if($text=~/kings/i) {
$qeynos = quest::saylink("Qeynos");
$freeport = quest::saylink("Freeport");
$rivervale = quest::saylink("Rivervale");
$halas = quest::saylink("Halas");
	
quest::say("So $name you want to be a King do you? Alright what kingdom do you wish to pledge yourself to? [$qeynos]-[$freeport]-[$rivervale]-[$halas]");
}

if ($text=~/qeynos/i) {
#they have now chosen a team. Set the leave var, and move them to nexus.
$playflag = $client->GetPVPFlag();
if($playflag > '0'){
quest::say("This shouldn't really happen, but I have logic here in case it does...never know..if you are seeing this bug report it please");
return;
}

if (!defined $qglobals{"LeaveBunny"}) {
#set them to their team (Qeynos -1)
$client->SetPVPValue(1);
#set the global so they cant do strange shit.
quest::setglobal("LeaveBunny", 1, 5, "F");
$client->SummonLootGroup(24); #nexus portal item
#bind them in nexus players can then use solbinders in their cities or elsewhere to change it
quest::rebind(1,0,10,5.8);
quest::movepc(152,0,0,-28,0);
return;
}
#something went wrong, since they chose a team, but it didnt flag them. Move them to nexus.
quest::say("You have already Chosen a team so to nexus you go!");
quest::rebind(152,0,0,-28);
quest::movepc(152,0,0,-28,0);
return;
}

if ($text=~/freeport/i) {
#they have now chosen a team. Set the leave var, and move them to nexus.
$playflag = $client->GetPVPFlag();
if($playflag > '0'){
quest::say("This shouldn't really happen, but I have logic here in case it does...never know..if you are seeing this bug report it please");
return;
}

if (!defined $qglobals{"LeaveBunny"}) {
#set them to their team (Freport - 3)
$client->SetPVPValue(3);
#set the global so they cant do strange shit.
quest::setglobal("LeaveBunny", 1, 5, "F");
$client->SummonLootGroup(24); #nexus portal item
#bind them in nexus players can then use solbinders in their cities or elsewhere to change it
quest::rebind(10,-946.9,-13.5,-54.1);
quest::movepc(152,0,0,-28,0);
return;
}
#something went wrong, since they chose a team, but it didnt flag them. Move them to nexus.
quest::say("You have already Chosen a team so to nexus you go!");
#quest::rebind(10,-946.9,-13.5,-54.1);
quest::rebind(152,0,0,-28);
quest::movepc(152,0,0,-28,0);
return;
}


if ($text=~/halas/i) {
#they have now chosen a team. Set the leave var, and move them to nexus.
$playflag = $client->GetPVPFlag();
if($playflag > '0'){
quest::say("This shouldn't really happen, but I have logic here in case it does...never know..if you are seeing this bug report it please");
return;
}

if (!defined $qglobals{"LeaveBunny"}) {
#set them to their team (Halas - 2)
$client->SetPVPValue(2);
#set the global so they cant do strange shit.
quest::setglobal("LeaveBunny", 1, 5, "F");
$client->SummonLootGroup(24); #nexus portal item
#bind them in nexus players can then use solbinders in their cities or elsewhere to change it
#quest::rebind(152,0,0,-28);
quest::rebind(29,0,0,3.0);
quest::movepc(152,0,0,-28,0);
return;
}
#something went wrong, since they chose a team, but it didnt flag them. Move them to nexus.
quest::say("You have already Chosen a team so to nexus you go!");
quest::rebind(152,0,0,-28);
quest::movepc(152,0,0,-28,0);
return;
}


if ($text=~/rivervale/i) {
#they have now chosen a team. Set the leave var, and move them to nexus.
$playflag = $client->GetPVPFlag();
if($playflag > '0'){
quest::say("This shouldn't really happen, but I have logic here in case it does...never know..if you are seeing this bug report it please");
return;
}

if (!defined $qglobals{"LeaveBunny"}) {
#set them to their team (Rivervale - 4)
$client->SetPVPValue(4);
#set the global so they cant do strange shit.
quest::setglobal("LeaveBunny", 1, 5, "F");
$client->SummonLootGroup(24); #nexus portal item
#bind them in nexus players can then use solbinders in their cities or elsewhere to change it
#quest::rebind(152,0,0,-28);
quest::rebind(19,0,0,4.0);
quest::movepc(152,0,0,-28,0);
return;
}
#something went wrong, since they chose a team, but it didnt flag them. Move them to nexus.
quest::say("You have already Chosen a team so to nexus you go!");
quest::rebind(152,0,0,-28);
quest::movepc(152,0,0,-28,0);
return;
}


if ($text=~/bandits/i) {
#they have now chosen a team. Set the leave var, and move them to nexus.
$playflag = $client->GetPVPFlag();
if($playflag eq '5'){
quest::say("This shouldn't really happen, but I have logic here in case it does...never know..if you are seeing this bug report it please");
return;
}
if (!defined $qglobals{"LeaveBunny"}) {
#set them to their team
$client->SetPVPValue(5);
#set the global so they cant do strange shit.
quest::setglobal("LeaveBunny", 1, 5, "F");
$client->SummonLootGroup(24); #nexus portal item
quest::say("Safe travels friend!");
#bind them in nexus players can then use solbinders in their cities or elsewhere to change it
quest::rebind(152,0,0,-28);
quest::movepc(152,0,0,-28,0);
return;
}
#something went wrong, since they chose a team, but it didnt flag them. Move them to nexus.
quest::say("You have already Chosen a team so to nexus you go!");
quest::rebind(152,0,0,-28);
quest::movepc(152,0,0,-28,0);
return;
}

}
