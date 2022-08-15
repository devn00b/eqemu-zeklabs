sub EVENT_SPAWN {
quest::settimer("walker",3);
my $myOwnerID = $npc->GetOwnerID();
my $mobOwner = $entity_list->GetMobByID($myOwnerID);
my $name = $mobOwner->GetName();
my $targetName = $mobOwner->GetTarget()->GetName();

if($mobOwner->GetTarget()) {
$npc->Attack($targetName);
}
}

sub EVENT_TIMER {

if ( $timer eq "walker" )
{
$npc->Depop();
quest::stoptimer("walker");
}	
}
