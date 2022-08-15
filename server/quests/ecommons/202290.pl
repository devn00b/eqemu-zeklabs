sub EVENT_SAY { 

if ($text=~/hail/i) { quest::say("Hello! If your a veteran please turn in your Ticket to me. I will grant you Veteran Jboots");
}
}

#item 85062
sub EVENT_ITEM {
if(plugin::check_handin(\%itemcount, 85062 => 1)) {
#quest::scribespells(3,3);
quest::summonitem(132845);
#quest::level(50);
quest::ding();
quest::givecash(0,0,0,5); 
quest::say("Thank you very much!! Here are some boots and some coin for your troubles!");
}
}
