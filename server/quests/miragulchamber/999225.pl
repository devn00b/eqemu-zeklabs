#Apparition of Miragul loot
sub EVENT_SPAWN {
quest::settimer("createmiragulloot",1);
}

sub EVENT_TIMER {
if ( $timer eq "createmiragulloot" ) { quest::stoptimer("createmiragulloot"); $npc->AddLootGroup(22); }
}

sub EVENT_KILLED_MERIT {
$client->CastSpell(28000, $userid, 10, 1, 0);
$client->Message(4,"You feel different.");
}