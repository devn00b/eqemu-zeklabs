sub EVENT_SAY {
  if($text=~/hail/i) {
    quest::say("Meat!! You need meat. Me gots meat!! High elf meat, dwarf meat, human meat, [HEHE meat], [froggy legs]. Me got a lot.");
  }
  elsif($text=~/HEHE meat/i) {
    quest::say("HEHE meat? It human, Erudite 'n high elf, mashed together. Mmmm. Very tasty!");
  }
  elsif($text=~/froggy legs/i) {
    quest::say("Froggy legs come from far away. Me get from friend, [Chef Dooga]. It not tasty. Trolls like it. Eat it up much. They eat too much froggy. Me glad [Nerbilik] leave Neriak!! He eat way too much froggy.");
  }
  elsif($text=~/Chef Dooga/i) {
    quest::say("Friend in Oggok is Chef Dooga. Chef she is. Used to work in Neriak. She beautiful. She supply the Gobbler with [froglok legs]. They very low now. Fat trolls eat too many!! Me need to [pick up froglok legs].");
  }
  elsif($text=~/pick up froglok legs/i) {
    quest::say("That good news. Oggok far away. Me got no time to go. If you go speak to Chef Dooga, say you pick up froglok legs. You bring back and me give you coin, maybe even something good.");
  }
  elsif($text=~/Nerbilik/i) {
    quest::say("Big and fat troll Nerbilik is. He leave to fish in tearsy ocean. Me promised to deliver him food or he come bak. The Gobbler needs some dumb person to [deliver grub locker] to Nerbilik.");
  }
  elsif($text=~/deliver grub locker/i) {
    quest::say("Good. Me wrap order of dwarf pickles in.. HEY!! Me out of special wrapping paper!! Hmm.. Here. Me use dis. Now, <..click.. Locks grub locker> Here. You take to Nerbilik in tearsy ocean and he pay you. No think of opening locker. Only Nerbilik has key.");
    quest::summonitem(12203);
  }

}

sub EVENT_ITEM {
  if(plugin::check_handin(\%itemcount, 13384 => 1)) {
    quest::say("Uggh!! Froglok legs!! Me hate smelly legs, but me need it for fat trolls.");
    quest::summonitem(13386);
    quest::givecash(0,0,3,0);
  }
  else {
    quest::say("Me not want dis.");
    plugin::return_items(\%itemcount);
  }
} 