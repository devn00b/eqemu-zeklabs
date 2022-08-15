sub EVENT_SAY {
if ( $class != "Monk" && $class != "Warrior" && $class != "Rogue" )
{
quest::say("I cannot provide you any disciplines, only a warrior, monk or rogue is worthy of my assistance.");
}
elsif ( $ulevel > 60 ) {
quest::traindiscs($ulevel,50);
}
}