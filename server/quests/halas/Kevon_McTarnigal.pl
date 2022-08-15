sub EVENT_SAY { 
plugin::bankerevent_say($npc,2,$client,$text);
}

sub EVENT_POPUPRESPONSE
{
plugin::bankpopup_response(2,$client,$popupid);
} #close event response