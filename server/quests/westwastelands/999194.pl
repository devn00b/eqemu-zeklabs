sub EVENT_SPAWN {
if ($moborigname=~/a_large fanged snake/i) { quest::npcsize(7); }
if ($moborigname=~/a_large rattle snake/i) { quest::npcsize(6); }

if ($moborigname=~/a_large razorback snake/i) { quest::settimer("createrazorloot",1); #razorback loot table
quest::npcsize(8);  quest::npctexture(3); }
if ($moborigname=~/a_long razorback snake/i) { quest::settimer("createrazorloot",1); #razorback loot table
quest::npcsize(8);  quest::npctexture(3); }

if ($moborigname=~/a_small fanged snake/i) { quest::npcsize(4); }
if ($moborigname=~/a_small rattle snake/i) { quest::npcsize(3); }

if ($moborigname=~/a_small razorback snake/i) { quest::settimer("createrazorloot",1); #razorback loot table
quest::npcsize(5); quest::npctexture(3); }

}

sub EVENT_TIMER {
if ( $timer eq "createrazorloot" ) { quest::stoptimer("createrazorloot"); $npc->AddLootGroup(8); }
}