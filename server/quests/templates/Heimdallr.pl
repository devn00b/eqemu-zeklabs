sub EVENT_SPAWN {
$npc->SetSlotTint(1, 153,0,0); #1 for chest
$npc->SetSlotTint(2, 204,0,0); #1 for arms
$npc->SetSlotTint(3, 255,255, 26); #1 for bracers
$npc->SetSlotTint(4, 204,0, 0); #1 for hands
$npc->SetSlotTint(5, 255,255,26); #1 for legs
$npc->SetSlotTint(6, 204,0,0); #1 for boots
$npc->SetSlotTint(0, 255,255, 26); #1 for helm
}

sub EVENT_SAY {
$status = $qglobals{disabledlevelupdates};
$statusTxt = "enabled";
if (defined $status)
{
$statusTxt = "disabled";
}

if ($text=~/hail/i)
{
$change = quest::saylink("change auto scribe");
$scribe = quest::saylink("manual scribe");
quest::say("Hello $name!  You currently have auto scribe updates $statusTxt, would you like to $change?  Would you otherwise like to do a $scribe of spells from level 1 to $ulevel?");
}
elsif ($text=~/manual scribe/i)
{
quest::popup("MANUALLY SCRIBE","You have requested to manually scribe spells from level 1 to $ulevel, ARE YOU SURE?",98765,60);
}
elsif ($text=~/change auto scribe/i)
{
if ( defined $status )
{
quest::popup("Enable Auto Scribe and Skills","You have requested to enable auto scribe, upon levelling spells will auto-scribe and abilities(skills) will auto enable, are you sure you wish to do this?",54321,60);
}
else
{
quest::popup("Enable Auto Scribe and Skills","You have requested to disable auto scribe, you will be required to MANUALLY retrieve your spells at a merchant and your skills at a guildmaster, are you sure you wish to do this?",12345,60);
}
}#end auto scribe block
}

sub EVENT_POPUPRESPONSE
{
if ( $popupid eq "12345" )
{
$client->Message(13,"Auto scribe and auto skill ups DISABLED.");
quest::setglobal("disabledlevelupdates",1,5,"F");
}
elsif ( $popupid eq "54321" )
{
$client->Message(13,"Auto scribe and auto skill ups ENABLED.");
quest::delglobal("disabledlevelupdates");
}
elsif ( $popupid eq "98765" )
{
$prevSet = 0;
$status = $qglobals{disabledlevelupdates};
if ( defined $status )
{
$prevSet = 1;
quest::setglobal("disabledlevelupdates",1,5,"S0");
}
$client->Message(13,"Beginning an auto scribe from level 1 to $ulevel.");
$client->LoadCharacterLevelUpdates(1);

if ( $prevSet == 1 )
{
quest::setglobal("disabledlevelupdates",1,5,"F");
}
}

}