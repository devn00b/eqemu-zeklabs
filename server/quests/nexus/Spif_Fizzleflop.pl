sub EVENT_SAY
{
 if($text=~/Hail/i){
$var = quest::saylink("lead");
quest::say("This cave is forbidden!! However... I can $var you in because I am such a nice guy!");
}
 if($text=~/lead/i){
if ( $client->GetGM() || $client->GetLevel() < 30 ) {
$client->MovePC(235,364.7,-764.64,-16.4,256); #rujb
}
else { $client->Message(4,"This zone is for players under level 30 only."); }
}
} #event_say