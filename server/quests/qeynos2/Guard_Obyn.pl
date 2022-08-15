	sub EVENT_SAY { 
	$var = plugin::guardevent_say($npc,1,$client,$text);
	if ( $var == 0 )
	{
if($text=~/Hail/i){
quest::say("Hello there. $name.  Obyn is my name.  Are you from Qeynos?  I was born here myself.  It's the best city in all of Norrath. if you ask me.  Well I'd better get back to guarding."); }
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
	quest::resetguardfields(1,$npc->GetNPCTypeID(),$userid);

	} #if charid and pvpflag


#END of FILE Zone:qeynos2  ID:2334 -- Guard_Obyn 

