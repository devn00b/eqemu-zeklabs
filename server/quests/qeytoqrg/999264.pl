# Quest Event NPC for Qeynos Hills Undead Attack
#npcid 999265 is the shield spawner, runs around and spawns shields randomly
#npcid 999266 is the qeynos diplomat to escort.

my $diplomatEscort = "1";

sub EVENT_SPAWN {
quest::spawn2(999265,113,0,140.0, -145.6, -4.4, 196.1);
quest::settimer("spawner",180);
quest::settimer("depopme",21600); #2 hours
}

sub EVENT_SAY{
   
   if($text=~/hail/i)
   {
	$link = quest::saylink("infested");
      quest::say("Hail adventurer! I am Corporal Gravlin, temporarily assigned to defend the path from Qeynos Hills to our great city of Qeynos.  
If you were planning to venture through Qeynos Hills I advise to turn back, the routes have become $link with undead.");
   }
   if($text=~/infested/i)
   {
     $level = quest::getlevel(0);
	if ( $level > 5 )
	{
		$link = quest::saylink("interested");
		$npc->Emote("tilts his visor back.");
		quest::say("Yes, unfortunately an old foe has unleashed massive undead upon us, our guards are struggling to hold their ground as we speak.  Although looking at you more closely I think you may be of use to us if you are $link.");
	}
	else
	{
      		quest::say("Yes, unfortunately an old foe has unleashed massive undead upon us, our guards are struggling to hold their ground as we speak.");
	}
   }
   if($text=~/interested/i)
   {
	quest::say("We have had many soldiers fall, we collected our dead but are running low on armor.  Help collect four Shields of the Qeynos Guard from the area and return them to me.");
   }
   if($text=~/escort/i)
   {
     $level = quest::getlevel(0);
	if ( $level > 5 )
	{
	if ( $diplomatEscort eq "1" )
	{
	$link = quest::saylink("challenge");
	quest::say("We need assistance escorting a Qeynos Diplomat to Surefall Glade.  If you would be willing to take the $link we would be in your debt.");
	}
	else
	{
	quest::say("Apologies at this time someone is already escorting our diplomat at this time.  Later we may need assistance as we have many diplomats that travel to Surefall Glade.");
	}
	}
   }
   if($text=~/challenge/i)
   {
     $level = quest::getlevel(0);
	if ( $level > 5 )
	{
	if ( $diplomatEscort eq "1" )
	{
	quest::spawn2(999266,0,0,140.0, -145.6, -4.4, 196.1);
	quest::signalwith(999266,$userid,0);
	$diplomatEscort = "0";
	quest::settimer("diplomatreset",600);
	}
	else
	{
	quest::say("Apologies at this time someone is already escorting our diplomat at this time.  Later we may need assistance as we have many diplomats that travel to Surefall Glade.");
	}
	}
   }
}

sub EVENT_TIMER {
if ( $timer eq "spawner" )
{
quest::spawn2(999265,113,0,140.0, -145.6, -4.4, 196.1);
}
elsif ( $timer eq "diplomatreset" )
{
quest::stoptimer("diplomatreset");
$diplomatEscort = "1";
}
elsif ( $timer eq "depopme" )
{
quest::say("I have been called back to the city - I must leave now.  Be safe adventurers!");
quest::depop(999264);
}
}

sub EVENT_SIGNAL {
if ( $signal eq "1" )
{
$diplomatEscort = "1";
quest::stoptimer("diplomatreset");
}
}

sub EVENT_ITEM {
    if(plugin::check_handin(\%itemcount, 25008 => 4)) { #qeynos guard shield
	$link = quest::saylink("escort");
      quest::say("Well done!  Your assistance is appreciated in this difficult time.  Here is a small token of our appreciation, it is not much but we are putting forth all our effort against these undead.  I have an additional task if you would like to $link a diplomat.");
      quest::givecash(0,0,5,0);
	quest::exp("5000");
    }
}