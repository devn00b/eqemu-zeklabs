sub EVENT_SAY {
## Item im linking in this case reefmans trans bs
my $ridold = 142156; #itemid for old sword
my $ridnew = 142583; #itemid for new sword
my $told = quest::varlink($ridold); #link for old sword
my $tnew = quest::varlink($ridnew); #link for new sword
#########3 End Item Linking Stuff

 if($text =~ /Hail/i) {
  $loore = quest::saylink("lore");
  $supercharge = quest::saylink("supercharge");
  quest::say("Hello! $uname, Are you interested in upgrading $told ($supercharge)? Or Perhaps you want to hear some $loore about it?");
  quest::doanim(15);
}

if($text=~/lore/i) {
quest::say("");
quest::say("$told , it was nothing more than a middle step between common broadswords, and the greatness that was $tnew . That was the final goal.");
quest::say("");
quest::say("Reefman once personally handed these items to his closest friends, so there are only a few in existence. The fact that you could afford one speaks volumes.");
quest::say("");
quest::say("With a little magic and the proper items they can be reverted to their former glory. But are you strong enough to $supercharge it. I have my doubts. But we shall see.");
quest::say("");

}

if($text=~/supercharge/i) {
quest::say("");
quest::say("All that I require is $told , Reefmans Necklace of Pure Awesomeness, a Flowing Black Silk Sash. I hear someone has the necklace for sale in The Bazaar, the rest is upto you.");
quest::say("");

}
}

sub EVENT_ITEM {

if (plugin::check_handin(\%itemcount, 142580 => 1) && plugin::check_handin(\%itemcount, 142156 => 1) && plugin::check_handin(\%itemcount, 1365 => 1)){ #### Reefmans Necklace of Pure Awesomeness, Reefmans broadsword, FBSS and get upgrade
    quest::say("");
    quest::say("Magnificent!! My path to world domination is almost complete!!...err I'm sorry. Here you go $tnew ! Now If you dont mind I have...Things to do");
    quest::say("");
    quest::ding();
    quest::summonitem(quest::ChooseRandom(7940,7941,7942,7943)); #Bracelets from Luclin (+15 to a stat, and resists, lore, notrade)
    quest::summonitem(142583); #Reefmans Transcendent Broadsword.
    $client->AddLevelBasedExp(95,51); ## 95% exp, upto level 51
  }

quest::say("Please, don't tell anyone about me....Thanks!");
}






