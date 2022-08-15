sub EVENT_SAY {
  if ($text=~/hail/i) {
    quest::say("Away from carver Cagrek you get!! Me's a busy troll. Must punish enmees of Grobb and feeds trolls a plenty. Trolls complain meats no good, complains dey want sum [special bread]. Dey says dey hears ogres have special meat which taste better den pris'ners of Grobb. Do you knows name of meat? What kinda [meat] dat be? Tell me!!");
  }
  elsif ($text=~/HEHE meat/i) {
    quest::say("HEHE meat!! What dat stand for? Mes need some of dis HEHE meat!! Gos and get me some. Cagrek not just want some. Cagrek want more dan twenny!! Cagrek want three HEHE meat and mes want HEHE recipe. Youg et and me makes crave honeraree carver. Me gives you Grobb Cleaver. It cut skins like dey butter!!");
  }
}

sub EVENT_ITEM {
  if (plugin::check_handin(\%itemcount, 13368 => 3,18940 => 1)) {
    quest::say("Dis is great!  Take from me you deserve.");
    quest::summonitem(5413);
    quest::faction( 131, 15);
  }
}