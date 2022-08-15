sub EVENT_SAY{
 if ($text=~/hail/i){
quest::say("Hail, $name. I am Deputy Budo Bullnose, the squad leader of the Great Wall Patrol. Be very careful in the lands beyond the wall. They are filled with dangerous beasts, as well as [Clan Runnyeye].  Would you like a guard [status] update?");
}
 elsif ($text=~/clan Runnyeye/i){
quest::say("Beyond this wall lies Runnyeye, the lair of the goblins of Clan Runnyeye. They have been a nuisance to Rivervale for quite some time. If you wish to join the fight, go to Rivervale and ask Sheriff Roglio about Clan Runnyeye.");
}
 elsif ($text=~/I am reporting for duty/i){
quest::say("Stand tall then, Deputy $name. Your orders are to keep the creatures from venturing into the community of Rivervale and to kill many goblins. Check the goblin bodies, if you should find unbroken warrior beads on them, take them to Sheriff Roglio Bruth and should you be lucky enough to find a bizarre grappling device, you are to take it to Marshal Ghobber Truppack immediately! One more thing - should you defeat any orcs in battle, bring me their leather belts and I shall reward you for the great victory. May the power of Underfoot be with you!");
}
elsif($text=~/nice hat/i){
		quest::say("Thank you, It is worn with pride and is the symbol of the Rivervale Militia");
	}
else
{
plugin::guardevent_say($npc,4,$client,$text);
}
}

sub EVENT_POPUPRESPONSE
{
plugin::bankpopup_response(4,$client,$popupid);
} #close event response

sub EVENT_SPAWN
{
	plugin::setupguard_enter(4,$npc);
}


sub EVENT_ENTER {
plugin::enterguard_area(4,$client,$npc);

} #end of sub event_enter

sub EVENT_DEATH
{
quest::resetguardfields(4,$npc->GetNPCTypeID(),$userid);

} #if charid and pvpflag

sub EVENT_ITEM { 
if(plugin::check_handin(\%itemcount, 13916 => 1)){
quest::say("Grand work. Deputy $name! You will become a full-fledged deputy in no time. You should check in with Marshal Ghobber from time to time to see if you have earned your squad ring. Just go up to him and ask. 'Am I one with the Wall?' He will know. but you must first show your bravery in many battles against the goblins.");
quest::givecash("0","0","9","0");
quest::summonitem(13024);
quest::faction(133, 4);
quest::faction(208, 4);
quest::faction(316, 4);
quest::faction(218, 4);
quest::faction(88, -10);
quest::exp(100);
 }
plugin::return_items(\%itemcount);
}
#END of FILE Zone:misty  ID:33081 -- Deputy_Budo 