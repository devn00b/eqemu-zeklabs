sub EVENT_SAY
{
if ($text=~/Hail/i){quest::say("Do you wish to [travel to kael]?"); }
if ($text=~/travel to kael/i){
quest::movepc(113,-623.4, -29, 123.7);
}
}