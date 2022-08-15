use feature qw/switch/; 

sub EVENT_ITEM_CLICK_CAST {
if ( $instanceid > 9999 )
{
	$client->Message(15,"Guild camps are not allowed in instances."); 
	$client->CastSpell(8558, 0, 10, 0, 0); 
  	return;
}
given($zoneid){
#freeport
  when(8) { 
	    $client->Message(15,"Guild cities cannot have camps spawned inside them."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
  when(9) { 
	    $client->Message(15,"Guild cities cannot have camps spawned inside them."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
  when(10) { 
	    $client->Message(15,"Guild cities cannot have camps spawned inside them."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#felwithe
  when(61) { 
	    $client->Message(15,"Guild cities cannot have camps spawned inside them."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
  when(62) { 
	    $client->Message(15,"Guild cities cannot have camps spawned inside them."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#erudin
  when(23) { 
	    $client->Message(15,"Guild cities cannot have camps spawned inside them."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
  when(24) { 
	    $client->Message(15,"Guild cities cannot have camps spawned inside them."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }

#tox camp
  when(38) { 
	    $client->Message(15,"Camps cannot be spawned in zones already occupied by an encampment."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#lavastorm
  when(27) { 
	    $client->Message(15,"Camps cannot be spawned in zones already occupied by an encampment."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#gfaydark
  when(54) { 
	    $client->Message(15,"Camps cannot be spawned in zones already occupied by an encampment."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#great divide
  when(118) { 
	    $client->Message(15,"Camps cannot be spawned in zones already occupied by an encampment."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }

#airplane
  when(71) { 
	    $client->Message(15,"Camps cannot be spawned the Planes of the Gods."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#fearplane
  when(72) { 
	    $client->Message(15,"Camps cannot be spawned the Planes of the Gods."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#hateplane
  when(76) { 
	    $client->Message(15,"Camps cannot be spawned the Planes of the Gods."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }

#sharvahl
  when(155) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#akanon
  when(55) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#grobb
  when(52) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#halas
  when(29) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#highpass
  when(5) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#highkeep
  when(6) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#kaladima
  when(60) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#kaladimb
  when(67) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#neriaka
  when(40) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#neriakb
  when(41) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#neriakc
  when(42) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#oggok
  when(49) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#paineel
  when(75) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#qeynos
  when(1) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#qeynos2
  when(2) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#qrg
  when(3) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#rivervale
  when(19) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#soltemple
  when(80) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#thurgadina
  when(115) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#icewell
  when(129) { 
	    $client->Message(15,"Camps cannot be spawned in cities."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }

#END CITIES
#END CITIES


#arena
  when(77) { 
	    $client->Message(15,"The arena cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#bazaar
  when(151) { 
	    $client->Message(15,"The bazaar cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#nexus
  when(152) { 
	    $client->Message(15,"The nexus cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }

#END ZONE SPECIFICS
#END ZONE SPECIFICS

#sebilis
  when(89) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#befallen
  when(77) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#beholder
  when(16) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#blackburrow
  when(17) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#cazic thule
  when(48) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#crushbone
  when(58) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#guktop
  when(65) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#gukbottom
  when(66) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#hole
  when(39) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#kedge
  when(64) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#kerraridge
  when(74) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#mistmoore
  when(59) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#najena
  when(44) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#paw
  when(18) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#permafrost
  when(73) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#qcat
  when(45) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#runnyeye
  when(11) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#soldunga
  when(31) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#soldungb
  when(32) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#unrest
  when(63) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#miragulchamber
  when(252) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#rujb
  when(235) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#scarlet
  when(175) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#warrens
  when(101) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#soldungc
  when(278) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#necropolis
  when(123) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#kael
  when(113) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#skyshrine
  when(114) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#velketor
  when(112) { 
	    $client->Message(15,"Dungeons cannot contain guild camps."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }

#END DUNGEONS
#END DUNGEONS

#westwastelands
  when(210) { 
	    $client->Message(15,"Guild camps are not currently supported in this zone."); 
	    $client->CastSpell(8558, 0, 10, 0, 0); 
  	    return;
	  }
#DEVN00BS NEW ZONES
#Tenebrus
  when(172) {
            $client->Message(15,"Guild camps are not currently supported in devnoob's zones.");
            $client->CastSpell(8558, 0, 10, 0, 0);
            return;
          }
#DSP
  when(174) {
            $client->Message(15,"Guild camps are not currently supported in devnoob's zones.");
            $client->CastSpell(8558, 0, 10, 0, 0);
            return;
          }
#PC
  when(156) {
            $client->Message(15,"Guild camps are not currently supported in devnoob's zones.");
            $client->CastSpell(8558, 0, 10, 0, 0);
            return;
          }
#DLands
  when(86) {
            $client->Message(15,"Guild camps are not currently supported in devnoob's zones.");
            $client->CastSpell(8558, 0, 10, 0, 0);
            return;
          }
#velks
  when(112) {
            $client->Message(15,"Guild camps are not currently supported in devnoob's zones.");
            $client->CastSpell(8558, 0, 10, 0, 0);
            return;
          }

#Kael
  when(113) {
            $client->Message(15,"Guild camps are not currently supported in devnoob's zones.");
            $client->CastSpell(8558, 0, 10, 0, 0);
            return;
          }
#kaesora
  when(88) {
            $client->Message(15,"Guild camps are not currently supported in devnoob's zones.");
            $client->CastSpell(8558, 0, 10, 0, 0);
            return;
          }
#Charasis
  when(105) {
            $client->Message(15,"Guild camps are not currently supported in devnoob's zones.");
            $client->CastSpell(8558, 0, 10, 0, 0);
            return;
          }
#crystal caverns
  when(121) {
            $client->Message(15,"Guild camps are not currently supported in devnoob's zones.");
            $client->CastSpell(8558, 0, 10, 0, 0);
            return;
          }
#echo
  when(153) {
            $client->Message(15,"Guild camps are not currently supported in devnoob's zones.");
            $client->CastSpell(8558, 0, 10, 0, 0);
            return;
          }
#karnor
  when(102) {
            $client->Message(15,"Guild camps are not currently supported in devnoob's zones.");
            $client->CastSpell(8558, 0, 10, 0, 0);
            return;
          }
#ToV
  when(124) {
            $client->Message(15,"Guild camps are not currently supported in devnoob's zones.");
            $client->CastSpell(8558, 0, 10, 0, 0);
            return;
          }
#seb
  when(89) {
            $client->Message(15,"Guild camps are not currently supported in devnoob's zones.");
            $client->CastSpell(8558, 0, 10, 0, 0);
            return;
          }
#Akheva
  when(179) {
            $client->Message(15,"Guild camps are not currently supported in devnoob's zones.");
            $client->CastSpell(8558, 0, 10, 0, 0);
            return;
          }
#ToV
  when(171) {
            $client->Message(15,"Guild camps are not currently supported in devnoob's zones.");
            $client->CastSpell(8558, 0, 10, 0, 0);
            return;
          }


#END BROKEN ZONES
#END BROKEN ZONES

}

if ( $client->GetLevel() < 30 )
{
     $client->Message(15,"You are too low level to spawn a guild camp, must be level 30 or higher."); 
     $client->CastSpell(8558, 0, 10, 0, 0); 
     return;
}

	my $camp_spawner = $entity_list->GetNPCByNPCTypeID(999577);
	if ( defined $camp_spawner )
	{
		$client->Message(15,"A camp assistant is already spawned in this area and only one can be spawned at a time.");
		$client->CastSpell(8558, 0, 10, 0, 0);
	}
	else
	{
		$client->Message(15,"The camp assistant is spawning to assist you.  You can request to [setup camp], determine if zone point is nearby [zonenear], determine if a conflicting npc is nearby [npcnear], determine if a conflicting location (another guild camp) is nearby [locationnear].");
		quest::spawn2(999577,0,0,$x,$y,$z,196.1);
		quest::signalwith(999577,$userid,0);
		$client->CastSpell(8558, 0, 10, 0, 0);
	}
}
