my $canTriggerEvent = "1";
my $count = "0";

sub EVENT_SAY {
 if($text=~/hail/i){
  quest::say("Greetings! I am Reefman The Immortal Keeper of the arena, I stand watch over all who come to this place, and tend to the Noobs. I Have a long [story] with Legacy of Zek. If you care to hear it just ask!");
 }
 elsif($text=~/story/i){
  quest::say("I once was a regular mortal player, like yourself. However late on the 21st of July in the year 2013 I passed on, Having suffered from a stroke due to Diabetes complications. When I was awoken I found myself here with Image and Devnoob looking over me.  They asked me if I would rather stay here, Immortal, but forever bound to this place, or face death eternal."); 
  quest::say("I Chose to stay here, amongst you my fellow players, as a reminder, that nothing is forever. Remember me always!");
# quest::summonitem(69230);
# quest::summonitem(69230);
 }
} 

sub EVENT_ATTACK {
if ( $canTriggerEvent == 1 )
{
quest::shout2("Party Time...Excellent!!");
$canTriggerEvent = 0;
quest::settimer("dancetimer",1);
quest::settimer("resettimer",60);
}
}

sub EVENT_TIMER {
if ( $timer eq "dancetimer") {
$npc->DoAnim(58,0);
if ( $count == "1")
{ 
quest::stoptimer("dancetimer");
}
else
{
quest::stoptimer("dancetimer");
quest::settimer("dancetimer",3);
$count = "1";
}
}
if ( $timer eq "resettimer") {
$npc->DoAnim(58,0);
$canTriggerEvent = "1";
quest::stoptimer("resettimer");
}
}

sub EVENT_WAYPOINT_ARRIVE {
my $rnd = int(rand(2500));
my $sayrnd = int(rand(4));
if ($rnd <20) {

if($sayrnd eq 0){
quest::say("I wonder how many players have died today..");
return;
}

if($sayrnd eq 1){
quest::say("You know in the time it took me to do one lap, Olscratch has died 20 times...Probably");
return;
}

if($sayrnd eq 2){
quest::say("...Let us never forget, never trust opeth math!");
return;
}

if($sayrnd eq 3){
$npc->DoAnim(58,0);
quest::emote("Hums to himself");
return;
}

if($sayrnd eq 4){
quest::say("You smell that? Smells like a good indica, Devn00b must be 'round.");
return;
}


quest::say("What a nice day it is. Dont you agree?");
return;
}
}

#Submitted by: devn00b