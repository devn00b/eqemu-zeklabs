sub EVENT_SPAWN
{
#leash stuff
quest::set_proximity($x - 100, $x + 100, $y - 100, $y + 100);

#lets do 3 items drops, so 3 randoms so we dont drop same item 3 times
#$rnd = quest::ChooseRandom(144586,9412,144592,144593,144594,144595,144596,144597,144598);
#$rndtwo = quest::ChooseRandom(144586,9412,144592,144593,144594,144595,144596,144597,144598,144602);
#$rndthree = quest::ChooseRandom(144586,9412,144592,144593,144594,144595,144596,144597,144598,46117);
#engraved Royal velium
$rnd = quest::ChooseRandom(8957,8958,8959,8971,8972,8973,8974);
#Misc Items CC shield, coldain skin boots/goves, OOIV, book of obo,tonpo shoulders, robe of azure, Ice Forged Shackles.
$rndtwo = quest::ChooseRandom(25287,25026,25027,25098,14745,1283,145366,30041);
#crystal chitin armor, lodi armor, regal band, shaded torch,infestation
$rndthree = quest::ChooseRandom(25283,25284,25285,25286,25287,25288,25289,25290,25291,5727,29206,22816,30069,2061);

$lewts = int(rand 300);

#add 3x loot some cash and a 200pt ticket
if($lewts <101){
$npc->AddItem($rnd,1,0);
$pltrnd = int(rand 1000);
$npc->AddCash(0,0,0,$pltrnd);
return;
}
if($lewts <101){
$pltrnd = int(rand 1000);
$npc->AddCash(0,0,0,$pltrnd);
$npc->AddItem($rndtwo,1,0);
return;
}

if($lewts <101){
$pltrnd = int(rand 1000);
$npc->AddCash(0,0,0,$pltrnd);
$npc->AddItem($rndthree,1,0);
return;
}

$pltrnd = int(rand 1000); 
$npc->AddCash(0,0,0,$pltrnd);
#$npc->AddItem(144586,1,0);
#just giving them 1, with a max of 4 (3 above + this one) so max of 800pts.


$whatshout = int(rand 100);
if ($whatshout <25){
quest::shout("Rooooaaaarrrr! I am the true meow pal, you cannot possibly think you had seen the last of me!!");
}
elsif ($whatshout>25 && $whatshout <50){
quest::shout("No one is worthy of my attention. Not a single one of you!");
}
elsif ($whatshout >50 && $whatshout <75) {
quest::shout("Who thinks they can best me in combat? I'll take you all on at once! Any Takers?");
}
else {
quest::shout("Looking for 1v1. Someone 1v1 me bro!");
}

}

sub EVENT_AGGRO {
     quest::settimer("CheckLeash", 1);
}
sub EVENT_TIMER {

#checkleash additions

        if ($timer eq "CheckLeash"){
                quest::set_proximity($x - 100, $x + 100, $y - 100, $y + 100);
                my $GuardX = $npc->GetGuardPointX();
                my $GuardY = $npc->GetGuardPointY();
                my $GuardZ = $npc->GetGuardPointZ();
                my $CurDist = $npc->CalculateDistance($GuardX, $GuardY, $GuardZ);
                #quest::say("Dist: $CurDist");
                if ($CurDist > 5)
                {
                        $npc->SetHP(533349);
			#let them know we wont leave this spotish
			#quest::shout("Fools I wont Chase you! Fight like men or dont waste my time!!!");
                        quest::stoptimer("CheckLeash");
                        # Get their current heading, since there is currently no way to get Guard Heading (yet)
                        my $CurH = $npc->GetHeading();
                        # Wipe the NPC's hate list as well
                        $npc->WipeHateList();
                        # Return them to their Guard Point
                        $npc->GMMove($GuardX,$GuardY,$GuardZ,$CurH);
                }
                else
                {
                quest::stoptimer("CheckLeash");
                quest::settimer("CheckLeash", 1);
                }
}

}

sub EVENT_DEATH {
#   plugin::handlenpc_epic15loot($client,$npc,1);
#   plugin::handlenpc_epic15loot($client,$npc,1);
   quest::faction(67, -2000);
}


