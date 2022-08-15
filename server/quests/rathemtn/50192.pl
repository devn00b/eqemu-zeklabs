sub EVENT_SAY {
## Item im linking in this case reefmans trans bs
my $ridold = 27212; #itemid for old sword
my $ridnew = 142676; #itemid for new sword
my $bufalnk = quest::varlink($ridold); #link for old sword
my $bufanew = quest::varlink($ridnew); #link for new sword
#########3 End Item Linking Stuff

if($text =~ /Hail/i) {
	$codex = quest::saylink("lore");
	$upgradex = quest::saylink("upgrade");
	quest::say("Hello! $uname, You really should hear what I have to say about $bufalnk . I know how to $upgradex them. Or perhaps you would like to know more $codex ? I can help you with that as well.");
}

if($text=~/lore/i) {
	quest::say("");
	quest::say("There is said to be an old man, located in a far off land that has the power to turn $bufalnk into $bufanew! Amazing isn't it? I traded two Blade of Carnages for this little tidbit I'll share with you.");
	quest::say("");
	quest::say("Rumor has it that the old man was once Bufas lover, Nintal. It is said he took human form to hide from some dragon hunters, and that she was killed three seasons ago.");
	quest::say("");
	quest::say("He supposedly trades trinkets for information about Bufa, so if you take him $bufalnk he will drain whatever memories or energy, or whatever dragons do with it and return you a new item as a reward.");
	quest::say("");
	quest::say("I was told he only will talk to the bravest of the brave, and for some, he requires a trial. I'm told that even those that survive are, well, changed forever.");
	quest::say("");
	quest::say("I've seen two friends come back myself. Let me say this, they weren't totally different people they were just somehow more powerful. They had a new confidence and seemed to care less for things that used to bother them.");
	quest::say("");
	quest::say("Anyway it is possible he will $upgradex it for you.");
}

if($text=~/upgrade/i) {
	quest::say("");
	quest::say("Well, you see I sort of lied a bit, I can't upgrade your item myself, however, I have directions to an old man that can. If you will do me a favor first. You see, I was asked to bring my client the following:");
	quest::say("");
	quest::say("1: a Hammered Golden Hoop.");
	quest::say("2: a Diamond Stud of Deception.");
	quest::say("3: Da Oogly Stick.");
	quest::say("4: Images Ear.");
	quest::say("");
	quest::say("Since I am stuck here, I task you with bringing me those items. The 1st two items I have never heard of, but number 3, I think I saw one of those in the bazaar. Number 4 has me at a loss as well. How do you get the ear of a god?");
	quest::say("");
	quest::say("If you can bring me those I'll tell you the directions to find him.");
	quest::say("");
	quest::say("...Please...");

}

}#end event say

sub EVENT_ITEM {
#####
# Colgon (mt rathe):
# Rd 1, Hammered Golden Hoop (5716,solb) / Diamond Stud of Deception (62348,paw) / Da ooly Stick (31861,bazzar) / Images ear (135039) -> Gives (135101-Signet of Worth- for old man)
# an old man (kerra):
# Rd 2, Tempitem (135101) // (31371 Silver Chains,Fear) Terror // (11648 DawnFire, Morning Star of Light,fear) Dread // (5728 Di'zok Signet of Service,hate) Magi_P`Tasa
#####


if (plugin::check_handin(\%itemcount, 5716 => 1) && plugin::check_handin(\%itemcount, 62348 => 1) && plugin::check_handin(\%itemcount, 31861 => 1) && plugin::check_handin(\%itemcount, 135039 => 1)){ #### Rd 1. See above. 
## Item linking Start
my $takethis = 135101; # Signet of Worth
my $tookthis = quest::varlink($takethis); # Signet of Worth
## Item Linking End

	quest::say("");
	quest::say("Magnificent $uname !! Take this ( $tookthis ) to the old man, it marks you as a friend.");
	quest::say("The last time I saw him he was in Befallen. If you Hurry you can find him.");
    	quest::say("");
	quest::say("And please let him know I'm ok. He tends to get worried easily.");
        quest::ding();
        quest::summonitem(135101); #Signet of Worth.
        $client->AddLevelBasedExp(99,60); 
}
##Should never really need anything here since we'll just eat shit...tooo fucking bad.
}#end event item
