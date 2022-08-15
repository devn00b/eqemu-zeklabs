sub EVENT_ATTACK {
if($client->GetPVPCareerPoints() > 4500 ) {
quest::movepc(152, 0, 0, 0, 0);
quest::say("You have too many points $name So to nexus you go!");
}
}

sub EVENT_DEATH {
my $random_result = 0;
my $random_result = int(rand(300));
my $rndpts = 50;
my $ptstogive = int(rand($rndpts));
$devpet = $npc->GetPetType();

if($devpet < 3 && $client->Admin() <1 )
{
if($client->GetPVPCareerPoints() <10)
{
if($client->GetPVPCareerPoints() == 0)
{
quest::say("Fuckoff Dick Face! Account flagged for further review.");
quest::shout2("** Community Warning ** $name likes to try and cheat.");
$client->Kill();
return;
}
$devxyz = $client->GetPVPCareerPoints();
quest::say("Your a fucking loser. So Die. Oh, and goodbye to all your points. Oh and enjoy your account being flagged for further review.");
quest::shout2("** Community Warning ** $name likes to try and cheat.");
$client->AddPVPPoints(-$devxyz);
$client->Kill();
return;
}

$petkiller = '-10';
$client->AddPVPPoints($petkiller);
quest::say("You killed a pet so I took 10 points. Also flagged your account for further reivew.");
#quest::shout2("** Community Warning ** $name likes to try and cheat.");
#$client->Kill();
return;
}


if($ulevel <50) {
quest::say("Sorry $name, this zone is for characters that are level 50 only. So Back to nexus you go.");
quest::movepc(152, 0, 0, 0, 0);
return;
}

if($client->GetPVPCareerPoints() <4501) {
if($random_result <40){
if($ptstogive eq '0') {
$ptstogive = 1;
}

$client->AddPVPPoints($ptstogive);
quest::say("Have some points $name ($ptstogive)!");
return;
}
quest::say("No Points for You");
return;
}

if($client->GetPVPCareerPoints() > 4500 ) {
quest::say("You have too many points ( $client-GetPVPCareerPoints() ) $name So to nexus you go!");

quest::movepc(152, 0, 0, 0, 0);
}
}

sub EVENT_HP {
if($client->GetPVPCareerPoints() > 4500 ) {
quest::movepc(152, 0, 0, 0, 0);
quest::say("You have too many points $name So to nexus you go!");
}
}

sub EVENT_LOOT {
if($client->GetPVPCareerPoints() > 4500 ) {
quest::movepc(152, 0, 0, 0, 0);
quest::say("You have too many points $name So to nexus you go!");
}
}

sub EVENT_SAY {
if($client->GetPVPCareerPoints() > 4500 ) {
quest::movepc(152, 0, 0, 0, 0);
quest::say("You have too many points $name So to nexus you go!");
}
}


