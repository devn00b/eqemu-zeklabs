my $qeynosShield = "5414";

sub EVENT_DEPOP{
quest::creategroundobject($qeynosShield, $x, $y, $z, $h, 300000);
}

sub EVENT_DEATH {
quest::creategroundobject($qeynosShield, $x, $y, $z, $h, 300000);
}