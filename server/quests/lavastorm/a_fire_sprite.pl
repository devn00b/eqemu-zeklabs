#########################################
# ZONE: Lavastorm Mountains (lavastorm)
# DATABASE: PEQ
# DEVELOPER: ATTEMPT37
#
# **NPC INFORMATION**
#
# NAME: a_Fire_Sprite
# ID: none
# Body type: Fire Imp
#
# **QUEST INFORMATION**
#
# Celestial Fists (Monk Epic 1.0)
#
#
#
#
#########################################

sub EVENT_SAY {
  if($text=~/hail/i) {
    quest::emote("ignores you.");
  }

  elsif($text=~/i challenge eejag/i) {
    quest::spawn(27119,0,0,197,1012,-198);
#     quest::uniquespawn(27119,0,0,197,1012,-198.4,0);	
    quest::depop();
  }
}
# End of File
