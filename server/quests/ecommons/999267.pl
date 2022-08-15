#Captain npc for quest event
#freeport weapon 22260
#weapon spawner 999268
#follow npc (merchant to take to wcommons) is 999269

my $isEscortAvailable = "1";

sub EVENT_SPAWN
{
quest::spawn2(999268,127,0,983.3, -1009.9, -13.5, 223.5);
quest::settimer("spawner",270);
quest::settimer("depop",21600);
}

sub EVENT_TIMER {
if ( $timer eq "spawner" )
{
quest::spawn2(999268,127,0,983.3, -1009.9, -13.5, 223.5);
}
elsif ( $timer eq "depop" )
{
quest::depop(999267);
}
elsif ( $timer eq "resettimer" )
{
$isEscortAvailable = "1";
quest::stoptimer("resettimer");
}
}

sub EVENT_SAY {
   if($text=~/hail/i)
   {
	$link = quest::saylink("weapons");
      quest::say("Hail adventurer!  No time for introductions I am afraid.  The undead masses are upon us!  I am setting up a temporary armory here and require assistance collecting $link.");
   }
   elsif($text=~/weapons/i)
   {
      quest::say("Yes, our weapons scatter the land around here, many have fallen to the undead, collect weapons on the ground and return them here.");
   }
   elsif ( $isEscortAvailable eq "0" )
   {
      quest::say("Please return later as I may have more tasks for you.");
   }
   elsif($text=~/task/i)
   {
      $link = quest::saylink("interested");
      quest::say("We have a need for a strong escort to help our citizens return home safely.  Would you be $link?");
   }
   elsif($text=~/interested/i)
   {
      $isEscortAvailable = "0";
      quest::spawn2(999269,0,0, 58.3, -124.9, -51.6, 3.6);
      quest::signalwith(999269,$userid,0);
      quest::settimer("resettimer",600);
   }
   
}

sub EVENT_ITEM {
    if(plugin::check_handin(\%itemcount, 22260 => 1)) { #freeport short sword
	$link = quest::saylink("task");
      quest::say("Very good adventurer.  I will add these to the armory.  If you would like to make yourself useful again I have another $link for you.");
      quest::givecash(0,0,0,1);
	quest::exp(275000);
    }
}

sub EVENT_SIGNAL {
if ( $signal eq "1" )
{
$isEscortAvailable = "1";
}
}