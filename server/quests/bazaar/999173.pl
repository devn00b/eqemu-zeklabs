sub EVENT_SAY
{
if($text=~/Hail/i){
$flag = $qglobals{skyflag};
if ( $flag ) { quest::say("yep"); } else { quest::say("nope"); }
}
elsif($text=~/signal/i){
$npc->SignalClient($client, 1);
}
}