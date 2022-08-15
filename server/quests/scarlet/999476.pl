sub handleScarletDespawn {

my $npc = shift;
my $entity_list = shift;
my $count = 0;
        my @nlist = $entity_list->GetNPCList();
        foreach my $n (@nlist){
	if ( $n->GetOwnerID() == 0 && $n->GetNPCTypeID() != $npc->GetNPCTypeID() && $n->GetNPCTypeID() != 999558 ) #999558 is the skelly man spell merchant
	{
		quest::depopall($n->GetNPCTypeID());
		return;
	}
}
}

sub EVENT_SPAWN {
#quest::shout("Im Depoping the zone since Im UP!");
quest::shout("All you foul creatures, begone while I deal with these mortals!");
#handleScarletDespawn($npc, $entity_list);
quest::spawn_condition("scarlet", 1, 0);
#quest::settimer("checkdespawn",5);
}

sub EVENT_TIMER {
#if ( $timer eq "checkdespawn" )
#{
#handleScarletDespawn($npc, $entity_list);
#}
}

sub EVENT_DEATH {
plugin::handlenpc_ooglyloot($client,$npc,1);
//added kringes tears
$tears='144560';
$npc->AddItem($tears,1);
quest::spawn_condition("scarlet", 1, 1);
#quest::stoptimer("checkdespawn");
}
