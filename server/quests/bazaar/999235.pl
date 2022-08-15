sub EVENT_SAY {
#my $careerptz = $client->GetPVPCareerPoints();

if($ulevel >30)
{
quest::say("Sorry $name, but you are too strong to join our little group. We are only for the weaklings trying to learn the system. You are level $ulevel and we only take players upto level 30.");
return;
}

if($uguild_id > 0)
{
quest::say("Why are you talking to me $name, you already have a guild. If you want to talk about joining Dentists please leave your guild 1st");
return;
}

if(($text=~/hail/i) && ($ulevel <31) && ($uguild_id eq '-1')){
quest::say("Welcome $name . Are you having trouble finding a guild? I'm here to recruit players like you to join devnoob in his guild. And I think you have what it takes.");
quest::say("All you have to do is [accept] the charter and your in");
return;
  }

if($ulevel <10) {
quest::say("Im sorry $name but you must be at least level 10 to join.");
return;
}

if(($text=~/accept/i) && ($ulevel >9) && $uguild_id eq '-1' ){
#$newguildname = "Soulblighters Dominion";

quest::say("You are now one of us $name, Please make your self at home. Be sure to read the MOTD!");
quest::say("Here you can feel safe. But **BEWARE**, once you reach level 31 you will be kicked out and you will have to find other friends.");
quest::setguild(69, 0)
}

#quest::say("Sorry What?");

} 
