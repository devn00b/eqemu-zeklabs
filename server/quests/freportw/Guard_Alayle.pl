############################################
# ZONE: West Freeport (freportw)
# DATABASE: PEQ-Velious
# LAST EDIT DATE: April 5,2005
# VERSION: 2.0
# DEVELOPER: MWMDRAGON
#
# *** NPC INFORMATION ***
#
# NAME: Guard_Alayle
# ID: 9135
# TYPE: Warrior
# RACE: Freeport guard
# LEVEL: 10
#
# *** ITEMS GIVEN OR TAKEN ***
#
# Sealed Letter ID-18817
# A Tattered Flier ID-18818
#
# *** QUESTS INVOLVED IN ***
#
#1 - Zimel's Blades (Quest for Soulfire)
#
# *** QUESTS AVAILABLE TO ***
#
#1 - Paladin
#
############################################
sub EVENT_SAY { 
plugin::guardevent_say($npc,3,$client,$text);
}

sub EVENT_SPAWN
{
	plugin::setupguard_enter(3,$npc);
}


sub EVENT_ENTER {
plugin::enterguard_area(3,$client,$npc);

} #end of sub event_enter

sub EVENT_DEATH
{
	quest::resetguardfields(3,$npc->GetNPCTypeID(), $userid);

} #if charid and pvpflag

sub EVENT_ITEM
{
  if($itemcount{18817} == 1 && $class != 'Paladin')
 {
 quest::say("You have no business handing me this junk.");
 plugin::return_items(\%itemcount);
 return;
 }
 else
     {
  # Sealed Letter ID-18817
  if($itemcount{18817} == 1)
  {
  	quest::say("This is not good news. I must leave immediately. Here. Take this to Kala.. I mean my father. I found it on the floor of Sir Lucan DLeres quarters. Thanks again, messenger. I got this just in time.");

  	quest::ding();
  	# A Tattered Flier ID-18818
  	quest::summonitem("18818");
  
    # Freeport Militia Faction
    quest::faction(105, -1);
    # Corrupt Guards of Qeynos Faction
    quest::faction(53, -1);
    # Steel Warriors Factions
    quest::faction(311,1);
    # Knights of Truth Faction
    quest::faction(184,1);
    # Give Guards of Qeynos Faction
    quest::faction(135, 1);
  }
}
}

# After this Krazen_Loosh is supposed to spawn but he does not exsist in the DB.

#END of FILE Zone:freportw  ID:9135 -- Guard_Alayle
