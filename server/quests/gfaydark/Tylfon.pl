sub EVENT_SAY { 
if($text=~/Hail/i){
quest::say("So. you think you have what it takes to be a Scout of Tunare? Come back with 2 [gold] and 2 [rusty dagger]s and I'll make it worth your while."); }
}

sub EVENT_ITEM { 
  if(($gold >= 2) && (plugin::check_handin(\%itemcount, 7007 => 1))){
	quest::say("Well, well. I didn't think you could do it. Here's your cut and don't be surprised that it's not much because it's your first lesson. Remember. the smaller the operation. the bigger the share, and the richest rogues have the tightest lips.");
	quest::summonitem("7021");
	quest::givecash("0","5","0","0");
        quest::faction(283,5);
        quest::exp(100);
  }
  #do all other handins first with plugin, then let it do disciplines
  plugin::try_tome_handins(\%itemcount, $class, 'Rogue');
  plugin::return_items(\%itemcount);
}

#END of FILE Zone:gfaydark  ID:54087 -- Tylfon 

