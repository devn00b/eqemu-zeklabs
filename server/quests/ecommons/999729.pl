sub EVENT_SAY {
#ban dumb fucks
#$banned = $client->GuildID();
#quest::say("Your guild is $banned");
#if($banned eq '3'){
#quest::say("Because your guild exploited and didnt report the bug your entire guild is *BANNED* from using this NPC.");
#return;
#}
##Setup quest vars.
## Setup Global for How many turn ins.
if (!defined $qglobals{"EarsIn"}) {
quest::setglobal("EarsIn", 0, 5, "F");
}
$curearptz= $client->GetGlobal("EarsIn");


if(($text=~/hail/i)){
$retard = quest::saylink("info");
$darth = quest::saylink("Opeth");
quest::say("Hello $name. I will take your pvp tickets and trade you for PVP gear. Hand me your tickets, and I'll store them till you have 40. Once you reach 40 you can then withdraw them for a chance at some PVP loot! Would you like more [$retard]?");
quest::say("Hmm have you ever heard the tragedy of Darth [$darth] The Mathematician?");
return;
  }
### TEST ADD POINTS REMOVE LATER
#if(($text=~/test/i)) {
#quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") + 100), 5, "F");
#quest::say("100pts added");
#}
###

if($text=~/opeth/i){
quest::say("I thought not. It’s not a story the Admins would tell you. It’s an old legend. Darth Opeth was a Dark Lord, so powerful and so wise he could use mana to influence the minds of others to create life… He had such a knowledge of the dark side that he could even keep the ones he cared about from dying. The dark side is a pathway to many abilities some consider to be unnatural (Enchanters!). He became so powerful… the only thing he was afraid of was losing his power, which eventually, of course, he did. Unfortunately, he taught his apprentice (Kongamon) everything he knew, then his apprentice killed him in his sleep. Ironic. He could save others from death, but not himself.");
return;
}
  
if(($text=~/info/i) ) {
if($curearptz < 40){
quest::say("$name you Currently have $curearptz. You do not have enough points to try a roll. You need 40 to continue. All you have to do is hand me your PVP tickets, 1, 2, 3, or even 4 at a time.");
quest::say("I will then tally them up, and once you have 40 you can then try your hand at some loot. You can even store your points with me and spend them later!");
return;
}
$retard = quest::saylink("try");
quest::say("$name you Currently have $curearptz that you can use.  You have enough points to [$retard] a roll. Let me know!");
return;
}
## check user level here and return if they are too low. We dont want them to be able to use any more features.
if($ulevel <10) {
quest::say("Sorry $name you must be at least level 10 to withdrawl points. You can still deposit, and check your points however.");
return;
}

##here we take their points and random
if(($text=~/try/i) && $ulevel >9) {
#### BETA WORK
#$curearptz = 100;
####
if($curearptz <40) {
quest::say("Sorry $name you have less than 40 tickets turned in. You currently have $curearptz / 40.");
return;
}


if($curearptz > 40) {
    $whatprize = int(rand(100));
    if($whatprize <5) {
	##epic code test
         #epics
        if ($class eq  'Bard'){
#		&& ($class ne 'Warrior' && $class ne 'Rogue' && $class ne 'Monk' && $class ne 'Berserker' && $class ne 'Shadowknight' && $class ne 'Paladin' && $class ne 'Ranger' && $class ne 'Magician' && $class ne 'Necromancer' && $class ne 'Enchanter' && $class ne 'wizard' && $class ne 'Cleric' && $class ne 'Druid' && $class ne 'Shaman') ) {
            $prze =  20542; # Singing Short Sword
			$winning =  quest::varlink($prze);
			
				quest::say("Congrats $name you won $winning!");
				quest::summonitem($prze);
				quest::ding();
				$client->AddLevelBasedExp(50, 45);
				quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
				$curearptz = $client->GetGlobal("EarsIn");
				quest::say("Your total was reduced by 40. Your new total is $curearptz");
				return;


        }
        if ($class eq  'Cleric'){
#		&& ($class ne 'Warrior' && $class ne 'Rogue' && $class ne 'Monk' && $class ne 'Berserker' && $class ne 'Shadowknight' && $class ne 'Paladin' && $class ne 'Ranger' && $class ne 'Magician' && $class ne 'Necromancer' && $class ne 'Enchanter' && $class ne 'Wizard' && $class ne 'Bard' && $class ne 'Druid' && $class ne 'Shaman') ) {
            $prze =  5532; # Water Sprinkler of Nem Ankh
			$winning =  quest::varlink($prze);
							quest::say("Congrats $name you won $winning!");
				quest::summonitem($prze);
				quest::ding();
				$client->AddLevelBasedExp(50, 45);
				quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
				$curearptz = $client->GetGlobal("EarsIn");
				quest::say("Your total was reduced by 40. Your new total is $curearptz");
return;
        }
        if ($class eq  'Druid'){
#		&& ($class ne 'Warrior' && $class ne 'Rogue' && $class ne 'Monk' && $class ne 'Berserker' && $class ne 'Shadowknight' && $class ne 'Paladin' && $class ne 'Ranger' && $class ne 'Magician' && $class ne 'Necromancer' && $class ne 'Enchanter' && $class ne 'Wizard' && $class ne 'Bard' && $class ne 'Cleric' && $class ne 'Shaman') ) {
            $prze =  20490; # Nature Walkers Scimitar
			$winning =  quest::varlink($prze);
							quest::say("Congrats $name you won $winning!");
				quest::summonitem($prze);
				quest::ding();
				$client->AddLevelBasedExp(50, 45);
				quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
				$curearptz = $client->GetGlobal("EarsIn");
				quest::say("Your total was reduced by 40. Your new total is $curearptz");
return;
        }
        if ($class eq  'Enchanter'){
#		&& ($class ne 'Warrior' && $class ne 'Rogue' && $class ne 'Monk' && $class ne 'Berserker' && $class ne 'Shadowknight' && $class ne 'Paladin' && $class ne 'Ranger' && $class ne 'Magician' && $class ne 'Necromancer' && $class ne 'Wizard' && $class ne 'Bard' && $class ne 'Druid' && $class ne 'Shaman') ) {
            $prze =  10650; # Staff of the Serpent
			$winning =  quest::varlink($prze);
							quest::say("Congrats $name you won $winning!");
				quest::summonitem($prze);
				quest::ding();
				$client->AddLevelBasedExp(50, 45);
				quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
				$curearptz = $client->GetGlobal("EarsIn");
				quest::say("Your total was reduced by 40. Your new total is $curearptz");
return;
        }
        if ($class eq  'Magician'){
#		&& ($class ne 'Warrior' && $class ne 'Rogue' && $class ne 'Monk' && $class ne 'Shadowknight' && $class ne 'Paladin' && $class ne 'Ranger' && $class ne 'Necromancer' && $class ne 'Enchanter' && $class ne 'Wizard' && $class ne 'Bard' && $class ne 'Druid' && $class ne 'Shaman' && $class ne 'Cleric')  ) {
            $prze =  28034; # Orb of Mastery
			$winning =  quest::varlink($prze);
							quest::say("Congrats $name you won $winning!");
				quest::summonitem($prze);
				quest::ding();
				$client->AddLevelBasedExp(50, 45);
				quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
				$curearptz = $client->GetGlobal("EarsIn");
				quest::say("Your total was reduced by 40. Your new total is $curearptz");
return;
        }
        if ($class eq  'Monk'){
#		&& ($class ne 'Warrior' && $class ne 'Rogue' && $class ne 'Berserker' && $class ne 'Shadowknight' && $class ne 'Paladin' && $class ne 'Ranger' && $class ne 'Magician' && $class ne 'Necromancer' && $class ne 'Enchanter' && $class ne 'Wizard' && $class ne 'Bard' && $class ne 'Druid' && $class ne 'Shaman' && $class ne 'Cleric' ) ) {
            $prze =  10652; # Celestial Fists
			$winning =  quest::varlink($prze);
							quest::say("Congrats $name you won $winning!");
				quest::summonitem($prze);
				quest::ding();
				$client->AddLevelBasedExp(50, 45);
				quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
				$curearptz = $client->GetGlobal("EarsIn");
				quest::say("Your total was reduced by 40. Your new total is $curearptz");
return;
        }
        if ($class eq  'Necromancer'){
#		&& ($class ne 'Warrior' && $class ne 'Rogue' && $class ne 'Monk' && $class ne 'Berserker' && $class ne 'Shadowknight' && $class ne 'Paladin' && $class ne 'Ranger' && $class ne 'Magician' && $class ne 'Enchanter' && $class ne 'Wizard' && $class ne 'Bard' && $class ne 'Druid' && $class ne 'Shaman' && $class ne 'Cleric') ) {
            $prze =  20544; # Scythe of the Shadowed Soul
			$winning =  quest::varlink($prze);
							quest::say("Congrats $name you won $winning!");
				quest::summonitem($prze);
				quest::ding();
				$client->AddLevelBasedExp(50, 45);
				quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
				$curearptz = $client->GetGlobal("EarsIn");
				quest::say("Your total was reduced by 40. Your new total is $curearptz");
return;
        }
        if ($class eq  'Paladin'){
#		&& ($class ne 'Warrior' && $class ne 'Rogue' && $class ne 'Monk' && $class ne 'Berserker' && $class ne 'Shadowknight' && $class ne 'Ranger' && $class ne 'Magician' && $class ne 'Necromancer' && $class ne 'Enchanter' && $class ne 'Wizard' && $class ne 'Bard' && $class ne 'Druid' && $class ne 'Shaman' && $class ne 'Cleric') ) {
            $prze =  10099; # Fiery Defender
			$winning =  quest::varlink($prze);
							quest::say("Congrats $name you won $winning!");
				quest::summonitem($prze);
				quest::ding();
				$client->AddLevelBasedExp(50, 45);
				quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
				$curearptz = $client->GetGlobal("EarsIn");
				quest::say("Your total was reduced by 40. Your new total is $curearptz");
return;
        }
        if ($class eq  'Ranger'){
#		&& ($class ne 'Warrior' && $class ne 'Rogue' && $class ne 'Monk' && $class ne 'Shadowknight' && $class ne 'Paladin' && $class ne 'Magician' && $class ne 'Necromancer' && $class ne 'Enchanter' && $class ne 'Wizard' && $class ne 'Bard' && $class ne 'Druid' && $class ne 'Shaman' && $class ne 'Cleric' ) ){
            
                $prze =  20488; # Earthcaller
				$prze2 =  20487; # Swiftwind
				$winning =  quest::varlink($prze);
				$winning2 = quest::varlink($prze2);
				quest::say("Congrats $name you won $winning!");
				quest::say("Congrats $name you won $winning2!");
				quest::summonitem($prze);
				quest::summonitem($prze2);
				quest::ding();
				$client->AddLevelBasedExp(50, 45);
				quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
				$curearptz = $client->GetGlobal("EarsIn");
				quest::say("Your total was reduced by 40. Your new total is $curearptz");
return;
         
          
        }
        if ($class eq  'Rogue'){
		#&& ($class ne 'Warrior' && $class ne 'Monk' && $class ne 'Berserker' && $class ne 'Shadowknight' && $class ne 'Paladin' && $class ne 'Ranger' && $class ne 'Magician' && $class ne 'Necromancer' && $class ne 'Enchanter' && $class ne 'Wizard' && $class ne 'Bard' && $class ne 'Druid' && $class ne 'Shaman' && $class ne 'Cleric')) {
            $prze =  11057; # Ragebringer
			$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
				quest::summonitem($prze);
				quest::ding();
				$client->AddLevelBasedExp(50, 45);
				quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
				$curearptz = $client->GetGlobal("EarsIn");
				quest::say("Your total was reduced by 40. Your new total is $curearptz");
				return;
        }
        if ($class eq  'Shadowknight'){
#		&& ($class ne 'Warrior' && $class ne 'Rogue' && $class ne 'Monk' && $class ne 'Paladin' && $class ne 'Ranger' && $class ne 'Magician' && $class ne 'Necromancer' && $class ne 'Enchanter' && $class ne 'Wizard' && $class ne 'Bard' && $class ne 'Druid' && $class ne 'Shaman' && $class ne 'Cleric')) {
            $prze =  14383; # Innoruuk's Curse
			$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
				quest::summonitem($prze);
				quest::ding();
				$client->AddLevelBasedExp(50, 45);
				quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
				$curearptz = $client->GetGlobal("EarsIn");
				quest::say("Your total was reduced by 40. Your new total is $curearptz");
				return;
        }
        if ($class eq  'Shaman'){
#		&& ($class ne 'Warrior' && $class ne 'Rogue' && $class ne 'Monk' && $class ne 'Berserker' && $class ne 'Shadowknight' && $class ne 'Paladin' && $class ne 'Ranger' && $class ne 'Magician' && $class ne 'Necromancer' && $class ne 'Enchanter' && $class ne 'Wizard' && $class ne 'Bard' && $class ne 'Druid' && $class ne 'Cleric')) {
            $prze =  10651; # Spear of Fate
			$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
				quest::summonitem($prze);
				quest::ding();
				$client->AddLevelBasedExp(50, 45);
				quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
				$curearptz = $client->GetGlobal("EarsIn");
				quest::say("Your total was reduced by 40. Your new total is $curearptz");
				return;
        }
        if ($class eq  'Warrior'){
#		&& ($class ne 'Rogue' && $class ne 'Monk' && $class ne 'Berserker' && $class ne 'Shadowknight' && $class ne 'Paladin' && $class ne 'Ranger' && $class ne 'Magician' && $class ne 'Necromancer' && $class ne 'Enchanter' && $class ne 'Wizard' && $class ne 'Bard' && $class ne 'Druid' && $class ne 'Shaman' && $class ne 'Cleric')) {
            $prze =  10908; # Jagged Blade of War
			$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
				quest::summonitem($prze);
				quest::ding();
				$client->AddLevelBasedExp(50, 45);
				quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
				$curearptz = $client->GetGlobal("EarsIn");
				quest::say("Your total was reduced by 40. Your new total is $curearptz");
				return;
        }
        if ($class eq  'Wizard'){
		#&& ($class ne 'Warrior' && $class ne 'Rogue' && $class ne 'Monk' && $class ne 'Berserker' && $class ne 'Shadowknight' && $class ne 'Paladin' && $class ne 'Ranger' && $class ne 'Magician' && $class ne 'Necromancer' && $class ne 'Enchanter' && $class ne 'Bard' && $class ne 'Druid' && $class ne 'Shaman' && $class ne 'Cleric')) {
            $prze =  14341; # Staff of the Four
			$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
				quest::summonitem($prze);
				quest::ding();
				$client->AddLevelBasedExp(50, 45);
				quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
				$curearptz = $client->GetGlobal("EarsIn");
				quest::say("Your total was reduced by 40. Your new total is $curearptz");
				return;
        }
##### ADDING IN BEASTLORD
        if ($class eq  'Beastlord'){
#		&& ($class ne 'Rogue' && $class ne 'Monk' && $class ne 'Berserker' && $class ne 'Warrior' && $class ne 'Shadowknight' && $class ne 'Paladin' && $class ne 'Ranger' && $class ne 'Magician' && $class ne 'Necromancer' && $class ne 'Enchanter' && $class ne 'Wizard' && $class ne 'Bard' && $class ne 'Druid' && $class ne 'Shaman' && $class ne 'Cleric')) {
            $prze  = 8495; # Jagged Blade of War
	    $prze2 = 8496;
                        $winning  = quest::varlink($prze);
			$winning2 = quest::varlink($prze2);
                                quest::say("Congrats $name you won $winning and $winning2!");
                                quest::summonitem($prze);
				quest::summonitem($prze2);
                                quest::ding();
                                $client->AddLevelBasedExp(50, 45);
                                quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
                                $curearptz = $client->GetGlobal("EarsIn");
                                quest::say("Your total was reduced by 40. Your new total is $curearptz");
                                return;
        }

#### END BEASTLORD
    }
	if($whatprize <70 and $whatprize >4) {
	$prze=0;
	}
    if($whatprize >=70) {
        #weapons
        #$prze = quest::ChooseRandom(,,,,,);
        ########
		# These items are all/all/
		########
		#146282,146283,6686,144117,47012,1622,2736,4315,64193,31851,31852,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,132845,27810,30367,21935,141320,144922,11911,145241,145349,145350,145351,17799,141321,32557,145377,
		########
	
		
		
		if ($class eq  'Bard') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                # win something your class can actually use!
				#4557,4558,4559,4560,4561,4562,4563 Kunark armor
                $prze = quest::ChooseRandom(146282,146283,145545,145377,145377,145370,145371,145369,145368,15742,24737,6686,7229,4557,4558,4559,4560,4561,4562,4563,144117,47012,1622,2736,4315,64193,31851,31852,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,132845,27810,30367,21935,141320,144922,11911,145241,145349,145350,145351,17799,141321,32557,145377);
			
            }
            else {
                # win nothing, sorry!
                $prze = 0;
            }
        }
        elsif ($class eq  'Cleric') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(146282,146283,145546,145377,145377,31854,4564,6686,4565,4566,4567,4568,4569,4570,144117,47012,1622,2736,4315,64193,31851,31852,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,132845,27810,30367,21935,141320,144922,11911,145241,145349,145350,145351,17799,141321,32557,145377);

            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Druid') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(145546,145377,146282,146283,31854,145377,4571,4572,6686,4573,4574,4575,4576,4577,144117,47012,1622,2736,4315,64193,31851,31852,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,132845,27810,30367,21935,141320,144922,11911,145241,145349,145350,145351,17799,141321,32557,145377);

            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Enchanter') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(145547,145377,145367,147662,146282,146283,145377,31854,1641,1644,1648,6686,1649,1650,1651,1652,144117,47012,1622,2736,4315,64193,31851,31852,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,132845,27810,30367,21935,141320,144922,11911,145241,145349,145350,145351,17799,141321,32557,145377);
				$winning =  quest::varlink($prze);

            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Magician') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(145547,145377,145367,147662,31854,145377,146282,146283,1641,1644,1648,1649,6686,1650,1651,1652,144117,47012,1622,2736,4315,64193,31851,31852,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,132845,27810,30367,21935,141320,144922,11911,145241,145349,145350,145351,17799,141321,32557,145377);
				$winning =  quest::varlink($prze);

            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Monk') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
$prze = quest::ChooseRandom(145548,145544,145377,145402,142736,24836,24839,24843,146282,146283,145377,24843,6686,24844,24845,24846,24847,143110,144117,47012,1622,2736,4315,64193,31851,31852,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,132845,27810,30367,21935,141320,144922,11911,145349,145351,17799,141321,32557,145377);
				$winning =  quest::varlink($prze);

            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Necromancer') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(145547,145377,145367,147662,31854,1641,1644,145377,1648,1649,1650,146282,146283,1651,6686,1652,144117,47012,1622,2736,4315,64193,31851,31852,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,132845,27810,30367,21935,141320,144922,11911,145241,145349,145350,145351,17799,141321,32557,145377);
				$winning =  quest::varlink($prze);

            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Paladin') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(145548,145544,145377,145402,4543,4544,4545,4547,4546,4548,4549,6686,145377,144117,146282,146283,47012,1622,2736,4315,64193,31851,31852,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,132845,27810,30367,21935,141320,144922,11911,145241,145349,145350,145351,17799,141321,32557,145377);
				$winning =  quest::varlink($prze);

            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Ranger') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(145545,145377,142737,5157,8407,4536,4537,4538,4539,4540,4541,145377,6686,4542,144117,47012,1622,146282,146283,2736,4315,64193,31851,31852,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,132845,27810,30367,21935,141320,144922,11911,145241,145349,145350,145351,17799,141321,32557,145377);
				$winning =  quest::varlink($prze);

            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Rogue') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(145545,145377,7229,4550,4551,4552,4553,4554,4555,4556,6686,144117,145377,47012,1622,2736,4315,64193,31851,31852,146282,146283,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,132845,27810,30367,21935,141320,144922,11911,145241,145349,145350,145351,17799,141321,32557,145377);
				$winning =  quest::varlink($prze);

            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Shadowknight') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(145548,145544,145377,145402,5157,145377,4522,4523,4524,4525,4526,4527,4528,144117,6686,47012,1622,2736,4315,64193,31851,31852,146282,146283,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,132845,27810,30367,21935,141320,144922,11911,145241,145349,145350,145351,17799,141321,32557,145377);
				$winning =  quest::varlink($prze);

            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Shaman') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(145546,145377,31854,145377,7229,4529,4530,4531,4532,4533,4534,4535,144117,47012,6686,1622,2736,4315,64193,31851,31852,31853,31854,31855,146282,146283,31856,31857,31858,31859,31860,31861,31862,31863,132845,27810,30367,21935,141320,144922,11911,145241,145349,145350,145351,17799,141321,32557,145377);
				$winning =  quest::varlink($prze);

            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Warrior') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(145548,146282,146283,145544,145377,145402,5157,145377,7229,4515,4516,4517,4518,4519,4520,4521,144117,47012,6686,1622,2736,4315,64193,31851,31852,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,132845,27810,30367,21935,141320,144922,11911,145241,145349,145350,145351,17799,141321,32557,145377);
				$winning =  quest::varlink($prze);

            }
            else {
                $prze = 0;
            }
        }
        elsif ($class eq  'Wizard') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
                $prze = quest::ChooseRandom(145547,146282,146283,145377,145367,145377,147662,31854,1641,1644,1648,1649,1650,1651,1652,144117,47012,1622,2736,4315,6686,64193,31851,31852,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,132845,27810,30367,21935,141320,144922,11911,145241,145349,145350,145351,17799,141321,32557,145377);
				$winning =  quest::varlink($prze);

            }
            else {
                $prze = 0;
            }
        }
##### BL EXPANDED ####
        elsif ($class eq  'Beastlord') {
            $whatprize = int(rand(100));
            if ($whatprize > 50) {
$prze = quest::ChooseRandom(145548,145544,145377,145402,142736,24836,24839,24843,146282,146283,145377,24843,6686,24844,24845,24846,24847,143110,144117,47012,1622,2736,4315,64193,31851,31852,31853,31854,31855,31856,31857,31858,31859,31860,31861,31862,31863,132845,27810,30367,21935,141320,144922,11911,145349,145351,17799,141321,32557,145377);
                                $winning =  quest::varlink($prze);

            }
            else {
                $prze = 0;
            }
        }

#### END BL EXPAN ####
    }
}
 


########
##These are loosers so give them crap
if($prze < 1)
{
$prze = quest::ChooseRandom(5803,66601,88047,88047,15957,53674,58444,66603,29762,33554,58379,58554,33529,98177,58637,45539,45458,33513,96622,93473,98238,98244,98243,98249,45605,53619,46135,61327,88050,87555,61994,33640,61993,88048,9766,96541,96537,96543,96540,96545,96544,22536,30796,13107,2441,13943,87167,87167,87167,87167);
quest::say("I am sorry $name, Your roll sucked so you get the consolation prize.");
}
				$winning =  quest::varlink($prze);
				quest::say("Congrats $name you won $winning!");
				quest::summonitem($prze);
				quest::ding();
				$client->AddLevelBasedExp(50, 45);
				quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") - 40), 5, "F");
				$curearptz = $client->GetGlobal("EarsIn");
				quest::say("Your total was reduced by 40. Your new total is $curearptz");
				


return;
}
}
#}
#}

sub EVENT_ITEM {
#good stuff
##$banned = $client->GuildID();
#if($banned eq '3'){
#quest::say("Because your guild exploited and didnt report the bug your entire guild is *BANNED* from using this NPC. Thanks for the shit.");
#return;
#}

if (!defined $qglobals{"EarsIn"}) {
quest::setglobal("EarsIn", 0, 5, "F");
}
$curearptz= $client->GetGlobal("EarsIn");

if (plugin::check_handin(\%itemcount,132487 =>4)){
$curearptz= $client->GetGlobal("EarsIn");

quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") + 4), 5, "F");
$curearptz= $client->GetGlobal("EarsIn");
quest::say("I've increased your total by 4 $name. Your new total is $curearptz.");
}
if (plugin::check_handin(\%itemcount,132487 =>3)){
quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") + 3), 5, "F");
$curearptz= $client->GetGlobal("EarsIn");
quest::say("I've increased your total by 3 $name. Your new total is $curearptz.");
}
if (plugin::check_handin(\%itemcount,132487 =>2)){

quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") + 2), 5, "F");
$curearptz= $client->GetGlobal("EarsIn");
quest::say("I've increased your total by 2 $name. Your new total is $curearptz.");
}
if (plugin::check_handin(\%itemcount,132487 =>1)){
quest::setglobal("EarsIn", ($client->GetGlobal("EarsIn") + 1), 5, "F");
$curearptz= $client->GetGlobal("EarsIn");
quest::say("I've increased your total by 1 $name. Your new total is $curearptz.");
}
}
