sub EVENT_SPAWN {
##Disable if max cap is less than 35.
$devmax = quest::GetRule("Character:MaxLevel");
#What we want the start of points to be
$ptstart = 45;

#if($npc->GetOwnerID() > 0) {
#quest::shout("meh");
#}

$random_result = 0;
$random_augs = 0;


#chance to get a drop
my $random_result = int(rand(150));
#aug drop roll
my $random_augs = int(rand(500));
quest::shout("item: $random_result augs: $random_augs");

#items to drop
$rnd  = quest::ChooseRandom(2774,2775,2776,2777,2778,2779,2780,2781,2782,2783,2784,2785);
$rnd2 = quest::ChooseRandom(2774,2775,2776,2777,2778,2779,2780,2781,2782,2783,2784,2785);

#augs
$rnd3 = quest::ChooseRandom(144476,144481,144474,144482,146728,144477,146729,144480);
$rnd4 = quest::ChooseRandom(144476,144481,144474,144482,146728,144477,146729,144480);

#rockhopper stuff
$rnd5 = quest::ChooseRandom(31035,31036,31037,31038,31039,31040,31041,31091,31092,31093,31094,31095,31096,31097,31021,31022,31023,31024,31025,31026,31027,31014,31015,31016,31017,31018,31019,31020,31028,31029,31030,31031,31032,31033,31034,31000,31001,31002,31003,31004,31005,31006,31084,31085,31086,31087,31088,31089,31090,31077,31078,31079,31080,31081,31082,31083,31070,31071,31072,31073,31074,31075,31076,31063,31064,31065,31066,31067,31068,31069,31056,31057,31058,31059,31060,31061,31062,31042,31043,31044,31045,31046,31047,31048,31049,31050,31051,31052,31053,31054,31055,31007,31008,31009,31010,31011,31012,31013,5446,5447,5448,5449,5450,5451,5452);
$rnd6 = quest::ChooseRandom(31035,31036,31037,31038,31039,31040,31041,31091,31092,31093,31094,31095,31096,31097,31021,31022,31023,31024,31025,31026,31027,31014,31015,31016,31017,31018,31019,31020,31028,31029,31030,31031,31032,31033,31034,31000,31001,31002,31003,31004,31005,31006,31084,31085,31086,31087,31088,31089,31090,31077,31078,31079,31080,31081,31082,31083,31070,31071,31072,31073,31074,31075,31076,31063,31064,31065,31066,31067,31068,31069,31056,31057,31058,31059,31060,31061,31062,31042,31043,31044,31045,31046,31047,31048,31049,31050,31051,31052,31053,31054,31055,31007,31008,31009,31010,31011,31012,31013,5446,5447,5448,5449,5450,5451,5452);


if($random_result <= 5){
$npc->AddItem($rnd,1,0);
$npc->AddItem($rnd2,1,0);
$npc->AddItem($rnd5,1,0);
$npc->AddItem($rnd6,1,0);
                       }

if($random_result <15 and $random_result>5){
$npc->AddItem($rnd5,1,0);
$npc->AddItem($rnd6,1,0);
					   }					 

if($random_result <40){
$npc->AddItem($rnd5,1,0);
                      }
					  
#######start augs########
if($random_augs <5){
$npc->AddItem($rnd3,1,0);
$npc->AddItem($rnd4,1,0);
#quest::shout("Woot I got $rnd3 and $rnd4 augs roll was $random_augs");
return;
                   }

if($random_augs <30){
$npc->AddItem($rnd3,1,0);
#quest::shout("Woot I got $rnd3 augs roll was $random_augs");
return;
                      }


return;
}
##################################


sub EVENT_DEATH {
$devval = $client->GetLevel();
my $random_result = int(rand(350));
my $rndpts = 30;
my $ptstogive = int(rand($rndpts));
#Lets setup so we can have this zone open day 1
#if the level cap is <35 we give them a max of 2 points.
$devmax = quest::GetRule("Character:MaxLevel");

$devmax = quest::GetRule("Character:MaxLevel");
#What we want the start of points to be
$ptstart = 45;

if ($devmax < 45)
{
#$ptstogive = int(rand(20));
quest::say("All Points disabled untill the cap is greater than $ptstart");
return;
}



if ($devmax < 35)
{
$ptstogive = int(rand(20));
}else{
$ptstogive = int(rand(30));
}

#if they try and kill a pet use this logic:
#if a pet then
#if they have less than 10 poins 
#if they have 0 points nuke and return
#if above not true then take all the points they have, and nuke
#if they have more than 10 pts take 10 points and nuke.
$devpet = $npc->GetPetType();

if($devpet < 3 && $client->Admin() <1 )
{
#quest::say("You killed a pet so you get no points, also flagged your acct for further review of possible hacks. If your a clean player no worries.");
return;
}

if($ulevel <= 35) {
#quest::say("hi! $random_result was rand");
if($random_result <30){
if($ptstogive eq '0') {
$ptstogive = 1;
}



       $g = $client->GetGroup();

                if(!$g)
                {
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

if($c->GetPVPCareerPoints() > 1500 )
{
return;
}

if($c->GetLevel() >=35)
{
return;
}

$name = $c->GetName();
$ptstogivegrp = $ptstogive / 2;
$roundedpts = int ( $ptstogivegrp + 0.5 ); 
$c->AddPVPPoints($roundedpts);
quest::say("-Group Points- $rounded points given to $name!");
                                }
                        }
return;
                }
}
quest::say("No Points for You");
return;
}

}



