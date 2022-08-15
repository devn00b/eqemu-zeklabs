sub EVENT_SAY {
quest::say("Disabled for this season");
return;
if($text=~/hail/i){
if (
 $class eq 'Warrior' ||
 $class eq 'Rogue' ||
 $class eq 'Monk' ||
 $class eq 'Berserker' ||
 $class eq 'Shadowkight' ||
 $class eq 'Paladin' ||
 $class eq 'Ranger' ||
 $class eq 'Bard' ||
 $class eq 'Beastlord' ||
 $class eq 'Cleric' ||
 $class eq 'Druid' ||
 $class eq 'Shaman' ) {
quest::say("Sorry $name we dont want to talk to scum like you. I'm only interested in talking to those who are of HIGH [INTELLIGENCE]");
return;
}
if ( $client->GetLevel() < 45 )
{
quest::say("Sorry, $name you must be level 45. What I need would be too much for you. Come back later.");
return;
}

quest::say("Well Well $name we have had our eye on you for quite some time. We need you to do something for us. Are you [interested]?");
 }

if($text=~/intelligence/i){
quest::say("Wow, you really are dumb aren't you? Here let me draw you a picture one of your low brain power can understand");
quest::say("I only want to speak to: Necromancers, Enchanters, Wizards, Magicians. You are a $class. Is $class in that list? No! So go away!");
}

if($text=~/interested/i){
if (
 $class eq 'Warrior' ||
 $class eq 'Rogue' ||
 $class eq 'Monk' ||
 $class eq 'Berserker' ||
 $class eq 'Shadowkight' ||
 $class eq 'Paladin' ||
 $class eq 'Ranger' ||
 $class eq 'Bard' ||
 $class eq 'Beastlord' ||
 $class eq 'Cleric' ||
 $class eq 'Druid' ||
 $class eq 'Shaman' ) {
quest::say("Sorry $name we dont want to talk to scum like you.");
return;
}
if ( $client->GetLevel() < 45 )
{
quest::summonitem(145460);
quest::say("Sorry, $name you must be level 45. Come back later.");
return;
}
$deveye = quest::varlink(145460);
quest::say("Fantastic $name ! Fan-Tas-Tic! I need to you to find and bring me a '$deveye'. The last time we saw one was in The Plane of Fear, and The Plane of Hate from some normal creatures.");
quest::say("Go Hurry and we will reward you with the extra spell casting slot we promised!")
}
}


sub EVENT_ITEM {
quest::say("Disabled for this season");
return;

$checkHandIn = plugin::check_handin(\%itemcount,145460=> 1);
if ( $checkHandIn ) {
#make sure they are casters if not stop!
if (
 $class eq 'Warrior' ||
 $class eq 'Rogue' ||
 $class eq 'Monk' ||
 $class eq 'Berserker' ||
 $class eq 'Shadowkight' ||
 $class eq 'Paladin' ||
 $class eq 'Ranger' ||
 $class eq 'Bard' ||
 $class eq 'Beastlord' ||
 $class eq 'Cleric' ||
 $class eq 'Druid' ||
 $class eq 'Shaman' ) {
quest::say("You aren't the correct class $name, You are a $class.");
quest::summonitem(145460);
return;
}

if ($client->GetGlobal("DEVADDSLOT") > 0)
{
quest::say("You have already claimed this! You only get one.");
quest::summonitem(145460);
return;
}


if ( $client->GetLevel() < 45 )
{
quest::summonitem(145460);
quest::say("Sorry, $name you must be 45 to claim this ticket. Come back later.");
return;
}
$client->IncrementAA(1071);
quest::say("You should now have an extra spell slot! Please camp, or zone to be sure!");
quest::setglobal("DEVADDSLOT", 1, 5, "F");
return;
}

quest::say("Whats this crap? I guess I can wipe my ass with it.");
}
