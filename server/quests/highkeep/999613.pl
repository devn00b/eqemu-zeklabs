## This is devn00bs s5 point catch up zones entry point.
## Goes on The Portal Master
## Also using for ports to other locations I may use

sub EVENT_SAY {

if($ulevel <5){
quest::say("Get to level 5 then lets talk noob");
return;
}

if(($text=~/hail/i) && ($ulevel >= 5) ){
##setup saylinks so it looks purdy
#$seb=que=quest::saylink("Sebilis -Disabled-");

$paludal=quest::saylink("Paludal");
$crystal=quest::saylink("Crystal");
$echo=quest::saylink("Echo");
$dreadlands=quest::saylink("Dreadlands");
$kaesora=quest::saylink("Kaesora");
$dawnshroud=quest::saylink("Dawnshroud");
$charasis=quest::saylink("Charasis");
$karnor=quest::saylink("Karnor");
$kael=quest::saylink("Kael");
$velketor=quest::saylink("Velketor");
$tov=quest::saylink("ToV");
$akheva=quest::saylink("Akheva");
$thegrey=quest::saylink("The Grey");

quest::say("Greetings! I can send you to a few zones that may be avaialbe at launch. Note Single Port in at the moment.");
quest::say("");
quest::say("[$paludal]             - Level 5+ *Random Solo and Group Port*");
quest::say("[$crystal] caverns     - Level 20+");
quest::say("[$echo] caverns        - Level 20+");
quest::say("[$dreadlands]          - Level 25+ *Random Solo and Group Port*");
quest::say("[$kaesora]             - Level 30+");
quest::say("[$dawnshroud]          - Level 30+ *Random Solo and Group Port*");
quest::say("[$charasis]            - Level 40+");
quest::say("[$karnor]              - Level 40+ *Random Solo and Group Port*");
#quest::say("[$seb]             - Level 45+ *Random Solo and Group Port*");
quest::say("[$kael]                - Level 45+ *Random Solo and Group Port*");
#quest::say("[$velketor]            - Level 45+ *Random Solo and Group Port*");
quest::say("[$thegrey]             - Level 45+ *Random Solo and Group Port*");
quest::say("[$akheva] Ruins        - Level 45+ *Random Solo and Group Port*");
quest::say("[$tov]                 - Level 50+");

quest::say(" ");
quest::say("*** Not all port in locations will be safe all of the time. Be Ready. *** ");


return;
  }


if($text=~/dreadlands/i && $ulevel >=25){
#######
#ZONE LOCATIONS
#86,9655.7,3033.6,1047.3, 183.2 
#86,9924.3,2569.2,104.3, 178.9
#86,9854.5,1973.0,1052.0,187.8
#86,9961.8,1128.0,1062.8,191.5
#86,9894.6,393.9,1062.8,187.8
#86,9000.7,-54.5,1047.3,241.9
#86,7003.0,457.3,1047.3,255.4
#86,7158.1,1388.8,1048.1,67.2
#86,3606.0,-1961.1,740.3,77.5
#86,5144.4,-946.7,1165.1,147.0
#86,3737.4,2234.0,700.9,150.4
#86,-2163.7,1006.2,24.9,255.2
#86,-2917.1,816.4,24.9,189.6
#86,-2907.9,106.5,24.9,130.4
#86,-1856,162.6,24.9,63.4
#86,-1703.7,439.6,24.9,65.5
#86,-3469.5,-1829.8,36.1,246.8
#86,-5560.7,-433.6,71.8,29.8
######
#lets decide where they zone in at
$zonep = int(rand (17));
$g = $client->GetGroup();

    if(!$g) {
$zonep = int(rand (16));
#quest::say("You rolled $zonep.");				
if ($zonep eq '0' ){
quest::say("Well, $name off you go!");
quest::movepc(86,9655.7,3033.6,1047.3, 183.2 );
return;
}
if ($zonep eq '1' ){
quest::say("$name, you sure seem to be ready to handle this!");
quest::movepc(86,9924.3,2569.2,104.3, 178.9);
return;
}
if ($zonep eq '2' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(86,9854.5,1973.0,1052.0,187.8);
return;
}
if ($zonep eq '3' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(86,9961.8,1128.0,1062.8,191.5);
return;
}
if ($zonep eq '4' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(86,9894.6,393.9,1062.8,187.8);
return;
}
if ($zonep eq '5' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(86,9000.7,-54.5,1047.3,241.9);
return;
}

if ($zonep eq '6' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(86,7158.1,1388.8,1048.1,67.2);
return;
}

if ($zonep eq '7' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(86,3606.0,-1961.1,740.3,77.5);
return;
}

if ($zonep eq '8' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(86,5144.4,-946.7,1165.1,147.0);
return;
}

if ($zonep eq '9' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(86,3737.4,2234.0,700.9,150.4);
return;
}

if ($zonep eq '10' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(86,-2163.7,1006.2,24.9,255.2);
return;
}

if ($zonep eq '11' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(86,-2917.1,816.4,24.9,189.6);
return;
}

if ($zonep eq '12' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(86,-2907.9,106.5,24.9,130.4);
return;
}

if ($zonep eq '13' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(86,-1856,162.6,24.9,63.4);
return;
}

if ($zonep eq '14' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(86,-1703.7,439.6,24.9,65.5);
return;
}

if ($zonep eq '15' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(86,-3469.5,-1829.8,36.1,246.8);
return;
}

if ($zonep eq '16' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(86,-5560.7,-433.6,71.8,29.8);
return;
}
				
            }
                else
                {
                        for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);


                                if($c)
                                {
$name = $c->GetName();
if ($zonep eq '0' ){
quest::say("- Group Port #$i - Bye $name!");
quest::movepc(86,9655.7,3033.6,1047.3, 183.2 );
}
if ($zonep eq '1' ){
quest::say("- Group Port #$i - Bye $name!");
$c->MovePC(86,9924.3,2569.2,104.3, 178.9);

}
if ($zonep eq '2' ){
quest::say("- Group Port #$i - Bye $name!");
$c->MovePC(86,9854.5,1973.0,1052.0,187.8);

}
if ($zonep eq '3' ){
quest::say("- Group Port #$i - Bye $name!");
$c->MovePC(86,9961.8,1128.0,1062.8,191.5);

}
if ($zonep eq '4' ){
quest::say("- Group Port #$i - Bye $name!");
$c->MovePC(86,9894.6,393.9,1062.8,187.8);

}
if ($zonep eq '5' ){
quest::say("- Group Port #$i - Bye $name!");
$c->MovePC(86,9000.7,-54.5,1047.3,241.9);

}

if ($zonep eq '6' ){
quest::say("- Group Port #$i - Bye $name!");
$c->MovePC(86,7158.1,1388.8,1048.1,67.2);

}

if ($zonep eq '7' ){
quest::say("- Group Port #$i - Bye $name!");
$c->MovePC(86,3606.0,-1961.1,740.3,77.5);

}

if ($zonep eq '8' ){
quest::say("- Group Port #$i - Bye $name!");
$c->MovePC(86,5144.4,-946.7,1165.1,147.0);

}

if ($zonep eq '9' ){
quest::say("- Group Port #$i - Bye $name!");
$c->MovePC(86,3737.4,2234.0,700.9,150.4);

}

if ($zonep eq '10' ){
quest::say("- Group Port #$i - Bye $name!");
$c->MovePC(86,-2163.7,1006.2,24.9,255.2);

}

if ($zonep eq '11' ){
quest::say("- Group Port #$i - Bye $name!");
$c->MovePC(86,-2917.1,816.4,24.9,189.6);

}

if ($zonep eq '12' ){
quest::say("- Group Port #$i - Bye $name!");
$c->MovePC(86,-2907.9,106.5,24.9,130.4);

}

if ($zonep eq '13' ){
quest::say("- Group Port #$i - Bye $name!");
$c->MovePC(86,-1856,162.6,24.9,63.4);

}

if ($zonep eq '14' ){
quest::say("- Group Port #$i - Bye $name!");
$c->MovePC(86,-1703.7,439.6,24.9,65.5);

}

if ($zonep eq '15' ){
quest::say("- Group Port #$i - Bye $name!");
$c->MovePC(86,-3469.5,-1829.8,36.1,246.8);

}

if ($zonep eq '16' ){
quest::say("- Group Port #$i - Bye $name!");
$c->MovePC(86,-5560.7,-433.6,71.8,29.8);

}
                                }
                        

                }
quest::say("This shouldn't happen");
return;
}

########################################################

quest::say("..Whee..");
quest::movepc(86,9565,2806,1050,0);
return;
}


	if($text=~/the grey/i && $ulevel >45){
$zonep = int(rand (6));
$g = $client->GetGroup();

    if(!$g) {
$zonep = int(rand (6));
#quest::say("You rolled $zonep.");

if ($zonep eq '0' ){
quest::say("Well, $name off you go!");
quest::movepc(171,343.9,-1991.7,-28.8,0);
}

if ($zonep eq '1' ){
quest::say("Well, $name off you go!");
quest::movepc(171,1620.9,-1539.2,-13.0,0);
return;
}

if ($zonep eq '2' ){
quest::say("Well, $name off you go!");
quest::movepc(171,-101.7,70.5,-44.7,0);
return;
}

if ($zonep eq '3' ){
quest::say("Well, $name off you go!");
quest::movepc(171,1644.0,1733.0,2.6,0);
return;
}

if ($zonep eq '4' ){
quest::say("Well, $name off you go!");
quest::movepc(171,-1694.3,1702.9,-16.2,0);
return;
}

if ($zonep eq '5' ){
quest::say("Well, $name off you go!");
quest::movepc(171,-1928.2,549.6,1.4,0);
return;
}
if ($zonep eq '6' ){
quest::say("Well, $name off you go!");
quest::movepc(171,1467.3,-1115.2,214.0,0);
return;
}
}
else{
                        for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);



                                if($c)
                                {
$name = $c->GetName();
my $bob = $i + 1;
if ($zonep eq '0' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(171,343.9,-1991.7,-28.8,0);
}

if ($zonep eq '1' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(171,1620.9,-1539.2,-13.0,0);
}

if ($zonep eq '2' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(171,-101.7,70.5,-44.7,0);
}

if ($zonep eq '3' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(171,1644.0,1733.0,2.6,0);
}

if ($zonep eq '4' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(171,-1694.3,1702.9,-16.2,0);
}

if ($zonep eq '5' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(171,-1928.2,549.6,1.4,0);
}

if ($zonep eq '6' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(171,1467.3,-1115.2,214.0,0);
}

                                }

}
return;
                }
quest::say("Report Error 420 in bug report. Sorry.");
return;
}




if($text=~/akheva/i && $ulevel >45){
$zonep = int(rand (6));
$g = $client->GetGroup();

    if(!$g) {
$zonep = int(rand (6));
#quest::say("You rolled $zonep.");

if ($zonep eq '0' ){
quest::say("Well, $name off you go!");
quest::movepc(179,60.0,-1395.0,22.8,0);
}

if ($zonep eq '1' ){
quest::say("Well, $name off you go!");
quest::movepc(179,-406.5,295.0,-16.1,0);
return;
}

if ($zonep eq '2' ){
quest::say("Well, $name off you go!");
quest::movepc(179,-241.4,295.3,-16.1,0);
return;
}

if ($zonep eq '3' ){
quest::say("Well, $name off you go!");
quest::movepc(179,-245.5,-94.7,-16.1,0);
return;
}

if ($zonep eq '4' ){
quest::say("Well, $name off you go!");
quest::movepc(179,-1440.7,1370.7,14.9,0);
return;
}

if ($zonep eq '5' ){
quest::say("Well, $name off you go!");
quest::movepc(179,-404.5,296.5,-16.1,0);
return;
}
if ($zonep eq '6' ){
quest::say("Well, $name off you go!");
quest::movepc(179,-969.6,-391.0,9.9,0);
return;
}
}
else{
                        for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);



                                if($c)
                                {
$name = $c->GetName();
my $bob = $i + 1;
if ($zonep eq '0' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(179,60,-1395,22,0.0);
}

if ($zonep eq '1' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(179,-406.5,295.0,-16.1,0);
}

if ($zonep eq '2' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(179,-241.4,295.3,-16.1,0);
}

if ($zonep eq '3' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(179,-245.5,-94.7,-16.1,0);
}

if ($zonep eq '4' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(179,-1440.7,1370.7,14.9,0);
}

if ($zonep eq '5' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(179,-404.5,296.5,-16.1,0);
}

if ($zonep eq '6' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(179,-969.6,-391.0,9.9,0);
}

                                }
                        
}
return;
                }
quest::say("Report Error 420 in bug report. Sorry.");
return;
}


if($text=~/velketor/i && $ulevel >45){
####DISABLE
return;
#zone id 112,
#112,-124.3,583.9,-154.1,126.2
#112,-8.4,534.0,-187.1,129.6
#112,67.4, 487.2, -188.1, 125.6
#112,-151.1, 427.0, -188.1, 125.6
#112,68.8,303.3,-188.1, 31.2
#112,-86.6,289.0,-118.2,64.1
#112,-53.0,466.4,-36.1, 54.9
#112,-52.9,378.6,-24.1,29.9
#112,-57.0,532.3,0.0,65.2
#112,141.4,329.3,34.9,199.1
#112,49.8,428.4,86.8,249.6
#112,-50.8,92.3,-72.9,62.8
$zonep = int(rand (11));
$g = $client->GetGroup();

    if(!$g) {
$zonep = int(rand (11));
#quest::say("You rolled $zonep.");				
if ($zonep eq '0' ){
quest::say("Well, $name off you go!");
quest::movepc(112,-124.3,583.9,-154.1,126.2);
return;
}
if ($zonep eq '1' ){
quest::say("$name, you sure seem to be ready to handle this!");
quest::movepc(112,-8.4,534.0,-187.1,129.6);
return;
}
if ($zonep eq '2' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(112,67.4, 487.2, -188.1, 125.6);
return;
}
if ($zonep eq '3' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(112,-151.1, 427.0, -188.1, 125.6);
return;
}
if ($zonep eq '4' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(112,68.8,303.3,-188.1, 31.2);
return;
}
if ($zonep eq '5' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(112,-86.6,289.0,-118.2,64.1);
return;
}

if ($zonep eq '6' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(112,-53.0,466.4,-36.1, 54.9);
return;
}

if ($zonep eq '7' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(112,-52.9,378.6,-24.1,29.9);
return;
}

if ($zonep eq '8' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(112,-57.0,532.3,0.0,65.2);
return;
}

if ($zonep eq '9' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(112,141.4,329.3,34.9,199.1);
return;
}

if ($zonep eq '10' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(112,49.8,428.4,86.8,249.6);
return;
}

if ($zonep eq '11' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(112,-50.8,92.3,-72.9,62.8);
return;
}

				
            }
                else
                {
                        for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);
								


                                if($c)
                                {
$name = $c->GetName();
my $bob = $i + 1;
#quest::say("Your bob is $bob, and your i is $i");
if ($zonep eq '0' ){
quest::say("- Group Port #$bob - Bye $name!");
quest::movepc(112,-124.3,583.9,-154.1,126.2);
}
if ($zonep eq '1' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(112,-8.4,534.0,-187.1,129.6);

}
if ($zonep eq '2' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(112,67.4, 487.2, -188.1, 125.6);

}
if ($zonep eq '3' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(112,-151.1, 427.0, -188.1, 125.6);

}
if ($zonep eq '4' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(112,68.8,303.3,-188.1, 31.2);

}
if ($zonep eq '5' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(112,-86.6,289.0,-118.2,64.1);

}

if ($zonep eq '6' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(112,-53.0,466.4,-36.1, 54.9);

}

if ($zonep eq '7' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(112,-52.9,378.6,-24.1,29.9);

}

if ($zonep eq '8' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(112,-57.0,532.3,0.0,65.2);

}

if ($zonep eq '9' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(112,141.4,329.3,34.9,199.1);

}

if ($zonep eq '10' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(112,49.8,428.4,86.8,249.6);

}

if ($zonep eq '11' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(112,-50.8,92.3,-72.9,62.8);

}

                                }
                        
}
return;
                }
quest::say("Report Error 420 in bug report. Sorry.");
return;
}

#quest::say("..Good Luck.");
#quest::movepc(112, -120, 587, -152, 129.0);
#return;
#}

if($text=~/kael/i && $ulevel >= 45){
#113,
#113, -633, -47, 128, 126.9
#113,120.1,-582.3,-118.1,53.6
#113,989.4,132.3,-138.1,138.0
#113,1420.5,-34.3,-182.4,13.9
#113,1398.2,411.7,-116.1,212.5
#113,1287.1,876.6,-58.1,5.5
########################

$zonep = int(rand (5));
$g = $client->GetGroup();

 if(!$g) {
$zonep = int(rand (5));
#$zonep = 5;
quest::say("You rolled $zonep.");				

if ($zonep eq '0' ){
quest::say("Well, $name off you go!");
quest::movepc(113, -633, -47, 128, 126.9);
return;
}
if ($zonep eq '1' ){
quest::say("$name, you sure seem to be ready to handle this!");
quest::movepc(113,120.1,-582.3,-118.1,53.6);
return;
}
if ($zonep eq '2' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(113,3166.1,-130.5,-395.5,0);
return;
}
if ($zonep eq '3' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(113,1678.1,-510.7,-184.4,0);
return;
}
if ($zonep eq '4' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(113,1164.3,1945.8,-36.0,0);
return;
}
if ($zonep eq '5' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(113,1287.1,876.6,-58.1,5.5);
return;
}

            }
                else
                {
                        for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);
								


                                if($c)
                                {
$name = $c->GetName();
my $bob = $i + 1;
my $lvl = $c->GetLevel();

if ($lvl >=45) {
if ($zonep eq '0' ){
quest::say("- Group Port #$bob - Bye $name!");
#$c->MovePC(113, -633, -47, 128, 126.9);
$c->MovePC(113, -633, -47, 128, 126.9);
}
if ($zonep eq '1' ){
quest::say("- Group Port # $bob - Bye $name!");
#$c->MovePC(113,120.1,-582.3,-118.1,53.6);
$c->MovePC(113,120.1,-582.3,-118.1,53.6);
}
if ($zonep eq '2' ){
quest::say("- Group Port # $bob - Bye $name!");
#$c->MovePC(113,989.4,132.3,-138.1,138.0);
$c->MovePC(113,1678.1,-510.7,-184.4,0);
}
if ($zonep eq '3' ){
quest::say("- Group Port # $bob - Bye $name!");
#$c->MovePC(113,1420.5,-34.3,-182.4,13.9);
$c->MovePC(113,1164.3,1945.8,-36.0,0);
}
if ($zonep eq '4' ){
quest::say("- Group Port # $bob - Bye $name!");
#$c->MovePC(113,1398.2,411.7,-116.1,212.5);
$c->MovePC(113,3166.1,-130.5,-395.5,0);
}
if ($zonep eq '5' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(113,1287.1,876.6,-58.1,5.5);

}

                  }else{
quest::say("- Group Port #$bob - Sorry $name but you are too low a level");
				  }				  
                        
}
#return;
                }
				
#quest::say("This shouldn't happen");
return;
}

#quest::say("..Good Luck.");
#quest::movepc(112, -120, 587, -152, 129.0);
return;


}

if($text=~/kaesora/i && $ulevel >=30){
quest::say("..Whee..");
quest::movepc(88,40,370,102,0);
return;
}

if($text=~/charasis/i && $ulevel >=40){
$client->KeyRingAdd("20600");
quest::summonitem("20600");
quest::say("..Whee..");
quest::movepc(105,0,0,4.0,0);
return;
}

if($text=~/dawnshroud/i && $ulevel >=30){
#quest::say("huh?");
#174,
#174,2071,1.5,89,192
#174,1665.9,519.4,102.4,193.2
#174,1025.6,1653.2,86.8,122.2
#174,-175,1889.5,103.3,125.2
#174,-576.8,1350.4,171.5,105.9
#174,-1335.5,133.1,85.8,105.4
#174,-1635.1,1314.0,252.4,44.6
#174,-877.7,-1823.4,101.3,253.9
#174,795.7,-1427.9,99.3,187.5
#174,1537.0,-1139.0,158.1,25.0
#174,-1236.2,59.4,171.4,38.9
#174,720.3,-442.8,130.1,190.2
########################

$zonep = int(rand (11));
$g = $client->GetGroup();

 if(!$g) {
$zonep = int(rand (11));
#quest::say("You rolled $zonep.");				
if ($zonep eq '0' ){
quest::say("Well, $name off you go!");
quest::movepc(174,2071,1.5,89,192);
return;
}
if ($zonep eq '1' ){
quest::say("$name, you sure seem to be ready to handle this!");
quest::movepc(174,1665.9,519.4,102.4,193.2);
return;
}
if ($zonep eq '2' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(174,1025.6,1653.2,86.8,122.2);
return;
}
if ($zonep eq '3' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(174,-175,1889.5,103.3,125.2);
return;
}
if ($zonep eq '4' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(174,-576.8,1350.4,171.5,105.9);
return;
}
if ($zonep eq '5' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(174,-1216.0,233.0,87.3,219.0);
return;
}

if ($zonep eq '6' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(174,-1635.1,1314.0,252.4,44.6);
return;
}

if ($zonep eq '7' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(174,-877.7,-1823.4,101.3,253.9);
return;
}

if ($zonep eq '8' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(174,795.7,-1427.9,99.3,187.5);
return;
}

if ($zonep eq '9' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(174,1537.0,-1139.0,158.1,25.0);
return;
}

if ($zonep eq '10' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(174,-1236.2,59.4,171.4,38.9);
return;
}

if ($zonep eq '11' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(174,720.3,-442.8,130.1,190.2);
return;
}

				
            }
                else
                {
                        for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);
								


                                if($c)
                                {
$name = $c->GetName();
my $bob = $i + 1;
my $lvl = $c->GetLevel();
#quest::say("$lvl");
if ($lvl >=30) {
if ($zonep eq '0' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(174,2071,1.5,89,192.0);
}
if ($zonep eq '1' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(174,1665.9,519.4,102.4,193.2);

}
if ($zonep eq '2' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(174,1025.6,1653.2,86.8,122.2);

}
if ($zonep eq '3' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(174,-175,1889.5,103.3,125.2);

}
if ($zonep eq '4' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(174,-576.8,1350.4,171.5,105.9);

}
if ($zonep eq '5' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(174,-1216.0,233.0,87.3,219.0);

}

if ($zonep eq '6' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(174,-1635.1,1314.0,252.4,44.6);

}

if ($zonep eq '7' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(174,-877.7,-1823.4,101.3,253.9);

}

if ($zonep eq '8' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(174,795.7,-1427.9,99.3,187.5);

}

if ($zonep eq '9' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(174,1537.0,-1139.0,158.1,25.0);

}

if ($zonep eq '10' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(174,-1236.2,59.4,171.4,38.9);

}

if ($zonep eq '11' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(174,720.3,-442.8,130.1,190.2);

}

                  }else{
quest::say("- Group Port #$bob - Sorry $name but you are too low a level");
				  }				  
                        
}
#return;
                }
				
#quest::say("This shouldn't happen");
return;
}

#quest::say("..Good Luck.");
#quest::movepc(112, -120, 587, -152, 129.0);
return;
}

if($text=~/crystal/i && $ulevel >=20){
quest::say("..Whee..");
quest::movepc(121,264,485,-76,182);
return;
}

if($text=~/echo/i && $ulevel >=20){
quest::say("..Whee..");
quest::movepc(153,-774.6,841.5,-29.8,61.9);
return;
}

if($text=~/karnor/i && $ulevel >= 40){
#102,302, 18, 6, 192
#102,141.8,278.7,1.9,189.9
#102,-172.3,278.7,1.9,107.0
#102,112.8,-250.2,-0.1,251.8
#102,-412.2,-208.5,-0.1,54.8
#102,-107.0,14.8,37.4,66.8
########################

$zonep = int(rand (5));
$g = $client->GetGroup();

 if(!$g) {
$zonep = int(rand (5));
#quest::say("You rolled $zonep.");				
if ($zonep eq '0' ){
quest::say("Well, $name off you go!");
quest::movepc(102,302, 18, 6, 192);
return;
}
if ($zonep eq '1' ){
quest::say("$name, you sure seem to be ready to handle this!");
quest::movepc(102,141.8,278.7,1.9,189.9);
return;
}
if ($zonep eq '2' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(102,-172.3,278.7,1.9,107.0);
return;
}
if ($zonep eq '3' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(102,112.8,-250.2,-0.1,251.8);
return;
}
if ($zonep eq '4' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(102,-412.2,-208.5,-0.1,54.8);
return;
}
if ($zonep eq '5' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(102,-107.0,14.8,37.4,66.8);
return;
}

            }
                else
                {
                        for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);
								


                                if($c)
                                {
$name = $c->GetName();
my $bob = $i + 1;
my $lvl = $c->GetLevel();

if ($lvl >=40) {
if ($zonep eq '0' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(102,302, 18, 6, 192);
}
if ($zonep eq '1' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(102,141.8,278.7,1.9,189.9);

}
if ($zonep eq '2' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(102,-172.3,278.7,1.9,107.0);

}
if ($zonep eq '3' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(102,112.8,-250.2,-0.1,251.8);

}
if ($zonep eq '4' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(102,-412.2,-208.5,-0.1,54.8);

}
if ($zonep eq '5' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(102,-107.0,14.8,37.4,66.8);

}

                  }else{
quest::say("- Group Port #$bob - Sorry $name but you are too low a level");
				  }				  
                        
}
#return;
                }
				
#quest::say("This shouldn't happen");
return;
}

#quest::say("..Good Luck.");
#quest::movepc(112, -120, 587, -152, 129.0);
return;



}

if($text=~/tov/i && $ulevel >= 50){
quest::say("..Good Luck.");
quest::movepc(124, -504, -2111, -36, 2.2);
return;
}

if($text=~/sebilis/i && $ulevel >= 45){
quest::say("What is this sebilis you speak of?");
return;
#89,0,250,44,128.2
#89,162.6,-362.5,-34.1,206.0
#89,-254.9,-419.5,-52.1,132.0
#89,518.4,-457.2, -52.1,193.5
#89,639.8,-484.3,-47.8,103.1
#89,554.3,-581.9,-52.1,193.9

$zonep = int(rand (5));
$g = $client->GetGroup();

 if(!$g) {
$zonep = int(rand (5));
if ($zonep eq '0' ){
quest::say("Well, $name off you go!");
quest::movepc(89,0,250,44,128.2);
return;
}
if ($zonep eq '1' ){
quest::say("$name, you sure seem to be ready to handle this!");
quest::movepc(89,162.6,-362.5,-34.1,206.0);
return;
}
if ($zonep eq '2' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(89,-254.9,-419.5,-52.1,132.0);
return;
}
if ($zonep eq '3' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(89,518.4,-457.2, -52.1,193.5);
return;
}
if ($zonep eq '4' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(89,639.8,-484.3,-47.8,103.1);
return;
}
if ($zonep eq '5' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(89,554.3,-581.9,-52.1,193.9);
return;
}

            }
                else
                {
                        for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);
								


                                if($c)
                                {
$name = $c->GetName();
my $bob = $i + 1;
my $lvl = $c->GetLevel();


if ($lvl >=45) {
if ($zonep eq '0' ){
quest::say("- Group Port #$bob - Bye $name!");
$c->MovePC(89,0,250,44,128.2);
}
if ($zonep eq '1' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(89,162.6,-362.5,-34.1,206.0);

}
if ($zonep eq '2' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(89,-254.9,-419.5,-52.1,132.0);

}
if ($zonep eq '3' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(89,518.4,-457.2, -52.1,193.5);

}
if ($zonep eq '4' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(89,639.8,-484.3,-47.8,103.1);

}
if ($zonep eq '5' ){
quest::say("- Group Port # $bob - Bye $name!");
$c->MovePC(89,554.3,-581.9,-52.1,193.9);

}

                  }else{
quest::say("- Group Port #$bob - Sorry $name but you are too low a level");
				  }				  
                        
}
#return;
                }
				
#quest::say("This shouldn't happen");
return;
}

#quest::say("..Good Luck.");
#quest::movepc(112, -120, 587, -152, 129.0);
return;



}

if(($text=~/paludal/i) && $ulevel >= 5){

if($ulevel >= 5  ) {
#lets decide where they zone in at
$zonep = int(rand (4));

$g = $client->GetGroup();

    if(!$g) {
$zonep = int(rand (4));
				
if ($zonep eq '0' ){
quest::say("Well, $name off you go!");
quest::movepc(156, -232, -3707, 196, 64.6);
return;
}
if ($zonep eq '1' ){
quest::say("$name, you sure seem to be ready to handle this!");
quest::movepc(156, 1021.7, -1976.8, -307.8, 0.1);
return;
}
if ($zonep eq '2' ){
quest::say("Just so you know $name, the last 3 people that have tried this, have died. Good luck.");
quest::movepc(156, -345.7, -2141.9, -10.1, 242.5);
return;
}
if ($zonep eq '3' ){
quest::say("Did you know, grokmol died to a fungus in here? A FREAKING FUNGUS! HAHAHHAHAHAH");
quest::movepc(156, 363, -714.7, -216.2, 47);
return;
}

				
            }
                else
                {
                        for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);


                                if($c)
                                {
$name = $c->GetName();


if ($zonep eq '0' ){
quest::say("-Group Port- Here you go $name. You weak pathetic being.");
$c->MovePC(156, -232, -3707, 196, 64.6);


}
if ($zonep eq '1' ){
quest::say("-Group Port- Here you go $name. You weak pathetic being.");
$c->MovePC(156, 1021.7, -1976.8, -307.8, 0.1);

}
if ($zonep eq '2' ){
quest::say("-Group Port- Here you go $name. You weak pathetic being.");
$c->MovePC(156, -345.7, -2141.9, -10.1, 242.5);

}
if ($zonep eq '3' ){
quest::say("-Group Port- Here you go $name. You weak pathetic being.");
$c->MovePC(156, 363, -714.7, -216.2, 47);

}

                                }
                        }
return;
                }
##Paludwhatever
#quest::say("Well here you go weakling");
#quest::movepc(156, -232, -3707, 196, 64.6);

quest::say("This shouldn't happen");
return;
}
}

quest::say("Sorry $name you are either to low a level, or I just dont want to deal with you, take your pick.");
} 


sub EVENT_ITEM {
if($ulevel <5){
quest::say("Get to level 5 then lets talk noob");
return;
}

#quest::say("[paludal]             - Level 5+ *Random Solo and Group Port*");
#quest::say("[crystal] caverns     - Level 20+");
#quest::say("[echo] caverns        - Level 20+");
#quest::say("[dreadlands]          - Level 25+ *Random Solo and Group Port*");
#quest::say("[kaesora]             - Level 30+");
#quest::say("[dawnshroud]          - Level 30+ *Random Solo and Group Port*");
#quest::say("[charasis]            - Level 40+");
#quest::say("[karnor]              - Level 40+ *Random Solo and Group Port*");
#quest::say("[sebilis]             - Level 45+ *Random Solo and Group Port*");
#quest::say("[kael]                - Level 45+ *Random Solo and Group Port*");
#quest::say("[Akheva]            - Level 45+ *Random Solo and Group Port*");
#quest::say("[tov]

#1900 is highest, above that adds chance to fail (no loot at all).
my $zonep = int(rand (4));
###### SOLO PORTS #####
$checkpaludal     = plugin::check_handin(\%itemcount,148004=> 1);
$checkcrystal     = plugin::check_handin(\%itemcount,148005=> 1);
$checkecho        = plugin::check_handin(\%itemcount,148006=> 1);
$checkdreadlands  = plugin::check_handin(\%itemcount,148007=> 1);
$checkkaesora     = plugin::check_handin(\%itemcount,148008=> 1);
$checkdawnshroud  = plugin::check_handin(\%itemcount,148009=> 1);
$checkcharasis    = plugin::check_handin(\%itemcount,148010=> 1);
$checkkarnor      = plugin::check_handin(\%itemcount,148011=> 1);
$checksebilis     = plugin::check_handin(\%itemcount,148012=> 1);
$checkkael        = plugin::check_handin(\%itemcount,148013=> 1);
$checkakheva      = plugin::check_handin(\%itemcount,148014=> 1);
$checktov         = plugin::check_handin(\%itemcount,148015=> 1);
$checkgrey        = plugin::check_handin(\%itemcount,148995=> 1);

############## GROUP PORTS ############
$checkgrppaludal     = plugin::check_handin(\%itemcount,148016=> 1);
$checkgrpcrystal     = plugin::check_handin(\%itemcount,148017=> 1);
$checkgrpecho        = plugin::check_handin(\%itemcount,148018=> 1);
$checkgrpdreadlands  = plugin::check_handin(\%itemcount,148019=> 1);
$checkgrpkaesora     = plugin::check_handin(\%itemcount,148020=> 1);
$checkgrpdawnshroud  = plugin::check_handin(\%itemcount,148021=> 1);
$checkgrpcharasis    = plugin::check_handin(\%itemcount,148022=> 1);
$checkgrpkarnor      = plugin::check_handin(\%itemcount,148023=> 1);
$checkgrpsebilis     = plugin::check_handin(\%itemcount,148024=> 1);
$checkgrpkael        = plugin::check_handin(\%itemcount,148025=> 1);
$checkgrpakheva      = plugin::check_handin(\%itemcount,148026=> 1);
$checkgrptov         = plugin::check_handin(\%itemcount,148027=> 1);
$checkgrpgrey        = plugin::check_handin(\%itemcount,148993=> 1);

###### SOLO PORT #####

if ( $checkpaludal ) {
$client->Message( 15,"We are sending you to ** PALUDAL **");

if ($zonep eq '0' ){
quest::movepc(156, -232, -3707, 196, 64.6);
return;
}
if ($zonep eq '1' ){
quest::movepc(156, 1021.7, -1976.8, -307.8, 0.1);
return;
}
if ($zonep eq '2' ){
quest::movepc(156, -345.7, -2141.9, -10.1, 242.5);
return;
}
if ($zonep eq '3' ){
quest::movepc(156, 363, -714.7, -216.2, 47);
return;
}

}

if ( $checkgrey ) {
$client->Message( 15,"We are sending you to ** THE GREY **");

if ($zonep eq '0' ){
quest::movepc(171,343.9,-1991.7,-28.8,0);
return;
}
if ($zonep eq '1' ){
quest::movepc(171,1620.9,-1539.2,-13.0,0);
return;
}
if ($zonep eq '2' ){
quest::movepc(171,-101.7,70.5,-44.7,0);
return;
}
if ($zonep eq '3' ){
quest::movepc(171,1644.0,1733.0,2.6,0);
return;
}
if ($zonep eq '4' ){
quest::movepc(171,-1694.3,1702.9,-16.2,0);
return;
}
if ($zonep eq '5' ){
quest::movepc(171,-1928.2,549.6,1.4,0);
return;
}
if ($zonep eq '5' ){
quest::movepc(171,1467.3,-1115.2,214.0,0);
return;
}

}



if ( $checkcrystal ) {
$client->Message( 15,"We are sending you to ** CRYSTAL CAVERNS **");
quest::movepc(121,264,485,-76,182);
}

if ( $checkecho ) {
$client->Message( 15,"We are sending you to ** ECHO CAVERNS **");

quest::movepc(153,-774.6,841.5,-29.8,61.9);
}

############## TESTING ##########
if ( $checkakheva ) {
$zonep = int(rand (6));
$client->Message( 15,"We are sending you to ** AKHEVA **");
if ($zonep eq '0' ){
quest::say("Well, $name off you go!");
quest::movepc(179,60.0,-1395.0,22.8,0);
}

if ($zonep eq '1' ){
quest::say("Well, $name off you go!");
quest::movepc(179,-406.5,295.0,-16.1,0);
return;
}

if ($zonep eq '2' ){
quest::say("Well, $name off you go!");
quest::movepc(179,-241.4,295.3,-16.1,0);
return;
}

if ($zonep eq '3' ){
quest::say("Well, $name off you go!");
quest::movepc(179,-245.5,-94.7,-16.1,0);
return;
}

if ($zonep eq '4' ){
quest::say("Well, $name off you go!");
quest::movepc(179,-1440.7,1370.7,14.9,0);
return;
}

if ($zonep eq '5' ){
quest::say("Well, $name off you go!");
quest::movepc(179,-404.5,296.5,-16.1,0);
return;
}
if ($zonep eq '6' ){
quest::say("Well, $name off you go!");
quest::movepc(179,-969.6,-391.0,9.9,0);
return;
}

}
############ END TESTING #######

if ( $checkdreadlands ) {
$client->Message( 15,"We are sending you to ** DREADLANDS **");
$zonep = int(rand (17));


if ($zonep eq '0' ){

quest::movepc(86,9655.7,3033.6,1047.3, 183.2 );
return;
}
if ($zonep eq '1' ){

quest::movepc(86,9924.3,2569.2,104.3, 178.9);
return;
}
if ($zonep eq '2' ){

quest::movepc(86,9854.5,1973.0,1052.0,187.8);
return;
}
if ($zonep eq '3' ){
quest::movepc(86,9961.8,1128.0,1062.8,191.5);
return;
}
if ($zonep eq '4' ){

quest::movepc(86,9894.6,393.9,1062.8,187.8);
return;
}
if ($zonep eq '5' ){

quest::movepc(86,9000.7,-54.5,1047.3,241.9);
return;
}

if ($zonep eq '6' ){

quest::movepc(86,7158.1,1388.8,1048.1,67.2);
return;
}

if ($zonep eq '7' ){

quest::movepc(86,3606.0,-1961.1,740.3,77.5);
return;
}

if ($zonep eq '8' ){

quest::movepc(86,5144.4,-946.7,1165.1,147.0);
return;
}

if ($zonep eq '9' ){

quest::movepc(86,3737.4,2234.0,700.9,150.4);
return;
}

if ($zonep eq '10' ){
quest::movepc(86,-2163.7,1006.2,24.9,255.2);
return;
}

if ($zonep eq '11' ){

quest::movepc(86,-2917.1,816.4,24.9,189.6);
return;
}

if ($zonep eq '12' ){

quest::movepc(86,-2907.9,106.5,24.9,130.4);
return;
}

if ($zonep eq '13' ){

quest::movepc(86,-1856,162.6,24.9,63.4);
return;
}

if ($zonep eq '14' ){

quest::movepc(86,-1703.7,439.6,24.9,65.5);
return;
}

if ($zonep eq '15' ){

quest::movepc(86,-3469.5,-1829.8,36.1,246.8);
return;
}

if ($zonep eq '16' ){

quest::movepc(86,-5560.7,-433.6,71.8,29.8);
return;
}
}

if ( $checkkaesora ) {
$client->Message( 15,"We are sending you to ** KAESORA**");
quest::movepc(88,40,370,102,0);
return;
}

if ( $checkdawnshroud ) {
$client->Message( 15,"We are sending you to ** DAWNSHROUD PEAKS **");
$zonep = int(rand (11));
if ($zonep eq '0' ){

quest::movepc(174,2071,1.5,89,192);
return;
}
if ($zonep eq '1' ){

quest::movepc(174,1665.9,519.4,102.4,193.2);
return;
}
if ($zonep eq '2' ){

quest::movepc(174,1025.6,1653.2,86.8,122.2);
return;
}
if ($zonep eq '3' ){

quest::movepc(174,-175,1889.5,103.3,125.2);
return;
}
if ($zonep eq '4' ){

quest::movepc(174,-576.8,1350.4,171.5,105.9);
return;
}
if ($zonep eq '5' ){

quest::movepc(174,-1216.0,233.0,87.3,219.0);
return;
}

if ($zonep eq '6' ){

quest::movepc(174,-1635.1,1314.0,252.4,44.6);
return;
}

if ($zonep eq '7' ){

quest::movepc(174,-877.7,-1823.4,101.3,253.9);
return;
}

if ($zonep eq '8' ){

quest::movepc(174,795.7,-1427.9,99.3,187.5);
return;
}

if ($zonep eq '9' ){

quest::movepc(174,1537.0,-1139.0,158.1,25.0);
return;
}

if ($zonep eq '10' ){

quest::movepc(174,-1236.2,59.4,171.4,38.9);
return;
}


}

if ( $checkcharasis ) {
$client->Message( 15,"We are sending you to ** CHARASIS **");
quest::movepc(105,0,0,4.0,0);
return;
}

if ( $checkkarnor ) {
$client->Message( 15,"We are sending you to ** KARNOR **");

$zonep = int(rand (5));


if ($zonep eq '0' ){
quest::movepc(102,302, 18, 6, 192);
return;
}
if ($zonep eq '1' ){
quest::movepc(102,141.8,278.7,1.9,189.9);
return;
}
if ($zonep eq '2' ){
quest::movepc(102,-172.3,278.7,1.9,107.0);
return;
}
if ($zonep eq '3' ){
quest::movepc(102,112.8,-250.2,-0.1,251.8);
return;
}
if ($zonep eq '4' ){
quest::movepc(102,-412.2,-208.5,-0.1,54.8);
return;
}
}

if ( $checksebilis ) {
$client->Message( 15,"We are sending you to ** SEBILIS **");

$zonep = int(rand (5));
#quest::say("You rolled $zonep.");				
if ($zonep eq '0' ){
quest::movepc(89,0,250,44,128.2);
return;
}
if ($zonep eq '1' ){
quest::movepc(89,162.6,-362.5,-34.1,206.0);
return;
}
if ($zonep eq '2' ){
quest::movepc(89,-254.9,-419.5,-52.1,132.0);
return;
}
if ($zonep eq '3' ){
quest::movepc(89,518.4,-457.2, -52.1,193.5);
return;
}
if ($zonep eq '4' ){
quest::movepc(89,639.8,-484.3,-47.8,103.1);
return;
}
if ($zonep eq '5' ){
quest::movepc(89,554.3,-581.9,-52.1,193.9);
return;
}
}

if ( $checkkael ) {
$client->Message( 15,"We are sending you to ** KAEL **");
$zonep = int(rand (5));

if ($zonep eq '0' ){
$c->MovePC(113, -633, -47, 128, 126.9);
return;
}
if ($zonep eq '1' ){
$c->MovePC(113,120.1,-582.3,-118.1,53.6);
return;
}
if ($zonep eq '2' ){
$c->MovePC(113,3166.1,-130.5,-395.5,0);
return;
}
if ($zonep eq '3' ){
$c->MovePC(113,1678.1,-510.7,-184.4,0);
return;
}
if ($zonep eq '4' ){
$c->MovePC(113,1164.3,1945.8,-36.0,0);
return;
}
if ($zonep eq '5' ){
$c->MovePC(113,1287.1,876.6,-58.1,5.5);
return;
}

}

if ( $checkvelketor ) {
$client->Message( 15,"We are sending you to ** VELKETOR **");
$zonep = int(rand (11));

if ($zonep eq '0' ){
quest::movepc(112,-124.3,583.9,-154.1,126.2);
return;
}
if ($zonep eq '1' ){
quest::movepc(112,-8.4,534.0,-187.1,129.6);
return;
}
if ($zonep eq '2' ){
quest::movepc(112,67.4, 487.2, -188.1, 125.6);
return;
}
if ($zonep eq '3' ){
quest::movepc(112,-151.1, 427.0, -188.1, 125.6);
return;
}
if ($zonep eq '4' ){
quest::movepc(112,68.8,303.3,-188.1, 31.2);
return;
}
if ($zonep eq '5' ){
quest::movepc(112,-86.6,289.0,-118.2,64.1);
return;
}
if ($zonep eq '6' ){
quest::movepc(112,-53.0,466.4,-36.1, 54.9);
return;
}
if ($zonep eq '7' ){
quest::movepc(112,-52.9,378.6,-24.1,29.9);
return;
}
if ($zonep eq '8' ){
quest::movepc(112,-57.0,532.3,0.0,65.2);
return;
}
if ($zonep eq '9' ){
quest::movepc(112,141.4,329.3,34.9,199.1);
return;
}
if ($zonep eq '10' ){
quest::movepc(112,49.8,428.4,86.8,249.6);
return;
}
if ($zonep eq '11' ){
quest::movepc(112,-50.8,92.3,-72.9,62.8);
return;
}

}

if ( $checktov ) {
$client->Message( 15,"We are sending you to ** TEMPLE OF VEESHAN **");
quest::movepc(124, -504, -2111, -36, 2.2);
return;
}


############# GROUP PORTS ############
#$checkgrppaludal
#$checkgrpcrystal
#$checkgrpecho
#$checkgrpdreadlands
#$checkgrpkaesora
#$checkgrpdawnshroud
#$checkgrpcharasis
#$checkgrpkarnor
#$checkgrpsebilis
#$checkgrpkael
#$checkgrpvelketor
#$checkgrptov

if ( $checkgrppaludal ) {
$g = $client->GetGroup();
if(!$g) {
quest::say("You aren't in a group get lost fucker");
return;
}
	$zonep = int(rand (4));
	quest::say("BOOM");
                        for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);


                                if($c)
                                {
$name = $c->GetName();
$c->Message( 15,"We are sending your GROUP to ** PALUDAL **");

if ($zonep eq '0' ){
$c->MovePC(156, -232, -3707, 196, 64.6);
}
if ($zonep eq '1' ){
$c->MovePC(156, 1021.7, -1976.8, -307.8, 0.1);
}
if ($zonep eq '2' ){
$c->MovePC(156, -345.7, -2141.9, -10.1, 242.5);
}
if ($zonep eq '3' ){
$c->MovePC(156, 363, -714.7, -216.2, 47);
}

}
}
}


if ( $checgrpcrystal ) {
return;
}

if ( $checkgrpecho ) {
return;
}

if ( $checkgrpdreadlands ) {
$g = $client->GetGroup();
if(!$g) {
quest::say("You aren't in a group get lost fucker");
return;
}
$zonep = int(rand (16));
$client->Message( 15,"We are sending your GROUP to ** DREADLANDS **");
####
                        for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);


                                if($c)
                                {

if ($zonep eq '0' ){
quest::movepc(86,9655.7,3033.6,1047.3, 183.2 );
}
if ($zonep eq '1' ){
$c->MovePC(86,9924.3,2569.2,104.3, 178.9);
}
if ($zonep eq '2' ){
$c->MovePC(86,9854.5,1973.0,1052.0,187.8);
}
if ($zonep eq '3' ){
$c->MovePC(86,9961.8,1128.0,1062.8,191.5);
}
if ($zonep eq '4' ){
$c->MovePC(86,9894.6,393.9,1062.8,187.8);
}
if ($zonep eq '5' ){
$c->MovePC(86,9000.7,-54.5,1047.3,241.9);
}
if ($zonep eq '6' ){
$c->MovePC(86,7158.1,1388.8,1048.1,67.2);
}
if ($zonep eq '7' ){
$c->MovePC(86,3606.0,-1961.1,740.3,77.5);
}
if ($zonep eq '8' ){
$c->MovePC(86,5144.4,-946.7,1165.1,147.0);
}
if ($zonep eq '9' ){
$c->MovePC(86,3737.4,2234.0,700.9,150.4);
}
if ($zonep eq '10' ){
$c->MovePC(86,-2163.7,1006.2,24.9,255.2);
}
if ($zonep eq '11' ){
$c->MovePC(86,-2917.1,816.4,24.9,189.6);
}
if ($zonep eq '12' ){
$c->MovePC(86,-2907.9,106.5,24.9,130.4);
}
if ($zonep eq '13' ){
$c->MovePC(86,-1856,162.6,24.9,63.4);
}
if ($zonep eq '14' ){
$c->MovePC(86,-1703.7,439.6,24.9,65.5);
}
if ($zonep eq '15' ){
$c->MovePC(86,-3469.5,-1829.8,36.1,246.8);
}
if ($zonep eq '16' ){
$c->MovePC(86,-5560.7,-433.6,71.8,29.8);
}
                                }
                        

                }
####
}

if ( $checkgrpkaesora ) {
#no group
return;

}

if ( $checkgrpdawnshroud ) {
$g = $client->GetGroup();
if(!$g) {
quest::say("You aren't in a group get lost fucker");
return;
}
$client->Message( 15,"We are sending your GROUP to ** DAWNSHROUD **");
$zonep = int(rand (11));
                      for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);
								


                                if($c)
                                {

if ($zonep eq '0' ){
$c->MovePC(174,2071,1.5,89,192.0);
}
if ($zonep eq '1' ){
$c->MovePC(174,1665.9,519.4,102.4,193.2);
}
if ($zonep eq '2' ){
$c->MovePC(174,1025.6,1653.2,86.8,122.2);
}
if ($zonep eq '3' ){
$c->MovePC(174,-175,1889.5,103.3,125.2);
}
if ($zonep eq '4' ){
$c->MovePC(174,-576.8,1350.4,171.5,105.9);
}
if ($zonep eq '5' ){
$c->MovePC(174,-1216.0,233.0,87.3,219.0);
}
if ($zonep eq '6' ){
$c->MovePC(174,-1635.1,1314.0,252.4,44.6);
}
if ($zonep eq '7' ){
$c->MovePC(174,-877.7,-1823.4,101.3,253.9);
}
if ($zonep eq '8' ){
$c->MovePC(174,795.7,-1427.9,99.3,187.5);
}
if ($zonep eq '9' ){
$c->MovePC(174,1537.0,-1139.0,158.1,25.0);
}
if ($zonep eq '10' ){
$c->MovePC(174,-1236.2,59.4,171.4,38.9);
}
if ($zonep eq '11' ){
$c->MovePC(174,720.3,-442.8,130.1,190.2);
}
}
}
}



if ( $checkgrpcharasis ) {
#no group
return;
}

if ( $checkgrpkarnor ) {
$g = $client->GetGroup();
if(!$g) {
quest::say("You aren't in a group get lost fucker");
return;
}
$client->Message( 15,"We are sending your GROUP to ** KARNOR **");
$zonep = int(rand (5));
                       for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);
								


                                if($c)
                                {

if ($zonep eq '0' ){
$c->MovePC(102,302, 18, 6, 192);
}
if ($zonep eq '1' ){
$c->MovePC(102,141.8,278.7,1.9,189.9);
}
if ($zonep eq '2' ){
$c->MovePC(102,-172.3,278.7,1.9,107.0);
}
if ($zonep eq '3' ){
$c->MovePC(102,112.8,-250.2,-0.1,251.8);
}
if ($zonep eq '4' ){
$c->MovePC(102,-412.2,-208.5,-0.1,54.8);
}
if ($zonep eq '5' ){
$c->MovePC(102,-107.0,14.8,37.4,66.8);
}

}
}
}
////////////////////////////////////////////////////

if ( $checkgrpgrey ) {
$g = $client->GetGroup();
if(!$g) {
quest::say("You aren't in a group get lost fucker");
return;
}
$zonep = int(rand (6));
$client->Message( 15,"We are sending your GROUP to ** THE GREY **");

                       for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);



                                if($c)
                                {
//								
if ($zonep eq '0' ){
$c->MovePC(171,343.9,-1991.7,-28.8,0);
}

if ($zonep eq '1' ){
$c->MovePC(171,1620.9,-1539.2,-13.0,0);
}

if ($zonep eq '2' ){
$c->MovePC(171,-101.7,70.5,-44.7,0);
}

if ($zonep eq '3' ){
$c->MovePC(171,1644.0,1733.0,2.6,0);
}

if ($zonep eq '4' ){
$c->MovePC(171,-1694.3,1702.9,-16.2,0);
}

if ($zonep eq '5' ){
$c->MovePC(171,-1928.2,549.6,1.4,0);
}

if ($zonep eq '6' ){
$c->MovePC(171,1467.3,-1115.2,214.0,0);
}
//
}
}
}

////////////////////////////////////////////////////


if ( $checkgrpsebilis ) {
$g = $client->GetGroup();
if(!$g) {
quest::say("You aren't in a group get lost fucker");
return;
}
$zonep = int(rand (5));
$client->Message( 15,"We are sending your GROUP to ** SEBILIS **");

                       for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);
								


                                if($c)
                                {
if ($zonep eq '0' ){
quest::movepc(89,0,250,44,128.2);
}
if ($zonep eq '1' ){
$c->MovePC(89,162.6,-362.5,-34.1,206.0);
}
if ($zonep eq '2' ){
$c->MovePC(89,-254.9,-419.5,-52.1,132.0);
}
if ($zonep eq '3' ){
$c->MovePC(89,518.4,-457.2, -52.1,193.5);
}
if ($zonep eq '4' ){
$c->MovePC(89,639.8,-484.3,-47.8,103.1);
}
if ($zonep eq '5' ){
$c->MovePC(89,554.3,-581.9,-52.1,193.9);
}
}
}
}

if ( $checkgrpkael ) {
$g = $client->GetGroup();
if(!$g) {
quest::say("You aren't in a group get lost fucker");
return;
}
$zonep = int(rand (5));
$client->Message( 15,"We are sending your GROUP to ** KAEL **");

                       for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);
                                if($c)
                                {
if ($zonep eq '0' ){
$c->MovePC(113, -633, -47, 128, 126.9);
}
if ($zonep eq '1' ){
$c->MovePC(113,120.1,-582.3,-118.1,53.6);
}
if ($zonep eq '2' ){
$c->MovePC(113,989.4,132.3,-138.1,138.0);
}
if ($zonep eq '3' ){
$c->MovePC(113,1420.5,-34.3,-182.4,13.9);
}
if ($zonep eq '4' ){
$c->MovePC(113,1398.2,411.7,-116.1,212.5);
}
if ($zonep eq '5' ){
$c->MovePC(113,1287.1,876.6,-58.1,5.5);
}
}
}
}

if ( $checkgrpvelketor ) {
$g = $client->GetGroup();
if(!$g) {
quest::say("You aren't in a group get lost fucker");
return;
}
$client->Message( 15,"We are sending your GROUP to ** VELKETOR **");
$zonep = int(rand (11));

                        for($i = 0; $i < 6; ++$i)
                        {
                                $c = $g->GetMember($i);
								


                                if($c)
                                {

if ($zonep eq '0' ){
$c->MovePC(112,-124.3,583.9,-154.1,126.2);
}
if ($zonep eq '1' ){
$c->MovePC(112,-8.4,534.0,-187.1,129.6);
}
if ($zonep eq '2' ){
$c->MovePC(112,67.4, 487.2, -188.1, 125.6);
}
if ($zonep eq '3' ){
$c->MovePC(112,-151.1, 427.0, -188.1, 125.6);
}
if ($zonep eq '4' ){
$c->MovePC(112,68.8,303.3,-188.1, 31.2);
}
if ($zonep eq '5' ){
$c->MovePC(112,-86.6,289.0,-118.2,64.1);
}
if ($zonep eq '6' ){
$c->MovePC(112,-53.0,466.4,-36.1, 54.9);
}
if ($zonep eq '7' ){
$c->MovePC(112,-52.9,378.6,-24.1,29.9);
}
if ($zonep eq '8' ){
$c->MovePC(112,-57.0,532.3,0.0,65.2);
}
if ($zonep eq '9' ){
$c->MovePC(112,141.4,329.3,34.9,199.1);
}
if ($zonep eq '10' ){
$c->MovePC(112,49.8,428.4,86.8,249.6);
}
if ($zonep eq '11' ){
$c->MovePC(112,-50.8,92.3,-72.9,62.8);
}
}
}
return;
}




if ( $checkgrptov ) {
#no group
return;
}

}
