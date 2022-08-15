sub EVENT_ATTACK {

if($ulevel >46) {
quest::say("Sorry $name, this zone is for characters upto level 46 only. So Back to nexus you go.");
quest::movepc(152, 0, 0, 0, 0);
return;
}

if($client->GetPVPCareerPoints() > 4100 ) {
quest::movepc(152, 0, 0, 0, 0);
quest::say("You have too many points $name So to nexus you go!");
}
}

sub EVENT_DEATH {
my $random_result = 1;
my $random_result = int(rand(400));
my $rndpts = 55;
my $ptstogive = int(rand($rndpts));
$devpet = $npc->GetPetType();

if($devpet < 3 && $client->Admin() <1 )
{
$petkiller = '-10';
$client->AddPVPPoints($petkiller);
quest::say("You killed a pet so I took 10 points. Also flagged your account for further reivew.");
return;
}


if($ulevel >46) {
quest::say("Sorry $name, this zone is for characters upto level 46 only. So Back to nexus you go.");
quest::movepc(152, 0, 0, 0, 0);
return;
}


if($client->GetPVPCareerPoints() <4101) {
if($random_result <60){
if($ptstogive eq '0') {
$ptstogive = 1;
}



       $g = $client->GetGroup();

                if(!$g)
                {
if($random_result <50) {
quest::say("-Solo Tickets- Congrats $name you get 4! Yes 4!!! pvp tickets.");
quest::summonitem(132487);
quest::summonitem(132487);
quest::summonitem(132487);
quest::summonitem(132487);
}

quest::say("-Solo Points- $name, you have earned $ptstogive!");
$client->AddPVPPoints($ptstogive);
return;

                }
                else
                {
                        for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);

                                if($c)
                                {

if($c->GetPVPCareerPoints() > 4101)
{
$name = $c->GetName();
$c->MovePC(152, 0, 0, 0, 0);
quest::say("$name had too many points. So to nexus they go!");
return;
}

$name = $c->GetName();
$ptstogivegrp = $ptstogive / 2;
$roundedpts = int ( $ptstogivegrp + 0.5 ); 
$c->AddPVPPoints($roundedpts);
if($random_result <50) {
quest::say("-Group Tickets- Congrats $name you got 2 tickets.");
$c->SummonItem(132487);
$c->SummonItem(132487);
}
quest::say("-Group Points- $rounded points given to $name!");
                                }
                        }
return;
                }
}
quest::say("No Points for You");
return;
}

if($client->GetPVPCareerPoints() > 4101 ) {
quest::say("You have too many points ( $client-GetPVPCareerPoints() ) $name So to nexus you go!");

quest::movepc(152, 0, 0, 0, 0);
}


}

sub EVENT_HP {

if($ulevel >46) {
quest::say("Sorry $name, this zone is for characters upto level 46 only. So Back to nexus you go.");
quest::movepc(152, 0, 0, 0, 0);
return;
}

if($client->GetPVPCareerPoints() > 4101 ) {
quest::movepc(152, 0, 0, 0, 0);
quest::say("You have too many points $name So to nexus you go!");
}

}

sub EVENT_LOOT {
if($ulevel >46) {
quest::say("Sorry $name, this zone is for characters upto level 46 only. So Back to nexus you go.");
quest::movepc(152, 0, 0, 0, 0);
return;
}

if($client->GetPVPCareerPoints() > 4101 ) {
quest::movepc(152, 0, 0, 0, 0);
quest::say("You have too many points $name So to nexus you go!");
return;
}
}

sub EVENT_SAY {
if($ulevel >46) {
quest::say("Sorry $name, this zone is for characters upto level 46 only. So Back to nexus you go.");
quest::movepc(152, 0, 0, 0, 0);
return;
}

if($client->GetPVPCareerPoints() > 4101 ) {
quest::movepc(152, 0, 0, 0, 0);
quest::say("You have too many points $name So to nexus you go!");
}


}


