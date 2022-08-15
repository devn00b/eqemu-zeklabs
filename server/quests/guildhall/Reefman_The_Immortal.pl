sub EVENT_SAY {
 if($text=~/hail/i){
  quest::say("Greetings! I am Reefman The Immortal Keeper of the Commons, I stand watch over all who come to this place, and tend to the Noobs.I Have a long [story] with Legacy of Zek. If you care to hear it just ask!");
 }
 elsif($text=~/story/i){
  quest::say("I once was a regular mortal player, like yourself. However some time near the 23rd of July in the year 2013 I passed on, Having suffered from a stroke durring an operation to remove cancer from my brain. When I was awaken I found myself here with Image, and Devnoob looking over me. They asked me if I would rather stay here, Immortal but forever bound to this place, or face death eternal."); 
  quest::say("I Chose to stay here, amongst you my fellow players, as a reminder, that nothing is forever. Remember me always!");
 quest::summonitem(69230);
quest::summonitem(69230);
 }
} 

#Submitted by: Jim Mills
