sub EVENT_SAY { 
plugin::guardevent_say($npc,3,$client,$text);
}

sub EVENT_SPAWN
{
	quest::setteamexpbonus(3,5); #reset qeynos team xp bonus
	plugin::setupguard_enter(3,$npc);
}


sub EVENT_ENTER {
plugin::enterguard_area(3,$client,$npc);

} #end of sub event_enter

sub EVENT_DEATH
{
#since the npc died we have to reset the values
	quest::resetguardfields(3,$npc->GetNPCTypeID(), $userid);

my $guildID = quest::getguardfield(3,0, "guildid");
#check to see if its a player, then make sure they aren't a neutral (0) player or the same team
if ( $guildID > 0 && $client->GuildID() != $guildID ) { 
quest::we(13,"The Freeport Kingdom's Morale Officer has been killed!  The Freeport Kingdom loses their experience bonus temporarily!");
quest::setteamexpbonus(3,0); #reset qeynos team xp bonus
}
}