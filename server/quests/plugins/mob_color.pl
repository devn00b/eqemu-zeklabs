sub SetMobColor {
	plugin::val('npc')->WearChange($_, plugin::val('npc')->GetTexture(), plugin::MobColor($_[0], $_[1], $_[2])) for (0..6);
}

sub MobColor {
	my $color = 0;
	$color += (((256 ** 2) * $_[0]) + (256 * $_[1]) + $_[2]);
	return $color;
}

return 1;
