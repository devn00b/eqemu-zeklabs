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

sub EVENT_SAY { 

if ($text=~/hail/i) { quest::say("Hello! If your here for a title please give me your paperwork");
}
}


sub EVENT_ITEM {

if(plugin::check_handin(\%itemcount, 132995 => 1)) {
quest::enabletitle(1);
quest::say("Here you go! Now go forth and slay other mortals!");
}

if(plugin::check_handin(\%itemcount, 132996 => 2)) {
quest::enabletitle(1);
quest::say("Here you go! Now go forth and slay other mortals!");
}

if(plugin::check_handin(\%itemcount, 132997 => 3)) {
quest::enabletitle(1);
quest::say("Here you go! Now go forth and slay other mortals!");
}

if(plugin::check_handin(\%itemcount, 132998 => 4)) {
quest::enabletitle(1);
quest::say("Here you go! Now go forth and slay other mortals!");
}

if(plugin::check_handin(\%itemcount, 132999 => 5)) {
quest::enabletitle(1);
quest::say("Here you go! Now go forth and slay other mortals!");
}

if(plugin::check_handin(\%itemcount, 133000 => 6)) {
quest::enabletitle(1);
quest::say("Here you go! Now go forth and slay other mortals!");
}

if(plugin::check_handin(\%itemcount, 133001 => 7)) {
quest::enabletitle(1);
quest::say("Here you go! Now go forth and slay other mortals!");
}

if(plugin::check_handin(\%itemcount, 133002 => 8)) {
quest::enabletitle(1);
quest::say("Here you go! Now go forth and slay other mortals!");
}

if(plugin::check_handin(\%itemcount, 133003 => 9)) {
quest::enabletitle(1);
quest::say("Here you go! Now go forth and slay other mortals!");
}

if(plugin::check_handin(\%itemcount, 133004 => 10)) {
quest::enabletitle(1);
quest::say("Here you go! Now go forth and slay other mortals!");
}

}
