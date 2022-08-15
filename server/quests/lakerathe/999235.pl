##Translocator_Fithop.pl
#Zone: butcher

sub EVENT_SAY{
  if ($text=~/Hail/i){
  quest::say("Hello there, $name. We have scouted a new land, would you like to [visit] there?");
  }
  if($text=~/visit/i)
  {
  quest::say("Be Safe!");   
##  quest::selfcast(2279);
quest::movepc(175, -1632, -1070, -99.4, 50);
  }
}
