sub EVENT_SPAWN {
$qglobals{kerafyrm} = undef;

 if((!defined $qglobals{kerafyrm}) || (defined $qglobals{kerafyrm} && $qglobals{kerafyrm} != 3)) {
  quest::settimer("kerafyrmkland",10);
}
}

sub EVENT_TIMER {

 if($timer eq "kerafyrmkland" && defined $qglobals{kerafyrm} && $qglobals{kerafyrm} == 1) {
  quest::shout("Damnation and doom! Some fool has awakened The Sleeper! Avoid those places where Dragons dwell, for Kerafyrm shall surely seek revenge upon us!");
  quest::stoptimer("kerafyrmkland");
}

}