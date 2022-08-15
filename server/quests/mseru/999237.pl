sub EVENT_SAY
{
if($text=~/Hail/i){
$return = quest::saylink("return to Nexus");
quest::say("Hello $name.  Would you like to $return?");
}
if($text=~/return to nexus/i){
quest::say("Safe travels friend!");
$client->MovePC(152,0,0,-20,0);
}
}