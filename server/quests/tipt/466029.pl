sub EVENT_SPAWN
{
quest::setnexthpevent(99);
quest::settimer("CheckLeash", 1);
#leash stuff
#quest::set_proximity($x - 100, $x + 100, $y - 100, $y + 100);

#lets do 3 items drops, so 3 randoms so we dont drop same item 3 times

#engraved Royal velium
$rnd = quest::ChooseRandom(8957,8958,8959,8971,8972,8973,8974);
#Misc Items CC shield, coldain skin boots/goves, OOIV, book of obo,tonpo shoulders, robe of azure, Ice Forged Shackles.
$rndtwo = quest::ChooseRandom(25287,25026,25027,25098,14745,1283,145366,30041);
#crystal chitin armor, lodi armor, regal band, shaded torch,infestation
$rndthree = quest::ChooseRandom(25283,25284,25285,25286,25287,25288,25289,25290,25291,5727,29206,22816,30069,2061,143063);

#Add 3x loot some cash.
$npc->AddItem($rnd,1,0);
$npc->AddItem($rndtwo,1,0);
$npc->AddItem($rndthree,1,0);
$pltrnd  = int(rand 1000); 
$gldrnd  = int(rand 900);
$silvrnd = int(rand 500);
$npc->AddCash(0,$silvrnd,$gldrnd,$pltrnd);

#just giving them 1, with a max of 4 (3 above + this one) so max of 800pts.


$whatshout = int(rand 100);
if ($whatshout <25){
#quest::shout2("Qeynos WILL be mine some day!");
}
elsif ($whatshout>25 && $whatshout <50){
#quest::shout2("No one is worthy of my attention. Not a single one of you!");
}
elsif ($whatshout >50 && $whatshout <75) {
#quest::shout2("Who thinks they can best me in combat? I'll take you all on at once! Any Takers?");
}
else {
#quest::shout2("Looking for 1v1. Someone 1v1 me bro! No? Cowards, Pussies, Loosers All of you.");
}

}

sub EVENT_AGGRO {
     quest::settimer("CheckLeash", .50);
}

sub EVENT_TIMER {

#checkleash additions

        if ($timer eq "CheckLeash"){
                quest::set_proximity($x - 100, $x + 100, $y - 100, $y + 100);
                my $GuardX = $npc->GetGuardPointX();
                my $GuardY = $npc->GetGuardPointY();
                my $GuardZ = $npc->GetGuardPointZ();
                my $CurDist = $npc->CalculateDistance($GuardX, $GuardY, $GuardZ);
#                quest::say("Dist: $CurDist");
                if ($CurDist > 2)
                {
                        $npc->SetHP(533349);
			#let them know we wont leave this spotish
			#quest::shout2("Fools I wont Chase you! Fight like men or dont waste my time!!!");
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
#quest::say("Dist: $CurDist");

                quest::stoptimer("CheckLeash");
                quest::settimer("CheckLeash", .50);
                }
}

}

sub EVENT_DEATH {
   quest::faction(1073, -4000);
}


sub EVENT_HP
{
 quest::settimer("CheckLeash", .50);
#quest::say("Event HP Fired");
}
