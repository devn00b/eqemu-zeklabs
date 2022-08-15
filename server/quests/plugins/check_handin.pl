# plugin::check_handin($item1 => #required_amount,...);
# autoreturns extra unused items on success
sub check_handin {
    my $hashref = shift;
    my %required = @_;
    foreach my $req (keys %required) {
	if ((!defined $hashref->{$req}) || ($hashref->{$req} != $required{$req})) {
            return(0);
	}
    }
     foreach my $req (keys %required) {
         if ($required{$req} < $hashref->{$req}) {
             $hashref->{$req} -= $required{$req};
         } else {
             delete $hashref->{$req};
         }
     }
     return 1;
}


sub return_items {    

}

