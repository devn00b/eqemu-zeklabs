sub EVENT_SAY {
  if($text =~ /Hail/i) {
$playflag = $client->GetPVPFlag();
$interestedLink = quest::saylink("interested"); 
  quest::say("Heyo $name! Image has sent me to help you mere mortals. I can now let you overcome the magic that is preventing everyone from going anon and roleplay are you [$interestedLink]"); 
  quest::doanim(15);
  if($playflag eq '5'){
$interested2 = quest::saylink("anon");
$interested3 = quest::saylink("roleplay");
quest::say("Oh, your a discord player. I can set you [$interested2] or [$interested3] if you want.");
return;
}

if(defined $qglobals{"anonroleplayx"}) {
$interested2 = quest::saylink("anon");
$interested3 = quest::saylink("roleplay");
quest::say("");
quest::say("");
quest::say("Since you already paid within the last 12 hours  I can set you [$interested2] or [$interested3] if you want.");
}

  }
  
  if($text=~/interested/i) {
quest::say("So as with everything with Image, he requires me to collect payment. He wants one Golden Orb of the Sun. I hear Devnoobs Forge Master in the bazaar makes and sells them. Once you have that come see me");
}

  if($text=~/anon/i && $playflag eq 5) {
    $client->SetAnon(1);
    quest::say("Congrats $name your now anon");
    return;
  }

  if($text=~/roleplay/i && $playflag eq 5) {
    $client->SetAnon(2);
    quest::say("Congrats $name your now roleplaying");
    return;
  }

##
  if($text=~/anon/i && $playflag ne 5 && defined $qglobals{"anonroleplayx"}) {
    $client->SetAnon(1);
    quest::say("Congrats $name your now anon");
    return;
  }

  if($text=~/roleplay/i && $playflag ne 5 && defined $qglobals{"anonroleplayx"}) {
    $client->SetAnon(2);
    quest::say("Congrats $name your now roleplaying");
    return;
  }


##
}

sub EVENT_ITEM {
#
# Golden Orb of The Sun (Anon)     - 146675 - 50pts
# Golden Orb of The Sun (Roleplay) - 146676 - 50pts
#

  if(plugin::check_handin(\%itemcount, 146675 => 1)){ 
    quest::say("Thank you! Please enjoy being Anon $name!");
    quest::ding();
    $client->SetAnon(1);
#quest::setglobal("Gamble", ($cl, 5, "H12");
quest::setglobal("anonroleplayx", 1, 7, "H12");
    return;
  }

  if(plugin::check_handin(\%itemcount, 146676 => 1)){
    quest::say("Thank you! Please enjoy being Roleplay $name");
    quest::ding();
    $client->SetAnon(1);
#quest::setglobal("Gamble", ($client->GetGlobal("Gamble") + $winnings), 5, "H12");
quest::setglobal("anonroleplayx", 1, 7, "H12");
    return;
  }


quest::say("Please, don't tell Devnoob or Image about me, I would hate to have to go on the run, and stop selling you my special wares.");
}

