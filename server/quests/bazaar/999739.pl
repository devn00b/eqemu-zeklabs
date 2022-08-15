sub EVENT_SAY {
if($ulevel <35) {
quest::say("No.No.NO $name. You just dont understand. No. Listen $name. You are just wrong.");
return;
}
if($ulevel >=35) {
quest::say("Soo, here is the deal, I got this code here, call it a..um..hack. You bring me these items, and I'll use my hacks and give you something cool in return!");
quest::say("");
return;
} 
}

sub EVENT_ITEM {

  if (plugin::check_handin(\%itemcount, 1365 => 1) && plugin::check_handin(\%itemcount, 2300 => 1)){ #### fbss, jboots
if($ulevel <35) {
quest::say("Why would I do shit for you $name. You haven't been around long enough to earn my trust. so take your crap and go away.");
quest::summonitem(2300);
quest::summonitem(1363);
return;
}
    quest::say("Wow you actually did it! One sec and I'll see what I can do");
    quest::say("Boom");
    quest::say("Wow, that was some leet hacks..here you go..oh and...Please dont tell devnoob he'll get pissed...");
    quest::ding();
    quest::summonitem(146734);
    quest::summonitem(146733);
    $client->AddLevelBasedExp(100, 40); ## 25% exp, upto level 40
    return;
  }
quest::say("What do you expect me to do with this shit? Do you even think?");
}
