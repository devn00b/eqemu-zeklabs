sub EVENT_SAY
{
	if ( $client->GetLevel() < 48 )
	{
		$client->SetLevel(50);
		$client->AddMoneyToPP(0,0,0,1500,true);
		$client->IncreaseAllSkills(200);
		quest::scribespells(49);
		quest::selfcast(1776);
		quest::selfcast(159);
		
	}
	else
	{
		quest::say("You need to be below level 48.");
	}
}
