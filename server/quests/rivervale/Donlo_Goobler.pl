sub EVENT_SAY { 
plugin::bankerevent_say($npc,4,$client,$text);
}

sub EVENT_POPUPRESPONSE
{
plugin::bankpopup_response(4,$client,$popupid);
} #close event response