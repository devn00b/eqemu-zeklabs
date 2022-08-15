sub EVENT_SPAWN {
  quest::shout("ZERZURA!");
}

sub EVENT_SLAY {
  quest::shout("Begone insect, I have much slaying yet to do!");
}

sub EVENT_NPC_SLAY {
  quest::shout("Begone insect, I have much slaying yet to do!");
}

sub EVENT_DEATH { #nearly forgot about this hehe
  quest::setglobal("kerafyrm",3,7,"F"); 
  quest::stoptimer("depop");   
  quest::depop();
}