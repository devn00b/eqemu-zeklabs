sub EVENT_SPAWN
{
if ( $npc->GetBaseRace() == 23 )
{
quest::settimer("mourn",15);
}
}

sub EVENT_TIMER
{
if ( $timer eq "mourn" )
{
$rnd = quest::ChooseRandom(0,1,2,3);
#      my @clientlist = $entity_list->GetClientList();
#      foreach $ent (@clientlist) {
#       $isexist = $ent->CheckGlobal("akanon_race_justfinished");

#         if ( $ent->GetGM() == 0 && $npc->CalculateDistance($ent->GetX(),$ent->GetY(),$ent->GetZ()) < 225 && $isexist == 0 && $ent->GetLevel() > 10 )
#        {
#		$npc->AddToHateList($ent, 1);
#	}
#	}
if ( $rnd == 1 )
{
quest::doanim(26);
quest::emote("mourns the loss of their brothers and sisters.");
}
}
}
