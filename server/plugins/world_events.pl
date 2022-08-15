use feature qw/switch/;

sub event60tier_loot {
	my $client = shift;
	my $npc = shift;
	plugin::handlenpc_epicv2loot($client,$npc,2);
	plugin::handlenpc_ooglyloot($client,$npc,2);
}

sub spawnevent_mob {
	my $x = shift;
	my $y = shift;
	my $z = shift;
	my $h = shift;
        my $result_npc_id = 0;
	my $maxlevel = quest::getmaxlevel();
	if ( $maxlevel <= 51 )
	{
        #lvl 30 tier
#	$result_npc_id = quest::ChooseRandom(999581,999582,999583,999584);
	}
	#elsif ( $maxlevel <= 44 )
	#{
        #lvl 40 tier
	#$result_npc_id = quest::ChooseRandom(999585,999586,999587,999588);
	#}
	#elsif ( $maxlevel <= 54 )
	#{
	#lvl 45 tier
	#$result_npc_id = quest::ChooseRandom(999589,999590,999591,999592,999678,999678);
	#}
	#elsif ( $maxlevel < 60 )
	#{
	#$result_npc_id = quest::ChooseRandom(999593,999594,999595,999596,999678,999678);
	#}
	#elsif ( $maxlevel == 60 )
	#{
	#$result_npc_id = quest::ChooseRandom(999593,999594,999595,999596,999597,999598,999599,999678,999678);
	#}

	if ( $result_npc_id > 0 )
	{
	quest::spawn2($result_npc_id,0,0,$x,$y,$z,$h);
	}
	return $result_npc_id;
}


sub handlenpc_epiclootmember {
my $client = shift;
my $npc = shift;
@items = undef;
				$playerclass = $client->GetClass();
				given($playerclass){
				when(1) {
				#warrior
				@items = (10908);
				#$itemid = 10908; #jagged blade of war
				}
				when(2) {
				#cleric
				@items = (5532);
				#$itemid = 5532; #water sprinkler of nem ankh
				}
				when(3) {
				#paladin
				@items = (10099);
				#$itemid = 10099; #fiery defender
				}
				when(4) {
				@items = (20488,20487,11696);
				#ranger
				#$itemid = 20488; #earthcaller
				#$itemid = 20487; #swiftwind
				#$itemid = 11696; #windstriker
				}
				when(5) {
				#shadowknight
				@items = (14383);
				#$itemid = 14383; #innoruuk's curse
				}
				when(6) {
				#druid
				@items = (20490);
				#$itemid = 20490; #nature walkers scimitar
				}
				when(7) {
				#monk
				@items = (6639,10652);
				#$itemid = 6639; #tranquil staff
				#$itemid = 10652; #celestial fists
				}
				when(8) {
				#bard
				@items = (20542);
				#$itemid = 20542; #singing short sword
				}
				when(9) {
				#rogue
				@items = (11057);
				#$itemid = 11057; #ragebringer
				}
				when(10) {
				#shaman
				@items = (10651);
				#$itemid = 10651; #spear of fate
				}
				when(11) {
				#necromancer
				@items = (20544);
				#$itemid = 20544; #scythe of the shadowed soul
				}
				when(12) {
				#wizard
				@items = (14341);
				#$itemid = 14341; #staff of the four
				}
				when(13) {
				#magician
				@items = (28034);
				#$itemid = 28034; #orb of mastery
				}
				when(14) {
				#enchanter
				@items = (10650);
				#$itemid = 10650; #staff of the serpent
				}
				when(15) {
				#beastlord
				@items = (8495,8496);
				}
				when(16) {
				#berserker
				@items = (68299);
				}

				} #end given

				$num = scalar @items;
				for (my $itm=0; $itm < $num; $itm++) {
					$itemnum = @items[$itm];
					if ( $itemnum == 10908 )
					{
					#warriors are special motherfuckers
					if ( $client->HasItem($itemnum) == -1 && $client->HasItem(10909) == -1 && $client->HasItem(10910) == -1 )
					{
						$npc->AddItem($itemnum);
						$count += 1;
						return 1;
					}
					}
					elsif ( $client->HasItem($itemnum) == -1 )
					{
						$npc->AddItem($itemnum);
						$count += 1;
						return 1;
					}
				}
	return 0;
}

sub handlenpc_epicloot {
	my $client = shift;
	my $npc = shift;
	my $maxitems = shift;
my $count = 0;
$startPt = quest::ChooseRandom(0,1,2,3,4,5);
my $group = $client->GetGroup();

if ( !defined $maxitems || $maxitems < 1 )
{
$maxitems = 1;
}

	if ( defined $group )
	{
		for (my $a=0; $a < $maxitems*3; $a++) {
		if ( $a < ($maxitems/2))
		{
			$startPt = quest::ChooseRandom(0,1,2,3,4,5);
		}
		else
		{
			$startPt = 0;
		}
		for (my $i=$startPt; $i < 6; $i++) {
			my $tmpclient = $group->GetMember($i);
				if ( defined $tmpclient )
				{
					if ( plugin::handlenpc_epiclootmember($tmpclient,$npc) == 1 )
					{
						$count += 1;
						if ( $count >= $maxitems)
#$maxcount )
						{
							return;
						}
					}
				}
			}
		}
	}
	else
	{
		plugin::handlenpc_epiclootmember($client,$npc);
	}
}





sub handlenpc_ooglylootmember {
my $client = shift;
my $npc = shift;
@items = undef;
				$playerclass = $client->GetClass();
				given($playerclass){
                                when(1) {
                                #warrior
                                @items = (quest::ChooseRandom(135017,141420));
                                #$itemid = 135017; #da oogly earring of the imperator
                                #$itemid = 141420; #da oogly earring of the crybaby
                                }
				when(2) {
				#cleric
				@items = (135006);
				#$itemid = 135006; #da oogly earring of the exemplar
				}
				when(3) {
				#paladin
				@items = (135009);
				#$itemid = 135009; #da oogly earring of the holy defender
				}
				when(4) {
				@items = (135011);
				#ranger
				#$itemid = 135011; #da oogly earring of the huntmaster
				}
				when(5) {
				#shadowknight
				@items = (135010);
				#$itemid = 135010; #da oogly earring of the bloodreaver
				}
				when(6) {
				#druid
				@items = (135007);
				#$itemid = 135007; #da oogly earring of the storm caller
				}
				when(7) {
				#monk
				@items = (135018);
				#$itemid = 135018; #da oogly earring of the ashenhand
				}
				when(8) {
				#bard
				@items = (135016);
				#$itemid = 135016; #da oogly earring of the lyricist
				}
				when(9) {
				#rogue
				@items = (135019);
				#$itemid = 135019; #da oogly earring of the shadowblade
				}
				when(10) {
				#shaman
				@items = (135008);
				#$itemid = 135008; #da oogly earring of the spiritwatcher
				}
				when(11) {
				#necromancer
				@items = (135012);
				#$itemid = 135012; #da oogly earring of the deathcaller
				}
				when(12) {
				#wizard
				@items = (135014);
				#$itemid = 135014; #da oogly earring of the pyromancer
				}
				when(13) {
				#magician
				@items = (135015);
				#$itemid = 135015; #da oogly earring of the grand summoner
				}
				when(14) {
				#enchanter
				@items = (135013);
				#$itemid = 135013; #da oogly earring of the entrancer
				}
                                when(15) {
                                #beastlord
                                @items = (143164);
                                #$itemid = 143164; #da oogly earring of the Animal Lover
                                }
                                #when(16) {
                                #Zerker
                                #@items = (143171);
                                #$itemid = 143171; #da oogly earring of the Deranged
                                #}

				} #end given

				$num = scalar @items;
				for (my $itm=0; $itm < $num; $itm++) {
					$itemnum = @items[$itm];
					if ( $client->HasItem($itemnum) == -1 )
					{
						$npc->AddItem($itemnum);
						$count += 1;
						return 1;
					}
				}
	return 0;
}



sub handlenpc_ooglyloot {
	my $client = shift;
	my $npc = shift;
	my $maxitems = shift;
my $count = 0;
$startPt = quest::ChooseRandom(0,1,2,3,4,5);
my $group = $client->GetGroup();

if ( !defined $maxitems || $maxitems < 1 )
{
$maxitems = 1;
}

	if ( defined $group )
	{
		for (my $a=0; $a < $maxitems*3; $a++) {
		if ( $a < ($maxitems/2))
		{
			$startPt = quest::ChooseRandom(0,1,2,3,4,5);
		}
		else
		{
			$startPt = 0;
		}
		for (my $i=$startPt; $i < 6; $i++) {
			my $tmpclient = $group->GetMember($i);
				if ( defined $tmpclient )
				{
					if ( plugin::handlenpc_ooglylootmember($tmpclient,$npc) == 1 )
					{
						$count += 1;
						if ( $count >= $maxcount )
						{
							return;
						}
					}
				}
			}
		}
	}
	else
	{
		plugin::handlenpc_ooglylootmember($client,$npc);
	}
}





sub handlenpc_epicv2lootmember {
my $client = shift;
my $npc = shift;
@items = undef;
				$playerclass = $client->GetClass();
				given($playerclass){
				when(1) {
				#warrior
				@items = (60332);
				}
				when(2) {
				#cleric
				@items = (20076);
				}
				when(3) {
				#paladin
				@items = (48147);
				}
				when(4) {
				@items = (62627);
				#ranger
				}
				when(5) {
				#shadowknight
				@items = (48136);
				}
				when(6) {
				#druid
				@items = (62880);
				}
				when(7) {
				#monk
				@items = (67742);
				}
				when(8) {
				#bard
				@items = (77640);
				}
				when(9) {
				#rogue
				@items = (52348);
				}
				when(10) {
				#shaman
				@items = (57405);
				}
				when(11) {
				#necromancer
				@items = (64067);
				}
				when(12) {
				#wizard
				@items = (16576);
				}
				when(13) {
				#magician
				@items = (19839);
				}
				when(14) {
				#enchanter
				@items = (52962);
				}
				when(15) {
				#beastlord
				@items = (57054);
				}
				when(16) {
				#berserker
				@items = (18609);
				}

				} #end given

				$num = scalar @items;
				for (my $itm=0; $itm < $num; $itm++) {
					$itemnum = @items[$itm];
					if ( $client->GetLevel() >= 55 && $client->HasItem($itemnum) == -1 )
					{
						$npc->AddItem($itemnum);
						$count += 1;
						return 1;
					}
				}
	return 0;
}

sub handlenpc_epicv2loot {
	my $client = shift;
	my $npc = shift;
	my $maxitems = shift;
my $count = 0;
$startPt = quest::ChooseRandom(0,1,2,3,4,5);
my $group = $client->GetGroup();

if ( !defined $maxitems || $maxitems < 1 )
{
$maxitems = 1;
}

	if ( defined $group )
	{
		for (my $a=0; $a < $maxitems*3; $a++) {
		if ( $a < ($maxitems/2))
		{
			$startPt = quest::ChooseRandom(0,1,2,3,4,5);
		}
		else
		{
			$startPt = 0;
		}
		for (my $i=$startPt; $i < 6; $i++) {
			my $tmpclient = $group->GetMember($i);
				if ( defined $tmpclient )
				{
					if ( plugin::handlenpc_epicv2lootmember($tmpclient,$npc) == 1 )
					{
						$count += 1;
						if ( $count >= $maxcount )
						{
							return;
						}
					}
				}
			}
		}
	}
	else
	{
		plugin::handlenpc_epicv2lootmember($client,$npc);
	}
}


sub worldmob_spawnhandler {
my $npc = shift;
my $opt_time = shift;

if ( !defined $opt_time || $opt_time <= 0 )
{
$opt_time = 4320;
}

quest::settimer("despawn",$opt_time);
}

sub worldmob_timerhandler {
my $npc = shift;
my $timer = shift;
if ( $timer eq "despawn" )
{
if ( $npc->IsEngaged() == 0 )
{
quest::depop();
}
}
}

sub worldmob_deathhandler {
my $npc = shift;
$name = $npc->GetCleanName();
quest::we(13,"[WORLD EVENT] $name has been vanquished!");
quest::stoptimer("despawn");
plugin::worldmob_resetzone();
}

sub worldmob_resetzone {
$currentvalue = quest::getzonepvp(0);
$zoneid = quest::getzoneid(0);
if ( $currentvalue > 1 && $zoneid != 77)
{
quest::setzonepvp(1);
quest::we(13,"[WORLD EVENT] Resetting zone with world mob to PVP with level restrictions (Zone reset after event).");
quest::setglobal("worldeventnpcspawned","0",7,"S5");
}
}

sub treasureGoblinDrop {
my $mob = shift;
my $x = shift;
my $y = shift;
my $prizeSize = shift;

#prizeSize = 0 if random junk, 1 if small, 2 = large
$rndItem = 0;


###S7 Im Forcing one prize to simplify things.
$prizeSize = 0;

if ( $prizeSize == 0 )
{
$rndItem = quest::ChooseRandom(21823,31851,31852,31853,31854,31857,31858,31859,31860,31861,31862,16868,48043,132487,132487,132487,132487,132487,0,0,0,0,0,1740,6762,11911,17156,32557,29751,144560,14548,28580,30441,68537,44392,21821,21822,14461,14388,11458,11459,2366,141322,6083,9666,146284,145492,7299,7475,64193,2960,30367,14666,14544,14545,14546,14547,14548,28580,30441,68537,44392,146319,11665,5730,68264,68270,28769,8073,28017,29207,26001,11058,30143,29635,27921,22899,42781,8096,5796,132485,26502,2738,54884,5067,146403,146404,146405,146406,146407,146408,46193,41518,61021,62262,53588,39320,52671,28906,3849,1727,7573,7573,7571,7571,27326,27326);
#$rndItem = quest::ChooseRandom(0,0,0,0,0,5657,9204,45081,25315,1098,61224,61220,61221,61222,61223,8149,26209,26200,26206,8150,8139,31851,31852,31853,31854,31857,31858,31859,31860,31861,31862,52671,28906,3849,1727,7573,7573,7571,7571,27326,27326,0,0,0,0,0,0,0,0,0);
}
elsif ( $prizeSize == 1 )
{
#orig
$rndItem = quest::ChooseRandom(135038,135038,135038,135038,135039,135039,135039,135039,22503);
#$rndItem = quest::ChooseRandom(132845,11571,144560,32151,32152,27266,21820,132842,20498,20663,25098,25297,3598,7389,32025,5626,5705,3748,2874,1727,0,0,0,0,0,0);
#$rndItem = quest::ChooseRandom(0,0,0,0,0,28853,27928,6203,14730,7359,7362,27939,17241,17242,7363,1154,7978,1136,10584,7387,11605,1134,1133,7977);
}
elsif ( $prizeSize == 2 )
{
#item 145492 and after special loots
$rndItem = quest::ChooseRandom(144586,144586,144586,10404,1360);
#$rndItem = quest::ChooseRandom(142736,142737);
}

if ( !defined $x )
{
quest::emote("drops an item to the ground. You got ($prizeSize)!");
quest::creategroundobject($rndItem, $mob->GetX(), $mob->GetY(), $mob->GetZ(), $mob->GetHeading(), 300000, 0, "treasuregoblin_drops");
}
else
{
$diffx = $x - int($mob->GetX());
$diffy = $y - int($mob->GetY());

if ( $diffx < 0 ) { $diffx = -$diffx; }
if ( $diffy < 0 ) { $diffy = -$diffy; }

if ( $diffx > 20 || $diffy > 20 )
{
quest::emote("drops an item to the ground ($prizeSize)!");
quest::creategroundobject($rndItem, $mob->GetX(), $mob->GetY(), $mob->GetZ(), $mob->GetHeading(), 300000, 0, "treasuregoblin_drops");
}
}
}

#devnoob add 1.5epics
sub handlenpc_epicloot15member {
my $client = shift;
my $npc = shift;
@items = undef;
				$playerclass = $client->GetClass();
				given($playerclass){
				when(1) {
				#warrior
				#60321	Champion's Sword of Eternal Power
				@items = (60321);
				}
				when(2) {
				#cleric
				@items = (9955);
				#9955	Harmony of the Soul
				}
				when(3) {
				#paladin
				@items = (64031);
				#64031	Redemption
				}
				when(4) {
				@items = (144574);
				#ranger
				#144574	Heartwood Blade
				}
				when(5) {
				#shadowknight
				@items = (50003);
				#50003	Innoruuk's Voice
				}
				when(6) {
				#druid
				@items = (62863);
				#62863	Staff of Living Brambles
				}
				when(7) {
				#monk
				@items = (61025);
				#61025	Fistwraps of Celestial Discipline
				}
				when(8) {
				#bard
				@items = (77631);
				#77631	Prismatic Dragon Blade
				}
				when(9) {
				#rogue
				@items = (52347);
				#52347	Fatestealer
				}
				when(10) {
				#shaman
				@items = (57400);
				#57400	Crafted Talisman of Fates
				}
				when(11) {
				#necromancer
				@items = (62581);
				#62581	Soulwhisper
				}
				when(12) {
				#wizard
				@items = (12665);
				#12665	Staff of Prismatic Power
				}
				when(13) {
				#magician
				@items = (19092);
				#19092	Staff of Elemental Essence
				}
				when(14) {
				#enchanter
				@items = (144575);
				#144575	Oculus of Persuasion
				}
				when(15) {
				#beastlord
				@items = (52911);
				#52911	Savage Lord's Totem
				}
				when(16) {
				#berserker
				@items = (18398);
				#18398	Raging Taelosian Alloy Axe
				}

				} #end given

				$num = scalar @items;
				for (my $itm=0; $itm < $num; $itm++) {
					$itemnum = @items[$itm];
					if ( $client->GetLevel() >= 55 && $client->HasItem($itemnum) == -1 )
					{
						$npc->AddItem($itemnum);
						$count += 1;
						return 1;
					}
				}
	return 0;
}

sub handlenpc_epic15loot {
	my $client = shift;
	my $npc = shift;
	my $maxitems = shift;
my $count = 0;
$startPt = quest::ChooseRandom(0,1,2,3,4,5);
my $group = $client->GetGroup();

if ( !defined $maxitems || $maxitems < 1 )
{
$maxitems = 1;
}

	if ( defined $group )
	{
		for (my $a=0; $a < $maxitems*3; $a++) {
		if ( $a < ($maxitems/2))
		{
			$startPt = quest::ChooseRandom(0,1,2,3,4,5);
		}
		else
		{
			$startPt = 0;
		}
		for (my $i=$startPt; $i < 6; $i++) {
			my $tmpclient = $group->GetMember($i);
				if ( defined $tmpclient )
				{
					if ( plugin::handlenpc_epicloot15member($tmpclient,$npc) == 1 )
					{
						$count += 1;
						if ( $count >= $maxcount )
						{
							return;
						}
					}
				}
			}
		}
	}
	else
	{
		plugin::handlenpc_epicloot15member($client,$npc);
	}
}


