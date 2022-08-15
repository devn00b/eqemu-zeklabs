sub EVENT_SAY { 
plugin::guardevent_say($npc,6,$client,$text);
}

sub EVENT_DEATH
{
#since the npc died we have to reset the values

quest::resetguardfields(6,$npc->GetNPCTypeID(), $userid);
	
my $guildID = quest::getguardfield(6,0, "guildid");
#check to see if its a player, then make sure they aren't a neutral (0) player or the same team
if ( $guildID > 0 && $client->GuildID() != $guildID ) { 
quest::we(13,"The Thurgadin Kingdom's Morale Officer has been killed!  The Thurgadin Kingdom loses their experience bonus temporarily!");
quest::setteamexpbonus(6,0); #reset qeynos team xp bonus
}
}
sub EVENT_ENTER
{
plugin::enterguard_area(6,$client,$npc);
}

sub EVENT_SPAWN
{
	quest::setteamexpbonus(6,5);
	plugin::setupguard_enter(6,$npc);
}