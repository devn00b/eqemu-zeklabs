sub EVENT_SAY { 
if($text=~/Hail/i){
quest::say("You $name.  We hear of you.  We need help.  You [help Crakneks] or you [help self]?");
}
if($text=~/elp crakneks/i){
quest::say("Ha!!  We hear of great adventurer.  You?!!  Me no think so.  You prove self to Crakneks before you help us.  Go to Innoth.. Innotu.. Innooth.. Arghh!!  You go to outside Oggok.  Find fat alligator bit Lork brother in two.  Bring brother. Nork. body back.  Then me know you strong.");
}
if($text=~/help self/i) {
 quest::say("You help self to leave Oggok before me bash you.  We no need cowards.");
}
}

sub EVENT_ITEM {
 if($itemcount{20523} == 1) {
   quest::say("Ay danks. Take dis to Uglan.");
   quest::summonitem(16547);
  } elsif($itemcount{18840} == 1) {
   quest::say("What this!! So, dark elves think they can bash ogres. Replace with blue orcs. Dumb Zulort friend with dark elf ambassador in Oggok. We kill him. We kill Crushbone dark elf ambassador also. This slow down plan. We need a hero. Me guess you do. You go. Go bring Lork both Crushbone and Oggok dark elf hearts. Then you be hero.");
   quest::summonitem(quest::ChooseRandom(5032,5036,5030,5027,5028,5029,5026,5035));
  } elsif($itemcount{13227} == 1 && $itemcount{13358} == 1) {
   quest::say("That show dark elves who strongest. Me hope you kill many blue orcs. You Craknek Hero now. You take this. It mine. Hero reward. You great ogre now. Smash best.");
   quest::summonitem(quest::ChooseRandom(13355,13359,2104));
  } elsif($itemcount{13227} == 1 || $itemcount{13358} == 1) {
   quest::say("This one dead. Where other? Lork say Crushbone heart and Oggok heart.");
    plugin::return_items(\%itemcount);
   }
  else {
    #do all other handins first with plugin, then let it do disciplines
    plugin::try_tome_handins(\%itemcount, $class, 'Warrior');
    plugin::return_items(\%itemcount);
 }
}
#END of FILE Zone:oggok  ID:49040 -- Lork 

