sub EVENT_SAY {
#quest::say("I am not permitted to train you at this time. $class");

if ( $class eq "Monk" or $class eq "Warrior" or $class eq "Rogue")
{
if ($ulevel < 51){
quest::say("Get to level 50 and then well talk");
return;
}
quest::untraindiscs();
quest::traindiscs($ulevel,50);
quest::say("You have been trained in your disciplines, go now and kill!");
return;
#quest::say("I cannot provide you any disciplines, only a warrior, monk or rogue is worthy of my assistance.");
}
#elsif ( $ulevel > 50 ) {
#quest::untraindiscs();
#quest::traindiscs($ulevel,50);
#quest::say("You have been trained in your disciplines, go now and kill!");
quest::say("I cannot provide you any disciplines, only a warrior, monk or rogue is worthy of my assistance.");

#}
}
