#BeginFile qeynos2\Velarte_Selire.pl (2052)

sub EVENT_ITEM {
  if(plugin::check_handin(\%itemcount, 2584  => 1,2585 => 1, 2586 => 1)) { #Live: Enchanted Rat Jar ...
    quest::say("You did it! You have proven you have the intelligence required and you may wear this Researcher's Badge with pride! Thank you for returning the jars.");
    quest::summonitem(2583); #Researcher's Briefing
    quest::exp(2000);
  }
  else {
    quest::say("I have no use for this, $name.");
    plugin::return_items(\%itemcount);
  }
}
#EndFile qeynos2\Velarte_Selire.pl (2052)