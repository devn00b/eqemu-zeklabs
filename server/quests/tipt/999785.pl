sub EVENT_SAY {

if($text=~/hail/i) {
$lvl20=quest::saylink("level 20");
$lvl25=quest::saylink("level 25");
$lvl30=quest::saylink("level 30");
$lvl35=quest::saylink("level 35");
$lvl40=quest::saylink("level 40");
$lvl45=quest::saylink("level 45");
$lvl50=quest::saylink("level 50");
$lvl55=quest::saylink("level 55");

quest::say("Hello there $name! Are you seeking an adventure? If so just tell me if you want to learn about opportunities for [$lvl20], [$lvl25], [$lvl30], [$lvl35], [$lvl40], [$lvl45], [$lvl50], or [$lvl55].");
}
if($text=~/level 20/i) {
quest::say("You may find the expansion zones Crystal Caverns, Echo Caverns perfect for you. For Classic Zones, you may want to try Oasis, there is a quest there for EXP potions.");
}
if($text=~/level 25/i) {
quest::say("Ah level 25, what a fun time. Expansion zones for you are limited but there is Dreadlands. For classic zones Befallen has some rare but good stuff, and there is always Najena for some JBoots");
}
if($text=~/level 30/i) {
quest::say("Ooo 30, a time when things get intense! The Expansion zones for this cap are a blast! There is Kaesora a fun zone, and Dawnshroud, a zone that drops augments and gear to put them in. For classic you can start heading into SolB and Lower Guk There you can get a manastone! For fun you should check out the casino in highkeep");
}
if($text=~/level 35/i) {
quest::say("Lets be honest 35 is boring! There are no new expansion zones. And the best zones for you are the same as level 30. Lower Guk, Solb");
}
if($text=~/level 40/i) {
quest::say("Things start to get really going now! Expansion zones available are Charasis and Karnor Where you can get a Tstaff and a Windstriker. Classic Zones would be GukBottom, Solb, Permafrost, and perhaps thinking about Dragons!");
}
if($text=~/level 45/i) {
quest::say("Whew your almost to the finish line now! Expansion zones are Kael and Akheva, But be warned Akheva is REALLY HARD. Classic Zones are the same as above, but you should be focusing on naggy, and vox and the Plane of Sky mobs!");
}
if($text=~/level 50/i) {
quest::say("Only 5 more to go! Expansion zone is just ToV. Zones are the same as before, get that gear and kill raid targets. May I suggest the Manastone Quest in East Commons, or getting your epic and starting the Epic Upgrade quests in East Commons");
}
if($text=~/level 55/i) {
quest::say("Whew thats it! You made it! You may wish to re-visit akheva and try the raid targets. Or how about Just PVPing!!");
}
quest::say("");
quest::say("..And, just so you know, the porter for all the expansion zones can be found in Highkeep, and Oasis. You can Port silently by buying a token from them, 
or via hail.");

}
