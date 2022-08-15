sub CountNPCTYPE {

	my $npc = plugin::val('npc');
	my $entity_list = plugin::val('$entity_list');
	my $NPC_TYPE_ID = $_[0];

	@npclist = $entity_list->GetNPCList();

		$Count_NPC_TYPE = 0;
     		foreach $cur (@npclist) {
		my $NPC_TYPEid = $cur->GetNPCTypeID(); 
				
			if ($NPC_TYPEid == $NPC_TYPE_ID) { 
			$Count_NPC_TYPE++;
			}
		}
	return $Count_NPC_TYPE;
}