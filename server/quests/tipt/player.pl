
sub EVENT_ENTERZONE{
 if ( $client->GetPVPFlag() == 5 ) {
 #&& $client->Admin() < 1 ) {
 my $pwn = $client->GetPVPFlag();
 $client->message(13," **BETA THIS BUFF WILL CHANGE ** ");
plugin::EnterZoneBanditHandler();
}
}

