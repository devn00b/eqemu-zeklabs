#sub EVENT_SPAWN {
#chance to get a drop
#my $random_result = int(rand(150));
#aug drop roll
#my $random_augs = int(rand(500));

#items to drop
#$rnd = quest::ChooseRandom(31035,31036,31037,31038,31039,31040,31041,31091,31092,31093,31094,31095,31096,31097,31021,31022,31023,31024,31025,31026,31027,31014,31015,31016,31017,31018,31019,31020,31028,31029,31030,31031,31032,31033,31034,31000,31001,31002,31003,31004,31005,31006,31084,31085,31086,31087,31088,31089,31090,31077,31078,31079,31080,31081,31082,31083,31070,31071,31072,31073,31074,31075,31076,31063,31064,31065,31066,31067,31068,31069,31056,31057,31058,31059,31060,31061,31062,31042,31043,31044,31045,31046,31047,31048,31049,31050,31051,31052,31053,31054,31055,31007,31008,31009,31010,31011,31012,31013);
#$rnd2 = quest::ChooseRandom(31035,31036,31037,31038,31039,31040,31041,31091,31092,31093,31094,31095,31096,31097,31021,31022,31023,31024,31025,31026,31027,31014,31015,31016,31017,31018,31019,31020,31028,31029,31030,31031,31032,31033,31034,31000,31001,31002,31003,31004,31005,31006,31084,31085,31086,31087,31088,31089,31090,31077,31078,31079,31080,31081,31082,31083,31070,31071,31072,31073,31074,31075,31076,31063,31064,31065,31066,31067,31068,31069,31056,31057,31058,31059,31060,31061,31062,31042,31043,31044,31045,31046,31047,31048,31049,31050,31051,31052,31053,31054,31055,31007,31008,31009,31010,31011,31012,31013);
#augments Low, and Medium from green notebooko
#$rnd3 = quest::ChooseRandom(144476,144481,144474,144482,146728,144477,146729,144480);
#$rnd4 = quest::ChooseRandom(144476,144481,144474,144482,146728,144477,146729,144480);


#$devpet = $npc->GetPetType();

#if($devpet < 3)
#{
#just exit script so they have no effect
#return;
#}

#if($random_result <5){
#$npc->AddItem($rnd,1,0);
#$npc->AddItem($rnd2,1,0);
#quest::shout("Woot I got $rnd and $rnd2");
#return;
#                      }

#if($random_result <30){
#$npc->AddItem($rnd,1,0);
#return;
#                      }
#######start augs########
#if($random_augs <5){
#$npc->AddItem($rnd3,1,0);
#$npc->AddItem($rnd4,1,0);
#quest::shout("Woot I got $rnd3 and $rnd4 augs roll was $random_augs");
#return;
#                      }

#if($random_augs <30){
#$npc->AddItem($rnd3,1,0);
#quest::shout("Woot I got $rnd3 augs roll was $random_augs");

#return;
#                      }


#return;
#}
##################################


sub EVENT_DEATH {
my $devval = $client->GetLevel();
my $random_result = int(rand(190));
my $rndpts = 200;
my $ptstogive = int(rand($rndpts));
my $orgpts = $ptstogive;
my $grouppts = $ptstogive /2;
my $oorgroup = $grouppts / 2;
my $devmax = quest::GetRule("Character:MaxLevel");

if ($devmax < 35)
{
$ptstogive = int(rand(20));
}

$devpet = $npc->GetPetType();

if($devpet < 3 && $client->Admin() <1 )
{
return;
}

if($ulevel <= 255) {
if($random_result <30){
if($ptstogive eq '0') {
$ptstogive = 1;
}



       $g = $client->GetGroup();

                if(!$g)
                {
if($ulevel >= 55)
{
$ptstogive = $ptstogive / 2;
$ptstogive = int ( $ptstogive - 0.5 );
if($ptstogive eq '0') {
$ptstogive = 1;
}

quest::say("Sorry $name you are out of level range and so you earn half points.");
quest::say("-Solo Points- $name, you have earned $ptstogive!");
$client->AddPVPPoints($ptstogive);
return;

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


if($c->GetLevel() <= 54)
{

$name = $c->GetName();
$roundedpts = int ( $grouppts - 0.5 );
$c->AddPVPPoints($roundedpts);
quest::say("-Group Points- $roundedpts points given to $name!");
} else {
if($oorgroup eq '0') {
$oorgroup = 1;
}
$name = $c->GetName();
$roundedptsx = int ( $oorgroup - 0.5);
if($roundedptsx eq '0') {
$roundedptsx = 1;
}
$c->AddPVPPoints($roundedptsx);
quest::say("-REDUCED Group Points- $roundedptsx points given to $name!");


}
                                }
                        }
return;
                }
}
quest::say("No Points for You");
return;
}

}



