#Spawns non-Destroy Aten if no boss mobs are up. 

sub EVENT_SPAWN {
	quest::settimer("aten",60);
}

sub EVENT_TIMER {
	if($timer eq "aten") {
    		if (!$entity_list->GetMobByNpcTypeID(158014) && !$entity_list->GetMobByNpcTypeID(158010) && !$entity_list->GetMobByNpcTypeID(158015) && !$entity_list->GetMobByNpcTypeID(158012) && !$entity_list->GetMobByNpcTypeID(158013) && !$entity_list->GetMobByNpcTypeID(158007) && !$entity_list->GetMobByNpcTypeID(158008) && !$entity_list->GetMobByNpcTypeID(158011) && !$entity_list->GetMobByNpcTypeID(158009) &&  $entity_list->GetMobByNpcTypeID(158006) && !defined $qglobals{aten}) {
      			quest::depopall(158006);
      			quest::spawn2(158096,0,0,1412,0,248.63,192);
                        quest::depop();
      		}
    	}
}      