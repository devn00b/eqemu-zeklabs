sub EVENT_SPAWN {
$qglobals{kerafyrm} = undef;

 if((!defined $qglobals{kerafyrm}) || (defined $qglobals{kerafyrm} && $qglobals{kerafyrm} != 3)) {
  quest::settimer("kerafyrmwuoshi",10);
}
}

sub EVENT_TIMER {

 if($timer eq "kerafyrmwuoshi" && defined $qglobals{kerafyrm} && $qglobals{kerafyrm} == 1) {
  quest::shout("The Sleeper has been awakened! Shun the Skyshrine if you value your lives, for he shall be here soon!");
  quest::stoptimer("kerafyrmwuoshi");
}

}