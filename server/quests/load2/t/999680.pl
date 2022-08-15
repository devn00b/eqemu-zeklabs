sub EVENT_SAY {
  if($text =~ /Hail/i) {
$interested = quest::saylink("help");
quest::say("We are currently at $devnoob tickets, please hand me a leveled golden ticket, or do you need [$interested]");
  }

  if($text =~ /help/i) {
quest::say("You bring me leveled golden tickets, and once we reach 100 of them I will shout something special, once I do that someone will need to take a picture and send it to devnoob in discord.");
  }


}

sub EVENT_ITEM {

if(plugin::check_handin(\%itemcount, 146739 => 1)){
if (!defined $qglobals{"devnoob"}) {
quest::setglobal("devnoob", 0, 7, "F");
}
if (!defined $qglobals{"devpersonal"}) {
quest::setglobal("devnoob", 0, 5, "F");
}
$devper = $client->GetGlobal("devpersonal") + 1;
quest::setglobal("devpersonal", $devper, 5, "F");
quest::say("Your personal total turn in count is $devper.");
quest::ding();
$devt = $devnoob;
quest::setglobal("devnoob", ($devt + 1), 7, "F");
##must be 1 less than the number we want, so 30 is 31 in game
if($devnoob == 48){
#quest::shout("WOOP");
if (!defined $qglobals{"devcompleted"}) {
quest::setglobal("devcompleted", 0, 7, "F");
}
$devliar = int(rand (99999999999999));
quest::shout2("Congrats! $name has turned in the last ticket. Show devn00b this screenshot!!! Completed Code: $devliar");
quest::setglobal("devcompleted", ($devliar), 7, "F");
}
my $devx = $devnoob + 1;
quest::say("Thank you $name, we are now at $devx tickets out of 100.");
    return;
  }


quest::say("...");
}

