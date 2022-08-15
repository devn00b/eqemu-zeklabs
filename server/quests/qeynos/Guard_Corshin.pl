sub EVENT_SAY { 
$var = plugin::guardevent_say($npc,1,$client,$text);
if ( $var == 0 )
{
if($text=~/Hail/i){
quest::say("Hail, $name!  Guard Corshin Kiniss, at your service. You should be careful in Qeynos after dark. We have had several [bodies] wash up under the docks over the past few weeks. We wouldn't want anything to happen to you.");
}
elsif($text=~/what bodies/i){
quest::say("We think the bodies are washing out of the aqueducts beneath Qeynos. It is a dangerous place to go and Commander Bayle refuses to send regular patrols down there.");
}
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