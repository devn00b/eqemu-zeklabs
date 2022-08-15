sub EVENT_SAY
{
$interestedLink = quest::saylink("[UPGRADE]");
$upgraded = quest::varlink(142750); 
$unupgraded = quest::varlink(11642);
###
#2065	Sanctum Guardian's Earring
#11642  lute
#5222	Breastplate of Afflation 
#20542  Singing Short Sword
###
$itemone = quest::varlink(2065); # Sanctum Guard Earring (Paw)
$itemtwo = quest::varlink(5222); #BP of afflation (Dragons)
$itemthree = quest::varlink(20542); # singing short sword (PVP)

if ($text=~/hail/i) {
quest::say("");
quest::say("Greetings $name, its nice of you to stop by. I have been waiting a long time for someone like you to stop by.");
quest::say("");
quest::say("Would you like to hear about how you can take $unupgraded and $interestedLink it to $upgraded. But I warn you");
quest::say("To go down this path, means pain and suffering. You seem to be the sort of person that can handle that sort of thing.");

}

if ($text=~/UPGRADE/i) {
quest::say("");
quest::say("Excellent! In order to turn the regular old $unupgraded into the better version I will require a few things. They are :");
quest::say("1: $unupgraded");
quest::say("2: $itemone");
quest::say("3: $itemtwo");
quest::say("4: $itemthree");
quest::say("");
quest::say("If you bring me those items I'll give you $upgraded and $itemthree AND a special item that I think you will like!");
}

}

sub EVENT_ITEM {

if (plugin::check_handin(\%itemcount, 11642 => 1) && plugin::check_handin(\%itemcount, 2065 => 1)&& plugin::check_handin(\%itemcount, 5222 => 1)&& plugin::check_handin(\%itemcount, 20542 => 1)){ 
$fibrus = quest::varlink(64193);
$upg = quest::varlink(142750);

    quest::say("Magnificent! Magnificent indeed! Here you go as promised! Also please enjoy these 10 $fibrus .");
    quest::ding();
    quest::summonitem(142750);
    quest::summonitem(64193,10);
    quest::summonitem(20542);
  }

}
