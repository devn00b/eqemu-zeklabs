#items for this are as follows 132995 : Title:The Skilled 132996 : Title:The Dedicated 132997 : Title:The Decimator 132998 : Title:The Devastator 132999 : Title: The Fatestealer 133000 : 
#Title: The Noob 133001 : Title: Class Title Ticket 133002 : Title: The Fail 133003 : Title: The Zonemaster 133004 : Title: The Unlucky
sub EVENT_SAY { if ($text=~/hail/i) { quest::say("Hello! I'm interested in collecting old keys, and turning them into new keys. If you bought a key and it didnt work Please let me take a 
look!");
}
}
sub EVENT_ITEM { if(plugin::check_handin(\%itemcount, 20884 => 1)) { quest::set_zone_flag(108); quest::summonitem(20884);  quest::say("Thank you! Here is your new key!");
}
elsif(plugin::check_handin(\%itemcount, 20883 => 1)) { quest::set_zone_flag(89); quest::summonitem(20883); quest::say("Thank you! Here is your new key!");
}
elsif(plugin::check_handin(\%itemcount, 20600 => 1)) {quest::set_zone_flag(105); quest::summonitem(20600); quest::say("Thank you! Here is your new key!");
}
elsif(plugin::check_handin(\%itemcount, 27265 => 1)) {quest::set_zone_flag(128); quest::summonitem(27265); quest::say("Thank you! Here is your new key!");
}
}
