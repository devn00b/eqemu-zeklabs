## This is devn00bs s5 point catch up zones entry point.
## Goes on The Portal Master
## Also using for ports to other locations I may use

sub EVENT_SAY {

##Setup quest vars.
## Setup Global for How many turn ins.
if (!defined $qglobals{"EarsIn"}) {
quest::setglobal("EarsIn", 100, 5, "F");
}
$curearptz= $client->GetGlobal("EarsIn");


if(($text=~/hail/i)){
quest::say("Hello $name. I will take your pvp tickets and trade you for PVP gear. Hand me your tickets, I'll store them till you have 40. Once you reach 40 you can then withdrawl them for a chance at some PVP loot! Would you like more [info]?");

return;
  }
### TEST ADD POINTS REMOVE LATER
if(($text=~/test/i) && $ulevel >10  ) {
quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") + 100), 5, "F");
quest::say("100pts added");
}
###
  
if(($text=~/info/i) ) {
if($curearptz < 40){
quest::say("$name you Currently have $curearptz. You do not have enough points to try a roll. You need 40 to continue.");
return;
}
quest::say("$name you Currently have $curearptz that you can use.  You have enough points to [try] a roll. Let me know!");
return;
}
## check user level here and return if they are too low. We dont want them to be able to use any more features.
if($ulevel <10) {
quest::say("Sorry $name you must be at least level 10 to withdrawl points. You can still deposit, and check your points however.");
return;
}

##here we take their points and random
if(($text=~/try/i) && $ulevel >9) {
quest::say("woot1");
#$prze = 0;
if($curearptz <40) {
quest::say("Sorry $name you have less than 40 tickets turned in");
return;
}

#if($curearptz > 40) {

#$whatprize = int(rand(100));
#if they roll less than 10 (so 1-9) they get an epic roll. Otherwise just normal junk.
#if($whatprize <10) {
#epics
#$prze = quest::ChooseRandom(17959,20542,5532,20490,10650,10652,20544,10099,20488,20487,11057,14383,10651,10908,14341,28034);
#}
#if($whatprize >=10) {
#weapons
#$prze = quest::ChooseRandom(146166,144117,7229,2736,4315,8407,1622,5157,6686,15742,24737,31851,31852,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,31863,64193,132844,132845,143069,30367,21935,141320,144922,27810,11911,145241,142737,142736,145349,145350,143351,17799,141321,32557,6078,145367,145368,145369,145370,145371,145377,145402,140000,145544,145545,145546,145547,145548,146282,146283,1641,1644,1648,1649,1650,1651,1652,4557,4558,4559,4560,4561,4562,4563,4571,4572,4573,4574,4575,4576,4577,4564,4565,4566,4567,4568,4569,4570,4529,4530,4531,4532,4533,4534,4535,4536,4537,4538,4539,4540,4541,4542,4550,4551,4552,4553,4554,4555,4556,4516,4517,4518,4519,4520,4521,4543,4544,4545,4547,4546,4548,4549,4522,4523,4524,4525,4526,4527,4528,24836,24839,24843,24843,24844,24845,24846,24847,143110);
#$useit = $client->CanClassEquipItem($prze);
#quest::say("test: $useit");

}
#quest::say("$whatprize");
#$random_result = int(rand(500));

##Opeth Test code start##
if($curearptz > 40) {
quest::say("woot2"); 
    $whatprize = int(rand(100));
	#### BETA TESTING ONLY FORCE A ROLL ###
	#$whatprize = 1;
    #if they roll less than 10 (so 1-9) they get an epic roll. Otherwise just normal junk.
	#quest::say("woot3 $whatprize"); 
    if($whatprize <10) {
        #epics
        #classes, not sure what your event handler is here in perl, but throw in your replacement for "e" here I guess
        #also not sure what item id 17959 is -- I have it removed from my DB.
        if ($class eq  'Bard') {
            $prze =  20542; # Singing Short Sword
			$winning =  quest::varlink($prze);
			quest::say("Congrats $name you won $winning!");
        }
        elsif ($class eq  'Cleric') {
            $prze =  5532; # Water Sprinkler of Nem Ankh
			$winning =  quest::varlink($prze);
			quest::say("Congrats $name you won $winning!");
        }
        elsif ($class eq  'Druid') {
            $prze =  20490; # Nature Walkers Scimitar
			$winning =  quest::varlink($prze);
			quest::say("Congrats $name you won $winning!");
        }
        elsif ($class eq  'Enchanter') {
            $prze =  10650; # Staff of the Serpent
			$winning =  quest::varlink($prze);
			quest::say("Congrats $name you won $winning!");
        }
        elsif ($class eq  'Magician') {
            $prze =  28034; # Orb of Mastery
			$winning =  quest::varlink($prze);
			quest::say("Congrats $name you won $winning!");
        }
        elsif ($class eq  'Monk') {
            $prze =  10652; # Celestial Fists
			$winning =  quest::varlink($prze);
			quest::say("Congrats $name you won $winning!");
        }
        elsif ($class eq  'Necromancer') {
            $prze =  20544; # Scythe of the Shadowed Soul
			$winning =  quest::varlink($prze);
			quest::say("Congrats $name you won $winning!");
        }
        elsif ($class eq  'Paladin') {
            $prze =  10099; # Fiery Defender
			$winning =  quest::varlink($prze);
			quest::say("Congrats $name you won $winning!");
        }
        elsif ($class eq  'Ranger') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze =  20488; # Earthcaller
				$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
            }
            else {
                $prze =  20487; # Swiftwind
				$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
            }
        }
        elsif ($class eq  'Rogue') {
            $prze =  11057; # Ragebringer
			$winning =  quest::varlink($prze);
			quest::say("Congrats $name you won $winning!");
        }
        elsif ($class eq  'Shadowknight') {
            $prze =  14383; # Innoruuk's Curse
			$winning =  quest::varlink($prze);
			quest::say("Congrats $name you won $winning!");
        }
        elsif ($class eq  'Shaman') {
            $prze =  10651; # Spear of Fate
			$winning =  quest::varlink($prze);
			quest::say("Congrats $name you won $winning!");
        }
        elsif ($class eq  'Warrior') {
            $prze =  10908; # Jagged Blade of War
			$winning =  quest::varlink($prze);
			quest::say("Congrats $name you won $winning!");
        }
        elsif ($class eq  'Wizard') {
            $prze =  14341; # Staff of the Four
			$winning =  quest::varlink($prze);
			quest::say("Congrats $name you won $winning!");
        }
        #$prze = quest::ChooseRandom(17959,20542,5532,20490,10650,10652,20544,10099,20488,20487,11057,14383,10651,10908,14341,28034);
    }
    if($whatprize >=10) {
        #weapons
        #$prze = quest::ChooseRandom(146166,144117,7229,2736,4315,8407,1622,5157,6686,15742,24737,31851,31852,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,31863,64193,132844,132845,143069,30367,21935,141320,144922,27810,11911,145241,142737,142736,145349,145350,143351,17799,141321,32557,6078,145367,145368,145369,145370,145371,145377,145402,140000,145544,145545,145546,145547,145548,146282,146283,1641,1644,1648,1649,1650,1651,1652,4557,4558,4559,4560,4561,4562,4563,4571,4572,4573,4574,4575,4576,4577,4564,4565,4566,4567,4568,4569,4570,4529,4530,4531,4532,4533,4534,4535,4536,4537,4538,4539,4540,4541,4542,4550,4551,4552,4553,4554,4555,4556,4516,4517,4518,4519,4520,4521,4543,4544,4545,4547,4546,4548,4549,4522,4523,4524,4525,4526,4527,4528,24836,24839,24843,24843,24844,24845,24846,24847,143110);
        if ($class eq  'Bard') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                # win something your class can actually use!
                $prze = quest::ChooseRandom(15742);
				$winning =  quest::varlink($prze);
quest::say("Congrats $name you won $winning!");
            }
            else {
                # win nothing, sorry!
                $prze = 0;
            }
        }
        elsif ($class eq  'Cleric') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(1001);
				$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Druid') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(1001);
				$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Enchanter') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(1001);
				$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Magician') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(1001);
				$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Monk') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(1001);
				$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Necromancer') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(1001);
				$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Paladin') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(1001);
				$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Ranger') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(1001);
				$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Rogue') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(1001);
				$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Shadowknight') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(1001);
				$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Shaman') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(1001);
				$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Warrior') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(1001);
				$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Wizard') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(1001);
				$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
            }
            else {
                $prze = 0;
            }
        }
    }
}
 


########


quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
$curearptz = $client->GetGlobal("EarsIn");
quest::say("We reduced total by 40. Your new total is $curearptz");
quest::summonitem($prze);

return;
}
}

}

sub EVENT_ITEM {

#good stuff
if (!defined $qglobals{"EarsIn"}) {
quest::setglobal("EarsIn", 100, 5, "F");
}
$curearptz= $client->GetGlobal("EarsIn");


if (plugin::check_handin(\%itemcount,132487 =>4)){
$curearptz= $client->GetGlobal("EarsIn");

quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") + 4), 5, "F");
$curearptz= $client->GetGlobal("EarsIn");
quest::say("We increased your total by 4. Your new total is $curearptz.");
}
if (plugin::check_handin(\%itemcount,132487 =>3)){

quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") + 3), 5, "F");
$curearptz= $client->GetGlobal("EarsIn");
quest::say("We increased your total by 3. Your new total is $curearptz.");
}
if (plugin::check_handin(\%itemcount,132487 =>2)){

quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") + 2), 5, "F");
$curearptz= $client->GetGlobal("EarsIn");
quest::say("We increased your total by 2. Your new total is $curearptz.");
}

quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") + 1), 5, "F");
$curearptz= $client->GetGlobal("EarsIn");
quest::say("We increased your total by 1. Your new total is $curearptz.");
}


}