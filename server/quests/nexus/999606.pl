sub EVENT_SAY
{
	if ( $client->GetLevel() < 20 )
	{
		quest::level(20);
		quest::ding();
		$client->AddMoneyToPP(0,0,0,1500,true);
		$client->IncreaseAllSkills(1);
		$client->AddPVPPoints(30000);
		quest::scribespells(20);
		quest::selfcast(1776);
		quest::selfcast(159);
	}
	else
	{
		if ( $client->GetPVPPoints() > 999 )
		{
		quest::say("No more pvp points for you!");
		}
		else
		{
		$client->AddPVPPoints(1000);
		quest::say("Added 1000 pvp points.");

		}
	}
}
