sub EVENT_SIGNAL {
if ( $client->GetGM() )
{
return;
}
if ( $client->GetLevel() > 29 )
{
$client->Message(4,"Warning! You will be forced to leave the heavens soon!");
$flag = $qglobals{skyflag};
if ( (!defined($flag) || $flag != 1)  )
{
quest::setglobal("skydisableflag","1",5,"M30");
$client->Message(4,"Your time in the heavens has ran out.  You have been forced to leave!");
if ( $client->GetPVPFlag() == 1 ) { $client->MovePC(2,176.2,293.6,47.7,75.2); }
elsif ( $client->GetPVPFlag() == 3 ) { $client->MovePC(9,-52.1,-108,-10.2,130.9); }
elsif ( $client->GetPVPFlag() == 2 ) { $client->MovePC(29,-290.6,116.2,3.8,127.1); }
elsif ( $client->GetPVPFlag() == 4 ) { $client->MovePC(19,-172.2,52.7,-.3,213); }
elsif ( $client->GetPVPFlag() == 5 ) { quest::selfcast(36); }
}
}

}

sub EVENT_ENTERZONE {
if ( $client->GetGM() || $client->GetLevel() < 30 )
{
return;
}

$flag = $qglobals{skyflag};
if ( !defined($flag) || $flag != 1 )
{
$client->Message(4,"You can remain in the heavens only for a short period of time.  You will be forced home in 10 minutes.");
quest::setglobal("skyflag","1",5,"M10");
}

}