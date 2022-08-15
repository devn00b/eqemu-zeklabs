#REVISED: Angelox
#Zone: timorous
sub EVENT_SAY { 
if ($text=~/Hail/i){quest::say("Hello there. We have most the ships working again. If you want to know more about [travel to Overthere], [travel to Oasis] or [travel to Firiona] I can help you with this."); }
if ($text=~/travel to oasis/i){quest::movepc(37,-833, 882, .07); }
if ($text=~/travel to overthere/i){quest::movepc(93,2744.7, 3415.6, -158.3); }
if ($text=~/travel to firiona/i){quest::movepc(84,1405,-4378.6,-103.4); }
}
