sub EVENT_SAY
{
 if($text=~/Hail/i){
$var = quest::saylink("cure");
quest::say("Our lair has been infested!! At this point I do not know what to do... Someone could try to $var that place of disease!");
}
 if($text=~/cure/i){
$var = quest::saylink("enter");
quest::say("Ohh yes someone could $var the lair and try to do that!");
}
 if($text=~/enter/i){
$var = quest::saylink("lead");
quest::say("I can $var you in but I warn you it is very spooky in there!!");
}
 if($text=~/lead/i){
quest::say("Maybe another time... I am weary of you right now.");
}
} #event_say