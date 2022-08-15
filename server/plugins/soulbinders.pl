#!/usr/bin/perl

sub soulbinder_say {
	my $text = shift;
	if($text=~/hail/i){
		quest::say("Greetings $name. When a hero of our world is slain  their soul returns to the place it was last bound and the body is reincarnated. As a member of the Order of Eternity  it is my duty to [bind your soul] to this location if that is your wish.");
	} elsif($text=~/bind my soul/i) {
	    quest::say("Binding your soul.  You will return here when you die.");
	    quest::selfcast(2049);
	}
} 

sub nexusbuff_enter {
	my $client = shift;
	$client->AddPVPPoints(1);
	quest::setglobal("nexusprobationperiod","1",5,"M5");
	quest::setglobal("nexusleavecooldown","1",5,"M30");
	$client->Message(15,"***You have been awarded a PVP point for leaving Nexus during the incentive period.");
	if ( $client->GetLevel() < 30 )
	{
		$hasManaHPRegen = $client->FindBuff(2176); #spiritual light
		if ( $hasManaHPRegen == 0 )
		{
			quest::selfcast(2176);
			sleep(1);
		}

		$hasSoW = $client->FindBuff(1776); #spirit of wolf
		if ( $hasSoW == 0 )
		{
			quest::selfcast(1776);
			sleep(1);
		}
	}
	elsif ( $client->GetLevel() < 40 )
	{
		$hasManaHPRegen = $client->FindBuff(2176); #spiritual light
		if ( $hasManaHPRegen == 0 )
		{
			$client->CastSpell(2176,$userid,10,0,0);
			sleep(1);
		}

		$hasSoW = $client->FindBuff(1776); #spirit of wolf
		if ( $hasSoW == 0 )
		{
			quest::selfcast(1776);
			sleep(1);
		}
	}
	elsif ( $client->GetLevel() < 50 )
	{
		$hasManaHPRegen = $client->FindBuff(2177); #spiritual blessing
		if ( $hasManaHPRegen == 0 )
		{
			quest::selfcast(2177);
			sleep(1);
		}

		$hasSoW = $client->FindBuff(1776); #spirit of wolf
		if ( $hasSoW == 0 )
		{
			quest::selfcast(1776);
			sleep(1);
		}
	}
	elsif ( $client->GetLevel() < 60 )
	{
		$hasManaHPRegen = $client->FindBuff(2177); #spiritual blessing
		if ( $hasManaHPRegen == 0 )
		{
			quest::selfcast(2177);
			sleep(1);
		}

		$hasSoW = $client->FindBuff(1776); #spirit of wolf
		if ( $hasSoW == 0 )
		{
			quest::selfcast(1776);
			sleep(1);
		}
	}
	else
	{
		$hasManaHPRegen = $client->FindBuff(3460); #spiritual dominion
		if ( $hasManaHPRegen == 0 )
		{
			quest::selfcast(3460);
			sleep(1);
		}

		$hasSoW = $client->FindBuff(1776); #spirit of wolf
		if ( $hasSoW == 0 )
		{
			quest::selfcast(1776);
			sleep(1);
		}
	}
}