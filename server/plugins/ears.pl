sub checkear_turnin {
	my $itemid = shift;
	my $itemname = shift;
	my $playername = shift;
	if($itemid == 0 )
	{
		return -1;
	}
	if($itemid == 106501 || $itemid == 67275 || $itemid == 9570 )
	{
		return 0;
	}
	elsif ( $itemid == 135039 || $itemid == 135038 )
	{
		#dev and image ears
		return 1;
	}
	elsif( $itemname=~/'s Ear\z/ )
	{
		$yourEar = $playername . "'s Ear";
		if ( $itemname=~/$yourEar/i) {
			quest::shout("KEEP YOUR OWN DISGUSTING EAR $playername!");
			quest::summonitem($itemid);
			return -1;
		}
		return 1;
	}

	return 0;
}

sub checkear_turnin_omitscarlet {
	my $itemid = shift;
	my $itemname = shift;
	my $playername = shift;
	if($itemid == 0 )
	{
		return 0;
	}

	if($itemid == 106501 || $itemid == 67275 || $itemid == 9570 || $itemid == 135039 || $itemid == 135038)
	{
		return 0;
	}
	elsif( $itemname=~/Image's Ear\z/ )
	{
		return 0;
	}
	elsif( $itemname=~/Devnoob's Ear\z/ )
	{
		return 0;
	}
	elsif( $itemname=~/'s Ear\z/ )
	{
		$yourEar = $playername . "'s Ear";
		if ( $itemname=~/$yourEar/i) {
			quest::shout("KEEP YOUR OWN DISGUSTING EAR $playername!");
			quest::summonitem($itemid);
			return -1;
		}
		return 1;
	}

	return 0;
}