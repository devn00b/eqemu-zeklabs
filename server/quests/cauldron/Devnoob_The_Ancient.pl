#devn00b is the shit just saying
#$test = $client->GetPVPPoints();
#Required items
# 
# Warrior, SK, Pal, Monk ,Rogue=> Cauldron => Jboots, Red Scale, White Scale, PVP item
# Ranger, Druid, Cleric, Shaman, Bard => OOT => Pegusus Feather Cloak, Scales, PVP item
# Enchanter, Necro, Mage,  Wiz => Lavastorm => Ishva Maul Brain, Scales, PVP item
sub EVENT_SAY {

#if(!($class=~/Paladin/i) || !($class=~/Warrior/i) || !($class=~/Shadow/i) || !($class=!/Monk/i) || !($class=~/Rogue/i)) { 
if($class ne 'Monk' && $class ne 'Warrior' && $class ne 'Shadowknight' && $class ne 'Monk' && $class ne 'Rogue' && $class ne 'Paladin') {
	quest::say("GO AWAY! You aren't worthy Enough to stand before me $class");
return;
	}	
	
	
	if($ulevel <='49') {
     quest::say("Go Away Weakling");		
   
 	} 
 	
 	
 	else {
 		
  if($text=~/hail/i) {
  	$interestedLink = quest::saylink("working");

  	
  	quest::doanim(15);
     quest::say("Hello there. Why are you bothering me? Cant you see I'm $interestedLink here?.");
 
  }
  if($text=~/working/i) {
  	$interestedLink = quest::saylink("interested");
    quest::say("I'm here searching for parts of my lost weapons. If your not $interestedLink in helping me please get lost.");
  }

if($text=~/interested/i) {
  	$interestedLink = quest::saylink("sure");
    quest::say("Are you $interestedLink? The task ahead wont be easy!");
    quest::ding();
  }

if($text=~/sure/i) {
  	$interestedLink = quest::saylink("Red Dragon Scales");
  	$interestedLink2 = quest::saylink("White Dragon Scales");
  	$interestedLink3 = quest::saylink("Journeymans Boots");
  	$interestedLink4 = quest::saylink("Kings Certificate of Epicness");
  	
    quest::say("Here are the items I require: $interestedLink, $interestedLink2, $interestedLink3, $interestedLink4. When you have collected all of these please bring them back to me and I will reassemble them!");
  }

if($text=~/Red Dragon Scales/i) {
  	quest::say("These magic scales can only be found one place, from the body of Lord Nagafen himself. I don't suspect he will just give them to you.");
  }

if($text=~/White Dragon Scales/i) {
  	quest::say("Oh yes! The White Dragon Scales that are forever cold to the touch! Rumor has it the only source for these is Lady Vox.");
  }

if($text=~/Journeymans Boots/i) {
  	quest::say("Mmmm Journeyman's Boots. I crave these items from the depths of my soul! Gather them for me from the mighty Magician Najena");
  }

if($text=~/Kings Certificate of Epicness/i) {
  	quest::say("Ah yes, this item I require to infuse the weapon with the blood of the fallen! You can find this item in the bazaar im told, but only the most experianced player killers can afford it!");
  }


}
}
sub EVENT_ITEM {
	
  
  if (plugin::check_handin(\%itemcount, 11622 => 1, 11602 => 1, 2300 => 1, 31161 => 1)) { # 
#  	if($class ne 'Monk' && $class ne 'Warrior' && $class ne 'Shadowknight' && $class ne 'Monk' && $class ne 'Rogue' && $class ne 'Paladin') {
#		quest::say("GO AWAY! You aren't worth to stand before me $class");
#		plugin::return_items(\%itemcount);
#		return;
#	}

if($class eq 'Monk') {
	
    quest::say("What is this? Oh My! I can feel the power coursing through these items!");
 
    quest::ding();
    quest::exp("1000");
    quest::summonitem("10652"); # Celestial fists or whtever
    return;
  }

if($class eq 'Warrior') {
#2h - Jagged Blade of War - 10908
#1h - Blade of tactics - 10909
#1h - Blade of Strategy - 10910

    quest::say("What is this? Oh My! I can feel the power coursing through these items!");
 
    quest::ding();
    quest::exp("1000");
    quest::summonitem("10908"); # Celestial fists or whtever
    quest::summonitem("10909"); 
    quest::summonitem("10910"); 
    return;
  }  
  
if($class eq 'Shadowknight') {
	
    quest::say("What is this? Oh My! I can feel the power coursing through these items!");
 
    quest::ding();
    quest::exp("1000");
	
    quest::summonitem("14383"); # Celestial fists or whtever
    return;
  }  

if($class eq 'Rogue') {
	
    quest::say("What is this? Oh My! I can feel the power coursing through these items!");
 
    quest::ding();
    quest::exp("1000");
	
    quest::summonitem("11057"); # Celestial fists or whtever
    return;
  }  
  
  if($class eq 'Paladin') {
	
    quest::say("What is this? Oh My! I can feel the power coursing through these items!");
 
    quest::ding();
    quest::exp("1000");
    quest::summonitem("10099"); # Celestial fists or whtever
    return;
  } 
 
  }
  quest::say("Hahahahahahahahahahaha Nice Try mortal");
 plugin::return_items(\%itemcount)
}


