# Romar Sunto
# Location: soltemple - Temple of Sol Ro
# npcID: 80026
# spawngroupID: 80030
# Quests:
#    Part of quest for Darkwood Staff by Mystic414
#    Part of quest for Spell: Tashania   by darkonig

sub EVENT_SAY {
  if($text=~/hail/i) {
   quest::say("Hello $name!  Provide me with 2 platinum or 20 gold pieces and I shall provide you an item to cancel buffs on your enemies.");
  }
}

sub EVENT_ITEM {
 if($gold >= 20 || $platinum >= 2) {
    quest::summonitem(14525);
    }
else
{
plugin::return_items(\%itemcount);
}
}
