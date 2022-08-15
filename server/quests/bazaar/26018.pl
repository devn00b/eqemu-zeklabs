#items for this are as follows
#132995 : Title:The Skilled
#132996 : Title:The Dedicated
#132997 : Title:The Decimator
#132998 : Title:The Devastator
#132999 : Title: The Fatestealer
#133000 : Title: The Noob
#133001 : Title: Class Title Ticket
#133002 : Title: The Fail
#133003 : Title: The Zonemaster
#133004 : Title: The Unlucky
#New
#141442 : Title: Master of PvP
#141441 : Title: The Grim Reaper

sub EVENT_SAY { 

if ($text=~/hail/i) { quest::say("Hello! If your here for a title please give me your paperwork");
}
}


sub EVENT_ITEM {

if(plugin::check_handin(\%itemcount, 132995 => 1)) {
quest::enabletitle(1);
quest::say("Here you go! Now go forth and slay other mortals!");
}

elsif(plugin::check_handin(\%itemcount, 132996 => 1)) {
quest::enabletitle(2);
quest::say("Here you go! Now go forth and slay other mortals!");
}

elsif(plugin::check_handin(\%itemcount, 132997 => 1)) {
quest::enabletitle(3);
quest::say("Here you go! Now go forth and slay other mortals!");
}

elsif(plugin::check_handin(\%itemcount, 132998 => 1)) {
quest::enabletitle(4);
quest::say("Here you go! Now go forth and slay other mortals!");
}

elsif(plugin::check_handin(\%itemcount, 132999 => 1)) {
quest::enabletitle(5);
quest::say("Here you go! Now go forth and slay other mortals!");
}

elsif(plugin::check_handin(\%itemcount, 133000 => 1)) {
quest::enabletitle(6);
quest::say("Here you go! Now go forth and slay other mortals!");
}

elsif(plugin::check_handin(\%itemcount, 133001 => 1)) {
quest::enabletitle(7);
quest::say("Here you go! Now go forth and slay other mortals!");
}

elsif(plugin::check_handin(\%itemcount, 133002 => 1)) {
quest::enabletitle(8);
quest::say("Here you go! Now go forth and slay other mortals!");
}

elsif(plugin::check_handin(\%itemcount, 133003 => 1)) {
quest::enabletitle(9);
quest::say("Here you go! Now go forth and slay other mortals!");
}

elsif(plugin::check_handin(\%itemcount, 133004 => 1)) {
quest::enabletitle(10);
quest::say("Here you go! Now go forth and slay other mortals!");
}

elsif(plugin::check_handin(\%itemcount, 141441 => 1)) {
quest::enabletitle(99);
quest::say("Here you go! Now go forth and slay other mortals!");
}
elsif(plugin::check_handin(\%itemcount, 141442 => 1)) {
quest::enabletitle(66);
quest::say("Here you go! Now go forth and slay other mortals!");
}
##### Donator stuff here #####
elsif(plugin::check_handin(\%itemcount, 146596 => 1)) {
quest::enabletitle(11);
quest::enabletitle(15);
quest::say("Thank you for saving our server! We Saute you!");
quest::shout2("Thank you $name for being a bronze donator!");
quest::summonitem(64193,5);
}
elsif(plugin::check_handin(\%itemcount, 146597 => 1)) {
quest::enabletitle(12);
quest::enabletitle(16);
quest::say("Here you go! Now go forth and slay other mortals!");
quest::shout2("Thank you $name for being a Silver donator!");
quest::summonitem(64193,10);
}
elsif(plugin::check_handin(\%itemcount, 146598 => 1)) {
quest::enabletitle(13);
quest::enabletitle(17);
quest::say("Here you go! Now go forth and slay other mortals!");
quest::shout2("Thank you $name for being a Platinum donator!");
quest::summonitem(64193,20);
}
elsif(plugin::check_handin(\%itemcount, 146599 => 1)) {
quest::enabletitle(14);
quest::enabletitle(18);
quest::say("Here you go! Now go forth and slay other mortals!");
quest::shout2("Thank you $name for being a Titanium donator!");
quest::summonitem(64193,50);
}
##### End Donator Stuff  #####

}
