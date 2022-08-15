sub EVENT_SAY {
  if($text =~ /Hail/i) {
  $interestedLink = quest::saylink("interested"); quest::say("Well hello there! Say, you wouldnt be interested in some, ahem, aftermarket upgrades are you?. Let me know if you're actually $interestedLink"); quest::doanim(15);
  }
if($text=~/interested/i) {
    quest::say("You are? I mean, of course you are! I have upgrades for Wrulon Eye and Earring of Twisted Leaves. If you can bring me an upgrade token from a pvp point merchant and one of those items We can deal.");
 #   quest::say("I also just got a small supply of Fabled Tranquil Staffs. If you are looking for that, bring me a Fabled Token of Tranquility and a normal Tranquil Staff. Word has it Dongle has a few tokens for sale. As for the normal Tranquil staff..well you are on your own there.");
    quest::say("My suppliers just brought me a small stock of Zoom Zoom boots. If you trade me your augment of Zoom Zoom I'll give you some boots!");
  #  quest::say("If you have any Massive Heartwood Thorns I'll take those in trade for a Legendary version. If your interesed bring me a Legendary Token of the Underworld and a Massive Heartwood Thorn");
    quest::say("If you bring me those items, I will make it worth your while!");
    quest::say("");
   # quest::say("***** Note, if your tranquil staff DOES not have aug slots I will not accept it and I will keep it for wasting my time *****");
  }
}

################### ITEM CODE ##################
sub EVENT_ITEM {
#if (plugin::check_handin(\%itemcount, 32428 => 1)){ #### Token Accepted for all items so check it here ####
  
  if (plugin::check_handin(\%itemcount, 5813 => 1) && plugin::check_handin(\%itemcount, 32428 => 1)){ #### If its earring of woven leaves
    quest::say("Thank you for this! Now here is your Legendary Earring of Woven Leaves");
    quest::ding();
    quest::summonitem(135005);
    $client->AddLevelBasedExp(25, 40); ## 25% exp, upto level 40
  }

  if (plugin::check_handin(\%itemcount, 135001 => 1) && plugin::check_handin(\%itemcount, 32428 => 1)){ #### If its the Wrulon Eye
    quest::say("Thank you for this! Now here is your Legendary Wrulon Eye");
    quest::ding();
    quest::summonitem(135002);
    $client->AddLevelBasedExp(25, 40); ## 25% exp, upto level 40
  }

  if (plugin::check_handin(\%itemcount, 135003 => 1) && plugin::check_handin(\%itemcount, 32428 => 1)){ #### If its the Wrulon Eye
    quest::say("Thank you for this! Now here is your Legendary Wrulon Eye");
    quest::ding();
    quest::summonitem(135004);
    $client->AddLevelBasedExp(25, 40); ## 25% exp, upto level 40
  }

 #if (plugin::check_handin(\%itemcount, 141372 => 1) && plugin::check_handin(\%itemcount, 6639 => 1)){ #### Tstaff and Token
#    quest::say("Finally! Here, take this Fabled Tranquil Staff and GO AWAY!!");
#    quest::ding();
#    quest::summonitem(82734);
#    $client->AddLevelBasedExp(50, 60); ## 25% exp, upto level 40
#  }

# if (plugin::check_handin(\%itemcount, 141372 => 1) && plugin::check_handin(\%itemcount, 142736 => 1)){ #### Junk Tstaff and Token
#    quest::say("A Junk T-staff eh? Well I guess I'll take this crap! Here, take this Fabled Tranquil Staff and GO AWAY!!");
#    quest::ding();
#    quest::summonitem(82734);
#    $client->AddLevelBasedExp(50, 60); ## 25% exp, upto level 40
#  }

 if (plugin::check_handin(\%itemcount, 135069 => 1) ){ #### Tstaff and Token
    quest::say("Finally! Here, take this and GO AWAY!!");
    quest::ding();
    quest::summonitem(141430);
    $client->AddLevelBasedExp(50, 60); ## 25% exp, upto level 40
  }

# if (plugin::check_handin(\%itemcount, 141437 => 1) && plugin::check_handin(\%itemcount, 2509 => 1)){ #### Tstaff and Token
#    quest::say("Enjoy your new Legendary Massive Heartwood Thorn");
#    quest::ding();
#    quest::summonitem(141436);
#    $client->AddLevelBasedExp(90, 60); ## 25% exp, upto level 40
#  }



quest::say("Please, don't tell Devnoob or Image about me, I would hate to have to go on the run, and stop selling you my special wares.");
}
#  else {
#     quest::say("Free Shit! Thanks!!!");
#     plugin::return_items(\%itemcount);
#  }
#}
