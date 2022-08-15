sub EVENT_SPAWN
{
$npc->SetSlotTint(1, 255,43, 43); #1 for chest
}

sub EVENT_SAY { 
if ( $client->GetPVPFlag() == 0 )
{
if($text=~/Hail/i){
$link = quest::saylink("home city");
quest::say("Greetings. $name.  I can bind you to your $link if you wish.");
}
elsif($text=~/freeport/i){
if($race=~/human/i){
quest::rebind(9,117.6,117.8,-24.6);
quest::say("You have been bound to Freeport.");
}
}
elsif($text=~/qeynos/i){
if($race=~/human/i){
quest::rebind(2,34.6,396.6,3.4);
quest::say("You have been bound to Qeynos.");
}
}
elsif($text=~/home city/i){
if($race=~/dark elf/i){
quest::rebind(40,157,-3,31.4);
quest::say("You have been bound to Neriak.");
}
elsif($race=~/human/i){
$fp = quest::saylink("Freeport");
$qey = quest::saylink("Qeynos");
quest::say("I can bind you to $fp or $qey.");
}
elsif($race=~/barbarian/i){
quest::rebind(29,10.3,-483.4,1.4);
quest::say("You have been bound to Halas.");
}
elsif($race=~/erudite/i){
quest::rebind(38,245.7,2161.9,-46.1);
quest::say("You have been bound to Erudin(Tox).");
}
elsif($race=~/gnome/i){
quest::rebind(56,578,-1713,-108);
quest::say("You have been bound to Akanon (Steamfont).");
}
elsif($race=~/wood elf/i){
quest::rebind(54,-267.7,80,-.4);
quest::say("You have been bound to Kelethin (Greater Faydark).");
}
elsif($race=~/high elf/i){
quest::rebind(61,171.1,-32.1,3.4);
quest::say("You have been bound to Felwithe.");
}
elsif($race=~/dwarf/i){
quest::rebind(60,-1.8,-31.2,3.4);
quest::say("You have been bound to Kaladim.");
}
elsif($race=~/troll/i){
quest::rebind(52,0,-100,3.4);
quest::say("You have been bound to Grobb.");
}
elsif($race=~/ogre/i){
quest::rebind(49,-71.4,-355.5,3.4);
quest::say("You have been bound to Oggok.");
}
elsif($race=~/half elf/i){
quest::rebind(3,154.7,-104.3,3.4);
quest::say("You have been bound to Surefall Glade.");
}
} #close home city

} #close pvpflag
else
{
quest::say("I only bind players to their neutral home cities.  If you wish to return to a kingdom or the bandit home town talk to Fani.");
}

}