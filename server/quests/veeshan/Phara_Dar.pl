sub EVENT_SPAWN {
$qglobals{kerafyrm} = undef;

 if((!defined $qglobals{kerafyrm}) || (defined $qglobals{kerafyrm} && $qglobals{kerafyrm} != 3)) {
  quest::settimer("kerafyrmphara",10);
}
}

sub EVENT_TIMER {

 if($timer eq "kerafyrmphara" && defined $qglobals{kerafyrm} && $qglobals{kerafyrm} == 1) {
  quest::shout("Attend me, members of the Ring. The Sleeper is free of his prison, and will surely be seeking revenge!");
  quest::stoptimer("kerafyrmphara");
}

}