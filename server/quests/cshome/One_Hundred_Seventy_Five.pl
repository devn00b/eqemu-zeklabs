##
# Handle Qcat - > Tox and back.
##

sub EVENT_SAY { 

if($text=~/hail/i) {
$interestedLink = quest::saylink("tale");
quest::doanim(15);
quest::say("Hello! Are you interested in hearing a $interestedLink?");
}

if($text=~/tale/i){
$interestedLink = quest::saylink("a torch");
quest::say("Long ago, I had a friend, he used to clean the Qeynos Catacombs of the muck. One day while cleaning there he found a firepot that was free of dirt and muck. Near it he found $interestedLink with writing on it. `To those that weild me goes the power of the pot`. He picked it up and slipped on the muck, while falling he touched his hand on the firepot and found himself in Toxxulia Forrest! ");
quest::say("Oh what a suprise that was!! I...I mean my friend landed so hard he nearly broke his ankle!! Haha");
}
if($text=~/a torch/i){
$interestedLink = quest::saylink("favor");
quest::say("Ok, listen, I am tired of the lies. It wasn't really a friend it was me! I used to use the firepot to smuggle goods between Erudin and Qeynos!! But I'm now old, and have no heir's and have been looking for somone for years to take this burden from me. I just ask you for 1 $interestedLink and the torch is yours!");
}
if($text=~/favor/i){
	quest::say("Before I die I want to see a few things I've only read about in books. The Herbalists Spade, a Ring of the Ancients, and a Pegasus Feather Cloak!");
	quest::say("Prove to me you are worthy of my legacy!");
}
}


sub EVENT_ITEM
{
 if(plugin::check_handin(\%itemcount, 5818 => 1, 2463 => 1, 12268 => 1))
  {
   quest::say("Ahahaha! You did it! You really did it! You Really did it!!!");
   quest::givecash(0,0,0,5);
  }
plugin::return_items(\%itemcount);
}

#END of FILE Zone:oggok  ID:49046 -- Clurg 

