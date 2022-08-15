sub EVENT_SAY {
  if($text=~/hail/i) {
    quest::say("What you want?!! Face so ugly you scare fish away!! ..sniff, sniff. Smell so bad, too!! Me sure is hungry. Wish had [grub locker].");
  }
  elsif($text=~/grub locker/i) {
    quest::say("You gots grub locker!! Give to me. Me sick of fish and not many dwarves fall off ship. Me pay a plat to get me grub locker from the Gobbler in Neriak. Go to him and say you [deliver grub locker] if you wants one plat.");
  }
  elsif($text=~/Give me the wrapper/i) {
    quest::say("You no get my wrapper!! Me use it later when me needs to make more room in stomach!! You never get it!!");
  }
}

sub EVENT_COMBAT {
if ( $combat_state > 0 )
{
quest::say("I shall pluck you limb from limb!!");
}
}

sub EVENT_ITEM {
  if(plugin::check_handin(\%itemcount, 12203 => 1)) {
    quest::say("Uggh!! Froglok legs!! Me hate smelly legs, but me need it for fat trolls.");
    quest::summonitem(13386);
    quest::givecash(0,0,0,2);
  }
  else {
    quest::say("Me not want dis.");
    plugin::return_items(\%itemcount);
  }
} 