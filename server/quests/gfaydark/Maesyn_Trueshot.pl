#BeginFile: gfaydark\Maesyn_Trueshot.pl (54084)
#Quest file for Greater Faydark - Maesyn Trueshot: Trueshot Longbow, Rain Caller and Guild Summons

sub EVENT_SAY {
  if($text=~/hail/i) {
    quest::say("Welcome to Kelethin, $name! I am Maesyn Trueshot, commander of Faydark's Champions. We are the finest marksmen in all of Norrath. With our trusty [Trueshot longbows] we can miss no target regardless of the distance or the conditions.");
  }
  if($text=~/trueshot longbows/i) {
    quest::say("The Trueshot Longbow was created by my famed father. Eldin Trueshot. It is quite accurate and takes a ranger's skill to wield. We use our new recruits to [gather materials] needed by my father.  We shall soon begin to release the formula to good elves so all may fletch such a bow.");
  }
  if(($text=~/gather materials/i) && ($class eq "Ranger")) {
my $factioncheck = undef;
$factioncheck = $client->GetCharacterFactionLevel(99);

    if($factioncheck > 100) { #Needs better than indifferent
      quest::say("Take this pack. Go to Kaladim, find Trantor Everhot and ask for dwarven wire. Then go to Freeport to meet Jyle Windshot. Search the inns for him and ask him for treant wood. Then, collect some spiderling silk from spiderlings and finally, in Steamfont, we have the permission of the gnomes to use any micro servos we find while destroying rogue spiders. Combine them all and return the pack to me.");
      quest::summonitem(17951); #Material Pack
    }
    else {
      quest::say("Faydark's Champions cannot call you foe, but you have yet to earn our trust.");
    }
  }
  if($text=~/correct components/i) {
    quest::say("Now that I have crafted the Treant Bow Staff, you shall need one Planing Tool, one Treant Bow Staff, one Micro Servo and one spool of Dwarven Wire. These items will be used with your Fletching Kit as all other bows. Be forewarned, only a Master Fletcher can create such a bow and even a master fails from time to time. Good Luck.");
  }
  if($text=~/next incarnation/i) {
    quest::say("The Trueshot Longbow was once enchanted by the Koada'Dal enchanters.  Once it was enchanted now it is no more.  I am sure if you were ask the Koada'Dal [where the enchanted bows] are you will get an answer.");
  }
}

sub EVENT_ITEM {
  if(plugin::check_handin(\%itemcount, 12112 => 1)) { #Pack of Materials
    quest::say("I shall see that my father gets the materials. I hope this can be of use to you. It will serve as your starting point toward fletching a Trueshot longbow. It is unfortunate that we are unable to enchant the bow to its [next incarnation], but it is still a fine weapon. You do know the [correct components] needed for fletching such a bow, do you not?");
    quest::givecash(0,6,0,0); #6 gold
    quest::summonitem(8091); #Treant Bow Staff
    quest::faction(99,10); #Faydark's Champions
    quest::faction(178,10); #King Tearis Thex
    quest::faction(43,10); #Clerics of Tunare got better. 
    quest::faction(304,10); #Soldiers of Tunare got better. 
    quest::faction(63,-10); #Crushbone Orcs got worse. 
  }
  elsif (plugin::check_handin(\%itemcount, 18785 => 1)) { #A tattered note
    quest::say("Hail, $name, and welcome.. I am Maesyn Trueshot, leader of Faydark's Champions. I will teach and train you, as I have done for many others. Let's get started.. Here, put this on.. it'll help protect you from the elements. Once you are ready to begin your training please make sure that you see Samatansyn Flamecaller, he can assist you in developing your hunting and gathering skills. Return to me when you have become more experienced in our art, I will be able to further instruct you on how to progress through your early ranks, as well as in some of the various [trades] you will have available to you.");
    quest::faction(99,10);  # Faydark's Champions
    quest::faction(178,10); # King Tearis Thex
    quest::faction(43,10);  # Clerics of Tunare
    quest::faction(304,10); # Soldiers of Tunare
    quest::faction(63,-30); # Crushbone Orcs
    quest::exp(24); # 3/5 blue bubble at level 1
    quest::summonitem(13536); # Dirty Green Tunic* 
  }
  else {
    #do all other handins first with plugin, then let it do disciplines
    plugin::try_tome_handins(\%itemcount, $class, 'Ranger');
    plugin::return_items(\%itemcount);
  }
}
#END of FILE Zone:gfaydark  ID:54084 -- Maesyn_Trueshot
