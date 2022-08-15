sub EVENT_SAY {
my $tstaff = quest::varlink(142736); #link for new sword
my $wstrike = quest::varlink(142737); #link for new sword

my $danme = $npc->GetCleanName();
#$mname;
######### End Item Linking Stuff

######## BACK STORY
#Marquis of the Lost Dragons
#I began my journey as a ruthless raider. I made my way to the Lost Dragons after many tribulations, one of which involved an ethical princess and an enchanted egg.

 if($text =~ /Hail/i) {
	quest::say("");
	quest::say("Hello ".$name.", I am ".$danme." Marquis of the Lost Dragons. Well met!");
	quest::doanim(67);
	quest::say("");
	quest::say("I began my life as a ruthless raider. A poor one at that.");
	quest::say("I eventually made my way to the Lost Dragons and after many tribulations, one of which involved an ethical princess and an enchanted egg, I became THE Marquis of the Lost Dragons");
	quest::say("");  
	quest::say("Now I come here to sell Items my people have collected over the years ( ".$tstaff." / ".$wstrike."). Alas no one here wants to buy in bulk so it seems I will be here a while. Hahaha Diplomat and Leader Turned Common Merchant. Oh how the gods mock me!");
  }

}

sub EVENT_SPAWN {
	plugin::SetMobColor(255,255,255);
quest::ChooseRandom(0..255), quest::ChooseRandom(0..255), quest::ChooseRandom(0..255));
}
