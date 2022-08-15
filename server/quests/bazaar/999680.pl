sub EVENT_SAY {
###disable till after ea
###
###
#return;
##################
  if($text =~ /Hail/i) {
$interested = quest::saylink("help");
$goldenticket = quest::varlink(146739);
my $devx = $devnoob;
my $devperx = $client->GetGlobal("devpersonal");
my $mxlvl = quest::GetRule("Character:MaxLevel");
my $mxl = $mxlvl + 10;

if (!defined $qglobals{"devnoob"}) {
quest::setglobal("devnoob", 0, 7, "F");
}
if (!defined $qglobals{"devpersonal"}) {
quest::setglobal("devpersonal", 0, 5, "F");
}

if($devperx <1){
$devperx=0;
}
#quest::say("We are currently at $devx tickets out of 100. Your personal turn in count is $devperx.");

quest::say("Hello $name, I'm sure your wondering why I'm here, or what sort of sadistic game is devn00b playing. Welp The game is this");
quest::say("We are currently capped at $mxlvl as our max level. This sucks since we are supposed to be $mxl. To get that cap to open before the next cap I need your [$interested].");
quest::say("If you are just checking in. We are currently at $devnoob tickets and you personally are at $devperx. Please hand me a $goldenticket.");
  }

  if($text =~ /help/i) {
quest::say("Devn00b is being a dick, as usual, and wants you players to collect 150 $goldenticket before he will raie the cap. To be an even bigger dick they only drop from Tayla Nybright in Lfaydark. Ugh.");
quest::say("So, you bring me $goldenticket and once we reach 150 of them I will shout something special, once I do that someone will need to take a picture and send it to devnoob in discord. Once he sees it Bam new level cap!");
quest::say("Oh, there will be a prize for the players to turn in the most tickets. Also, I hear Tayla has some new items she has been showing off, you can keep those when you kill her.");
  }



}

sub EVENT_ITEM {
###disable till after ea
###
###
#return;
##################




if(plugin::check_handin(\%itemcount, 146739 => 4)){
if (!defined $qglobals{"devnoob"}) {
quest::setglobal("devnoob", 0, 7, "F");
}
if (!defined $qglobals{"devpersonal"}) {
quest::setglobal("devpersonal", 0, 5, "F");
}
if($devnoob>=151){
quest::say("Sorry we have already reached the max. Save your tickets for the next cap, maybe");
return;
}
$devper = $client->GetGlobal("devpersonal") + 4;
quest::setglobal("devpersonal", $devper, 5, "F");
quest::say("Your personal total turn in count is $devper. I have added 4.");
quest::ding();
$devt = $devnoob;
quest::setglobal("devnoob", ($devt + 4), 7, "F");
##must be 1 less than the number we want, so 30 is 31 in game
if($devnoob == 149){
#quest::shout("WOOP");
if (!defined $qglobals{"devcompleted"}) {
quest::setglobal("devcompleted", 0, 7, "F");
}
$devliar = int(rand (99999999999999));
quest::shout2("Congrats! $name has completed the quest!!. Show devn00b this screenshot!!! Completed Code: $devliar");
quest::setglobal("devcompleted", ($devliar), 7, "F");
}
my $devx = $devnoob + 4;
quest::say("Thank you $name I've added 4 and we are now at $devx tickets out of 150. ");
    return;
  }



if(plugin::check_handin(\%itemcount, 146739 => 3)){
if (!defined $qglobals{"devnoob"}) {
quest::setglobal("devnoob", 0, 7, "F");
}
if (!defined $qglobals{"devpersonal"}) {
quest::setglobal("devpersonal", 0, 5, "F");
}
if($devnoob>=151){
quest::say("Sorry we have already reached the max. Save your tickets for the next cap, maybe");
return;
}
$devper = $client->GetGlobal("devpersonal") + 3;
quest::setglobal("devpersonal", $devper, 5, "F");
quest::say("I've added 4 to your personal total turn in count, it is now $devper.");
quest::ding();
$devt = $devnoob;
quest::setglobal("devnoob", ($devt + 3), 7, "F");
##must be 1 less than the number we want, so 30 is 31 in game
if($devnoob == 149){
#quest::shout("WOOP");
if (!defined $qglobals{"devcompleted"}) {
quest::setglobal("devcompleted", 0, 7, "F");
}
$devliar = int(rand (99999999999999));
quest::shout2("Congrats! $name has completed the quest!!. Show devn00b this screenshot!!! Completed Code: $devliar");
quest::setglobal("devcompleted", ($devliar), 7, "F");
}
my $devx = $devnoob + 3;
quest::say("Thank you $name I've added 3 and we are now at $devx tickets out of 150.");
    return;
  }



if(plugin::check_handin(\%itemcount, 146739 => 2)){
if (!defined $qglobals{"devnoob"}) {
quest::setglobal("devnoob", 0, 7, "F");
}
if (!defined $qglobals{"devpersonal"}) {
quest::setglobal("devpersonal", 0, 5, "F");
}
if($devnoob>=151){
quest::say("Sorry we have already reached the max. Save your tickets for the next cap, maybe");
return;
}
$devper = $client->GetGlobal("devpersonal") + 2;
quest::setglobal("devpersonal", $devper, 5, "F");
quest::say("Your personal total turn in count has increased by (2) it is now $devper.");
quest::ding();
$devt = $devnoob;
quest::setglobal("devnoob", ($devt + 2), 7, "F");
##must be 1 less than the number we want, so 30 is 31 in game
if($devnoob == 149){
#quest::shout("WOOP");
if (!defined $qglobals{"devcompleted"}) {
quest::setglobal("devcompleted", 0, 7, "F");
}
$devliar = int(rand (99999999999999));
quest::shout2("Congrats! $name has completed the quest!!. Show devn00b this screenshot!!! Completed Code: $devliar");
quest::setglobal("devcompleted", ($devliar), 7, "F");
}
my $devx = $devnoob + 2;
quest::say("Thank you $name, I added 2 and we are now at $devx tickets out of 150.");
    return;
  }



if(plugin::check_handin(\%itemcount, 146739 => 1)){
if (!defined $qglobals{"devnoob"}) {
quest::setglobal("devnoob", 0, 7, "F");
}
if (!defined $qglobals{"devpersonal"}) {
quest::setglobal("devpersonal", 0, 5, "F");
}
if($devnoob>=151){
quest::say("Sorry we have already reached the max. Save your tickets for the next cap, maybe");
return;
}
$devper = $client->GetGlobal("devpersonal") + 1;
quest::setglobal("devpersonal", $devper, 5, "F");
quest::say("Your personal total turn in count is $devper.");
quest::ding();
$devt = $devnoob;
quest::setglobal("devnoob", ($devt + 1), 7, "F");
##must be 1 less than the number we want, so 30 is 31 in game
if($devnoob == 149){
#quest::shout("WOOP");
if (!defined $qglobals{"devcompleted"}) {
quest::setglobal("devcompleted", 0, 7, "F");
}
$devliar = int(rand (99999999999999));
quest::shout2("Congrats! $name has completed the quest!!. Show devn00b this screenshot!!! Completed Code: $devliar");
quest::setglobal("devcompleted", ($devliar), 7, "F");
}
my $devx = $devnoob + 1;
quest::say("Thank you $name, we are now at $devx tickets out of 150.");
    return;
  }


quest::say("...");
}

