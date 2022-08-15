sub EVENT_SAY {
  if($text =~ /Hail/i) {
  $interestedLink = quest::saylink("Whaaaaat"); 
  quest::say("[$interestedLink] ... Who .... Where ..."); quest::doanim(15);
  }

  if($text=~/Whaaaaat/i) {
  $itemlink = quest::varlink(71363);
  $itemlink2 = quest::varlink(147691);
  $qstlnk = quest::saylink(" undertake ");
  quest::say("You..I've seen you in my dreams. You are the one who obtains $itemlink then goes on to aquire the amazing $itemlink2. Please [$qstlnk] this quest and save the world.");

  }

  if($text =~ /undertake/i) {
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
  $itemlink4 = quest::varlink(147694);

  $qstlnk = quest::saylink("time");
quest::say("Good...Good. Now first you must bring me the 3 talismans of lesser powers ($itemlink4, $itemlink3, $itemlink2) and $itemlink. The talismans are found in Solb, and the vine is located deep inside Cazic Thule from The Avatar of Fear.");
quest::say(" ");
quest::say("Bring me these and we can start your path to power!");
  }

}

################### ITEM CODE ##################
sub EVENT_ITEM {
# 1st:
# Talismans Int/wis/str - 147693/147694/147695
# Vine - 147692
#
# Bow of Magma  - 71363
# Bow of Flames - 147691
#
# 2nd:
# Talismans dex/agi/sta - 147696/147697/147698


  if (plugin::check_handin(\%itemcount, 147692 => 1) && plugin::check_handin(\%itemcount, 147693 => 1) && plugin::check_handin(\%itemcount, 147694 => 1) && plugin::check_handin(\%itemcount, 147695 => 1)){ 
    quest::say("Now lets see if this works");
    quest::ding();
    quest::summonitem(71363);
    $client->AddLevelBasedExp(90, 50); ## 25% exp, upto level 40
return;
  }



  if (plugin::check_handin(\%itemcount, 71363 => 1) && plugin::check_handin(\%itemcount, 147696 => 1) && plugin::check_handin(\%itemcount, 147697 => 1) && plugin::check_handin(\%itemcount, 147698 => 1)){
    quest::say("Now lets see if this works..you");
    quest::ding();
    quest::summonitem(147691);
    $client->AddLevelBasedExp(99, 60); ## 25% exp, upto level 40
return;
  }



quest::say("Please, don't tell Devnoob or Image about me, I would hate to have to go on the run, and stop selling you my special wares.");
}
