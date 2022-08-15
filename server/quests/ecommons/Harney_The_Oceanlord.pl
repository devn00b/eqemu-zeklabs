my $canTriggerEventx = "1";
my $countx = "0";

sub EVENT_SAY {
 if($text=~/hail/i){
  quest::say("Hello! I am harney the greatest player in all of GuildWars history. You dont even compair to my greatness. You should tremble before my might!");
  quest::settimer("dancetimerx",1);
  quest::settimer("resettimerx",60);
 }
}

sub EVENT_ATTACK {
if ( $canTriggerEvent == 1 )
{

quest::shout2("All fear the wrath of Harney The Oceanlord! $name Your death stands as a warning to all!");
$canTriggerEvent = 0;
quest::settimer("dancetimerx",1);
quest::settimer("resettimerx",60);
}
}

sub EVENT_TIMER {
if ( $timer eq "dancetimerx") {
$npc->DoAnim(58,0);
if ( $count == "1")
{
quest::stoptimer("dancetimerx");
}
else
{
quest::stoptimer("dancetimerx");
quest::settimer("dancetimerx",3);
$count = "1";
}
}
if ( $timer eq "resettimerx") {
$npc->DoAnim(58,0);
$canTriggerEvent = "1";
quest::stoptimer("resettimerx");
}
}

