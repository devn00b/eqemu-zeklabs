sub EVENT_SAY {
## Item im linking in this case reefmans trans bs
my $ridold = 27212; #itemid for old bufas
my $ridnew = 142676; #itemid for new sword
my $bufalnk = quest::varlink($ridold); #link for old sword
my $bufanew = quest::varlink($ridnew); #link for new sword
#########3 End Item Linking Stuff

 if($text =~ /Hail/i) {
  quest::say("I'm not interested in speaking to someone as unworthy as you. Perhaps Colgon Dursaar can make use of you.");
  quest::say("");
  quest::say("Unless of course he sent you? If so, give me the proof that he did.");
  quest::say("Now go away!");
  quest::doanim(30);
  }

}

sub EVENT_ITEM {
#####
# Colgon (mt rathe):
# Rd 1, Hammered Golden Hoop (5716,solb) / Diamond Stud of Deception (62348,paw) / Da ooly Stick (31861,bazzar) / Images ear (135039) -> Gives (135101-Signet of Worth- for old man)
# an old man (kerra):
# Rd 2, Tempitem (135101) // (31371 Silver Chains,Fear) Terror // (11648 DawnFire, Morning Star of Light,fear) Dread // (5728 Di'zok Signet of Service,hate) Magi_P`Tasa
#####


## Item im linking in this case reefmans trans bs
my $ridold = 27212; #itemid for old bufas
my $ridnew = 142676; #itemid for new talisman
my $bufalnk = quest::varlink($ridold); #link for old sword
my $bufanew = quest::varlink($ridnew); #link for new sword
my $takethis = 135102; # Signet of Worth
my $tookthis = quest::varlink($takethis); # Signet of Worth
my $signet = quest::varlink(135105);
#########3 End Item Linking Stuff


if (plugin::check_handin(\%itemcount, 135101 => 1)){
    quest::say("");
    quest::say("Well it seems you aren't totally useless $uname . It seems Colgon at least thinks you are worthy. I wonder.....");
	quest::say("");
	quest::say("Now lets test your worth. First, take this, DO NOT LOSE IT. I want it back once you are done. Now on to your trial. I require you to bring me the following items: Silver Chains, DawnFire, Di'zok Signet of Service, and the item I just gave you ( $tookthis )");
    quest::say("");
    quest::ding();
    quest::summonitem(135102); #generic item for part 2
  }

if (plugin::check_handin(\%itemcount, 135102 => 1) && plugin::check_handin(\%itemcount, 31371 => 1) && plugin::check_handin(\%itemcount, 11648 => 1) && plugin::check_handin(\%itemcount, 5728 => 1)){ #### Rd 1. See above.

	quest::say("");
    quest::say("Aren't you just an interesting little mongrel. Here Take this Charm ( $signet ). It is nothing more than a trinket but you might find it useful.");
	quest::say("");
	quest::say("Also if you want me to upgrade your $bufalnk to $bufanew I guess I can do that. Just bring me back my charm and $bufalnk and I shall reward you with my table scraps.");
	quest::say("");
	quest::ding();
    quest::summonitem(135105); #Decent Charm Slot for ALL classes.
  }

if (plugin::check_handin(\%itemcount, 135105 => 1) && plugin::check_handin(\%itemcount, 27212 => 1)){
    quest::say("Hrmph. Here take $bufanew and your $signet and be gone with you!");
    quest::ding();
    quest::summonitem(135105); # GIVE THEM BACK THE CHARM!
    quest::summonitem(142676); # THIS IS THE REWARD FINAL RE ENABLE
    $client->AddLevelBasedExp(99,60); ## 150% of level @lvl 60
  }
### Shouldn't really ever get here.
#quest::say("Please, don't tell anyone about me....Thanks!");
}