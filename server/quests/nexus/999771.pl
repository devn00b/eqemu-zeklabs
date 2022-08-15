sub EVENT_SAY {
if($class != 'Monk' || $class != 'Beastlord' || $class != 'Ranger'){
quest::say("Why would I talk to you $name, you aren't a Beastlord, Ranger or Monk!");
#quest::say("You are a $class");
return;
}


  if($text =~ /Hail/i) {



  $interestedLink = quest::saylink("Whaaaaat"); 
  quest::say("[$interestedLink] ... Who .... Where ..."); quest::doanim(15);
  }

  if($text=~/Whaaaaat/i) {
if($class eq 'Monk' || $class eq 'Beastlord'){
  $itemlink = quest::varlink(6639);
  $itemlink2 = quest::varlink(82734);
  $qstlnk = quest::saylink(" undertake ");
  quest::say("You..I've seen you in my dreams. You are the one who obtains $itemlink then goes on to aquire the amazing $itemlink2. Please [$qstlnk] this quest and save the world.");

return;
}
  $itemlink = quest::varlink(71363);
  $itemlink2 = quest::varlink(147691);
  $qstlnk = quest::saylink(" undertake ");
  quest::say("You..I've seen you in my dreams. You are the one who obtains $itemlink then goes on to aquire the amazing $itemlink2. Please [$qstlnk] this quest and save the world.");

  }

  if($text =~ /undertake/i) {
if($class eq 'Monk' || $class eq 'Beastlord'){
  $itemlink = quest::varlink(6639);
  $itemlink2 = quest::varlink(82734);
  $qstlnk = quest::saylink("time");

  quest::say("There is a great evil coming $name, and only you can stop it. Before you do, your going to have to become more powerful. You need to get me $itemlink. With this item and some magical talismans I can then, with  some luck, grant you $itemlink2 which will help you stop the coming evil.");
  quest::say("");
  quest::say("Dont you think its [$qstlnk] to get started.");

return;
}
  $itemlink = quest::varlink(71363);
  $itemlink2 = quest::varlink(147691);
  $qstlnk = quest::saylink("time");

  quest::say("There is a great evil coming $name, and only you can stop it. Before you do, your going to have to become more powerful. You need to get me $itemlink. With this item and some magical talismans I can then, with  some luck, grant you $itemlink2 which will help you stop the coming evil.");
  quest::say("");
  quest::say("Dont you think its [$qstlnk] to get started.");
  }

  if($text =~ /time/i) {
  $itemlink = quest::varlink(147692);
  $itemlink2 = quest::varlink(147693);
  $itemlink3 = quest::varlink(147694);
  $itemlink4 = quest::varlink(147695);

  $qstlnk = quest::saylink("time");
  quest::say("Good...Good. Now first you must bring me the 3 talismans of lesser powers ($itemlink4, $itemlink3, $itemlink2) and $itemlink. The talismans are found in Solb, and the vine is located deep inside Cazic Thule from The Avatar of Fear.");
  quest::say(" ");
  quest::say("Bring me these and we can start your path to power!");
  }

if($text =~ /move/i) {
if($class eq 'Monk' || $class eq 'Beastlord'){
  $itemlink = quest::varlink(82734);
  $itemlink2 = quest::varlink(147696);
  $itemlink3 = quest::varlink(147697);
  $itemlink4 = quest::varlink(147698);
  $itemlinkx = quest::varlink(6639);
  $itemlinky = quest::varlink(147698);


quest::say("You really are something special $name. In order to create $itemlink I need you to gather me 3 Greater Talismans of Power ($itemlink2, $itemlink3, $itemlink4 ) and bring me $itemlinkx. The Talismans of Power were last seen being used by Lady Vox, and the creatures on the 1st two islands in Plane of Sky. Get them and with all of that I'll be able to create $itemlink wich will complete my dream and hopefully stop the coming darkness");

return;
}
  $itemlink = quest::varlink(147691);
  $itemlink2 = quest::varlink(147696);
  $itemlink3 = quest::varlink(147697);
  $itemlink4 = quest::varlink(147698);
  $itemlinkx = quest::varlink(71363);
  $itemlinky = quest::varlink(147698);


quest::say("You really are something special $name. In order to create $itemlink I need you to gather me 3 Greater Talismans of Power ($itemlink2, $itemlink3, $itemlink4 ) and bring me $itemlinkx. The Talismans of Power were last seen being used by Lady Vox, and the creatures on the 1st two islands in Plane of Sky. Get them and with all of that I'll be able to create $itemlink wich will complete my dream and hopefully stop the coming darkness");
}

}

################### ITEM CODE ##################

sub EVENT_ITEM {
#if($class ne 'Monk' or $class ne 'Beastlord' or $class ne 'Ranger'){
if($class != 'Monk' || $class != 'Beastlord' || $class != 'Ranger'){
quest::say("Why would I talk to you $name, you aren't a Beastlord, Ranger or Monk!");
return;
}
# 1st:
# Talismans Int/wis/str - 147693/147694/147695
# Vine - 147692
#
# Ranger :
# Bow of Magma  - 71363
# Bow of Flames - 147691
#
# Monk :
# Tranquil Staff        -  6639
# Fabled Tranquil Staff - 82734
#
# 2nd:
# Talismans dex/agi/sta - 147696/147697/147698


  if (plugin::check_handin(\%itemcount, 147692 => 1) && plugin::check_handin(\%itemcount, 147693 => 1) && plugin::check_handin(\%itemcount, 147694 => 1) && plugin::check_handin(\%itemcount, 147695 => 1)){ 
if ($class eq 'Monk' || $class eq 'Beastlord'){
    $itemlink = quest::varlink(6639);
    $itemlinkx = quest::varlink(82734);
    $saylink = quest::saylink("move");

    quest::say("You did it! You actually did it! While I knew deep down you would, there was a bit of doubt I must admit $name.");
    quest::say("Now that you have $itemlink you are ready to [$saylink] on and get $itemlinkx and save our world!");
    quest::ding();
    quest::summonitem(6639);
    $client->AddLevelBasedExp(90, 50); ## 90% exp, upto level 50

return;
}
    $itemlink = quest::varlink(71363);
    $itemlinkx = quest::varlink(147691);
    $saylink = quest::saylink("move");

    quest::say("You did it! You actually did it! While I knew deep down you would, there was a bit of doubt I must admit $name.");
    quest::say("Now that you have $itemlink you are ready to [$saylink] on and get $itemlinkx and save our world!");
    quest::ding();
    quest::summonitem(71363);
    $client->AddLevelBasedExp(90, 50); ## 90% exp, upto level 50
return;
  }

if (plugin::check_handin(\%itemcount, 6639 => 1) && plugin::check_handin(\%itemcount, 147696 => 1) && plugin::check_handin(\%itemcount, 147697 => 1) && plugin::check_handin(\%itemcount, 147698 => 1)){
if ($class eq 'Monk' || $class eq 'Beastlord') {
    quest::say("Well done $name you are now ready to face what is coming. Good Luck!");
    quest::ding();
    quest::summonitem(82734);
    $client->AddLevelBasedExp(99, 60); ## 99% exp, upto level 60
return;

}
return;
  }


  if (plugin::check_handin(\%itemcount, 71363 => 1) && plugin::check_handin(\%itemcount, 147696 => 1) && plugin::check_handin(\%itemcount, 147697 => 1) && plugin::check_handin(\%itemcount, 147698 => 1)){
    quest::say("Well done $name you are now ready to face what is coming. Good Luck!");
    quest::ding();
    quest::summonitem(147691);
    $client->AddLevelBasedExp(99, 60); ## 99% exp, upto level 60
return;
  }



quest::say("Please...When am I? Where...am I...");
}
