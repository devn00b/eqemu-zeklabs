sub EVENT_SAY {

if(($text=~/hail/i) ){
$link = quest::saylink("epic");
quest::say("Hello $name are you interested in your [$link]?");
}

if(($text=~/epic/i) ){
$sure = quest::saylink("sure");
quest::say("Yes, sadly someone bought all of our stock. Now the only way to get them is to trade for them. I have a contact that can get them, but his price is high.");
quest::say("You see. The crazy old man, he uh, will only accept them for Golden Tickets, and well, I only know of 2 ways to get them. Are you [$sure] you want to attempt this? I won't be responsible if you end up dead!");
}

if(($text=~/sure/i) ){
$whatsnext = quest::saylink("next");

quest::say("Ok, well the first and probably easier and safer way is to go talk to Fibrus the Tall, a ridiculous name as he is a gnome, but whatever. He runs a casino in Highkeep and I've heard of winners there getting them.");
quest::say("The second and most difficult way is to fight Lord Nagafen, Lady Vox, or Innoruuk himself. I beg you to not go this route. None who I've sent out have come back from trying it this way...The choice is yours, however. After that we can talk about what comes [$whatsnext].");
}

if(($text=~/next/i) ){
quest::say("The final thing is quite easy. You just need to buy one Epic Marker from here in the bazaar. Bring me the marker and the golden ticket and I'll get you your epic. Assuming you live.");
}

} 


sub EVENT_ITEM {
  if (plugin::check_handin(\%itemcount, 145560 => 1) && plugin::check_handin(\%itemcount, 145561 => 1)){ ## Golden Tix (145560) and Epic Marker (145561)
    quest::say("Wow! I never thought you could do it. Really! Well here you go, your epic, and I'm going to go ahead and give you a whole level as well!");
    quest::ding();
    $client->AddLevelBasedExp(100, 50); ## 100% exp upto level 50.

##decide who gets what
if($class eq 'Warrior'){
quest::summonitem(17859);
quest::summonitem(10908);
return;
}
if($class eq 'Bard'){
quest::summonitem(20542);
return;
}
if($class eq 'Cleric'){
quest::summonitem(5532);
return;
}
if($class eq 'Enchanter'){
quest::summonitem(10650);
return;
}
if($class eq 'Druid'){
quest::summonitem(20490);
return;
}
if($class eq 'Monk'){
quest::summonitem(10652);
return;
}
if($class eq 'Necromancer'){
quest::summonitem(20544);
return;
}
if($class eq 'Paladin'){
quest::summonitem(10099);
return;
}
if($class eq 'Ranger'){
quest::summonitem(20488);
quest::summonitem(20487);
return;
}
if($class eq 'Rogue'){
quest::summonitem(11057);
return;
}
if($class eq 'Shadowknight'){
quest::summonitem(14383);
return;
}
if($class eq 'Shaman'){
quest::summonitem(10651);
return;
}
if($class eq 'Wizard'){
quest::summonitem(14341);
return;
}
if($class eq 'Magician'){
quest::summonitem(28034);
return;
}
if($class eq ''){
return;
}
#shouldnt get here since it returns once it gives epic above but lets be sure
return;
  }
quest::say("Whats this shit $name? Well, to the trash that goes. God, I hate $class 's");
}

