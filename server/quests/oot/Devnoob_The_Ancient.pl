#devn00b is the shit just saying
#$test = $client->GetPVPPoints();
#Required items
# 
# Warrior, SK, Pal, Monk ,Rogue=> Cauldron => Jboots, Red Scale, White Scale, PVP item
# Ranger, Druid, Cleric, Shaman, Bard => OOT => Pegasus Feather Cloak, Red Scale, White Scale. PVP item
# Enchanter, Necro, Mage,  Wiz => Lavastorm
sub EVENT_SAY {

#if(!($class=~/Paladin/i) || !($class=~/Warrior/i) || !($class=~/Shadow/i) || !($class=!/Monk/i) || !($class=~/Rogue/i)) { 
if($class ne 'Ranger' && $class ne 'Druid' && $class ne 'Cleric' && $class ne 'Shaman' && $class ne 'Bard') {
	quest::say("GO AWAY! You aren't worthy enough to stand before me $class");
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
  	$interestedLink3 = quest::saylink("Pegasus Feather Cloak");
  	$interestedLink4 = quest::saylink("Kings Certificate of Epicness");
  	
    quest::say("Here are the items I require: $interestedLink, $interestedLink2, $interestedLink3, $interestedLink4. When you have collected all of these please bring them back to me and I will reassemble them!");
  }

if($text=~/Red Dragon Scales/i) {
  	quest::say("These magic scales can only be found one place, from the body of Lord Nagafen himself. I don't suspect he will just give them to you.");
  }

if($text=~/White Dragon Scales/i) {
  	quest::say("Oh yes! The White Dragon Scales that are forever cold to the touch! Rumor has it the only source for these is Lady Vox.");
  }

if($text=~/Pegasus Feather Cloak/i) {
  	quest::say("The floating cloak of legend! Please strip this from Pegasus in South Karana");
  }

if($text=~/Kings Certificate of Epicness/i) {
  	quest::say("Ah yes, this item I require to infuse the weapon with the blood of the fallen! You can find this item in the bazaar im told, but only the most experianced player killers can afford it!");
  }


}
}
sub EVENT_ITEM {

#if($class ne 'Ranger' && $class ne 'Druid' && $class ne 'Cleric' && $class ne 'Shaman' && $class ne 'Bard') {	
  
  if (plugin::check_handin(\%itemcount, 11622 => 1, 11602 => 1, 2463 => 1, 31161 => 1)) { # 
#  	if($class ne 'Monk' && $class ne 'Warrior' && $class ne 'Shadowknight' && $class ne 'Monk' && $class ne 'Rogue' && $class ne 'Paladin') {
#		quest::say("GO AWAY! You aren't worth to stand before me $class");
#		plugin::return_items(\%itemcount);
#		return;
#	}

if($class eq 'Ranger') {
#Swiftwind - 20487 (graphix)
#Earthcaller - 20488 (grapix)
    quest::say("What is this? Oh My! I can feel the power coursing through these items!");
    #quest::emote("Your mind is filled with the sound of battle and a voice says to you 'Thank you young $class As a reward for setting me free take this!'");
    #quest::emote("addresses you as you turn to leave. 'One more thing. Gartrog over there has told me of a great discovery. Seems there are all sorts of stones being found in the recently discovered lost dungeons scattered around Antonica.  The Wayfarer's Brotherhood has found a way to make items even more powerful by using the stones with existing items.'");
    quest::ding();
    quest::exp("1000");
	
    quest::summonitem("20487"); # Celestial fists or whtever
    quest::summonitem("20488");
    return;
  }

if($class eq 'Druid') {
    quest::say("What is this? Oh My! I can feel the power coursing through these items!");
    #quest::emote("Your mind is filled with the sound of battle and a voice says to you 'Thank you young $class As a reward for setting me free take this!'");
    #quest::emote("addresses you as you turn to leave. 'One more thing. Gartrog over there has told me of a great discovery. Seems there are all sorts of stones being found in the recently discovered lost dungeons scattered around Antonica.  The Wayfarer's Brotherhood has found a way to make items even more powerful by using the stones with existing items.'");
    quest::ding();
    quest::exp("1000");

    quest::summonitem("20490"); 
    return;
  }  
  
if($class eq 'Cleric') {
    quest::say("What is this? Oh My! I can feel the power coursing through these items!");
    #quest::emote("Your mind is filled with the sound of battle and a voice says to you 'Thank you young $class As a reward for setting me free take this!'");
    #quest::emote("addresses you as you turn to leave. 'One more thing. Gartrog over there has told me of a great discovery. Seems there are all sorts of stones being found in the recently discovered lost dungeons scattered around Antonica.  The Wayfarer's Brotherhood has found a way to make items even more powerful by using the stones with existing items.'");
    quest::ding();
    quest::exp("1000");
	
    quest::summonitem("5532"); # Celestial fists or whtever
    return;
  }  

if($class eq 'Shaman') {
	    quest::say("What is this? Oh My! I can feel the power coursing through these items!");
    #quest::emote("Your mind is filled with the sound of battle and a voice says to you 'Thank you young $class As a reward for setting me free take this!'");
    #quest::emote("addresses you as you turn to leave. 'One more thing. Gartrog over there has told me of a great discovery. Seems there are all sorts of stones being found in the recently discovered lost dungeons scattered around Antonica.  The Wayfarer's Brotherhood has found a way to make items even more powerful by using the stones with existing items.'");
    quest::ding();
    quest::exp("1000");

    quest::summonitem("10651"); # Celestial fists or whtever
    return;
  }  
  
  if($class eq 'Bard') {
	    quest::say("What is this? Oh My! I can feel the power coursing through these items!");
    #quest::emote("Your mind is filled with the sound of battle and a voice says to you 'Thank you young $class As a reward for setting me free take this!'");
    #quest::emote("addresses you as you turn to leave. 'One more thing. Gartrog over there has told me of a great discovery. Seems there are all sorts of stones being found in the recently discovered lost dungeons scattered around Antonica.  The Wayfarer's Brotherhood has found a way to make items even more powerful by using the stones with existing items.'");
    quest::ding();
    quest::exp("1000");

    quest::summonitem("20542"); # Celestial fists or whtever
    return;
  }

  }
  quest::say("Nice try weakling!");
  plugin::return_items(\%itemcount);
}



