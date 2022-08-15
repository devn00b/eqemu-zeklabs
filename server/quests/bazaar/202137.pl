sub EVENT_SAY {
  if($text =~ /Hail/i) {
  $moore = quest::saylink("supercharging");quest::say("Hello! $uname, are you interested in $moore your Shield of Elders?"); quest::doanim(15);
}

if($text=~/supercharging/i) {
quest::say("Bring me A Shield of Elders, and a token from Dongle, once I have those I can get to work on your shield.");
  }

}

sub EVENT_ITEM {
if (plugin::check_handin(\%itemcount, 10851 => 1) && plugin::check_handin(\%itemcount, 141372 => 1)){ #### Shield of Elders Upgrade
    quest::say("Magnificent! As a reward here is your shield and some xp!");
    quest::ding();
    quest::summonitem(142460);
    $client->AddLevelBasedExp(95,50); ## 95% exp, upto level 50
  }




quest::say("Please, don't tell Devnoob or Image about me, I would hate to have to go on the run, and stop selling you my special wares.");
}
#  else {
#     quest::say("Free Shit! Thanks!!!");
#     plugin::return_items(\%itemcount);
#  }
#}
