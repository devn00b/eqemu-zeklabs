sub EVENT_SAY
{
#if ( $client->GetGM() == 1 )
#{
#quest::setglobal("akanon_race_besthour","0",7,"S1");
#quest::setglobal("akanon_race_bestmin","50",7,"S1");
#quest::setglobal("akanon_race_bestsec","11",7,"S1");
#quest::setglobal("akanon_race_bestname","Madmartigan",7,"S1");
#}

if ( $client->GetGM() == 0 && $client->GetBaseRace() != 12 )
{
quest::say("Stranger danger! Stranger danger!!");
quest::doanim(30);
return;
}
if ( !defined $qglobals{akanon_race_playerstep} || $qglobals{akanon_race_playerstep} < 9 )
{
quest::say("I have nothing to say to you at this time.");
return;
}

if ( defined $qglobals{akanon_racefinish_playerflag} )
{

if ( !$qglobals{akanon_finish_returnping} )
{
quest::setglobal("akanon_finish_returnping","1",5,"F");
quest::say("Thank you so much for your assistance! You have saved what is left of my tattered reputation... take this gift with my warmest regards!!");
quest::givecash(0.0,0,500,0);
$client->SummonItem(142822); #gnome tinkered augment MMXVII (2017)
}
else
{
quest::say("Thank you again for what you have done, I hope the coin I have provided is sufficient for your troubles as I have nothing left.  I shall always be in your debt, good travels to you.");
}
return;
}

if($text=~/Hail/i){
$text = quest::saylink("creations");

if ( $client->GetLevel() < 4 )
{
quest::level(4);
}
quest::say("Oh dear.. not again.. not again.. My $text are only intended for good!! I swear it!");
}
elsif($text=~/creation/i){
$text = quest::saylink("what is happening");
quest::doanim(56);
quest::say("Mmm... I am not sure I should be talking to you... I don't want word to get back home about $text...");
}
elsif($text=~/happening/i){
$text = quest::saylink("help");
quest::doanim(50);
quest::say("Argh.  A mechanical creation of mine has lost its mind!  I don't know what to do it will not respond to me!  Can you please $text me stop it?");
}
elsif($text=~/help/i){
quest::doanim(62);
quest::say("Thank you so much!! It has been running rampant in the area, it should not be far off from here.");

$item = $client->HasItem(142754);

if ( $client->HasItem(142754) == 5 && $client->GetBaseRace() == 12)
{
quest::say("..should you be able to stop this nightmare, the chest of the creature can be used to infuse other components into that Unicorn Eye Pendant dangling on your neck.  I imagine being a gnome on your adventures you have come by Unicorn Tears, Green Fairy Blood, and Sporali's Shrooms, yes??  These may come in valuable for you along with a few components from my creation.");
}
}
}