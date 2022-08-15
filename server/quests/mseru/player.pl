sub EVENT_ENTERZONE {
    plugin::EnterZoneHandler();

#if its a GM don't mess with them
if ( $client->GetGM() == 0 )
{
#qeynos start point
if ( $client->GetPVPFlag() == 1 ) {
$client->MovePC(168,247.9,667.5,-20.2,83.5); }

#halas start point
if ( $client->GetPVPFlag() == 2 ) {
$client->MovePC(168,1443.6,237.8,-21.0,200.4); }

#freeport start point
if ( $client->GetPVPFlag() == 3 ) {
$client->MovePC(168,1090.3,-1008.6,-22.6,161.0); }

#rivervale start point
if ( $client->GetPVPFlag() == 4 ) {
$client->MovePC(168,-1561.8,1333.8,-40.4,129.5); }

#bandits start point
if ( $client->GetPVPFlag() == 5 ) {
$client->MovePC(168,-1027.5,-2138.9,-27.7,253.4); }
}
}

sub EVENT_LEVEL_UP {
    plugin::PlayerLevelUpHandler();
}

sub EVENT_DEATH {
    plugin::PlayerDeathHandler();
}

sub EVENT_POPUPRESPONSE {
    plugin::PlayerPopupResponseHandler($popupid);
}

sub EVENT_TIMER {
    plugin::PlayerTimerHandler($timer);
}
