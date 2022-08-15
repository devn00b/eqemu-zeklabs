#Lieutenant_Dagarok.pl
#The Crate (evil)
	sub EVENT_SAY { 
	$var = plugin::guardevent_say($npc,1,$client,$text);
	if ( $var == 0 )
	{
	quest::say("How dare you disturb me! You people need to just mind your pathetic lives and leave the rest to us. Leave me now!");
	}
	}

	sub EVENT_POPUPRESPONSE
	{
	plugin::bankpopup_response(1,$client,$popupid);
	} #close event response

	sub EVENT_SPAWN
	{
		plugin::setupguard_enter(1,$npc);
	}


	sub EVENT_ENTER {
	plugin::enterguard_area(1,$client,$npc);

	} #end of sub event_enter

	sub EVENT_DEATH
	{
	quest::resetguardfields(1,$npc->GetNPCTypeID(), $userid);

	} #if charid and pvpflag

sub EVENT_ITEM {
	if (plugin::check_handin(\%itemcount, 18824 => 1)) { #Slip of Parchment
		quest::say("What is this? Hmmm. It seems Weleth has a problem with following orders. I will deal with him. Good work in bringing this to my attention. Our job here would be much more difficult without the help of loyal citizens like yourself. Take this as a token of Commander Kane's appreciation.");
		quest::faction(53, 10);#Corrupt Qeynos Guards
		quest::faction(135, -20);#Guards of Qeynos
		quest::faction(164, 10);#Kane Bayle
		quest::faction(33, 10);#Circle Of Unseen Hands
		quest::faction(21, 10);#Bloodsabers
		quest::exp(32000);
		my $cp = int(rand(16));
		my $sp = int(rand(8));
		quest::givecash($cp, $sp, 0, 0);
	}
	
	else {
		plugin::return_items(\%itemcount);
	}
}

#End of File zone:qeynos2 ID: 2020 -- Lieutenant_Dagarok.pl