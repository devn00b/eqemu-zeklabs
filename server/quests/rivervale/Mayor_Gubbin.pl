sub EVENT_SAY { 

	if($text=~/hail/i){
		my $ownerTeam = quest::getguardfield(4,0, "ownerteamid");
		if ( $ownerTeam > 0 && $ownerTeam == $client->GetPVPFlag() )
		{
		quest::say("Hello! Feel free to ask me about the guard [status] or I can otherwise [shrink] you!");
		}
		else
		{
		quest::say("Well hello...!");
		}
	}
	elsif($text=~/nice hat/i){
		quest::say("Thank you, It is worn with pride and is the symbol of the Rivervale Militia");
	}
	elsif($text=~/shrink/i){
		my $ownerTeam = quest::getguardfield(4,0, "ownerteamid");
		if ( $ownerTeam > 0 && $ownerTeam == $client->GetPVPFlag() )
		{
			quest::selfcast(13130);
		}
		else
		{
			quest::say("I cannot help you with that");
		}
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
  if (plugin::check_handin(\%itemcount, 9475 => 1)) {#rogue 1.5 handin
    quest::say("Ah, thank you. I was a bit parched.' Mayor Gubbin unstops the flask and takes a healthy swig of the brew. He makes a strange face and licks his lips while staring at the bottle. 'Wow, this is delicious. Like no other drink I've ever tried. This is even better than that Spiced Jumjum that Lanena is so fond of. Wait a moment . . . Is this the brew that Deeppockets keeps trying to hawk in my town? Well now, I suppose it's not so bad. Hmm. Why don't you tell him I've changed my mind. Tell Lendel he has my blessing and is welcome to bring in as much of this as he can sell.");
    quest::summonitem(13588);
  }
}
#END of FILE Zone:rivervale  ID:19104 -- Mayor_Gubbin 

