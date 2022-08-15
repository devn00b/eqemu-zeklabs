#This is actually Rodcet Nife in Koalindl fish form. He's here to make life hard for anyone trying to mass-farm Koalindl fish for the Stein of Moggok quest.
sub EVENT_AGGRO {
 quest::say("Foolish mortal! You have disturbed the Prime Healer as he swims in the guise of his creatures. For this... you shall perish!");
}

sub EVENT_SLAY {
 quest::say("That will teach you to interfere with the prime healer as he swims with his creatures!"); #Need actual text, but this is close
}

#Submitted by Jim Mills