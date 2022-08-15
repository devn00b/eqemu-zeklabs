
sub EVENT_SAY { 

if ($text=~/hail/i) { quest::say("Hello! If your here for a title please give me your paperwork");
}
}


sub EVENT_ITEM {

#20542	Singing Short Sword
if(plugin::check_handin(\%itemcount, 20542 => 1)) {
#147629 - Singing Short Sword 1 slot.
quest::summonitem(147629);
quest::say("Here you go! Now go forth and slay other mortals!");
}


#5532	Water Sprinkler of Nem Ankh
elsif(plugin::check_handin(\%itemcount, 5532 => 1)) {
#147631 - Water Sprinkler of Nem Ankh 1 slot.
quest::summonitem(147631);
quest::say("Here you go! Now go forth and slay other mortals!");
}

#20490	Nature Walkers Scimitar
elsif(plugin::check_handin(\%itemcount, 20490 => 1)) {
quest::summonitem(147631);
quest::say("Here you go! Now go forth and slay other mortals!");
}

#10650	Staff of the Serpent
elsif(plugin::check_handin(\%itemcount, 10650 => 1)) {
#147635 - Staff of the Serpent
quest::summonitem(147635);
quest::say("Here you go! Now go forth and slay other mortals!");
}

#10652	Celestial Fists
elsif(plugin::check_handin(\%itemcount, 10652 => 1)) {
#147637 - Celestial Fists
quest::summonitem(147637);
quest::say("Here you go! Now go forth and slay other mortals!");
}

#20544	Scythe of the Shadowed Soul
elsif(plugin::check_handin(\%itemcount, 20544 => 1)) {
#147639 Scythe of the Shadowed Soul
quest::summonitem(147639);
quest::say("Here you go! Now go forth and slay other mortals!");
}

#10099	Fiery Defender
elsif(plugin::check_handin(\%itemcount, 10099 => 1)) {
#147641 - Fiery Defender
quest::summonitem(147641);
quest::say("Here you go! Now go forth and slay other mortals!");
}

#20488	Earthcaller
elsif(plugin::check_handin(\%itemcount, 20488 => 1)) {
#147643 - Earthcaller
quest::summonitem(147643);
quest::say("Here you go! Now go forth and slay other mortals!");
}

#11057	Ragebringer
elsif(plugin::check_handin(\%itemcount, 11057 => 1)) {
#147647 - Ragebringer
quest::summonitem(147647);
quest::say("Here you go! Now go forth and slay other mortals!");
}

#14383	Innoruuk's Curse
elsif(plugin::check_handin(\%itemcount, 14383 => 1)) {
#147649 - Innoruuk's Curse
quest::summonitem(147649);
quest::say("Here you go! Now go forth and slay other mortals!");
}

#10651	Spear of Fate
elsif(plugin::check_handin(\%itemcount, 10651 => 1)) {
#147651 - Spear of Fate
quest::summonitem(147651);
quest::say("Here you go! Now go forth and slay other mortals!");
}

#10908	Jagged Blade of War
elsif(plugin::check_handin(\%itemcount, 10908 => 1)) {
#147653 - Jagged Blade of War
quest::summonitem(147653);
quest::say("Here you go! Now go forth and slay other mortals!");
}

#14341	Staff of the Four
elsif(plugin::check_handin(\%itemcount, 14341 => 1)) {
#147655 - Staff of the Four
quest::summonitem(147655);
quest::say("Here you go! Now go forth and slay other mortals!");
}

#28034	Orb of Mastery
elsif(plugin::check_handin(\%itemcount, 28034 => 1)) {
#147657 - Orb of Mastery
quest::summonitem(147657);
quest::say("Here you go! Now go forth and slay other mortals!");
}

return;

}
