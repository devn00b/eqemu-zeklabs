sub EVENT_SAY {
if ( $class != "Monk" && $class != "Warrior" && $class != "Rogue" )
{
quest::say("I cannot provide you any disciplines, only a warrior, monk or rogue is worthy of my assistance.");
}
elsif ( $ulevel > 50 ) {
#quest::traindiscs($ulevel,50);
quest::say("Sorry. I seem to of developed some memory loss. What am I doing here exactly?");

}
}
