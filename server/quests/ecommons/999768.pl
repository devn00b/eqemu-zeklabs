
sub EVENT_SAY { 
if (!defined $qglobals{"EpicV1"}) {
quest::say("You must complete your epic upgrade V1 first. I'm not interested in speaking to scum like you $name.");
return;
}
if(defined $qglobals{"EpicV2"}){
quest::say("Grrr Bark");
return;
}


if ($text=~/hail/i) { 
my $upgrd = quest::saylink("upgrade");
quest::say("Hello! $name so you want to [$upgrd] your epic again?");
}
 
if ($text=~/upgrade/i) { 
my $epictoken1 = quest::varlink(147606);
my $epictoken2 = quest::varlink(147607);
my $epictoken3 = quest::varlink(147608);
my $epictoken4 = quest::varlink(147609);



quest::say("So I need 4 items. One from Solb [$epictoken1], One from Gukbottom [$epictoken2], One from Fear [$epictoken3], and finally One from Air Plane [$epictoken4]. Bring me these and we can add a 2nd augment slot to your epic! I will also take your epic V1 and toss it in the trash for you.");
$client->Message(13,"*** NOTE Make sure you remove any augments, because I WILL eat them ***");

}

}


sub EVENT_ITEM {
if (!defined $qglobals{"EpicV1"}) {
quest::say("You must complete your epic upgrade V1 first. Because you tried to go arround that I'm keeping your stuff...Thanks!");
return;
}
if(defined $qglobals{"EpicV2"}){
quest::say("Grrr Bark");
return;
}


############## V2 EPIC UPGRADE ##############


if(plugin::check_handin(\%itemcount, 147606 => 1) && plugin::check_handin(\%itemcount, 147607 => 1) && plugin::check_handin(\%itemcount, 147608 => 1) && plugin::check_handin(\%itemcount, 147609 => 1)) {
if (!defined $qglobals{"EpicV2"}) {
quest::setglobal("EpicV2", 1, 5, "F");
}

if($class eq 'Bard') {
#147629 - Singing Short Sword 1 slot.
#delete old item
$client->NukeItem(147629);
#summon new item
quest::summonitem(147630);
quest::say("Here you go! Now go forth and slay other mortals!");
return;
}


if($class eq 'Cleric') {
#147631 - Water Sprinkler of Nem Ankh 1 slot.
#delete old item
$client->NukeItem(147631);
#summon new item
quest::summonitem(147632);
quest::say("Here you go! Now go forth and slay other mortals!");
return;
}

if($class eq 'Druid') {
#147633 - Nature Walkers
#delete old item
$client->NukeItem(147633);
#summon new item
quest::summonitem(147634);
quest::say("Here you go! Now go forth and slay other mortals!");
return;
}


if($class eq 'Enchanter') {
#147633 - Nature Walkers
#delete old item
$client->NukeItem(147635);
#summon new item
quest::summonitem(147636);
quest::say("Here you go! Now go forth and slay other mortals!");
return;
}

if($class eq 'Monk') {
#147633 - Nature Walkers
#delete old item
$client->NukeItem(147637);
#summon new item
quest::summonitem(147638);
quest::say("Here you go! Now go forth and slay other mortals!");
return;
}


if($class eq 'Necromancer') {
#147633 - Nature Walkers
#delete old item
$client->NukeItem(147639);
#summon new item
quest::summonitem(147640);
quest::say("Here you go! Now go forth and slay other mortals!");
return;
}


if($class eq 'Paladin') {
#147633 - Nature Walkers
#delete old item
$client->NukeItem(147641);
#summon new item
quest::summonitem(147642);
quest::say("Here you go! Now go forth and slay other mortals!");
return;
}

if($class eq 'Ranger') {
#147633 - Nature Walkers
#delete old item
$client->NukeItem(147643);
#summon new item
quest::summonitem(147644);
quest::say("Here you go! Now go forth and slay other mortals!");
return;
}


if($class eq 'Rogue') {
#147633 - Nature Walkers
#delete old item
$client->NukeItem(147647);
#summon new item
quest::summonitem(147648);
quest::say("Here you go! Now go forth and slay other mortals!");
return;
}

if($class eq 'Shadow Knight') {
#147633 - Nature Walkers
#delete old item
$client->NukeItem(147649);
#summon new item
quest::summonitem(147650);
quest::say("Here you go! Now go forth and slay other mortals!");
return;
}


if($class eq 'Shaman') {
#147633 - Nature Walkers
#delete old item
$client->NukeItem(147651);
#summon new item
quest::summonitem(147652);
quest::say("Here you go! Now go forth and slay other mortals!");
return;
}

if($class eq 'Warrior') {
#147633 - Nature Walkers
#delete old item
$client->NukeItem(147653);
$client->NukeItem(147660);
$client->NukeItem(10909);
#summon new item
quest::summonitem(147654);
#1h epic primary
quest::summonitem(147661);
#1h epic secondary
quest::summonitem(10909);
quest::say("Here you go! Now go forth and slay other mortals!");
return;
}



if($class eq 'Wizard') {
#147633 - Nature Walkers
#delete old item
$client->NukeItem(147655);
#summon new item
quest::summonitem(147656);
quest::say("Here you go! Now go forth and slay other mortals!");
return;
}


if($class eq 'Magician') {
#147633 - Nature Walkers
#delete old item
$client->NukeItem(147657);
#summon new item
quest::summonitem(147658);
quest::say("Here you go! Now go forth and slay other mortals!");
return;
}


if($class eq 'Beastlord') {
#148606 - BL offhand
#delete old item
$client->NukeItem(148606);
#summon new item
quest::summonitem(148607);
quest::say("Here you go! Now go forth and slay other mortals!");
return;
}


}

############## V2 EPIC UPGRADE END ##############

quest::say("Welp, this shit is mine. Thanks!!!");
return;

}
