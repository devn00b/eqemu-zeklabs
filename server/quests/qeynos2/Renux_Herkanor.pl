sub EVENT_SAY { 
if($text=~/Hail/i){
quest::say("You need something. or are you just browsing?"); }
}

sub EVENT_ITEM {
  #do all other handins first with plugin, then let it do disciplines
  plugin::try_tome_handins(\%itemcount, $class, 'Rogue');
  plugin::return_items(\%itemcount);
}
#END of FILE Zone:qeynos2  ID:2020 -- Renux_Herkanor 
